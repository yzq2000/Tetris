#include <QGraphicsView>
#include <QPushButton>
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QFileInfo>

//??
class BoxGroup;

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    //关键字explicit是为了防止隐式类型转换
    explicit MyView(QWidget *parent = 0);

public slots:
    void start_game();
    void clear_full_rows();
    void move_box();
    void game_over();
    void restart_game();
    void finish_game();
    void pause_game();
    void return_game();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    //场景指针
    QGraphicsScene *screne;
    //黑色遮罩
    QWidget *mask;

    QPushButton *button1;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    //模糊效果对象
    QGraphicsBlurEffect *blur_effect;
    //动态效果，尺寸变换效果
    QPropertyAnimation *animation;
    //四条边界
    QGraphicsLineItem *top_line;//
    QGraphicsLineItem *bottom_line;//
    QGraphicsLineItem *left_line;//
    QGraphicsLineItem *right_line;//
    BoxGroup *box_group;
    BoxGroup *next_box_group;
    qreal game_speed;
    QList<int> rows;
    //分数文本图形对象
    QGraphicsTextItem *game_score;
    //游戏等级文本图形对象
    QGraphicsTextItem *game_level;
    QGraphicsWidget *mask_widget;

    //首页和游戏中需要用到的各种按钮
    QGraphicsWidget *start_button;
    QGraphicsWidget *finish_button;
    QGraphicsWidget *restart_button;
    QGraphicsWidget *pause_button;
    QGraphicsWidget *return_button;
    QGraphicsWidget *exit_button;
    QGraphicsWidget *show_menu_button;

    //显示人机交互的文本信息
    QGraphicsTextItem *game_welcome_text;
    QGraphicsTextItem *game_pause_text;
    QGraphicsTextItem *game_over_text;

    void init_view();
    void init_game();
    void update_score(const int full_row_num = 0);

signals:

};
