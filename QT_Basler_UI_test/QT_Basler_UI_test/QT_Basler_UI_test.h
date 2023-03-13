#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include "ui_QT_Basler_UI_test.h"

#define MAXCAMNUM 2

class QT_Basler_UI_test : public QMainWindow
{
    Q_OBJECT

public:
    QT_Basler_UI_test(QWidget *parent = nullptr);
    ~QT_Basler_UI_test();

private slots:
	void on_OpenSelectedButton_clicked();
	void on_CloseCamButton_clicked();
	void on_SingleShotButton_clicked();
	void on_ContinuousShotButton_clicked();
	void on_StopButton_clicked();
	void on_ExposureTimeSlider_valueChanged(int value);
	void on_CameraListComboBox_currentIndexChanged(int index);

private:
    Ui::QT_Basler_UI_testClass* ui;
};
