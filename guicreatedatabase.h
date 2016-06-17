#ifndef GUICREATEDATABASE_H
#define GUICREATEDATABASE_H

#include <QDialog>
#include <dbconnection.h>

namespace Ui {
class GuiCreateDatabase;
}

class GuiCreateDatabase : public QDialog
{
    Q_OBJECT
    DBConnectionPtr Connection;
public:
    explicit GuiCreateDatabase(DBConnectionPtr conn, QWidget *parent = 0);
    ~GuiCreateDatabase();
private:
    Ui::GuiCreateDatabase *ui;
protected slots:
    virtual void done(int r);
};

#endif // GUICREATEDATABASE_H
