#ifndef GUICONNECTION_H
#define GUICONNECTION_H

#include <QDialog>
#include <dbconnection.h>

namespace Ui {
class GuiConnection;
}

class GuiConnection : public QDialog
{
    Q_OBJECT
    DBConnectionPtr Connection;    
public:
    explicit GuiConnection(DBConnectionPtr conn, QWidget *parent = 0);
    ~GuiConnection();
private:
    Ui::GuiConnection *ui;
protected slots:
    void test(bool);
    void showData();
    virtual void done(int r);
};

#endif // GUICONNECTION_H
