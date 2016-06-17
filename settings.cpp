#include "settings.h"
#include <QFile>

Settings * Settings::Self = 0L;
QJsonObject Settings::Connections = QJsonObject();
QJsonObject Settings::TableTemplates = QJsonObject();
QJsonObject Settings::Options = QJsonObject();
QJsonObject Settings::Queries = QJsonObject();
QJsonObject Settings::Engines = QJsonObject();
QJsonObject Settings::ColumnTypes = QJsonObject();
void Settings::init()
{
    if (Self == 0L)
        Self = new Settings();
    read();
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
        Connections = cf["Connections"].toObject();
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
        Engines = cf["Engines"].toObject();
    else
       qDebug(QString("Engines section not found, %1").arg(Self->Config).toStdString().c_str());

    if (cf.contains("ColumnTypes"))
        ColumnTypes = cf["ColumnTypes"].toObject();
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
    cf["Connections"] = Connections;
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
