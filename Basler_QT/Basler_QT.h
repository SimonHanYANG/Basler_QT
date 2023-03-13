#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Basler_QT.h"

// Basler camera control
#include "BalserCameraControl.h"

class Basler_QT : public QMainWindow
{
    Q_OBJECT

public:
    Basler_QT(QWidget *parent = nullptr);
    ~Basler_QT();

private:
    Ui::Basler_QTClass ui;
	BalserCameraControl* m_camera;
};
