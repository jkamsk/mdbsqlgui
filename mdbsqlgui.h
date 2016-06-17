#ifndef MDBSQLGUI_H
#define MDBSQLGUI_H

#include <QMainWindow>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QHash>
#include <QMenu>
#include <dbconnection.h>

#define ACT_NEWCONNECTION   100
#define ACT_EDITCONNECTION  101
#define ACT_CONNECT         102
#define ACT_DISCONNECT      103
#define ACT_CREATEDB        104
#define ACT_REMOVEDB        105
#define ACT_DROPCONN        106
#define ACT_RELOAD          107
#define ACT_SHOWSYSDB       108
#define ACT_SHOWDATA        109
#define ACT_EDITDATA        110
#define ACT_NEWQUERY        111
#define ACT_DROPTABLE       112
#define ACT_ADDTABLE        113
#define ACT_USERMNGMT       114
#define ACT_EDITTABLE       115
#define ACT_EXPANDALL       116
#define ACT_COLLAPSEALL     117

namespace Ui {
class MdbSqlGui;
}

class MdbSqlGui : public QMainWindow
{
    enum ItemState
    {
        isEspanded = 1,
        isCollapsed = 2
    };
    Q_OBJECT
    DBConnectionPtrListPtr Connections;
    DBConnectionPtr connection(QString name);
    QHash<QString, ItemState> ItemsState;
    bool ShowSystemDatabases;
    QStringList SystemDatabases;
public:
    void placeWindow();
    explicit MdbSqlGui(QWidget *parent = 0);
    ~MdbSqlGui();
private:
    Ui::MdbSqlGui *ui;
    QString actionData(int actionType, QString objPath="");
    int actionDataType(QString data);
    QString actionDataPath(QString data);
    void checkPages();
protected slots:
    void noViewClickedSlot();
    void knowledgeBasePageSlot();
    void startNewConnectionSlot();
    void showConnections();    
    void doAction(bool);
    void showDbTreeContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);
    void customContextMenuRequestedSlot(const QPoint& pos);
    void itemExpandedSlot(QTreeWidgetItem * ewi);
    void itemCollapsedSlot(QTreeWidgetItem * cwi);
    void tabCloseRequestSlot(int tabIndex);
};

#endif // MDBSQLGUI_H
