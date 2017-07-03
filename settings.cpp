#include "settings.h"
#include <QFile>

Settings * Settings::Self = 0L;
QJsonObject Settings::ConnectionsJS = QJsonObject();
QJsonObject Settings::TableTemplates = QJsonObject();
QJsonObject Settings::Options = QJsonObject();
QJsonObject Settings::Queries = QJsonObject();
QJsonArray Settings::Engines = QJsonArray();
QJsonArray Settings::ColumnTypes = QJsonArray();
DBConnectionPtrListPtr Settings::Connections = DBConnection::NewList();
void Settings::init()
{
    if (Self == 0L)
        Self = new Settings();
    read();
    if (ColumnTypes.isEmpty())
    {
        ColumnTypes.append(QJsonValue("varchar(25)"));
        ColumnTypes.append(QJsonValue("varchar(100)"));
        ColumnTypes.append(QJsonValue("varchar(500)"));
        ColumnTypes.append(QJsonValue("int(11)"));
        ColumnTypes.append(QJsonValue("longtext"));
        ColumnTypes.append(QJsonValue("mediumtext"));
        ColumnTypes.append(QJsonValue("text"));
        ColumnTypes.append(QJsonValue("decimal(21,9)"));
        ColumnTypes.append(QJsonValue("decimal(19,8)"));
        ColumnTypes.append(QJsonValue("time"));
        ColumnTypes.append(QJsonValue("date"));
        ColumnTypes.append(QJsonValue("datetime"));
        ColumnTypes.append(QJsonValue("char(1)"));
        ColumnTypes.append(QJsonValue("timestamp"));
    }
    if (!Options.contains("default.column.type"))
        Options["default.column.type"] = "varchar(25)";
    if (!Options.contains("default.column.name"))
        Options["default.column.name"] = "Column%1";
    if (!Options.contains("default.column.collation"))
        Options["default.column.collation"] = "utf8_general_ci";
    sync();
}
void Settings::read()
{
    if (Self == 0L)
    {
        qDebug(QString("Failed read config json file, Yout need call Settings:init() function").toStdString().c_str());
        return;
    }

    QFile f(Self->Config);
    if (!f.open(QFile::ReadOnly))
    {
        qDebug(QString("Failed read config json file, %1").arg(Self->Config).toStdString().c_str());
        return;
    }

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &pe);
    if (pe.error != QJsonParseError::NoError)
    {
        qDebug(QString("Parsing JSON config failed, %1").arg(Self->Config).toStdString().c_str());
        return;
    }
    QJsonObject cf = doc.object();
    if (cf.contains("Connections"))
        ConnectionsJS = cf["Connections"].toObject();
    else
       qDebug(QString("Connections section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("TableTemplates"))
        TableTemplates = cf["TableTemplates"].toObject();
    else
       qDebug(QString("TableTemplates section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("Options"))
        Options = cf["Options"].toObject();
    else
       qDebug(QString("Options section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("Queries"))
        Queries = cf["Queries"].toObject();
    else
       qDebug(QString("Queries section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("Engines"))
        Engines = cf["Engines"].toArray();
    else
       qDebug(QString("Engines section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("ColumnTypes"))
        ColumnTypes = cf["ColumnTypes"].toArray();
    else
       qDebug(QString("ColumnTypes section not found, %1").arg(Self->Config).toStdString().c_str());
}
void Settings::write()
{
    if (Self == 0L)
    {
        qDebug(QString("Failed write to config json file, Yout need call Settings:init() function").toStdString().c_str());
        return;
    }
    QFile f(Self->Config);
    if (!f.open(QFile::WriteOnly))
    {
        qDebug(QString("Failed write to config json file, %1").arg(Self->Config).toStdString().c_str());
        return;
    }

    QJsonObject cf;
    cf["Connections"] = ConnectionsJS;
    cf["TableTemplates"] = TableTemplates;
    cf["Options"] = Options;
    cf["Queries"] = Queries;
    cf["Engines"] = Engines;
    cf["ColumnTypes"] = ColumnTypes;

    QJsonDocument doc;
    doc.setObject(cf);
    f.write(doc.toJson(QJsonDocument::Indented));
    f.waitForBytesWritten(60000);
    f.close();
}
void Settings::sync()
{
    write();
    read();
}
Settings::Settings()
{
    QSettings *cf = new QSettings("mdbsqlgui", "client");
    Config = cf->fileName();
    delete cf;
}
