#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet("QDialog { background: cyan }");

    MainWindow w;
#ifdef __arm__
    w.showFullScreen();
#endif
    w.show();

    return a.exec();
}
