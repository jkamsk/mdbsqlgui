#ifndef DBENTITY_H
#define DBENTITY_H

#include <QString>
#include <QStringList>
#include <boost/shared_ptr.hpp>

/*
 *  Entity string
 *  CONNECTION/DATABASE/tables/TABLE/COLUMN
 *  CONNECTION/DATABASE/procedures/PROCEDURE
 *  CONNECTION/DATABASE/functions/FUNCTION
 */

enum DbEntityType
{
    dbeUnknown = 0,
    dbeConnection = 1,
    dbeDatabase = 2,
    dbeTable = 3,    
    dbeProcedure = 4,
    dbeFunction = 5,
    dbeColumn = 6
};

class DbEntity;
typedef boost::shared_ptr<DbEntity> DbEntityPtr;
class DbEntity
{
    DbEntity(QString path);
    DbEntityType Type;
    QString Connection;
    QString Database;
    QString Table;
    QString Column;
    QString Procedure;
    QString Function;
    void decode(QString);
public:
    DbEntityType type();
    QString connection();
    QString database();
    QString table();
    QString column();
    QString procedure();
    QString function();
    static DbEntityPtr Make(QString path);
    virtual ~DbEntity();
};

#endif // DBENTITY_H
