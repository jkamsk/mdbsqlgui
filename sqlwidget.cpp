#include "sqlwidget.h"
#include "ui_sqlwidget.h"
#include <syntax.h>

//void SqlWidget::selectTab(QString name)
//{
//    int ix = -1;
//    for (int i=0;i<ui->Taber->count();i++)
//    {
//        if (ui->Taber->widget(i)->objectName() == name)
//        {
//            ix = i;
//            break;
//        }
//    }
//    ui->Taber->setCurrentIndex(ix);
//}
//void SqlWidget::removeTab(QString name)
//{
//    int ix = -1;
//    for (int i=0;i<ui->Taber->count();i++)
//    {
//        if (ui->Taber->widget(i)->objectName() == name)
//        {
//            ix = i;
//            break;
//        }
//    }
//    ui->Taber->removeTab(ix);
//}
void SqlWidget::execSlot()
{
    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlError err;
    if ((int)Ent->type() >= (int)dbeDatabase)
        Con->runSql(QString("USE %1;").arg(Ent->database()), &qry, &err);
    switch (Mode)
    {
    default:
    case swmEditTableData:
    {
        QSqlTableModel * tm = new QSqlTableModel(this, db);
        tm->setTable(Ent->table());
        tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tm->select();
        Model = (QSqlQueryModel *)tm;
        break;
    }
    case swmQuery:
    {
        Model = new QSqlQueryModel(this);
        Model->setQuery(ui->Cmd->toPlainText(), db);
        break;
    }
    }
    ui->Table->setModel(Model);
    ui->Table->show();
    ui->Table->resizeColumnsToContents();
    err = Model->lastError();
    QString errStr;

    Syntax *synt = new Syntax(ui->Cmd->document());
    synt = new Syntax(ui->Status->document());
    if (err.type() != QSqlError::NoError)
    {
        errStr = tr("Query failed\n");
        errStr += tr("DB desc: ") + err.text() + "\n";        
        ui->Status->setText(errStr);
        //selectTab("TabMessages");
    }
    else
    {
        errStr = tr("Query success\n");
        errStr += tr("Executed query: ") + Model->query().executedQuery();
        ui->Status->setText(errStr);
        //selectTab("TabData");
    }
}
void SqlWidget::addSlot()
{
    bool a = Model->insertRow(Model->rowCount());
    a = 5;
}
void SqlWidget::removeSlot()
{
    QItemSelectionModel * sel = ui->Table->selectionModel();
    QModelIndexList list = sel->selectedRows();
    for (int i=0;i<list.count();i++)
    {
        int row = list.at(i).row();
        Model->removeRow(row);
        ui->Table->hideRow(row);
    }
}
void SqlWidget::syncSlot()
{
    QString errStr;
    QSqlError err;
    QSqlTableModel * tm = (QSqlTableModel *)Model;
    if (!tm->submitAll())
    {
        err = Model->lastError();
        errStr = tr("Query failed\n");
        errStr += tr("DB desc: ") + err.text() + "\n";        
        ui->Status->setPlainText(errStr);
    }
    else
    {
        errStr = tr("Query success\n");
        errStr += tr("Executed query: ") + Model->query().executedQuery();
        ui->Status->setPlainText(errStr);
    }
    execSlot();
}
SqlWidget::SqlWidget(DBConnectionPtr con, DbEntityPtr ent, SqlWidgetMode wm, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SqlWidget)
{
    Mode = wm;
    Model = 0L;
    Con = con;
    Ent = ent;
    ui->setupUi(this);

    //ui->ShowStatus->setChecked(false);
    //ui->Status->setVisible(false);

    //Eventy
    connect(ui->Exec, SIGNAL(released()), this,
            SLOT(execSlot()));
    connect(ui->Refresh, SIGNAL(released()), this,
            SLOT(execSlot()));
    connect(ui->Add, SIGNAL(released()), this,
            SLOT(addSlot()));
    connect(ui->Remove, SIGNAL(released()), this,
            SLOT(removeSlot()));
    connect(ui->Sync, SIGNAL(released()), this,
            SLOT(syncSlot()));

    if (Mode == swmQuery)
    {
        QList<int> sizes;
        sizes.append(50);
        sizes.append(900);
        sizes.append(50);
        ui->splitter->setSizes(sizes);
        ui->Add->setVisible(false);
        ui->Remove->setVisible(false);
        ui->Sync->setVisible(false);
        ui->Refresh->setVisible(false);

        if ((int)Ent->type() >= (int)dbeTable)
        {
            QString sql = QString("SELECT * FROM %1;").arg(ent->table());
            ui->Cmd->setText(sql);
            execSlot();
        }
        else
            ui->Cmd->setText("SELECT * FROM <TABLE> WHERE <CONDITION>;");
    }
    if (Mode == swmEditTableData)
    {
        QList<int> sizes;
        sizes.append(0);
        sizes.append(950);
        sizes.append(50);
        ui->splitter->setSizes(sizes);
        execSlot();
    }
}

SqlWidget::~SqlWidget()
{
    delete ui;
}
