#include "Basler_QT.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	// ��ʼ�� Pylon ��
	Pylon::PylonInitialize();

    QApplication a(argc, argv);
    Basler_QT w;
    w.show();
    return a.exec();
}
