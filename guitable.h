#ifndef GUITABLE_H
#define GUITABLE_H

#include <QWidget>
#include <dbconnection.h>
#include <QJsonObject>
#include <QListWidgetItem>
#include <dbentity.h>

namespace Ui {
class GuiTable;
}

class GuiTable : public QWidget
{
    Q_OBJECT
    bool Alter;
    bool Create;
    QString TableName;
    DBConnectionPtr Con;
    DbEntityPtr Ent;
    QJsonObject Data;    
    QJsonObject OriginData;
    QString SelectColumn;
    QString SelectFok;
    void showData();
    bool validateColumnData();
    bool validateFokData();
    bool validateData();
    void errMsg(QString str="");
    void showColumnData(QString cname="");
    void showFokData(QString fokname="");
    QStringList sqlCommand();
    void fillEngines();
    void fillColumnTypes();
    void fillCollations();
    void makeTableData();
    QStringList tableAlter();
    void setData(bool disable_validate=false);
public:
    explicit GuiTable(DBConnectionPtr con, DbEntityPtr ent, QString Name="", QJsonObject data=QJsonObject(), QWidget *parent = 0);
    ~GuiTable();
private slots:
    void columnActivatedSlot(QListWidgetItem *);
    void fokActivatedSlot(QListWidgetItem *);
    void newColumnSlot();
    void newFokSlot();
    void deleteColumnSlot();
    void deleteFokSlot();
    void refresh();
    void setColumnSlot();
    void setFokSlot();
    void applySlot();
    void saveAsTemplateSlot();
    void loadFromTemplateSlot();
    void removeTemplateSlot();    
    void showSqlCmdSlot();
private:
    Ui::GuiTable *ui;
signals:
    void dbRefresh();
};

#endif // GUITABLE_H
