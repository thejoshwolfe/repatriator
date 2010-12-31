#ifndef SETTINGS_H
#define SETTINGS_H

#include "ConnectionSettings.h"

#include <QList>

class QSettings;

class Settings
{
public:
    static QList<ConnectionSettings *> connections;

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
