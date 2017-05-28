#include <QtGui/QApplication>
#include "toolbox1.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    toolbox1 w;
    w.resize(300, 600);
    w.show();

    return a.exec();
}
