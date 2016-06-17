#ifndef SQLWIDGET_H
#define SQLWIDGET_H

#include <QWidget>
#include <QSqlTableModel>
#include <dbconnection.h>
#include <dbentity.h>

namespace Ui {
class SqlWidget;
}

enum SqlWidgetMode
{
    swmEditTableData,
    swmQuery,
};

class SqlWidget : public QWidget
{
private:
    Q_OBJECT
    DBConnectionPtr Con;
    DbEntityPtr Ent;
    QSqlQueryModel * Model;
    SqlWidgetMode Mode;
//    void selectTab(QString name);
//    void removeTab(QString name);
public:
    explicit SqlWidget(DBConnectionPtr con, DbEntityPtr ent, SqlWidgetMode wm=swmEditTableData, QWidget *parent = 0);
    ~SqlWidget();
private:
    Ui::SqlWidget *ui;
protected slots:
    void execSlot();
    void addSlot();
    void removeSlot();
    void syncSlot();
};

#endif // SQLWIDGET_H
