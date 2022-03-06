#include "box.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>


//OneBox是从QGraphicsObject继承而来的
OneBox::OneBox(const QColor &color) : brushColor(color) {

}


//该函数为指定后面的绘图区域的外边框
QRectF OneBox::boundingRect() const {

    qreal pen_width = 1;
    //小方块的边长为20.5像素
    return QRectF(-10-pen_width/2, -10-pen_width/2, 20+pen_width, 20+pen_width);

}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

    //贴图，看起来有质感,否则单独用颜色去看，会感觉那些方块颜色很单一
    painter->drawPixmap(-10, -10, 20, 20, QPixmap(":/images/box.gif"));
    //设置画刷颜色
    painter->setBrush(brushColor);
    QColor penColor = brushColor;
    //将颜色的透明度减小，使方框边界和填充色直接能区分开
    penColor.setAlpha(20);
    //色绘制画笔
    painter->setPen(penColor);
    //这里画矩形框，框内填充部分用画刷画，框外线条用画笔画
    painter->drawRect(-10, -10, 20, 20);//画矩形框
}


//在局部坐标点上返回item的shape
QPainterPath OneBox::shape() const{
    //QPainterPath是一个绘图操作的容器
    QPainterPath path;
    path.addRect(-9.5, -9.5, 19, 19);
    return path;
}


//BoxGroup是从QGraphicsItemGroup，QObject继承而来的
BoxGroup::BoxGroup() {
    //允许设置输入焦点
    setFlags(QGraphicsItem::ItemIsFocusable);
    //返回当前item的变换矩阵,当BoxGroup进行旋转后，可以使用它来进行恢复
    old_transform = transform();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(move_one_step()));
    current_shape = RandomShape;
}


QRectF BoxGroup::boundingRect() const {
    qreal pen_width = 1;
    //2*2个小方块组成一个小方块组
    return QRectF(-40-pen_width/2, -40-pen_width/2, 80+pen_width, 80+pen_width);
}


void BoxGroup::keyPressEvent(QKeyEvent *event) {
    static qreal angle = 0;
    switch(event->key())
    {
        //向下键位坠落键
        case Qt::Key_Down:
            moveBy(0, 20);//moveBy是系统自带的函数，不需要我们自己去实现
            while(!isColliding()) {
                moveBy(0, 20);
            }
            moveBy(0, -20);//往回跳
            clear_box_group();//到达底部后就将当前方块组的4个item移除,不销毁方块组
            emit need_new_box();//发射信号，在MyView中接收
            break;
        case Qt::Key_Left:
            moveBy(-20, 0);
            if(isColliding()) {
                moveBy(20, 0);
            }
            break;
        case Qt::Key_Right:
            moveBy(20, 0);
            if(isColliding()) {
                moveBy(-20, 0);
            }
            break;
         //实现小方块组变形
         case Qt::Key_Space:
            setRotation(angle + 90.0);
            angle = angle + 90.0;
            if(isColliding()){
                //变形后碰撞了，就逆向变形回去
                setRotation(angle - 90.0);
                angle = angle - 90.0;
            }
            break;
    }
}


//检测是否有碰撞
bool BoxGroup::isColliding() {
    //返回子item列表
    QList<QGraphicsItem *> item_list = childItems();
    QGraphicsItem *item;
    foreach(item, item_list) {
        //collidingItems返回与当前item碰撞的子item列表
        if(item->collidingItems().count()>1)
            //代表至少有一个item发生了碰撞
            return true;
    }
    return false;
}


//将方块组从视图中移除掉，如果有需要(即参数为true的情况下)则销毁掉
//其本质是将所有的小方块从方块组中移除掉，达到从视图中将方块组移除的目的
void BoxGroup:: clear_box_group(bool destroy_box) {
    QList<QGraphicsItem *> item_list = childItems();
    QGraphicsItem *item;
    foreach(item, item_list) {
        //将item从方块组中移除掉
        removeFromGroup(item);
        if(destroy_box) {
            OneBox *box = (OneBox*)item;
            //当控制返回到事件循环时，该目标被删除，即销毁
            box->deleteLater();
        }
    }
}


