#include "guitable.h"
#include "ui_guitable.h"
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <settings.h>

void GuiTable::newSlot()
{
    ui->ColumnName->clear();
    ui->Type->setCurrentText("");
    ui->DefaultValue->clear();
    ui->Unique->setChecked(false);
    ui->NotNull->setChecked(false);
    ui->ColumnName->setFocus();
}
void GuiTable::deleteSlot()
{
    QList<QListWidgetItem *> items = ui->Columns->selectedItems();
    QListWidgetItem * item;
    QJsonObject cols;
    if (Data.contains(".columns"))
        cols = Data[".columns"].toObject();
    QJsonObject col;
    QString cname;
    for (int i=0;i<items.count();i++)
    {
        item = items.at(i);
        cname = item->text();
        if (cols.contains(cname))
            cols.remove(cname);
    }
    Data[".columns"] = QJsonObject(cols);
    SelectColumn.clear();
    showData();
    if (cols.count() == 0)
        newSlot();
}
void GuiTable::setSlot()
{
    if (!validateColumnData())
        return;
    QJsonObject cols;
    if (Data.contains(".columns"))
        cols = Data[".columns"].toObject();
    QJsonObject col;
    QString cname = ui->ColumnName->text();
    if (cols.contains(cname))
        col = cols[cname].toObject();
    col["Type"] = ui->Type->currentText();
    col["NotNull"] = ui->NotNull->isChecked();
    col["Unique"] = ui->Unique->isChecked();
    col["DefaultValue"] = ui->DefaultValue->text();
    col["Comment"] = ui->Comment->text();
    cols[cname] = col;
    SelectColumn = cname;
    Data[".columns"] = QJsonObject(cols);
    showData();
}
void GuiTable::showColumnData(QString cname)
{
    if (cname.isEmpty())
    {
        errMsg(tr("Column definition is empty"));
        return;
    }
    QJsonObject cols, col;
    cols = Data[".columns"].toObject();
    col = cols[cname].toObject();
    ui->ColumnName->setText(cname);
    ui->Type->setCurrentText(col["Type"].toString());
    ui->NotNull->setChecked(col["NotNull"].toBool());
    ui->Unique->setChecked(col["Unique"].toBool());
    ui->DefaultValue->setText(col["DefaultValue"].toString());
    ui->Comment->setText(col["Comment"].toString());
}
void GuiTable::loadFromTemplateSlot()
{
    QStringList tmpls = Settings::TableTemplates.keys();
    if (tmpls.count() == 0)
    {
        errMsg("No templates available!");
        return;
    }
    bool ok = false;
    QString tmplname = QInputDialog::getItem(this, tr("Template selection"), tr("Select template"), tmpls, 0, false, &ok);
    if (!ok)
        return;
    Data = Settings::TableTemplates[tmplname].toObject();
    showData();
}
void GuiTable::removeTemplateSlot()
{
    QStringList tmpls = Settings::TableTemplates.keys();
    if (tmpls.count() == 0)
    {
        errMsg("No templates available!");
        return;
    }
    bool ok = false;
    QString tmplname = QInputDialog::getItem(this, tr("Template selection"), tr("Select template to remove"), tmpls, 0, false, &ok);
    if (!ok)
        return;
    Settings::TableTemplates.remove(tmplname);
    Settings::sync();
}
void GuiTable::saveAsTemplateSlot()
{
    bool ok = false;
    if (!validateData())
        return;
    setData();
    QString tmplName;
    while (!ok)
    {
        QStringList tmpls = Settings::TableTemplates.keys();
        tmpls.insert(0, tr("Template%1").arg(Settings::TableTemplates.count()+1));
        tmplName = QInputDialog::getItem(this, tr("Table selection"), tr("Name of table template"), tmpls, 0, true, &ok);
        if (!ok)
            return;
        ok = false;
        //tmplName = QInputDialog::getText(this, tr("Table template"), tr("Name of table template"),QLineEdit::Normal, tr("Template%1").arg(Settings::TableTemplates.count()+1));
        if (Settings::TableTemplates.contains(tmplName))
        {
            int rc = QMessageBox::question(this, tr("Template confirmation"), tr("Table template with name '%1' already exists, Do You want replace it ?").arg(tmplName), QMessageBox::No, QMessageBox::Yes);
            if (rc == (int)QMessageBox::Yes)
                ok = true;
        }
        else
            ok = true;
    }
    Settings::TableTemplates[tmplName] = Data;
    Settings::sync();
}
QString GuiTable::sqlCommand()
{
    QString sql;
    QString key, tmp;
    QJsonObject iobj;
    QString idef;
    if (Create)
    {
        sql = QString("CREATE TABLE %1 ( \n").arg(Data["Name"].toString());
        QJsonObject cols = Data[".columns"].toObject();
        QStringList keys = cols.keys();
        for (int i=0;i<keys.count();i++)
        {
            key = keys.at(i);
            iobj = cols[key].toObject();
            //column name
            idef = key;
            // type
            idef += " " + iobj["Type"].toString();
            if (iobj["NotNull"].toBool())
                idef += " NOT NULL";
            else
                idef += " NULL";
            // defvalue
            tmp = iobj["DefaultValue"].toString();
            if (!tmp.isEmpty())
                idef += " DEFAULT " + tmp;
            if (iobj["AutoIncrement"].toBool())
                idef += " AUTO_INCREMENT";
            if (iobj["Unique"].toBool())
                idef += " UNIQUE KEY";
            if (iobj["PrimaryKey"].toBool())
                idef += " PRIMARY KEY";
            tmp = iobj["Comment"].toString();
            if (!tmp.isEmpty())
                idef += QString(" COMMENT '%1'").arg(tmp);
            sql += idef;
            if (i < keys.count() -1)
                sql += ",\n";
        }
        sql += "\n );";
    }
    else
    {
        sql = QString("ALTER TABLE %1 ( ").arg(Data["Name"].toString());
        sql += " );";
    }
    return sql;
}
void GuiTable::applySlot()
{
    setData();
    if (!validateData())
        return;

    QString sql = sqlCommand();

    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlError err;
    Con->runSql(QString("USE %1;").arg(Ent->database()), &qry, &err);
    if (!Con->runSql(sql, &qry, &err))
    {
        QMessageBox::critical(this, "Error", err.databaseText());
        return;
    }
    else
    {
        emit dbRefresh();
        QString msg = "";
        if (Create)
            msg = tr("Table %1 was successfully created").arg(Data["Name"].toString());
        if (Alter)
            msg = tr("Table %1 was successfully changed").arg(Data["Name"].toString());
        QMessageBox::information(this, tr("success"), msg);
    }

}
void GuiTable::setData()
{
    if (!validateData())
        return;
    Data["Name"] = ui->TableName->text();
    Data["Temporary"] = ui->Temporary->isChecked();
    Data["Engine"] = ui->Engine->currentText();
}
bool GuiTable::validateColumnData()
{
    if (ui->ColumnName->text().isEmpty())
    {
        errMsg(tr("Column name is empty!"));
        ui->ColumnName->setFocus();
        return false;
    }
    if (ui->Type->currentText().isEmpty())
    {
        errMsg(tr("Column type is empty!"));
        ui->Type->setFocus();
        return false;
    }
    errMsg();
    return true;
}
bool GuiTable::validateData()
{
    if (ui->TableName->text().isEmpty())
    {
        errMsg(tr("Table name is empty!"));
        ui->TableName->setFocus();
        return false;
    }
    if (ui->Columns->count() == 0)
    {
        errMsg(tr("Least one column is required"));
        newSlot();
        return false;
    }
    errMsg();
    return true;
}
void GuiTable::showSqlCmdSlot()
{
    setData();
    if (!validateData())
        return;
    QInputDialog::getMultiLineText(this, tr("Sql command"), "Sql", sqlCommand());
}
void GuiTable::showData()
{
    if (Create)
    {
        ui->Apply->setText(tr("Create"));
        ui->TableName->setText(Data["Name"].toString());
    }
    if (Alter)
    {
        ui->Apply->setText(tr("Alter"));
        ui->TableName->setText(TableName);
        ui->TableName->setReadOnly(true);
    }

    ui->Temporary->setChecked(Data["Temporary"].toBool());
    ui->Engine->setCurrentText(Data["Engine"].toString());
    QJsonObject cols, col;
    if (Data.contains(".columns"))
        cols = Data[".columns"].toObject();
    QStringList keys = cols.keys();
    QString cname;
    ui->Columns->clear();
    int sci = -1;
    for (int i=0;i<keys.count();i++)
    {
        cname = keys.at(i);
        if (SelectColumn == cname)
            sci = i;
        ui->Columns->addItem(cname);
    }
    if (sci >=0 && sci < ui->Columns->count())
        ui->Columns->setCurrentRow(sci);
}

