#ifndef USERMNG_H
#define USERMNG_H

#include <QWidget>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <dbconnection.h>
#include <dbentity.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMapIterator>
#include <QVariant>
#include <QFile>
#include <QTreeWidgetItem>
#include <QHash>
#include <QMenu>
#include <QAction>

#define ROLE_TYPE Qt::UserRole+0
#define ROLE_USER Qt::UserRole+1
#define ROLE_HOST Qt::UserRole+2
#define ROLE_DB   Qt::UserRole+3
#define ROLE_RIGS Qt::UserRole+4
#define ROLE_PATH Qt::UserRole+5

#define ACT_ADD_USER    120
#define ACT_ADD_HOST    121
#define ACT_ADD_DB      122
#define ACT_ADD_TABLE   123
#define ACT_ADD_COLUMN  124

#define ACT_DEL_USER    130
#define ACT_DEL_HOST    131
#define ACT_DEL_DB      132
#define ACT_DEL_TABLE   133
#define ACT_DEL_COLUMN  134

#define ACT_SET_PASWD   140
#define ACT_SET_GRANTS  141
#define ACT_CLONE_HOST  142
#define ACT_RENAME_USER 143

#define ACT_COLLALL     144
#define ACT_EXPDALL     145
#define ACT_USERMNG_RELOAD      146

namespace Ui {
class UserMng;
}

class UserMng : public QWidget
{
    Q_OBJECT
    bool SetGrantsInProgress;
    DBConnectionPtr Con;
    DbEntityPtr Ent;
    QJsonObject UsersJson;
    QString CurrentPath;
    QString CurrentType;
    QString CurrentUser;
    QString CurrentHost;
    QString CurrentDb;
    QJsonObject CurrentRigs;
    //QString CurrentTable;
    //QString CurrentColumn;
    QStringList ExpandedPaths;
    QString SelectedPath;
    QString ParentPath;
    QHash<QString,QString> GrantObjMap;
    QHash<QString,QString> ObjGrantMap;
    void showUsers();
    void readUsersFromDb();    
    void showDbTreeContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);
    QStringList userHosts(QString user);
    QString userPassword(QString user, QString host);
    bool removeAllDbGrants(QString cUser, QString cHost);
    bool applyDbGrants(QString cUser, QString cHost, QString cDb, QJsonObject grants);
    void renameExpanded(QString search, QString newText);
public:
    static QString path(QString type, QString user="", QString host="", QString db="");
    static QString pathType(QString path);
    static QString user(QString path);
    static QString host(QString path);
    static QString db(QString path);
    explicit UserMng(DBConnectionPtr con, DbEntityPtr ent, QWidget *parent = 0);
    ~UserMng();
private:
    Ui::UserMng *ui;
    void fillMaps();
private slots:
    void itemActivatedSlot(QTreeWidgetItem*,int);
    void customContextMenuRequestedSlot(const QPoint& pos);
    void itemExpandedSlot(QTreeWidgetItem *);
    void itemColapsedSlot(QTreeWidgetItem *);
    void reloadSlot();
    void grantClickedSlot(bool);
    void doAction(bool);
    void applyGrants();
};

#endif // USERMNG_H
