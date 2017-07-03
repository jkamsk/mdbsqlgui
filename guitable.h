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
    void showData();
    bool validateColumnData();
    bool validateData();
    void errMsg(QString str="");
    void showColumnData(QString cname="");
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
    void newColumnSlot();
    void deleteColumnSlot();
    void refresh();
    void setColumnSlot();
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
