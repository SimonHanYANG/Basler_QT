#include "QT_Basler_UI_test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QT_Basler_UI_test w;
    w.show();
    return a.exec();
}