//创建俄罗斯方块组，根据形状参数选择方块组的颜色和形状
void BoxGroup::create_box(const QPointF &point, BoxShape shape) {

    static const QColor color_table[7] = {
        QColor(200, 0, 0, 100), QColor(255, 200, 0, 100), QColor(0, 0, 200, 100),
        QColor(0, 200, 0, 100), QColor(0, 200, 255, 100), QColor(200, 0, 255, 100),
        QColor(150, 100, 100, 100)
    };
    //Box_Shape是枚举型，其实也是整型，因为它相当于整型的宏定义
    int shape_id = shape;
    if(shape == RandomShape) {
        //随机取一个颜色
        shape_id = qrand() % 7;
    }
    //根据id选颜色
    QColor color = color_table[shape_id];
    QList<OneBox *> list;
    //恢复方块组前的变换矩阵
    setTransform(old_transform);
    for(int i = 0; i < 4; ++i) { //4个小方块组成一个方块组
        OneBox *temp  = new OneBox(color);
        list << temp;//将小方块加入list列表
        addToGroup(temp);
    }
    switch(shape_id) {
        case IShape:
            current_shape = IShape;//横着的一杆
            list.at(0)->setPos(-30, -10);
            list.at(1)->setPos(-10, -10);
            list.at(2)->setPos(10, -10);
            list.at(3)->setPos(30, -10);
            break;
        case JShape:
            current_shape = JShape;//J型
            list.at(0)->setPos(10, -10);
            list.at(1)->setPos(10, 10);
            list.at(2)->setPos(10, 30);
            list.at(3)->setPos(-10, 30);
            break;
        case LShape:
            current_shape = LShape;//L型的方块组
            list.at(0)->setPos(-10, -10);
            list.at(1)->setPos(-10, 10);
            list.at(2)->setPos(-10, 30);
            list.at(3)->setPos(10, 30);
            break;
        case OShape://田字型
            current_shape = OShape;
            list.at(0)->setPos(-10, -10);
            list.at(1)->setPos(10, -10);
            list.at(2)->setPos(-10, 10);
            list.at(3)->setPos(10, 10);
            break;
        case SShape://S型
            current_shape = SShape;
            list.at(0)->setPos(10, -10);
            list.at(1)->setPos(30, -10);
            list.at(2)->setPos(-10, 10);
            list.at(3)->setPos(10, 10);
            break;
        case TShape: //土子型
            current_shape = TShape;
            list.at(0)->setPos(-10, -10);
            list.at(1)->setPos(10, -10);
            list.at(2)->setPos(30, -10);
            list.at(3)->setPos(10, 10);
            break;
        case ZShape://Z字型
            current_shape = ZShape;
            list.at(0)->setPos(-10, -10);
            list.at(1)->setPos(10, -10);
            list.at(2)->setPos(10, 10);
            list.at(3)->setPos(30, 10);
            break;
        default:
            break;
    }
    setPos(point);//将准备好的俄罗斯方块放入指定的位置，然后进行碰撞检测
    if(isColliding()) {
        //如果俄罗斯方块一出现后就发生了碰撞，因为它是从中间出来的，所以一开始不可能是与左右两边发生碰撞，
        //只能是与下面碰撞，因此如果发生了碰撞，说明游戏已经结束，就可以发送游戏结束信号了，且定时器停止。
        stop_timer();
        emit game_finished();
    }
}


//这个是系统里的函数，本程序中是在主函数中启动的
//其实是该子类中的timeEvent()函数调用的
void BoxGroup::startTimer(int interval) {
    //启动定时器并且设置定时器间隔，然后在BoxGroup()的构造函数中设置了该定时器的信号与槽函数
    timer->start(interval);
}


//每当定时器到时间了，小方块组就向下移一步
void BoxGroup::move_one_step() {
    //该函数是父类的函数，这里指向下移动一个单位，因为向下为正坐标
    moveBy(0, 20);
    //发生碰撞的情况下
    if(isColliding()) {
        moveBy(0, -20);
        //将方块组移除视图
        clear_box_group();
        //发生信号通知程序需要新的方块组出现
        emit need_new_box();
    }
}


//定时器停止
void BoxGroup::stop_timer() {
    timer->stop();
}


