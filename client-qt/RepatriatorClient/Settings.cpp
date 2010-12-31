#include "Settings.h"
#include <QSettings>

QList<ConnectionSettings *> Settings::connections;
const char * Settings::settings_file = "repatriator.ini";
QSettings * Settings::settings = NULL;

void Settings::load()
{
    initialize();

    // trash the current data
    foreach (ConnectionSettings * cs, connections) {
        delete cs;
    }
    connections.clear();

    // read an entirely new list from settings
    int connection_count = settings->value("connections/count", 0).toInt();
    for (int i = 0; i < connection_count; i++) {
        QString prefix = QString("connections/") + QString::number(i) + QString("/");
        connections.append(ConnectionSettings::loadSettings(settings, prefix));
    }
    settings->sync();
}

void Settings::save()
{
    initialize();

    settings->setValue("connections/count", connections.count());
    for (int i = 0; i < connections.count(); i++) {
        QString prefix = QString("connections/") + QString::number(i) + QString("/");
        connections.at(i)->saveSettings(settings, prefix);
    }
    settings->sync();
}

void Settings::initialize()
{
    if (settings)
        return;
    settings = new QSettings(settings_file, QSettings::IniFormat);
}
