#include "mdbsqlgui.h"
#include <QApplication>
#include <settings.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    MdbSqlGui w;
    w.show();
    w.placeWindow();
    return a.exec();
}
