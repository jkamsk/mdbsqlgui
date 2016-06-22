#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QString>
#include <QList>
#include <QSettings>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <boost/shared_ptr.hpp>
#include <QJsonObject>
#include <config.h>

class DBConnection;
typedef boost::shared_ptr<DBConnection> DBConnectionPtr;
typedef QList<DBConnectionPtr> DBConnectionPtrList;
typedef boost::shared_ptr<DBConnectionPtrList> DBConnectionPtrListPtr;
class DBConnection
{

    bool Connected;
    DBConnection(QString name);
    QSqlError LastError;
public:
    QString Name;
    QString Driver;
    QString Host;
    int Port;
    QString User;
    QString Password;
    QJsonObject Defaults;
    QString name();

    static DBConnectionPtr New(QString name);
    static DBConnectionPtrListPtr NewList();
    static DBConnectionPtrListPtr readConnections();
    static void saveConnections(DBConnectionPtrListPtr);

    bool test();
    bool login();
    void logout();
    bool isConnected();
    QSqlError lastError();

    bool selectDatabase(QString name);
    bool tables(QStringList & list);
    bool databases(QStringList & list);
    bool collations(QStringList & list);
    bool runSql(QString command, QSqlQuery * qry=0L, QSqlError * err=0L);

    bool createDatabase(QString name, QString collate);
    bool dropDatabase(QString name);

    virtual ~DBConnection();

};

#endif // DBCONNECTION_H
