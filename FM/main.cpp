#include "fmwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FMWidget w;
    w.show();
    return a.exec();
}
