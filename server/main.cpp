#include <QApplication>
#include <QTranslator>
#include <QDebug>

#include "watchdog.h"
#include "keypresseater.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *translator = new QTranslator(&a);
    translator->load(":/skynet_zh_CN");
    //translator->load("qt_zh_CN");
    a.installTranslator(translator);

    KeyPressEater *keyPressEater = new KeyPressEater();
    a.installEventFilter(keyPressEater);

    a.setStyleSheet("QDialog { background: cyan }");

    Watchdog *watchdog= Watchdog::instance();
    watchdog->keepAlive();

    MainWindow w;
#ifdef __arm__
    w.showFullScreen();
#endif
    w.show();

    return a.exec();
}
