#include "guiconnection.h"
#include "ui_guiconnection.h"
#include <QDateTime>

void GuiConnection::test(bool)
{
    ui->Status->clear();
    DBConnectionPtr dbc = DBConnection::New("Test" + QDateTime::currentDateTime().toString(Qt::ISODate));
    dbc->Driver = "QMYSQL";
    dbc->Name = ui->Name->text();
    dbc->Host = ui->Host->text();
    dbc->Port = ui->Port->value();
    dbc->User = ui->User->text();
    dbc->Password = ui->Password->text();
    if (dbc->test())
        ui->Status->setText(tr("Success"));
    else
        ui->Status->setText(tr("Failed"));
}
void GuiConnection::done(int r)
{
    if (r == QDialog::Accepted)
    {
        Connection->Driver = ui->Driver->currentData().toString();
        Connection->Name = ui->Name->text();
        Connection->Host = ui->Host->text();
        Connection->Port = ui->Port->value();
        Connection->User = ui->User->text();
        Connection->Password = ui->Password->text();
    }
    QDialog::done(r);
}
void GuiConnection::showData()
{
    ui->Driver->setCurrentIndex(0);
    ui->Name->setText(Connection->Name);
    ui->Host->setText(Connection->Host);
    ui->Port->setValue(Connection->Port);
    ui->User->setText(Connection->User);
    ui->Password->setText(Connection->Password);
}
GuiConnection::GuiConnection(DBConnectionPtr conn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuiConnection)
{
    Connection = conn;
    ui->setupUi(this);

    ui->Driver->clear();
    ui->Driver->addItem(tr("MariaDB/MySQL"), QVariant("QMYSQL"));

    connect(ui->Test, SIGNAL(clicked(bool)), this, SLOT(test(bool)));
    showData();
}
GuiConnection::~GuiConnection()
{
    delete ui;
}
