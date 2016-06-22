#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <dbconnection.h>

class Settings
{    
    QString Config;
    static Settings * Self;
    explicit Settings();
public:    
    static DBConnectionPtrListPtr Connections;
    static QJsonObject ConnectionsJS;
    static QJsonObject TableTemplates;
    static QJsonObject Options;
    static QJsonObject Queries;
    static QJsonArray Engines;
    static QJsonArray ColumnTypes;
    static void init();
    static void read();
    static void write();
    static void sync();
};

#endif // SETTINGS_H
