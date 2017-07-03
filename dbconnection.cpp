#include "dbconnection.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <settings.h>

bool DBConnection::dropDatabase(QString name)
{
    QString cmd = QString("DROP DATABASE %1;").arg(name);
    QSqlDatabase db = QSqlDatabase::database(Name);
    QSqlQuery qry(db);
    QSqlError err;
    bool ok = runSql(cmd, &qry, &err);
    return ok;
}
bool DBConnection::createDatabase(QString name, QString collate)
{
    QString cmd = QString("CREATE DATABASE %1 COLLATE = '%2';").arg(name).arg(collate);
    QSqlDatabase db = QSqlDatabase::database(Name);
    QSqlQuery qry(db);
    QSqlError err;
    bool ok = runSql(cmd, &qry, &err);
    return ok;
}
bool DBConnection::selectDatabase(QString name)
{
    return runSql(QString("USE %1;").arg(name));
}
bool DBConnection::tables(QStringList & list)
{
    QSqlDatabase db = QSqlDatabase::database(Name);
    QSqlQuery qry(db);
    QSqlError err;
    bool ok = runSql("SHOW TABLES;", &qry, &err);
    if (ok)
    {
        //QSqlRecord rec = qry.record();
        list.clear();
        while(qry.next())
            list.append(qry.value(0).toString().trimmed());
    }
    return ok;
}
bool DBConnection::databases(QStringList & list)
{
    QSqlDatabase db = QSqlDatabase::database(Name);
    QSqlQuery qry(db);
    QSqlError err;
    bool ok = runSql("SHOW DATABASES;", &qry, &err);
    if (ok)
    {
        //rec = qry.record();
        list.clear();
        while(qry.next())
            list.append(qry.value(0).toString().trimmed());
    }
    return ok;
}
bool DBConnection::collations(QStringList & list)
{
    QSqlDatabase db = QSqlDatabase::database(Name);
    QSqlQuery qry(db);
    QSqlError err;
    bool ok = runSql("SHOW COLLATION;", &qry, &err);
    if (ok)
    {
        //rec = qry.record();
        list.clear();
        while(qry.next())
            list.append(qry.value(0).toString().trimmed());
    }
    return ok;
}
void DBConnection::saveConnections(DBConnectionPtrListPtr cons)
{
    if (!cons.get())
        return;
    DBConnectionPtr dbc;
    QJsonObject con;
    Settings::ConnectionsJS = QJsonObject();
    for (int i=0;i<cons->count();i++)
    {
        dbc = cons->at(i);
        con = QJsonObject();
        con["driver"] = dbc->Driver;
        con["host"] = dbc->Host;
        con["port"] = dbc->Port;
        con["user"] = dbc->User;
        con["pwd"] = QString(dbc->Password.toLatin1().toHex().toBase64());
        con["Defaults"] = dbc->Defaults;
        Settings::ConnectionsJS[dbc->name()] = con;
    }    
    Settings::sync();
}
DBConnectionPtrListPtr DBConnection::readConnections()
{
    QSqlDatabase::drivers();
    DBConnectionPtrListPtr ret = DBConnection::NewList();

    Settings::read();

    QJsonObject conns = Settings::ConnectionsJS;
    QJsonObject con;
    QStringList keys = conns.keys();
    QString cname;
    for (int i=0;i<keys.count();i++)
    {
        cname = keys.at(i);
        con = conns[cname].toObject();
        DBConnectionPtr dbc = DBConnection::New(cname);
        dbc->Host = con["host"].toString();
        dbc->Port = con["port"].toInt();
        dbc->User = con["user"].toString();
        dbc->Password = QString(QByteArray::fromHex(QByteArray::fromBase64(con["pwd"].toString().toLatin1())));
        dbc->Driver = con["driver"].toString();
        dbc->Defaults = con["Defaults"].toObject();
        ret->append(dbc);
    }
    return ret;
}
bool DBConnection::isConnected()
{
    return Connected;
}
QSqlError DBConnection::lastError()
{
    return LastError;
}
bool DBConnection::test()
{
    bool ok = false;
    ok = login();
    if (ok)
        logout();
    return ok;
}
bool DBConnection::login()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(Driver, QLatin1String(Name.toLatin1()));
    db.setHostName(Host);
    db.setUserName(User);
    db.setPassword(Password);    
    db.setPort(Port);
    if (Driver == "QMYSQL")
        db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    if (db.open())
    {
        bool ok = runSql("SELECT 'OK' as 'Test';");
        if (ok)
        {
            LastError = db.lastError();
            Connected = true;
            return true;

        }
    }
    LastError = db.lastError();
    return false;
}
QString DBConnection::lastErrorText()
{
   QString ret;
   ret += QString("Driver: %1, %2\n\n").arg(LastError.number()).arg(LastError.driverText());
   ret += QString("DB: %1, %2").arg(LastError.nativeErrorCode()).arg(LastError.databaseText());
   return ret;
}
void DBConnection::logout()
{
    {
        QSqlDatabase db = QSqlDatabase::database(Name);
        db.close();
    }
    QSqlDatabase::removeDatabase(Name);
    Connected = false;
}
bool DBConnection::runSql(QString command, QSqlQuery * qry, QSqlError * err)
{
    bool ok = false;
    if (qry != 0L)
    {
        ok = qry->exec(command);
        if (!ok)
            LastError = qry->lastError();
        if (err != 0L)
        {
            *err = qry->lastError();
            if (err->type() != QSqlError::NoError)
                return false; // error occurred
        }
    }
    else
    {
        QSqlDatabase db = QSqlDatabase::database(Name);
        QSqlQuery q(db);
        ok = q.exec(command);
        if (!ok)
            LastError = q.lastError();
    }    
    return ok; // without error
}
DBConnectionPtr DBConnection::New(QString name)
{
    DBConnectionPtr ret(new DBConnection(name));
    return ret;
}
DBConnectionPtrListPtr DBConnection::NewList()
{
    DBConnectionPtrListPtr ret(new DBConnectionPtrList());
    return ret;
}
DBConnection::DBConnection(QString name)
{
    Name = name;
    Driver = DEF_DRIVER;
    Host = DEF_HOST;
    Port = DEF_PORT;
    User = DEF_USER;
    Password = DEF_PASSWORD;
    Connected = false;
}
QString DBConnection::name()
{
    return Name;
}
DBConnection::~DBConnection()
{
}


