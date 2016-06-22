#include "mdbsqlgui.h"
#include "ui_mdbsqlgui.h"
#include <guiconnection.h>
#include <QStringList>
#include <QMessageBox>
#include <QDesktopWidget>
#include <dbentity.h>
#include <guicreatedatabase.h>
#include <sqlwidget.h>
#include <usermng.h>
#include <guitable.h>
#include <QDesktopServices>
#include <QUrl>
#include <settings.h>

void MdbSqlGui::checkPages()
{
    if (ui->DBTree->topLevelItemCount() == 0)
    {
        ui->DBTree->setVisible(false);
        ui->StartNewConnection->setVisible(true);
    }
    else
    {
        ui->DBTree->setVisible(true);
        ui->StartNewConnection->setVisible(false);
    }
    if (ui->Pager->count() == 0)
    {
        ui->Pager->setVisible(false);
        ui->NoView->setVisible(true);
    }
    else
    {
        ui->Pager->setVisible(true);
        ui->NoView->setVisible(false);
    }
}
void MdbSqlGui::showConnections()
{    
    ui->DBTree->clear();
    QString ps_conn;
    QString ps_db;
    QString path;
    QList<QTreeWidgetItem *> expand;
    ItemState istate;
    for (int i=0;i<Settings::Connections->count();i++)
    {
        DBConnectionPtr con = Settings::Connections->at(i);
        ps_conn = con->name();
        path = QString("%1").arg(ps_conn);
        QTreeWidgetItem * w_conn = new QTreeWidgetItem(ui->DBTree);
        w_conn->setText(0, ps_conn);
        w_conn->setData(0, Qt::UserRole, path);
        if (con->isConnected())
            w_conn->setIcon(0, QIcon(":/icons/server-on.png"));
        else
            w_conn->setIcon(0, QIcon(":/icons/server-off.png"));

        istate = ItemsState.value(path, isCollapsed);
        if (istate != isCollapsed)
            expand.append(w_conn);

        ui->DBTree->addTopLevelItem(w_conn);
        if (con->isConnected())
        {
            QBrush green(Qt::darkGreen);
            w_conn->setForeground(0 ,green);

            // Load Databases
            QStringList databases;
            if (con->databases(databases))
            {
                for (int d=0;d<databases.count();d++)
                {
                    QString ps_db = databases.at(d);
                    if (!ShowSystemDatabases)
                        if (SystemDatabases.contains(ps_db))
                            continue;
                    path = QString("%1/%2").arg(ps_conn).arg(ps_db);
                    QTreeWidgetItem * w_db = new QTreeWidgetItem(w_conn);
                    w_db->setText(0, ps_db);
                    w_db->setData(0, Qt::UserRole, path);
                    w_db->setIcon(0, QIcon(":/icons/database.png"));
                    w_conn->addChild(w_db);

                    istate = ItemsState.value(path, isCollapsed);
                    if (istate != isCollapsed)
                        expand.append(w_db);

                    // Load Tables
                    con->selectDatabase(ps_db);
                    QStringList tables;
                    if (con->tables(tables))
                    {
                        for (int t=0;t<tables.count();t++)
                        {
                            QString ps_tab = tables.at(t);
                            path = QString("%1/%2/tables/%3").arg(ps_conn).arg(ps_db).arg(ps_tab);
                            QTreeWidgetItem * w_tab = new QTreeWidgetItem(w_db);
                            w_tab->setText(0, ps_tab);
                            w_tab->setData(0, Qt::UserRole, path);
                            w_tab->setIcon(0, QIcon(":/icons/table.png"));
                            w_db->addChild(w_tab);

                            istate = ItemsState.value(path, isCollapsed);
                            if (istate != isCollapsed)
                                expand.append(w_tab);
                        }
                    }
                }
            }
        }
        else
        {
            QBrush black(Qt::black);
            w_conn->setForeground(0 ,black);
        }
    }

    QTreeWidgetItem *wi;
    for (int i=0;i<expand.count();i++)
    {
        wi = expand.at(i);
        ui->DBTree->expandItem(wi);
    }
}
DBConnectionPtr MdbSqlGui::connection(QString name)
{
    DBConnectionPtr ret;
    for (int i=0;i<Settings::Connections->count();i++)
    {
        ret = Settings::Connections->at(i);
        if (ret->name() == name)
            return ret;
    }
    return ret;
}
QString MdbSqlGui::actionData(int actionType, QString objPath)
{
    QString str = QString("%1;%2").arg(actionType).arg(objPath);
    return str;
}
int MdbSqlGui::actionDataType(QString data)
{
    QStringList list = data.split(";");
    QString val = "-1";
    if (list.count() >= 1)
        val = list.at(0);
    return val.toInt();
}
QString MdbSqlGui::actionDataPath(QString data)
{
    QStringList list = data.split(";");
    QString val = "";
    if (list.count() >= 2)
        val = list.at(1);
    return val;
}
void MdbSqlGui::doAction(bool /*toggled*/)
{
    QAction * act = (QAction*)sender();
    QString data = act->data().toString();
    int acttype = actionDataType(data);
    QString path = actionDataPath(data);
    DbEntityPtr entity = DbEntity::Make(path);
    switch(acttype)
    {
    case ACT_EXPANDALL:
        ui->DBTree->expandAll();
        break;
    case ACT_COLLAPSEALL:
        ui->DBTree->collapseAll();
        break;
    case ACT_EDITTABLE:
    {
        DBConnectionPtr conn = connection(entity->connection());
        GuiTable * gt = new GuiTable(conn, entity, entity->table());
        connect(gt, SIGNAL(dbRefresh()), this, SLOT(showConnections()));
        int ix = ui->Pager->addTab(gt, QIcon(":/icons/edit.png"), tr("Edit Table") + QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);
        break;
    }
    case ACT_ADDTABLE:
    {
        DBConnectionPtr conn = connection(entity->connection());
        GuiTable * gt = new GuiTable(conn, entity, "");
        connect(gt, SIGNAL(dbRefresh()), this, SLOT(showConnections()));
        int ix = ui->Pager->addTab(gt, QIcon(":/icons/add.png"), tr("New Table") + QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);        
        break;
    }
    case ACT_DROPTABLE:
    {
        int rc = QMessageBox::question(this, tr("Drop confirmation"), tr("Do You want drop table %1 ?").arg(entity->table()), QMessageBox::No, QMessageBox::Yes);
        if (rc != (int)QMessageBox::Yes)
            break;
        DBConnectionPtr conn = connection(entity->connection());
        QSqlDatabase db = QSqlDatabase::database(entity->connection());
        QSqlQuery qry(db);
        QSqlError err;
        conn->runSql(QString("USE %1;").arg(entity->database()), &qry, &err);
        conn->runSql("START TRANSACTION;", &qry, &err);
        if (!conn->runSql(QString("DROP TABLE `%1`;").arg(entity->table()), &qry, &err))
        {
            conn->runSql("ROLLBACK;", &qry, &err);
            QMessageBox::critical(this, tr("Drop table %1 failed").arg(entity->table()), err.databaseText());
            break;
        }
        conn->runSql("COMMIT;", &qry, &err);
        showConnections();
        break;
    }
    case ACT_USERMNGMT:
    {        
        DBConnectionPtr conn = connection(entity->connection());
        UserMng * umng = new UserMng(conn, entity);
        int ix = ui->Pager->addTab(umng, QIcon(":/icons/users.png"), tr("Users") + QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);
        break;
    }
    case ACT_EDITDATA:
    {
        DBConnectionPtr conn = connection(entity->connection());
        SqlWidget * sw = new SqlWidget(conn, entity, swmEditTableData);
        int ix = ui->Pager->addTab(sw, QIcon(":/icons/table_edit.png"), entity->table()+ QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);
        break;
    }
    case ACT_NEWQUERY:
    {
        DBConnectionPtr conn = connection(entity->connection());
        SqlWidget * sw = new SqlWidget(conn, entity, swmQuery);
        int ix = ui->Pager->addTab(sw, QIcon(":/icons/query.png"), tr("Query")+ QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);
        break;
    }
    case ACT_SHOWDATA:
    {
        DBConnectionPtr conn = connection(entity->connection());
        SqlWidget * sw = new SqlWidget(conn, entity, swmQuery);
        int ix = ui->Pager->addTab(sw, QIcon(":/icons/query.png"), entity->table()+ QString("/%1").arg(entity->connection()));
        ui->Pager->setCurrentIndex(ix);
        break;
    }
    case ACT_SHOWSYSDB:
        ShowSystemDatabases = !ShowSystemDatabases;
    case ACT_RELOAD:
    {
        showConnections();
        break;
    }
    case ACT_NEWCONNECTION:
    {
        DBConnectionPtr newc = DBConnection::New(QString("Connection %1").arg(Settings::Connections->count()+1));
        GuiConnection dialog(newc, this);
        int rc = dialog.exec();
        if (rc == QDialog::Accepted)
        {
            Settings::Connections->append(newc);
            ItemsState.insert(newc->name(), isEspanded);
        }
        DBConnection::saveConnections(Settings::Connections);
        showConnections();
        break;
    }
    case ACT_DROPCONN:
    {
        DBConnectionPtr conn = connection(entity->connection());
        int rc = QMessageBox::question(this, tr("Drop confirmation"), tr("Do You want drop connection %1 ?").arg(conn->name()), QMessageBox::No, QMessageBox::Yes);
        if (rc != (int)QMessageBox::Yes)
            break;
        if (conn.get())
        {
            Settings::Connections->removeAt(Settings::Connections->indexOf(conn));
            DBConnection::saveConnections(Settings::Connections);
            showConnections();
        }
        break;
    }
    case ACT_EDITCONNECTION:
    {
        DBConnectionPtr con = connection(entity->connection());
        if (con.get())
        {
            GuiConnection dialog(con, this);
            dialog.exec();
            DBConnection::saveConnections(Settings::Connections);
            showConnections();
        }
        break;
    }
    case ACT_CONNECT:
    {
        DBConnectionPtr con = connection(entity->connection());
        if (con.get())
        {
            if (con->login())
                ItemsState.insert(con->name(), isEspanded);
            showConnections();
        }
        break;
    }
    case ACT_DISCONNECT:
    {
        DBConnectionPtr con = connection(entity->connection());
        if (con.get())
        {
            con->logout();
            showConnections();
        }
        break;
    }
    case ACT_CREATEDB:
    {
        DBConnectionPtr con = connection(entity->connection());
        GuiCreateDatabase dialog(con, this);
        dialog.exec();
        showConnections();
        break;
    }
    case ACT_REMOVEDB:
    {
        DBConnectionPtr con = connection(entity->connection());
        QMessageBox msgBox;
        msgBox.setText(tr("Database %1 will be dropped.").arg(entity->database()));
        msgBox.setInformativeText("Do you want really to drop ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == (int)QMessageBox::Yes)
        {
            if (con->dropDatabase(entity->database()))
                showConnections();
            else
            {
                QMessageBox errBox;
                errBox.setText(tr("Database %1 can not be dropped.").arg(entity->database()));
                errBox.setInformativeText(con->lastError().databaseText());
                errBox.setStandardButtons(QMessageBox::Ok);
                errBox.setDefaultButton(QMessageBox::Ok);
                errBox.exec();
            }
        }
        break;
    }
    case ACT_MDBHOMEPG:
        noViewClickedSlot();
        break;
    case ACT_KNOWLBASE:
        knowledgeBasePageSlot();
        break;
    }//switch
    checkPages();
}
void MdbSqlGui::noViewClickedSlot()
{
    QDesktopServices::openUrl(QUrl("https://mariadb.com/"));
}
void MdbSqlGui::knowledgeBasePageSlot()
{
    QDesktopServices::openUrl(QUrl("https://mariadb.com/kb/en/"));
}
void MdbSqlGui::startNewConnectionSlot()
{
    DBConnectionPtr newc = DBConnection::New(QString("Connection %1").arg(Settings::Connections->count()+1));
    GuiConnection dialog(newc, this);
    int rc = dialog.exec();
    if (rc == QDialog::Accepted)
    {
        Settings::Connections->append(newc);
    }
    DBConnection::saveConnections(Settings::Connections);
    showConnections();
    checkPages();
}
void MdbSqlGui::tabCloseRequestSlot(int tabIndex)
{
    ui->Pager->removeTab(tabIndex);
    checkPages();
}
void MdbSqlGui::itemExpandedSlot(QTreeWidgetItem * ewi)
{
    QString pth = ewi->data(0, Qt::UserRole).toString();
    if (ItemsState.contains(pth))
        ItemsState.remove(pth);
    ItemsState.insert(pth, isEspanded);
}
void MdbSqlGui::itemCollapsedSlot(QTreeWidgetItem * cwi)
{
    QString pth = cwi->data(0, Qt::UserRole).toString();
    if (ItemsState.contains(pth))
        ItemsState.remove(pth);
    ItemsState.insert(pth, isCollapsed);
}
void MdbSqlGui::customContextMenuRequestedSlot(const QPoint& pos)
{
    QTreeWidgetItem* item = ui->DBTree->itemAt(pos);
    showDbTreeContextMenu(item, ui->DBTree->viewport()->mapToGlobal(pos));
}
void MdbSqlGui::showDbTreeContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
    QMenu menu;


    QAction * act = menu.addAction(tr("New Connection"), this, SLOT(doAction(bool)));
    act->setData(actionData(ACT_NEWCONNECTION));
    act->setIcon(QIcon(":/icons/add.png"));

    menu.addSeparator();

    act = menu.addAction(tr("Expand All"), this, SLOT(doAction(bool)));
    act->setData(actionData(ACT_EXPANDALL));
    act->setIcon(QIcon(":/icons/expand.png"));

    act = menu.addAction(tr("Collapse All"), this, SLOT(doAction(bool)));
    act->setData(actionData(ACT_COLLAPSEALL));
    act->setIcon(QIcon(":/icons/collapse.png"));

    menu.addSeparator();

    if (item != 0L)
    {
        QString path = item->data(0, Qt::UserRole).toString();
        DbEntityPtr entity = DbEntity::Make(path);

        if (entity->type() == dbeConnection)
        {
            QAction * act = menu.addAction(tr("Edit"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_EDITCONNECTION, path));
            act->setIcon(QIcon(":/icons/edit.png"));

            menu.addSeparator();

            DBConnectionPtr con = connection(entity->connection());
            if (con.get())
            {
                if (con->isConnected())
                {
                    act = menu.addAction(tr("Disconnect"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_DISCONNECT, path));
                    act->setIcon(QIcon(":/icons/disconnect.png"));

                    act = menu.addAction(tr("Refresh"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_RELOAD, path));
                    act->setIcon(QIcon(":/icons/refresh.png"));

                    act = menu.addAction(tr("Create Database"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_CREATEDB, path));
                    act->setIcon(QIcon(":/icons/add.png"));

                    act = menu.addAction(tr("Manage Users"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_USERMNGMT, path));
                    act->setIcon(QIcon(":/icons/users.png"));

                    act = menu.addAction(tr("Show system databases"), this, SLOT(doAction(bool)));
                    act->setCheckable(true);
                    act->setChecked(ShowSystemDatabases);
                    act->setData(actionData(ACT_SHOWSYSDB, path));
                    act->setIcon(QIcon(":/icons/database.png"));
                }
                else
                {
                    act = menu.addAction(tr("Connect"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_CONNECT, path));
                    act->setIcon(QIcon(":/icons/connect.png"));

                    menu.addSeparator();

                    act = menu.addAction(tr("Drop Connection"), this, SLOT(doAction(bool)));
                    act->setData(actionData(ACT_DROPCONN, path));
                    act->setIcon(QIcon(":/icons/remove.png"));
                }
            }
        }        
        if (entity->type() == dbeDatabase)
        {
            QAction *act;

            act = menu.addAction(tr("New query"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_NEWQUERY, path));
            act->setIcon(QIcon(":/icons/new_query.png"));

            act = menu.addAction(tr("New Table"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_ADDTABLE, path));
            act->setIcon(QIcon(":/icons/add.png"));

            menu.addSeparator();

            if (!SystemDatabases.contains(entity->database()))
            {
                act = menu.addAction(tr("Drop Database"), this, SLOT(doAction(bool)));
                act->setData(actionData(ACT_REMOVEDB, path));
                act->setIcon(QIcon(":/icons/remove.png"));
            }

        }
        if (entity->type() == dbeTable)
        {
            QAction *act;

            act = menu.addAction(tr("Edit Table"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_EDITTABLE, path));
            act->setIcon(QIcon(":/icons/edit.png"));

            menu.addSeparator();

            act = menu.addAction(tr("Query data"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_SHOWDATA, path));
            act->setIcon(QIcon(":/icons/query.png"));

            act = menu.addAction(tr("Edit Data"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_EDITDATA, path));
            act->setIcon(QIcon(":/icons/table_edit.png"));

            menu.addSeparator();

            act = menu.addAction(tr("Drop Table"), this, SLOT(doAction(bool)));
            act->setData(actionData(ACT_DROPTABLE, path));
            act->setIcon(QIcon(":/icons/remove.png"));
        }
    }

    menu.addSeparator();

    act = menu.addAction(tr("MariaDB Home page"), this, SLOT(doAction(bool)));
    act->setData(actionData(ACT_MDBHOMEPG));
    act->setIcon(QIcon(":/icons/mariadb-logo.png"));

    act = menu.addAction(tr("Knowledge base"), this, SLOT(doAction(bool)));
    act->setData(actionData(ACT_KNOWLBASE));
    act->setIcon(QIcon(":/icons/doc.png"));

    menu.exec(globalPos);
}
void MdbSqlGui::placeWindow()
{
    QDesktopWidget * dw = QApplication::desktop();
    QSize dws = dw->geometry().size();

    double ddw = (double)dws.width();
    double ddh = (double)dws.height();

    double dww = 0.75*ddw;
    double dwh = 0.85*ddh;

    if (dww > 1280.0)
        dww = 1280.0;
    if (dwh > 768.0)
        dwh = 768.0;

    double dwt = (ddh - dwh) / 2.0;
    double dwl = (ddw - dww) / 2.0;

    int top = (int)dwt;
    int left = (int)dwl;
    int width = (int)dww;
    int height = (int)dwh;

    this->setGeometry(left, top, width, height);
    QList<int> sizes;
    sizes.append(250);
    sizes.append(750);
    ui->splitter->setSizes(sizes);

}
MdbSqlGui::MdbSqlGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MdbSqlGui)
{
    SystemDatabases.append("performance_schema");
    SystemDatabases.append("information_schema");
    SystemDatabases.append("mysql");
    ShowSystemDatabases = false;
    ui->setupUi(this);
    ui->DBTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->Version->setText(VERSION);
    Settings::init();
    Settings::Connections = DBConnection::readConnections();

    // Events
    connect(ui->DBTree, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(customContextMenuRequestedSlot(QPoint)));
    connect(ui->DBTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this,
            SLOT(itemCollapsedSlot(QTreeWidgetItem*)));
    connect(ui->DBTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this,
            SLOT(itemExpandedSlot(QTreeWidgetItem*)));
    connect(ui->Pager, SIGNAL(tabCloseRequested(int)), this,
            SLOT(tabCloseRequestSlot(int)));
    connect(ui->StartNewConnection, SIGNAL(pressed()),
            this, SLOT(startNewConnectionSlot()));
    connect(ui->HomePage, SIGNAL(pressed()),
            this, SLOT(noViewClickedSlot()));
    connect(ui->KnowledgeBase, SIGNAL(pressed()),
            this, SLOT(knowledgeBasePageSlot()));
    showConnections();
    checkPages();
}
MdbSqlGui::~MdbSqlGui()
{
    delete ui;
}
