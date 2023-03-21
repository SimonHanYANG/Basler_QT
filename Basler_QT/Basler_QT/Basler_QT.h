#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>

#include "ui_Basler_QT.h"

// Include files to use the pylon API.
#include "CameraControl.h"

class Basler_QT : public QMainWindow
{
    Q_OBJECT

public:
    Basler_QT(QWidget *parent = nullptr);
    ~Basler_QT();

	// Camera Control
	int EnumerateDevices();

	void ShowWarning(QString warningText);





private slots:
	// Slots for GuiCamera signals
	void OnNewGrabResult(int userHint);
	//void OnStateChanged(int userHint, bool isGrabbing);
	//void OnDeviceRemoved(int userHint);
	//void OnNodeUpdated(int userHint, GenApi::INode* pNode);

	// Slots for GUI signals
	void on_DiscoverCam_clicked();
	void on_OpenSelectedButton_clicked();
	void on_CloseCamButton_clicked();
	void on_SingleShotButton_clicked();
	void on_ContinuousShotButton_clicked();
	void on_StopButton_clicked();
	void on_ExposureTimeSlider_valueChanged(int value);
	// void on_CameraListComboBox_currentIndexChanged(int index);

private:
	Ui::Basler_QTClass ui;

	// all activate cameras
	Pylon::DeviceInfoList_t m_devices;
	// max camera number
	static const int MaxCamera = 2;
	// CameraControl 
	CameraControl m_camera[MaxCamera];


	
};
