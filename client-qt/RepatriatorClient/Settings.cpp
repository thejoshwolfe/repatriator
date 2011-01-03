#include "Settings.h"
#include <QSettings>

const char * Settings::settings_file = "repatriator.ini";
QSettings * Settings::settings = NULL;

QList<ConnectionSettings *> Settings::connections;
int Settings::last_connection_index = 0;

void Settings::load()
{
    initialize();

    // trash the current data
    foreach (ConnectionSettings * cs, connections) {
        delete cs;
    }
    connections.clear();

    // read an entirely new list from settings
    settings->sync();
    last_connection_index = settings->value("connections/most_recent", 0).toInt();
    int connection_count = settings->value("connections/count", 0).toInt();
    for (int i = 0; i < connection_count; i++) {
        QString prefix = QString("connections/") + QString::number(i) + QString("/");
        connections.append(ConnectionSettings::loadSettings(settings, prefix));
    }
}

void Settings::save()
{
    initialize();

    settings->setValue("connections/most_recent", last_connection_index);
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
