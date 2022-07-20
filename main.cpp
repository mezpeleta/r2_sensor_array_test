#include "sensorarraytest.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SensorArrayTest w;
    w.show();
    return a.exec();
}