void GuiTable::columnActivatedSlot(QListWidgetItem *row)
{
    QString cname = row->text();
    showColumnData(cname);
}
void GuiTable::errMsg(QString msg)
{
    if (msg.isEmpty())
    {
        ui->Msg->clear();
        //ui->Msg->setVisible(false);
        return;
    }
    //ui->Msg->setVisible(true);
    ui->Msg->setText(msg);
}
GuiTable::GuiTable(DBConnectionPtr con, DbEntityPtr ent, QString name, QJsonObject data, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuiTable)
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes.append(250);
    sizes.append(750);
    ui->splitter->setSizes(sizes);
    //ui->Msg->setHidden(true);

    Con = con;
    Ent = ent;
    Data = data;

    connect(ui->New, SIGNAL(released()), this, SLOT(newSlot()));
    connect(ui->Delete, SIGNAL(released()), this, SLOT(deleteSlot()));
    connect(ui->Set, SIGNAL(released()), this, SLOT(setSlot()));
    connect(ui->Apply, SIGNAL(released()), this, SLOT(applySlot()));
    connect(ui->SaveAsTemplate, SIGNAL(released()), this, SLOT(saveAsTemplateSlot()));
    connect(ui->LoadFromTemplate, SIGNAL(released()), this, SLOT(loadFromTemplateSlot()));
    connect(ui->RemoveTemplate, SIGNAL(released()), this, SLOT(removeTemplateSlot()));
    connect(ui->Columns, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(columnActivatedSlot(QListWidgetItem*)));
    connect(ui->ShowSql, SIGNAL(released()), this, SLOT(showSqlCmdSlot()));

    Alter = false;
    Create = false;
    TableName = name;
    if (TableName.isEmpty())
        Create = true;
    else
        Alter = true;

    showData();
}

GuiTable::~GuiTable()
{
    delete ui;
}
