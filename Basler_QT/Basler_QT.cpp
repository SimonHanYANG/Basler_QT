#include "Basler_QT.h"

Basler_QT::Basler_QT(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	// Camera Control
	m_camera = new BalserCameraControl(this);
	m_camera->initSome();

}

Basler_QT::~Basler_QT()
{}
