#include <QtGui/QApplication>
#include "ConnectionWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConnectionWindow::instance()->show();
    return a.exec();
}
