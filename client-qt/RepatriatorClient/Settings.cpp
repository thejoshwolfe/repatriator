#include "Settings.h"
#include <QSettings>

const char * Settings::settings_file = "repatriator.ini";
QSettings * Settings::settings = NULL;

QList<ConnectionSettings *> Settings::connections;
int Settings::last_connection_index = 0;
QVector<int> Settings::connection_column_width;
QByteArray Settings::main_window_geometry;
QByteArray Settings::main_window_state;
QByteArray Settings::connection_window_geometry;

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
    for (int i = 0; i < connection_column_width.count(); i++) {
        QString setting_name = QString("windows/connection_column_width/") + QString::number(i);
        connection_column_width.replace(i, settings->value(setting_name, 0).toInt());
    }
    connection_window_geometry = settings->value("windows/geometry/connection_window").toByteArray();
    main_window_geometry = settings->value("windows/geometry/main_window").toByteArray();
    main_window_state = settings->value("windows/state/main_window").toByteArray();
}

void Settings::save()
{
    initialize();

    settings->setValue("windows/geometry/connection_window", connection_window_geometry);
    settings->setValue("windows/geometry/main_window", main_window_geometry);
    settings->setValue("windows/state/main_window", main_window_state);
    settings->setValue("connections/most_recent", last_connection_index);
    settings->setValue("connections/count", connections.count());
    for (int i = 0; i < connections.count(); i++) {
        QString prefix = QString("connections/") + QString::number(i) + QString("/");
        connections.at(i)->saveSettings(settings, prefix);
    }
    for (int i = 0; i < connection_column_width.count(); i++) {
        QString setting_name = QString("windows/connection_column_width/") + QString::number(i);
        settings->setValue(setting_name, connection_column_width.at(i));
    }
    settings->sync();
}

void Settings::initialize()
{
    if (settings)
        return;
    settings = new QSettings(settings_file, QSettings::IniFormat);

    connection_column_width.resize(3);
}
