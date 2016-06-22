#include "guicreatedatabase.h"
#include "ui_guicreatedatabase.h"

void GuiCreateDatabase::done(int r)
{
    if (!ui->Name->text().isEmpty())
    {
        if (!Connection->createDatabase(ui->Name->text(), ui->Collation->currentText()))
        {
            ui->Status->setText(Connection->lastError().databaseText());
            return;
        }
    }
    QDialog::done(r);
}
GuiCreateDatabase::GuiCreateDatabase(DBConnectionPtr conn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuiCreateDatabase)
{
    ui->setupUi(this);
    Connection = conn;
    QStringList colls;
    if (Connection->collations(colls))
        ui->Collation->addItems(colls);
    ui->Collation->setCurrentText(conn->Defaults["Collation"].toString());
}

GuiCreateDatabase::~GuiCreateDatabase()
{
    delete ui;
}
