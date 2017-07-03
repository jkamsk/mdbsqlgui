#include "usermng.h"
#include "ui_usermng.h"
#include <guiuser.h>
#include <QMessageBox>
#include <QInputDialog>

void UserMng::doAction(bool)
{
    QAction * act = (QAction*)sender();
    int actID = act->data().toInt();

    if (actID == ACT_USERMNG_RELOAD)
        reloadSlot();
    if (actID == ACT_COLLALL)
        ui->Users->collapseAll();
    if (actID == ACT_EXPDALL)
        ui->Users->expandAll();

    if (actID == ACT_ADD_USER)
    {
        GuiUser gu(Con, Ent, gutUser, this);
        if (gu.exec() == (int)QDialog::Accepted)
        {
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            Con->runSql("START TRANSACTION;", &qry, &err);
            if (!Con->runSql(QString("CREATE USER '%1'@'%2' IDENTIFIED BY '%3';").arg(gu.User).arg(gu.Host).arg(gu.Password), &qry, &err))
            {
                Con->runSql("ROLLBACK;", &qry, &err);
                QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                return;
            }

            if (gu.GrantType == gugtAll)
            {
                if (!Con->runSql(QString("GRANT ALL PRIVILEGES ON %3.* TO '%1'@'%2' WITH GRANT OPTION;").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }
            if (gu.GrantType == gugtNone)
            {

            }
            if (gu.GrantType == gugtReadOnly)
            {
                if (!Con->runSql(QString("GRANT SELECT ON %3.* TO '%1'@'%2';").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }

            Con->runSql("COMMIT;", &qry, &err);
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }
    if (actID == ACT_ADD_HOST || actID == ACT_CLONE_HOST)
    {
        GuiUser gu(Con, Ent, gutHost, this);
        gu.User = CurrentUser;
        if (actID == ACT_CLONE_HOST)
        {
            gu.Host = "";
            gu.hidePassword();
        }
        if (gu.exec() == (int)QDialog::Accepted)
        {
            if (actID == ACT_CLONE_HOST)
                gu.Password = userPassword(CurrentUser, CurrentHost);
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            Con->runSql("START TRANSACTION;", &qry, &err);
            QString pwdkw;
            if (actID == ACT_CLONE_HOST)
                pwdkw = "PASSWORD";
            if (!Con->runSql(QString("CREATE USER '%1'@'%2' IDENTIFIED BY %4 '%3';").arg(gu.User).arg(gu.Host).arg(gu.Password).arg(pwdkw), &qry, &err))
            {
                Con->runSql("ROLLBACK;", &qry, &err);
                QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                return;
            }

            if (gu.GrantType == gugtAll)
            {
                if (!Con->runSql(QString("GRANT ALL PRIVILEGES ON %3.* TO '%1'@'%2' WITH GRANT OPTION;").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }
            if (gu.GrantType == gugtNone)
            {

            }
            if (gu.GrantType == gugtReadOnly)
            {
                if (!Con->runSql(QString("GRANT SELECT ON %3.* TO '%1'@'%2';").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }

            Con->runSql("COMMIT;", &qry, &err);
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }
    if (actID == ACT_ADD_DB)
    {        
        GuiUser gu(Con, Ent, gutDatabase, this);

        if (gu.exec() == (int)QDialog::Accepted)
        {
            gu.User = CurrentUser;
            gu.Host = CurrentHost;
            gu.Password = userPassword(CurrentUser, CurrentHost);
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            Con->runSql("START TRANSACTION;", &qry, &err);

            if (gu.GrantType == gugtAll)
            {
                if (!Con->runSql(QString("GRANT ALL PRIVILEGES ON %3.* TO '%1'@'%2' WITH GRANT OPTION;").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }
            if (gu.GrantType == gugtNone)
            {

            }
            if (gu.GrantType == gugtReadOnly)
            {
                if (!Con->runSql(QString("GRANT SELECT ON %3.* TO '%1'@'%2';").arg(gu.User).arg(gu.Host).arg(gu.Database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Creating user failed"), err.databaseText());
                    return;
                }
            }

            Con->runSql("COMMIT;", &qry, &err);
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }
    if (actID == ACT_SET_PASWD)
    {
        GuiUser gu(Con, Ent, gutPassword, this);
        if (gu.exec() == (int)QDialog::Accepted)
        {
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            if (!Con->runSql(QString("SET PASSWORD FOR '%1'@'%2' = PASSWORD('%3');").arg(CurrentUser).arg(CurrentHost).arg(gu.Password), &qry, &err))
            {
                QMessageBox::critical(this, tr("Set Password failed"), err.databaseText());
                return;
            }
            QMessageBox::information(this, tr("Information"), tr("Password set successfully"));
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }
    if (actID == ACT_SET_GRANTS)
    {
        GuiUser gu(Con, Ent, gutGrants, this);        
        if (gu.exec() == (int)QDialog::Accepted)
        {
            QString database = CurrentDb;
            if (database.isEmpty())
                database = "*";
            if (gu.GrantType == gugtAll)
            {
                QSqlDatabase db = QSqlDatabase::database(Ent->connection());
                QSqlQuery qry(db);
                QSqlError err;
                Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
                Con->runSql("START TRANSACTION;", &qry, &err);
                if (!Con->runSql(QString("GRANT ALL PRIVILEGES ON %3.* TO '%1'@'%2' WITH GRANT OPTION;").arg(CurrentUser).arg(CurrentHost).arg(database), &qry, &err))
                {
                    Con->runSql("ROLLBACK;", &qry, &err);
                    QMessageBox::critical(this, tr("Aplying grants failed"), err.databaseText());
                    return;
                }
                Con->runSql("COMMIT;", &qry, &err);
                Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            }
            if (gu.GrantType == gugtNone)
            {
                QJsonObject obj;
                obj["SELECT"] = false;
                obj["INSERT"] = false;
                obj["UPDATE"] = false;
                obj["DELETE"] = false;
                obj["EXECUTE"] = false;
                obj["SHOW_VIEW"] = false;
                obj["DROP"] = false;
                obj["CREATE"] = false;
                obj["CREATE_VIEW"] = false;
                obj["CREATE_ROUTINE"] = false;
                obj["ALTER"] = false;
                obj["ALTER_ROUTINE"] = false;
                obj["INDEX"] = false;
                obj["REFERENCES"] = false;
                obj["CREATE_TEMPORARY_TABLES"] = false;
                obj["GRANT_OPTION"] = false;
                obj["LOCK_TABLES"] = false;
                obj["EVENT"] = false;
                if (CurrentType != "db")
                {
                    obj["CREATE_USER"] = false;
                    obj["FILE"] = false;
                    obj["PROCESS"] = false;
                    obj["SHOW_DATABASES"] = false;
                    obj["SHUTDOWN"] = false;
                }

                if (!applyDbGrants(CurrentUser, CurrentHost, CurrentDb, obj))
                {
                    QMessageBox::critical(this, tr("Error"), "Canot preset privileges !");
                    return;
                }
            }
            if (gu.GrantType == gugtReadOnly)
            {
                QJsonObject obj;
                obj["SELECT"] = true;
                obj["INSERT"] = false;
                obj["UPDATE"] = false;
                obj["DELETE"] = false;
                obj["EXECUTE"] = false;
                obj["SHOW_VIEW"] = false;
                obj["DROP"] = false;
                obj["CREATE"] = false;
                obj["CREATE_VIEW"] = false;
                obj["CREATE_ROUTINE"] = false;
                obj["ALTER"] = false;
                obj["ALTER_ROUTINE"] = false;
                obj["INDEX"] = false;
                obj["REFERENCES"] = false;
                obj["CREATE_TEMPORARY_TABLES"] = false;
                obj["GRANT_OPTION"] = false;
                obj["LOCK_TABLES"] = false;
                obj["EVENT"] = false;
                if (CurrentType != "db")
                {
                    obj["CREATE_USER"] = false;
                    obj["FILE"] = false;
                    obj["PROCESS"] = false;
                    obj["SHOW_DATABASES"] = false;
                    obj["SHUTDOWN"] = false;
                }

                if (!applyDbGrants(CurrentUser, CurrentHost, CurrentDb, obj))
                {
                    QMessageBox::critical(this, tr("Error"), "Canot preset privileges !");
                    return;
                }
            }
            reloadSlot();
        }
    }

    if (actID == ACT_DEL_USER)
    {
        int rc = QMessageBox::question(this, tr("Drop user confirmation"), tr("Do you really want to drop '%1' user").arg(CurrentUser), QMessageBox::Yes, QMessageBox::No);
        if (rc == (int)QMessageBox::Yes)
        {
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            Con->runSql("START TRANSACTION;", &qry, &err);
            bool ok = true;
            QStringList user_hosts = userHosts(CurrentUser);
            QString host;
            for (int i=0;i<user_hosts.count();i++)
            {
                host = user_hosts.at(i);
                if (!Con->runSql(QString("DROP USER '%1'@'%2';").arg(CurrentUser).arg(host), &qry, &err))
                {
                    ok = false;
                    break;
                }
            }
            if (!ok)
            {
                Con->runSql("ROLLBACK;", &qry, &err);
                QMessageBox::critical(this, tr("Drop user failed"), err.databaseText());
                return;
            }
            Con->runSql("COMMIT;", &qry, &err);
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }

    if (actID == ACT_DEL_HOST)
    {
        int rc = QMessageBox::question(this, tr("Drop host confirmation"), tr("Do you really want to drop '%1'@'%2' user host").arg(CurrentUser).arg(CurrentHost), QMessageBox::Yes, QMessageBox::No);
        if (rc == (int)QMessageBox::Yes)
        {
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            if (!Con->runSql(QString("DROP USER '%1'@'%2';").arg(CurrentUser).arg(CurrentHost), &qry, &err))
            {
                QMessageBox::critical(this, tr("Drop user host failed"), err.databaseText());
                return;
            }
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }

    if (actID == ACT_DEL_DB)
    {
        int rc = QMessageBox::question(this, tr("Drop database grants confirmation"), tr("Do you really want to drop '%1'@'%2'/%3 database grants").arg(CurrentUser).arg(CurrentHost).arg(CurrentDb), QMessageBox::Yes, QMessageBox::No);
        if (rc == (int)QMessageBox::Yes)
        {
            QSqlDatabase db = QSqlDatabase::database(Ent->connection());
            QSqlQuery qry(db);
            QSqlError err;
            Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
            if (!Con->runSql(QString("DELETE FROM db WHERE User='%1' AND Host='%2' AND Db='%3';").arg(CurrentUser).arg(CurrentHost).arg(CurrentDb), &qry, &err))
            {
                QMessageBox::critical(this, tr("Drop database grants failed"), err.databaseText());
                return;
            }
            Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
            reloadSlot();
        }
    }

    if (actID == ACT_RENAME_USER)
    {
        QString oldName = CurrentUser;
        QString newName = QInputDialog::getText(this, tr("Input"), tr("New User Name"), QLineEdit::Normal, oldName);
        if (oldName == newName || newName.isEmpty())
            return;
        int rc = QMessageBox::question(this, tr("Rename User confirmation"), tr("Do you really want rename %1 to %3").arg(oldName).arg(newName), QMessageBox::Yes, QMessageBox::No);
        if (rc != (int)QMessageBox::Yes)
            return;
        QSqlDatabase db = QSqlDatabase::database(Ent->connection());
        QSqlQuery qry(db);
        QSqlError err;
        Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
        if (!Con->runSql(QString("RENAME USER '%1'@'%2' TO '%3'@'%2';").arg(oldName).arg(CurrentHost).arg(newName), &qry, &err))
        {
            QMessageBox::critical(this, tr("Renaming user %1 failed").arg(oldName), err.databaseText());
            return;
        }
        Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
        reloadSlot();
    }
}
void UserMng::applyGrants()
{
    int rc = QMessageBox::question(this, tr("Apply grants confirmation"), tr("Do you really want to set this grant options ?"), QMessageBox::Yes, QMessageBox::No);
    if (rc != (int)QMessageBox::Yes)
        return;

    QJsonObject jCh = CurrentRigs[".change"].toObject();

    applyDbGrants(CurrentUser, CurrentHost, CurrentDb, jCh);

    reloadSlot();
}
bool UserMng::removeAllDbGrants(QString cUser, QString cHost)
{
    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlError err;
    Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
    bool ret = Con->runSql(QString("REVOKE ALL PRIVILEGES, GRANT OPTION FROM '%1'@'%2';").arg(cUser).arg(cHost), &qry, &err);
    Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
    return ret;
}
void UserMng::renameExpanded(QString search, QString newText)
{
    QString ipath;
    QString newPath;
    for (int i=0;i<ExpandedPaths.count();i++)
    {
        ipath = ExpandedPaths.at(i);
        if (ipath.contains(search))
        {
            newPath = ipath;
            newPath = newPath.replace(search, newText);
            ExpandedPaths.replace(i, newPath);
        }
    }
}
bool UserMng::applyDbGrants(QString cUser, QString cHost, QString cDb, QJsonObject grants)
{
    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlError err;
    Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);

    QString grant;
    QString revoke;
    QJsonObject jCh = grants;
    QStringList keys = jCh.keys();
    QString key;
    bool revoke_grant = false;
    bool val;
    for (int i=0;i<keys.count();i++)
    {
        key = keys.at(i);
        val = jCh[key].toBool();
        if (!val)
            if (key == "GRANT_OPTION")
            {
                revoke_grant = true;
                continue;
            }
        if (key.contains("_"))
            key = key.replace("_", " ");
        if (val)
        {
            if (!grant.isEmpty())
                grant += ",";
            grant += key;
        }
        else
        {
            if (!revoke.isEmpty())
                revoke += ",";
            revoke += key;
        }
    }

    QString database = cDb;
    if (database.isEmpty())
        database = "*";

    QString sqlGrant = QString("GRANT %1 ON %2.* TO '%3'@'%4';").arg(grant).arg(database).arg(cUser).arg(cHost);
    QString sqlRevoke = QString("REVOKE %1 ON %2.* FROM '%3'@'%4';").arg(revoke).arg(database).arg(cUser).arg(cHost);
    QString sqlRevokeGrant = QString("UPDATE user set Grant_priv='N' WHERE User='%1' AND host='%2';").arg(cUser).arg(cHost);
    if (database != "*")
        sqlRevokeGrant = QString("UPDATE db set Grant_priv='N' WHERE User='%1' AND Host='%2' AND Db='%3';").arg(cUser).arg(cHost).arg(cDb);

    Con->runSql("START TRNASACTION;", &qry, &err);
    if (!revoke.isEmpty())
        if (!Con->runSql(sqlRevoke, &qry, &err))
        {
            QMessageBox::critical(this, tr("Revokation failed"), err.databaseText());
            Con->runSql("ROLLBACK;", &qry, &err);
            return false;
        }
    if (revoke_grant)
        if (!Con->runSql(sqlRevokeGrant, &qry, &err))
        {
            QMessageBox::critical(this, tr("Revokation GRAND OPTION failed"), err.databaseText());
            Con->runSql("ROLLBACK;", &qry, &err);
            return false;
        }

    if (!grant.isEmpty())
        if (!Con->runSql(sqlGrant, &qry, &err))
        {
            QMessageBox::critical(this, tr("Granting failed"), err.databaseText());
            Con->runSql("ROLLBACK;", &qry, &err);
            return false;
        }
    Con->runSql("COMMIT;", &qry, &err);
    Con->runSql("FLUSH PRIVILEGES;", &qry, &err);
    return true;
}
void UserMng::itemActivatedSlot(QTreeWidgetItem* wi,int /*col*/)
{
    QJsonObject rights;
    bool applicable = false;
    QString type= "";
    if (wi)
    {
        CurrentPath = wi->data(0, ROLE_PATH).toString();
        SelectedPath = CurrentPath;
        QString guimsg = CurrentPath;
        ui->Msg->setText(guimsg.replace(";","/"));
        CurrentType = pathType(CurrentPath);
        CurrentUser = user(CurrentPath);
        CurrentHost = host(CurrentPath);
        CurrentDb = db(CurrentPath);
        if (CurrentType == "host" || CurrentType == "db")
        {
            rights = wi->data(0, ROLE_RIGS).toJsonObject();
            applicable = true;
            CurrentRigs = rights;
        }
        QTreeWidgetItem * pwi = wi->parent();
        if (pwi)
            ParentPath = pwi->data(0, ROLE_PATH).toString();
    }
    SetGrantsInProgress = true;
    QList<QCheckBox *> boxs = this->findChildren<QCheckBox *>();
    QCheckBox * box;
    QString name;
    for (int i=0;i<boxs.count();i++)
    {
        box = boxs.at(i);
        name = box->objectName();
        if (applicable)
        {
            box->setChecked(rights[name].toBool());
            box->setEnabled(true);
        }
        else
        {
            box->setChecked(false);
            box->setEnabled(false);
        }
    }

    if (applicable)
    {
        bool isDb = (CurrentType == "db");
        ui->CREATE_USER->setEnabled(!isDb);
        ui->SHUTDOWN->setEnabled(!isDb);
        ui->PROCESS->setEnabled(!isDb);
        ui->FILE->setEnabled(!isDb);
        ui->SHOW_DATABASES->setEnabled(!isDb);
    }
    ui->Apply->setDisabled(true);
    SetGrantsInProgress = false;
//    ui->Objects->setCheckState(Qt::PartiallyChecked);
//    ui->Ddl->setCheckState(Qt::PartiallyChecked);
//    ui->Global->setCheckState(Qt::PartiallyChecked);
}
void UserMng::itemExpandedSlot(QTreeWidgetItem * item)
{
    QString itemPath = item->data(0, ROLE_PATH).toString();
    if (ExpandedPaths.contains(itemPath))
        return;
    ExpandedPaths.append(itemPath);
}
void UserMng::itemColapsedSlot(QTreeWidgetItem * item)
{
    QString itemPath = item->data(0, ROLE_PATH).toString();
    if (ExpandedPaths.contains(itemPath))
        ExpandedPaths.removeOne(itemPath);
}
void UserMng::showUsers()
{
    ui->Users->clear();
    QStringList users = UsersJson.keys();
    QString u;
    QJsonObject jUser;
    QJsonObject jHost;
    QJsonObject jDbs;
    QJsonObject jDb;
    QString ipath;
    QList<QTreeWidgetItem *> ToExpand;
    QTreeWidgetItem * selected = 0L;
    QTreeWidgetItem * selected_parent = 0L;
    for (int i=0;i<users.count();i++)
    {
        u = users.at(i);
        jUser = UsersJson[u].toObject();
        QTreeWidgetItem * w_user = new QTreeWidgetItem(ui->Users);
        w_user->setText(0, u);
        w_user->setIcon(0, QIcon(":/icons/user.png"));
        w_user->setData(0, ROLE_TYPE, "user");
        w_user->setData(0, ROLE_USER, u);
        ipath = path("user", u);
        if (ipath == SelectedPath || ExpandedPaths.contains(ipath))
            ToExpand.append(w_user);
        if (ipath == SelectedPath)
            selected = w_user;
        if (ipath == ParentPath)
            selected_parent = w_user;
        w_user->setData(0, ROLE_PATH, ipath);
        ui->Users->addTopLevelItem(w_user);

        QStringList hosts = jUser.keys();
        QString h;
        for (int j=0;j<hosts.count();j++)
        {
            h = hosts.at(j);
            jHost = jUser[h].toObject();
            QTreeWidgetItem * w_host = new QTreeWidgetItem(w_user);
            w_host->setText(0, h);
            w_host->setIcon(0, QIcon(":/icons/server-on.png"));
            w_host->setData(0, ROLE_TYPE, "host");
            w_host->setData(0, ROLE_USER, u);
            w_host->setData(0, ROLE_HOST, h);
            ipath = path("host", u, h);
            if (ipath == SelectedPath || ExpandedPaths.contains(ipath))
                ToExpand.append(w_host);
            if (ipath == SelectedPath)
                selected = w_host;
            if (ipath == ParentPath)
                selected_parent = w_host;
            w_host->setData(0, ROLE_PATH, ipath);
            w_host->setData(0, ROLE_RIGS, QVariant(jHost[".default"].toObject()));
            w_user->addChild(w_host);

            if (jHost.contains("Databases"))
            {
                jDbs = jHost["Databases"].toObject();
                QStringList dbs = jDbs.keys();
                QString db;
                for (int k=0;k<dbs.count();k++)
                {
                    db = dbs.at(k);
                    jDb = jDbs[db].toObject();
                    QTreeWidgetItem * w_db = new QTreeWidgetItem(w_host);
                    w_db->setText(0, db);
                    w_db->setIcon(0, QIcon(":/icons/database.png"));
                    w_db->setData(0, ROLE_TYPE, "db");
                    w_db->setData(0, ROLE_USER, u);
                    w_db->setData(0, ROLE_HOST, h);
                    w_db->setData(0, ROLE_DB, db);
                    ipath = path("db", u, h, db);
                    if (ipath == SelectedPath || ExpandedPaths.contains(ipath))
                        ToExpand.append(w_db);
                    if (ipath == SelectedPath)
                        selected = w_db;
                    if (ipath == ParentPath)
                        selected_parent = w_db;
                    w_db->setData(0, ROLE_PATH, ipath);
                    w_db->setData(0, ROLE_RIGS, QVariant(jDb));
                    w_host->addChild(w_db);
                }
            }
        }
    }
    QTreeWidgetItem * ew;
    for (int i=0;i<ToExpand.count();i++)
    {
        ew = ToExpand.at(i);
        ui->Users->expandItem(ew);
    }
    if (selected)
    {
        ui->Users->setCurrentItem(selected);
        itemActivatedSlot(selected, 0);
    }
    else if (selected_parent)
    {
        ui->Users->setCurrentItem(selected_parent);
        itemActivatedSlot(selected_parent, 0);
    }
}
void UserMng::readUsersFromDb()
{
    QVariant val;
    QString sval;
    QSqlDatabase db = QSqlDatabase::database(Ent->connection());
    QSqlQuery qry(db);
    QSqlQuery qry2(db);
    QSqlError err;
    if ((int)Ent->type() >= (int)dbeConnection)
    {
        Con->runSql(QString("USE %1;").arg("mysql"), &qry, &err);
        Con->runSql(QString("SELECT * FROM user;"), &qry, &err);
        Con->runSql(QString("SELECT * FROM db;"), &qry2, &err);
    }
    else
        return;
    //FillJson
    UsersJson = QJsonObject();
    QJsonObject jUsers;
    while (qry.next())
    {
        QJsonObject jUser;
        QString user = qry.value("User").toString().trimmed();
        if (jUsers.contains(user))
            jUser = jUsers[user].toObject();
        else        
            jUser = QJsonObject();                

        QJsonObject jHost;
        QString host = qry.value("Host").toString().trimmed();
        if (jUser.contains(host))
            jHost = jUser[host].toObject();
        else        
            jHost = QJsonObject();

        QJsonObject defRights;

        for (int i=0;i<qry.record().count();i++)
        {
            QString key = qry.record().fieldName(i);
            if (key == "User" || key == "Host")
                continue;

            val = qry.value(key);
            sval = val.toString().trimmed();
            if (key == "Password")
            {
                jHost[key] = sval;
                continue;
            }
            if (sval == "Y")
                defRights[GrantObjMap.value(key)] = true;
            else if (val == "N")
                defRights[GrantObjMap.value(key)] = false;
            else
                defRights[GrantObjMap.value(key)] = val.toString();
        }
        jHost[".default"] = defRights;

        jUser[host] = jHost;
        jUsers[user] = jUser;
    }

    while (qry2.next())
    {
        QJsonObject jUser;
        QString user = qry2.value("User").toString().trimmed();
        if (jUsers.contains(user))
            jUser = jUsers[user].toObject();
        else
            jUser = QJsonObject();

        QJsonObject jHost;
        QString host = qry2.value("Host").toString().trimmed();
        if (jUser.contains(host))
            jHost = jUser[host].toObject();
        else
            jHost = QJsonObject();

        QJsonObject jDbs;
        if (jHost.contains("Databases"))
            jDbs = jHost["Databases"].toObject();
        else
            jDbs = QJsonObject();

        QJsonObject jDb;
        QString db = qry2.value("Db").toString().trimmed();
        if (jDbs.contains(db))
            jDb = jDbs[db].toObject();
        else
            jDb = QJsonObject();

        for (int i=0;i<qry2.record().count();i++)
        {
            QString key = qry2.record().fieldName(i);
            if (key == "User" || key == "Host" || key == "Db")
                continue;

            val = qry2.value(key);
            sval = val.toString().trimmed();
            if (sval == "Y")
                jDb[GrantObjMap.value(key)] = true;
            else if (val == "N")
                jDb[GrantObjMap.value(key)] = false;
            else
                jDb[GrantObjMap.value(key)] = val.toString();
        }
        jDbs[db] = jDb;
        jHost["Databases"] = jDbs;
        jUser[host] = jHost;
        jUsers[user] = jUser;
    }
    UsersJson = jUsers;
}
void UserMng::customContextMenuRequestedSlot(const QPoint& pos)
{
    QTreeWidgetItem* item = ui->Users->itemAt(pos);
    itemActivatedSlot(item, 0);
    showDbTreeContextMenu(item, ui->Users->viewport()->mapToGlobal(pos));
}
QString UserMng::path(QString type, QString user, QString host, QString db)
{
    return QString("%1;%2;%3;%4").arg(type).arg(user).arg(host).arg(db);
}
QString UserMng::pathType(QString path)
{
    QStringList sl = path.split(";");
    if (sl.count() > 0)
        return sl.at(0);
    return "";
}
QString UserMng::user(QString path)
{
    QStringList sl = path.split(";");
    if (sl.count() > 1)
        return sl.at(1);
    return "";
}
QString UserMng::host(QString path)
{
    QStringList sl = path.split(";");
    if (sl.count() > 2)
        return sl.at(2);
    return "";
}
QString UserMng::db(QString path)
{
    QStringList sl = path.split(";");
    if (sl.count() > 3)
        return sl.at(3);
    return "";
}
QString UserMng::userPassword(QString user, QString host)
{
    QString ret;
    if (UsersJson.contains(user))
    {
        QJsonObject juser = UsersJson[user].toObject();
        ret = juser[host].toObject()["Password"].toString();
    }
    return ret;
}
QStringList UserMng::userHosts(QString user)
{
    QStringList ret;
    if (UsersJson.contains(user))
    {
        QJsonObject juser = UsersJson[user].toObject();
        ret = juser.keys();
    }
    return ret;
}
void UserMng::showDbTreeContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
    QMenu Menu;
    QAction * act;
    act = Menu.addAction(tr("New User"), this, SLOT(doAction(bool)));
    act->setData(ACT_ADD_USER);
    act->setIcon(QIcon(":/icons/user.png"));
    Menu.addSeparator();

    act = Menu.addAction(tr("Expand All"), this, SLOT(doAction(bool)));
    act->setData(ACT_EXPDALL);
    act->setIcon(QIcon(":/icons/expand.png"));

    act = Menu.addAction(tr("Collapse All"), this, SLOT(doAction(bool)));
    act->setData(ACT_COLLALL);
    act->setIcon(QIcon(":/icons/collapse.png"));

    Menu.addSeparator();

    if (item != 0L)
    {
        QString path = item->data(0, ROLE_PATH).toString();
        QString type = pathType(path);
        if (type == "user")
        {
            act = Menu.addAction(tr("New Host"), this, SLOT(doAction(bool)));
            act->setData(ACT_ADD_HOST);
            act->setIcon(QIcon(":/icons/add.png"));            

            Menu.addSeparator();           

            act = Menu.addAction(tr("Remove User"), this, SLOT(doAction(bool)));
            act->setData(ACT_DEL_USER);
            act->setIcon(QIcon(":/icons/remove.png"));
        }
        else if (type == "host")
        {
            act = Menu.addAction(tr("Preset Host grants"), this, SLOT(doAction(bool)));
            act->setData(ACT_SET_GRANTS);
            act->setIcon(QIcon(":/icons/exec.png"));

            act = Menu.addAction(tr("Password"), this, SLOT(doAction(bool)));
            act->setData(ACT_SET_PASWD);
            act->setIcon(QIcon(":/icons/pwd.png"));

            act = Menu.addAction(tr("New DB grants"), this, SLOT(doAction(bool)));
            act->setData(ACT_ADD_DB);
            act->setIcon(QIcon(":/icons/add.png"));

            act = Menu.addAction(tr("Clone"), this, SLOT(doAction(bool)));
            act->setData(ACT_CLONE_HOST);
            act->setIcon(QIcon(":/icons/clone.png"));

            Menu.addSeparator();

            act = Menu.addAction(tr("Rename User"), this, SLOT(doAction(bool)));
            act->setData(ACT_RENAME_USER);
            act->setIcon(QIcon(":/icons/refresh.png"));

            act = Menu.addAction(tr("Remove Host"), this, SLOT(doAction(bool)));
            act->setData(ACT_DEL_HOST);
            act->setIcon(QIcon(":/icons/remove.png"));
        }
        else if (type == "db")
        {
            act = Menu.addAction(tr("Preset DB grants"), this, SLOT(doAction(bool)));
            act->setData(ACT_SET_GRANTS);
            act->setIcon(QIcon(":/icons/exec.png"));

            Menu.addSeparator();

            act = Menu.addAction(tr("Remove DB grants"), this, SLOT(doAction(bool)));
            act->setData(ACT_DEL_DB);
            act->setIcon(QIcon(":/icons/remove.png"));
        }
    }

    Menu.addSeparator();

    act = Menu.addAction(tr("Reload"), this, SLOT(doAction(bool)));
    act->setData(ACT_USERMNG_RELOAD);
    act->setIcon(QIcon(":/icons/refresh.png"));

    Menu.exec(globalPos);
}
void UserMng::reloadSlot()
{
    readUsersFromDb();
    showUsers();    
    CurrentDb = "";
    CurrentHost = "";
    CurrentType = "";
    CurrentUser = "";
}
void UserMng::fillMaps()
{
    GrantObjMap.clear();
    ObjGrantMap.clear();

    GrantObjMap.insert("Alter_priv", "ALTER");
    GrantObjMap.insert("Alter_routine_priv", "ALTER_ROUTINE");
    GrantObjMap.insert("Create_priv", "CREATE");
    GrantObjMap.insert("Create_routine_priv","CREATE_ROUTINE");
    GrantObjMap.insert("Create_tablespace_priv","CREATE_TABLESPACE");
    GrantObjMap.insert("Create_tmp_table_priv","CREATE_TEMPORARY_TABLES");
    GrantObjMap.insert("Create_user_priv","CREATE_USER");
    GrantObjMap.insert("Create_view_priv","CREATE_VIEW");
    GrantObjMap.insert("Delete_priv","DELETE");
    GrantObjMap.insert("Drop_priv","DROP");
    GrantObjMap.insert("Event_priv","EVENT");
    GrantObjMap.insert("Execute_priv","EXECUTE");
    GrantObjMap.insert("File_priv","FILE");
    GrantObjMap.insert("Grant_priv","GRANT_OPTION");
    GrantObjMap.insert("Index_priv","INDEX");
    GrantObjMap.insert("Insert_priv","INSERT");
    GrantObjMap.insert("Lock_tables_priv","LOCK_TABLES");
    GrantObjMap.insert("Process_priv","PROCESS");
    GrantObjMap.insert("References_priv","REFERENCES");
    GrantObjMap.insert("Reload_priv","RELOAD");
    GrantObjMap.insert("Repl_client_priv","REPLICATION_CLIENT");
    GrantObjMap.insert("Repl_slave_priv","REPLICATION_SLAVE");
    GrantObjMap.insert("Select_priv","SELECT");
    GrantObjMap.insert("Show_db_priv","SHOW_DATABASES");
    GrantObjMap.insert("Show_view_priv","SHOW_VIEW");
    GrantObjMap.insert("Shutdown_priv","SHUTDOWN");
    GrantObjMap.insert("Super_priv","SUPER");
    GrantObjMap.insert("Trigger_priv","TRIGGER");
    GrantObjMap.insert("Update_priv","UPDATE");
}
void UserMng::grantClickedSlot(bool val)
{
    if (SetGrantsInProgress)
        return;
    if (CurrentRigs.isEmpty())
        return;
    QCheckBox * box = (QCheckBox*)sender();
    QString name = box->objectName();
    if (!CurrentRigs.contains(name))
        return;
    bool cur_val = CurrentRigs[name].toBool();
    bool changed = (cur_val != val);

    QJsonObject jChange;
    if (CurrentRigs.contains(".change"))
        jChange = CurrentRigs[".change"].toObject();
    if (changed)
        jChange[name] = val;
    else
        jChange.remove(name);

    if (!jChange.isEmpty())
        CurrentRigs[".change"] = jChange;
    else
        CurrentRigs.remove(".change");

    ui->Apply->setEnabled(CurrentRigs.contains(".change"));
}
UserMng::UserMng(DBConnectionPtr con, DbEntityPtr ent, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserMng)
{
    SetGrantsInProgress = false;
    Con = con;
    Ent = ent;
    fillMaps();
    ui->setupUi(this);
    QList<int> sizes;
    sizes.append(250);
    sizes.append(750);
    ui->splitter->setSizes(sizes);
    ui->Users->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->Users, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(itemActivatedSlot(QTreeWidgetItem*,int)));
    connect(ui->Reload, SIGNAL(released()),
            this, SLOT(reloadSlot()));
    connect(ui->Users, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customContextMenuRequestedSlot(QPoint)));
    connect(ui->Users, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(itemColapsedSlot(QTreeWidgetItem*)));
    connect(ui->Users, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(itemExpandedSlot(QTreeWidgetItem*)));
    connect(ui->Apply, SIGNAL(released()),
            this, SLOT(applyGrants()));
    QList<QCheckBox *> boxs = this->findChildren<QCheckBox *>();
    QCheckBox * box;
    for (int i=0;i<boxs.count();i++)
    {
        box = boxs.at(i);
        connect(box, SIGNAL(toggled(bool)), this, SLOT(grantClickedSlot(bool)));
    }
    reloadSlot();
    itemActivatedSlot(0L, 0);

    ui->Msg->setHidden(true);
    ui->Reload->setHidden(true);
}

UserMng::~UserMng()
{
    delete ui;
}
