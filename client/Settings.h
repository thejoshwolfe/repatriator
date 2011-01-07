#ifndef SETTINGS_H
#define SETTINGS_H

#include "ConnectionSettings.h"

#include <QList>
#include <QVector>
#include <QByteArray>

class QSettings;

class Settings
{
public:
    static QList<ConnectionSettings *> connections;
    static int last_connection_index;
    static QVector<int> connection_column_width;
    static QByteArray connection_window_geometry;
    static QByteArray main_window_geometry;
    static QByteArray main_window_state;

    static QByteArray dock_controls_geometry;
    static QByteArray dock_files_geometry;
    static QByteArray dock_bookmarks_geometry;
    static QByteArray dock_locations_geometry;

    // read the settings on disk into this class
    static void load();

    // save the settings in this class to disk
    static void save();

private:
    static const char * settings_file;

    static QSettings * settings; // NULL if class is not initialized

private:
    static void initialize();
};

#endif // SETTINGS_H
