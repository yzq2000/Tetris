#include "myview.h"
#include "box.h"
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QFileInfo>
#include <QGraphicsView>

//游戏的初始化速度
static const qreal INITSSPEED = 500;

MyView::MyView(QWidget *parent) :
    QGraphicsView(parent)
{
    box_group = new BoxGroup;
    next_box_group = new BoxGroup;
    screne = new QGraphicsScene;
    game_score = new QGraphicsTextItem();
    game_level = new QGraphicsTextItem();
    mask = new QWidget;
    game_welcome_text = new QGraphicsTextItem();
    game_pause_text = new QGraphicsTextItem();
    game_over_text = new QGraphicsTextItem();
    // 游戏中使用的按钮
    button1 = new QPushButton(tr("开    始"));
    button4 = new QPushButton(tr("退    出"));
    button5 = new QPushButton(tr("重新开始"));
    button6 = new QPushButton(tr("暂    停"));
    button7 = new QPushButton(tr("主 菜 单"));
    button8 = new QPushButton(tr("返回游戏"));
    button9 = new QPushButton(tr("结束游戏"));

    start_button = new QGraphicsWidget;
    finish_button = new QGraphicsWidget;
    restart_button = new QGraphicsWidget;
    pause_button = new QGraphicsWidget;
    return_button = new QGraphicsWidget;
    exit_button = new QGraphicsWidget;
    show_menu_button = new QGraphicsWidget;

    mask_widget = new QGraphicsWidget;

    init_view();
}

void MyView :: init_view() {
    //使用抗锯齿的方式渲染
    setRenderHint(QPainter::Antialiasing);
    //设置缓存背景，这样可以加快渲染速度
    setCacheMode(CacheBackground);
    setWindowTitle(tr("Tetris"));
    //设置标题处的图标
    setWindowIcon(QIcon(":/images/icon.png"));
    //视图尺寸810*510不可更改
    setMinimumSize(810, 510);
    setMaximumSize(810, 510);

    screne->setSceneRect(5, 5, 800, 500);
    screne->setBackgroundBrush(QPixmap(":/images/background.png"));
    //设置场景
    setScene(screne);
    screne->addItem(game_score);
    screne->addItem(game_level);
    screne->addItem(game_welcome_text);
    screne->addItem(game_pause_text);
    screne->addItem(game_over_text);

    //俄罗斯方块可移动区域外界的4条线,与外界预留3个像素方便进行碰撞检测
    top_line = screne->addLine(197, 47, 403, 47);
    bottom_line = screne->addLine(197, 453, 403, 453);
    left_line = screne->addLine(197, 47, 197, 453);
    right_line = screne->addLine(403, 47, 403, 453);

    //添加当前方块组
    //调用box的2个类
    connect(box_group, SIGNAL(need_new_box()), this, SLOT(clear_full_rows()));
    connect(box_group, SIGNAL(game_finished()), this, SLOT(game_over()));
    screne->addItem(box_group);

    //添加提示方块组
    screne->addItem(next_box_group);

    //??upd
    game_score->setFont(QFont("Times", 28, QFont::Bold));//为文本设置字体
    game_score->setPos(450, 350);//分数在场景中出现的位置

    game_level->setFont(QFont("Times", 32, QFont::Bold));
    game_level->setPos(20, 150);

    //开始游戏
    //start_game();
    //初始状态时不显示游戏区域，不显示游戏分数和游戏等级
    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();
    game_score->hide();
    game_level->hide();

    mask->setAutoFillBackground(true);
    //(red, green, blue, alpha), alpha为不透明度
    mask->setPalette(QPalette(QColor(0, 0, 0, 50)));
    mask->resize(900, 600);
    mask->hide();
    //addWidget()函数的返回值是QGraphicsProxyWidget，如果不添加相应的头文件，则此处会报错
    mask_widget = screne->addWidget(mask);
    mask_widget->setPos(-50, -50);
    //该层薄纱放在原图的上面，这里有点类似于opengl中的3维绘图
    mask_widget->setZValue(1);

    //游戏欢迎文本
    game_welcome_text->setHtml(tr("<font color=green>Lucky next time!</font>"));
    game_welcome_text->setFont(QFont("Times", 32, QFont::Bold));
    game_welcome_text->setPos(200, 100);
    game_welcome_text->setZValue(2);
    game_welcome_text->hide();

    //游戏暂停文本
    game_pause_text->setHtml(tr("<font color=green>Game suspended.</font>"));
    game_pause_text->setFont(QFont("Times", 32, QFont::Bold));
    game_pause_text->setPos(200, 100);
    game_pause_text->setZValue(2);
    game_pause_text->hide();

    //游戏结束文本
    game_over_text->setHtml(tr("<font color=green>GAME　OVER!</font>"));
    game_over_text->setFont(QFont("Times", 32, QFont::Bold));
    game_over_text->setPos(200, 100);
    game_over_text->setZValue(2);
    game_over_text->hide();

    connect(button1, SIGNAL(clicked()), this, SLOT(start_game()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));//此处槽函数的接收对象为应用程序本身
    connect(button5, SIGNAL(clicked()), this, SLOT(restart_game()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pause_game()));
    connect(button7, SIGNAL(clicked()), this, SLOT(finish_game()));
    connect(button8, SIGNAL(clicked()), this, SLOT(return_game()));//返回主菜单
    connect(button9, SIGNAL(clicked()), this, SLOT(finish_game()));

    start_button = screne->addWidget(button1);//restart_button并不是QPushbutton类型，而是QGraphicsItem类型,后面的类似
    exit_button = screne->addWidget(button4);
    restart_button = screne->addWidget(button5);
    pause_button = screne->addWidget(button6);
    show_menu_button = screne->addWidget(button7);
    return_button = screne->addWidget(button8);
    finish_button = screne->addWidget(button9);

    //设置位置
    start_button->setPos(370, 250);
    exit_button->setPos(370, 300);
    restart_button->setPos(600, 150);
    pause_button->setPos(600, 200);
    show_menu_button->setPos(600, 250);
    return_button->setPos(370, 200);
    finish_button->setPos(370, 250);

    //将这些按钮都放在z方向的第二层
    start_button->setZValue(2);
    exit_button->setZValue(2);
    restart_button->setZValue(2);
    return_button->setZValue(2);
    finish_button->setZValue(2);

    //一部分按钮隐藏起来
    restart_button->hide();
    finish_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    return_button->hide();
}

