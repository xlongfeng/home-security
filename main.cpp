#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("skynet_zh_CN");
    a.installTranslator(&translator);

    a.setStyleSheet("QDialog { background: cyan }");

    MainWindow w;
#ifdef __arm__
    w.showFullScreen();
#endif
    w.show();

    return a.exec();
}
