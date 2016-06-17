#include "guiuser.h"
#include "ui_guiuser.h"


void GuiUser::done(int rc)
{
    if (rc == (int)QDialog::Accepted)
    {
        if (ui->User->isVisible())
            User = ui->User->text();
        if (ui->Password->isVisible())
            Password = ui->Password->text();
        if (ui->Host->isVisible())
            Host = ui->Host->text();
        Database = ui->Database->currentData().toString();
        GrantType = gugtUntouched;
        if (ui->GrantsAll->isChecked())
            GrantType = gugtAll;
        else if (ui->GrantsNone->isChecked())
            GrantType = gugtNone;
        else if (ui->GrantsReadOnly->isChecked())
            GrantType = gugtReadOnly;
        else
            GrantType = gugtUntouched;

        ui->Msg->setText("");
        if (Gt == gutUser)
        {
            if (User.isEmpty())
            {
                ui->Msg->setText(tr("User name is empty !"));
                ui->User->setFocus();
                return;
            }
            if (Password.isEmpty())
            {
                ui->Msg->setText(tr("Password is empty !"));
                ui->Password->setFocus();
                return;
            }
            if (Host.isEmpty())
            {
                ui->Msg->setText(tr("Host is empty ! Preset, Defaults % (all hosts)"));
                ui->Host->setText("%");
                ui->Host->setFocus();
                return;
            }
        }
    }
    QDialog::done(rc);
}
void GuiUser::hidePassword()
{
    ui->label_Password->setHidden(true);
    ui->Password->setHidden(true);
}
GuiUser::GuiUser(DBConnectionPtr con, DbEntityPtr ent, GuiUserType gt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuiUser)
{
    ui->setupUi(this);
    Con = con;
    Ent = ent;
    Gt = gt;

    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlError err;
    Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
    Con->runSql(QString("SHOW DATABASES;"), &qry, &err);
    ui->Database->clear();
    ui->Database->addItem(tr("All"), "*");

    int dbix = 0;

    QString dbname;
    while(qry.next())
    {
        dbname = qry.value(0).toString();
        if (dbname == "performance_schema")
            continue;
        if (dbname == "information_schema")
            continue;
        if (dbname == "mysql")
            continue;
        ui->Database->addItem(dbname, dbname);
        if (dbname == Database)
            dbix = ui->Database->count() -1;
    }
    ui->Database->setCurrentIndex(dbix);

    ui->User->setText(User);
    ui->Password->setText(Password);
    ui->Host->setText(Host);

    ui->GrantNoChange->setChecked(true);
    ui->GrantsAll->setChecked(false);
    ui->GrantsNone->setChecked(false);
    ui->GrantsReadOnly->setChecked(false);

    switch(Gt)
    {
    case gutUser:
    default:
        setWindowTitle(tr("User"));
        ui->GrantNoChange->setHidden(true);
        ui->GrantsAll->setChecked(true);
        break;
    case gutHost:
        setWindowTitle(tr("Host"));
        ui->GrantNoChange->setHidden(true);
        ui->GrantsAll->setChecked(true);
        ui->label_User->setHidden(true);
        ui->User->setHidden(true);
        break;
    case gutDatabase:
        setWindowTitle(tr("Database"));
        ui->GrantNoChange->setHidden(true);
        ui->GrantsAll->setChecked(true);
        ui->label_User->setHidden(true);
        ui->User->setHidden(true);
        ui->label_Password->setHidden(true);
        ui->Password->setHidden(true);
        ui->label_Host->setHidden(true);
        ui->Host->setHidden(true);
        ui->Database->removeItem(0);
        break;
    case gutPassword:
        setWindowTitle(tr("Password"));
        ui->label_User->setHidden(true);
        ui->User->setHidden(true);
        ui->label_Host->setHidden(true);
        ui->Host->setHidden(true);
        ui->label_Database->setHidden(true);
        ui->Database->setHidden(true);
        ui->label_Grants->setHidden(true);
        ui->GrantsAll->setHidden(true);
        ui->GrantsNone->setHidden(true);
        ui->GrantsReadOnly->setHidden(true);
        ui->GrantNoChange->setHidden(true);
        break;
    case gutGrants:
        setWindowTitle(tr("Grants"));
        ui->label_User->setHidden(true);
        ui->User->setHidden(true);
        ui->label_Password->setHidden(true);
        ui->Password->setHidden(true);
        ui->label_Host->setHidden(true);
        ui->Host->setHidden(true);
        ui->label_Database->setHidden(true);
        ui->Database->setHidden(true);
        break;
    }

}

GuiUser::~GuiUser()
{
    delete ui;
}

