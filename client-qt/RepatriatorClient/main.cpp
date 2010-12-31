#include <QtGui/QApplication>
#include "ConnectionWindow.h"
#include "Settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Settings::load();
    ConnectionWindow::instance()->show();
    return a.exec();
}
