#include "dbentity.h"

void DbEntity::decode(QString path)
{
    QStringList entities = path.split("/");
    int ec = entities.count();
    switch(ec)
    {
    case 3:
    case 0:
        Type = dbeUnknown;
        break;
    case 1:
        Type = dbeConnection;
        break;
    case 2:
        Type = dbeDatabase;
        break;
    case 4:
        if (entities.at(2) == "tables")
            Type = dbeTable;
        else if (entities.at(2) == "procedures")
            Type = dbeProcedure;
        else if (entities.at(2) == "functions")
            Type = dbeFunction;
        else
            Type = dbeUnknown;
        break;
    }
    if (ec >= 1)
        Connection = entities.at(0);
    if (ec >= 2)
        Database = entities.at(1);
    if (ec >= 4)
    {
        if (Type == dbeTable)
            Table = entities.at(3);
        if (Type == dbeProcedure)
            Procedure = entities.at(3);
        if (Type == dbeFunction)
            Function = entities.at(3);
    }
    if (ec >= 5)
        Column = entities.at(4);
}
DbEntityType DbEntity::type()
{
    return Type;
}
QString DbEntity::connection()
{
    return Connection;
}
QString DbEntity::database()
{
    return Database;
}
QString DbEntity::table()
{
    return Table;
}
QString DbEntity::column()
{
    return Column;
}
QString DbEntity::procedure()
{
    return Procedure;
}
QString DbEntity::function()
{
    return Function;
}
DbEntityPtr DbEntity::Make(QString path)
{
    DbEntityPtr ret(new DbEntity(path));
    return ret;
}
DbEntity::DbEntity(QString path)
{
    Type = dbeUnknown;
    decode(path);
}
DbEntity::~DbEntity()
{

}

