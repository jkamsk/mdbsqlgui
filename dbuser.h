#ifndef DBUSER_H
#define DBUSER_H

#include <QString>
#include <QHash>
#include <boost/shared_ptr.hpp>

class DbUser;
class DbUserHost;
typedef boost::shared_ptr<DbUser> DbUserPtr;
typedef boost::shared_ptr<DbUserHost> DbUserHostPtr;

enum DbGlobalPrivilege
{
    dbpGlobCreateUser
};

class DbUserHost
{
private:
    DbUserHost(QString name, QString host);
public:
    static DbUserHostPtr New(QString name, QString host);
};

class DbUser
{
private:
    QList<DbUserHostPtr> Hosts;
    DbUser(QString name);
public:
    static DbUserPtr New(QString name);
};

#endif // DBUSER_H
