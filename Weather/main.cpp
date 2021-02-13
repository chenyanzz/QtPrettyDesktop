#include "Weather.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Weather w;
    w.show();
    return a.exec();
}
