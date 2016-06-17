#ifndef GUIUSER_H
#define GUIUSER_H

#include <QDialog>
#include <dbconnection.h>
#include <dbentity.h>

namespace Ui {
class GuiUser;
}

enum GuiUserType
{
    gutUser,
    gutHost,
    gutDatabase,
    gutPassword,
    gutGrants
};

enum GuiUserGrandType
{
    gugtUntouched,
    gugtNone,
    gugtReadOnly,
    gugtAll
};

class GuiUser : public QDialog
{
    Q_OBJECT
    DBConnectionPtr Con;
    DbEntityPtr Ent;
    GuiUserType Gt;
public:
    void hidePassword();
    QString User;
    QString Password;
    QString Host;
    QString Database;
    GuiUserGrandType GrantType;
    explicit GuiUser(DBConnectionPtr con, DbEntityPtr ent, GuiUserType gt, QWidget *parent = 0);
    ~GuiUser();
private:
    Ui::GuiUser *ui;
private slots:
    virtual void done(int rc);
};

#endif // GUIUSER_H
