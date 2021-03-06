#include <QApplication>
#include <QImageReader>
#include <QMessageBox>
#include <QDebug>

#include "ConnectionWindow.h"
#include "Settings.h"
#include "MetaTypes.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("RepatriatorClient");
    a.setApplicationVersion("1.0.0");

    // warn if can't find jpeg plugin
    bool support_jpeg = QImageReader::supportedImageFormats().contains("jpg") ||
                        QImageReader::supportedImageFormats().contains("jpeg");
    Q_ASSERT(support_jpeg);
    if (! support_jpeg) {
        QMessageBox::warning(NULL, QObject::tr("imageformats/qjpeg4.dll not found"),
            QObject::tr("Missing imageformats/qjpeg4.dll. You will not be able to see live view. Please reinstall this program."),
           QMessageBox::Ok);
    }

    MetaTypes::registerMetaTypes();
    Settings::load();
    ConnectionWindow::instance()->show();
    QStringList args = a.arguments();
    if (args.count() == 2)
        ConnectionWindow::instance()->handleUrl(args.at(1));
    return a.exec();
}