//开始游戏
void MyView::start_game() {
    game_welcome_text->hide();
    start_button->hide();
    exit_button->hide();
    mask_widget->hide();
    init_game();
}


//初始化游戏
void MyView::init_game() {
    //创建方块组,在中间位置处出现
    box_group->create_box(QPointF(300, 90));
    //设置人机交互焦点，这样就可以使用键盘来控制它
    box_group->setFocus();
    //启动定时器
    box_group->startTimer(INITSSPEED);
    //游戏速度，暂停时需要用到
    game_speed = INITSSPEED;
    //创建提示方块组
    next_box_group->create_box(QPoint(500, 90));

    //??
    screne->setBackgroundBrush(QPixmap(":/images/background01.png"));
    game_score->setHtml(tr("<font color = blue >0</font>"));
    game_level->setHtml(tr("<font color = blue>level 1</font>"));//br为换行

    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    game_score->show();
    game_level->show();
    top_line->show();
    bottom_line->show();
    left_line->show();
    right_line->show();
    box_group->show();
}


void MyView::clear_full_rows() {
    //每隔20行取一个item出来，括号里面的参数不能弄错，否则没有方块消失的效果
    for(int y = 429; y > 50; y -= 20) {
        // upd??
        //返回指定区域内所有可见的item
        QList<QGraphicsItem *> list = screne->items(199, y, 202, 22, Qt::ContainsItemShape, Qt::AscendingOrder);
        //如果一行已满，则销毁该行的所有小方块
        if(list.count() == 10) {
            foreach(QGraphicsItem *item, list) {
                OneBox *box = (OneBox *) item;
                //采用动态效果消失方块行
                //创建模糊效果对象
                blur_effect = new QGraphicsBlurEffect;
                //给每个小方块添加模糊设置
                box->setGraphicsEffect(blur_effect);
                //添加动态效果，尺寸变换效果
                animation = new QPropertyAnimation(box, "scale");
                //为动态效果设置缓冲取曲线，是用来插值的
                animation->setEasingCurve(QEasingCurve::OutBounce);
                //持续250毫秒
                animation->setDuration(250);
                //起始尺寸
                animation->setStartValue(4);
                //结束尺寸
                animation->setEndValue(0.25);
                //动画结束后删除该类
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                //动画结束完后才调用小方块销毁函数
                connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
            }
            rows << y;//将满行的行号保存到rows中
        }
    }

    //如果满行，则下移上面的方块
    if(rows.count()>0) {
        //只执行一次定时器，等动态效果完后再下移上面的方块
        QTimer::singleShot(400, this, SLOT(move_box()));
    }
    //没有满行，则新出现提示方块，且提示方块出更新新的提示方块
    else {
        box_group->create_box(QPointF(300, 90), next_box_group->getCurrentShape());
        next_box_group->clear_box_group(true);
        next_box_group->create_box(QPointF(500, 90));//
    }
}


