#include <QTime>
#include <QSplashScreen>
#include <QApplication>

#include "myview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    //启动画面
    QPixmap pix(":/images/logo.png");
    QSplashScreen splash(pix);
    splash.resize(pix.size());
    splash.show();

    //类似于多线程，使程序在启动画面的同时仍然能够响应鼠标事件
    a.processEvents();

    //调用窗口
    MyView view;
    view.show();

    //窗口初始化完成后结束启动画面
    splash.finish(&view);

    return a.exec();
}
