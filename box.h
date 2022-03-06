#include <QGraphicsObject>
#include <QGraphicsItemGroup>

class OneBox : public QGraphicsObject
{
public:
    OneBox(const QColor &color = Qt::red);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

private:
    QColor brushColor;

};

class BoxGroup : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    //8种俄罗斯方框形状
    enum BoxShape {IShape, JShape, LShape, OShape, SShape, TShape, ZShape, RandomShape};
    BoxGroup();
    QRectF boundingRect() const;
    void clear_box_group(bool destroy_box = false);
    void create_box(const QPointF &point, BoxShape shape = RandomShape);
    bool isColliding();
    //获得当前俄罗斯方块的形状
    BoxShape getCurrentShape() {return current_shape;}


protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void need_new_box();
    void game_finished();

public slots:
    void move_one_step();
    void startTimer(int interval);
    void stop_timer();

private:
    BoxShape current_shape;
    QTransform old_transform;
    QTimer *timer;
};