void MyView::move_box() {
    //取出满行的行号,从最上面的位置开始
    for(int i = rows.count(); i > 0; --i) {
        int row = rows.at(i-1);
        //取出从区域上边界到当前满行之间所形成的矩形区域
        foreach(QGraphicsItem *item, screne->items(199, 49, 202, row-47, Qt::ContainsItemShape, Qt::AscendingOrder)) {
            item->moveBy(0, 20);
        }
    }
    //更新分数
    update_score(rows.count());
    //出现新的方块组
    rows.clear();
    box_group->create_box(QPointF(300, 90), next_box_group->getCurrentShape());
    next_box_group->clear_box_group(true);
    next_box_group->create_box(QPoint(500, 90));
}


void MyView::update_score(const int full_row_num) {
    //每消一行得100
    long score = full_row_num*100;
    int current_score = game_score->toPlainText().toInt() + score;
    game_score->setHtml(tr("<font color = blue>%1</font>").arg(current_score));
    // score >= 3000分，进入第五关
    if(current_score >= 3000) {
        game_level->setHtml(tr("<font color = blue>level 5</font>"));
        //速度变快
        game_speed = 100;
        //重新设置定时器参数
        box_group->stop_timer();
        box_group->startTimer(game_speed);
    }
    // 2000 <= score < 3000，进入第四关
    else if(current_score >= 2000) {
        game_level->setHtml(tr("<font color = blue>level 4</font>"));
        //速度变快
        game_speed = 200;
        //重新设置定时器参数
        box_group->stop_timer();
        box_group->startTimer(game_speed);
    }
    // 1000 <= score < 2000，进入第三关
    else if(current_score >= 1000) {
        game_level->setHtml(tr("<font color = blue>level 3</font>"));
        //速度变快
        game_speed = 300;
        //重新设置定时器参数
        box_group->stop_timer();
        box_group->startTimer(game_speed);
    }
    // 500 <= score < 1000，进入第二关
    else if(current_score >= 500) {
        game_level->setHtml(tr("<font color = blue>level 2</font>"));
        //速度变快
        game_speed = 400;
        //重新设置定时器参数
        box_group->stop_timer();
        box_group->startTimer(game_speed);
    }
    // score < 500，在第一关
}


void MyView::game_over() {
    pause_button->hide();
    show_menu_button->hide();
    //mask_widget->show();
    game_over_text->show();
    restart_button->setPos(370, 200);
    finish_button->show();
}

void MyView::restart_game() {
    mask_widget->hide();
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);
    //销毁当前方块组和当前方块中的所有小方块
    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide();
    foreach(QGraphicsItem *item, screne->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect, Qt::AscendingOrder)) {
        screne->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }
    init_game();
}


void MyView::finish_game()
{
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    game_score->hide();
    game_level->hide();
    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();

    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide(); //199 49 202 402
    foreach(QGraphicsItem *item, screne->items(199, 49, 202, 402, Qt::ContainsItemBoundingRect, Qt::AscendingOrder)) {
        screne->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }

    //mask_widget->show();
    game_welcome_text->show();
    start_button->show();
    exit_button->show();
    screne->setBackgroundBrush(QPixmap(":/images/background.png"));
}

void MyView::pause_game()
{
    //中断游戏最主要的是停止方块下移的定时器工作
    box_group->stop_timer();
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_pause_text->show();
    return_button->show();
}

void MyView::return_game()
{
    return_button->hide();
    game_pause_text->hide();
    mask_widget->hide();
    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    box_group->startTimer(game_speed);
}

void MyView::keyPressEvent(QKeyEvent *event) {
    //当屏幕中可以看到暂停按钮时，代表游戏还在运行，这时候的焦点给下降中的方块组
    if(pause_button->isVisible())
        box_group->setFocus();
    //如果游戏暂停，则方块组不能获得焦点
    else
        box_group->clearFocus();
    QGraphicsView::keyPressEvent(event);
}






