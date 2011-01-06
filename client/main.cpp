#include <QtGui/QApplication>
#include "ConnectionWindow.h"
#include "Settings.h"
#include "MetaTypes.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MetaTypes::registerMetaTypes();
    Settings::load();
    ConnectionWindow::instance()->show();
    return a.exec();
}
