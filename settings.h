#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>

class Settings
{    
    QString Config;
    static Settings * Self;
    explicit Settings();
public:    
    static QJsonObject Connections;
    static QJsonObject TableTemplates;
    static QJsonObject Options;
    static QJsonObject Queries;
    static QJsonObject Engines;
    static QJsonObject ColumnTypes;
    static void init();
    static void read();
    static void write();
    static void sync();
};

#endif // SETTINGS_H
