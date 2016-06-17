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
    QString SelectColumn;
    void showData();
    bool validateColumnData();
    bool validateData();
    void errMsg(QString str="");
    void showColumnData(QString cname="");
    QString sqlCommand();
public:
    explicit GuiTable(DBConnectionPtr con, DbEntityPtr ent, QString Name="", QJsonObject data=QJsonObject(), QWidget *parent = 0);
    ~GuiTable();
private slots:
    void columnActivatedSlot(QListWidgetItem *);
    void newSlot();
    void deleteSlot();
    void setSlot();
    void applySlot();
    void saveAsTemplateSlot();
    void loadFromTemplateSlot();
    void removeTemplateSlot();
    void setData();
    void showSqlCmdSlot();
private:
    Ui::GuiTable *ui;
signals:
    void dbRefresh();
};

#endif // GUITABLE_H
