#include "Basler_QT.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	// ≥ı ºªØ Pylon ø‚
	Pylon::PylonInitialize();

    QApplication a(argc, argv);
    Basler_QT w;
    w.show();
    return a.exec();
}
