#include "sqlwidget.h"
#include "ui_sqlwidget.h"
#include <syntax.h>
#include <settings.h>
#include <QInputDialog>
#include <QMessageBox>

void SqlWidget::saveSlot()
{
    bool ok = false;
    QString tmplName;
    while (!ok)
    {
        QStringList tmpls = Settings::Queries.keys();
        tmpls.insert(0, tr("Query%1").arg(Settings::Queries.count()+1));
        tmplName = QInputDialog::getItem(this, tr("Query selection"), tr("Name of query"), tmpls, 0, true, &ok);
        if (!ok)
            return;
        ok = false;
        if (Settings::Queries.contains(tmplName))
        {
            int rc = QMessageBox::question(this, tr("Query confirmation"), tr("Query with name '%1' already exists, Do You want replace it ?").arg(tmplName), QMessageBox::No, QMessageBox::Yes);
            if (rc == (int)QMessageBox::Yes)
                ok = true;
        }
        else
            ok = true;
    }
    Settings::Queries[tmplName] = ui->Cmd->toPlainText();
    Settings::sync();
}
void SqlWidget::loadSlot()
{
    QStringList tmpls = Settings::Queries.keys();
    if (tmpls.count() == 0)
    {
        QMessageBox::critical(this, tr("Information"), tr("No Query available!"));
        return;
    }
    bool ok = false;
    QString tmplname = QInputDialog::getItem(this, tr("Query selection"), tr("Select query"), tmpls, 0, false, &ok);
    if (!ok)
        return;
    QString str = Settings::Queries[tmplname].toString();
    ui->Cmd->setPlainText(str);
}
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
    Q_UNUSED(synt);
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
    Model->insertRow(Model->rowCount());
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
    QSqlTableModel * tm = reinterpret_cast<QSqlTableModel *>(Model);
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
    connect(ui->QueryOpen, SIGNAL(released()), this,
            SLOT(loadSlot()));
    connect(ui->QuerySave, SIGNAL(released()), this,
            SLOT(saveSlot()));

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
        Syntax *synt = new Syntax(ui->Cmd->document());
        synt = new Syntax(ui->Status->document());
        Q_UNUSED(synt);

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
