#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QDateTime>

#include "ui_Basler_QT.h"

// Include files to use the pylon API.
#include "CameraControl.h"

// opencv API
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;


class Basler_QT : public QMainWindow
{
    Q_OBJECT

public:
    Basler_QT(QWidget *parent = nullptr);
    ~Basler_QT();

	// Camera Control
	int EnumerateDevices();

	void ShowWarning(QString warningText);

	// open camera function
	bool InternalOpenCamera(const Pylon::CDeviceInfo& devInfo, int cameraId);

	// close camera function
	void InternalCloseCamera(int cameraId);


protected:
	virtual void paintEvent(QPaintEvent *) override;



private slots:
	// Slots for GuiCamera signals
	void OnNewGrabResult(int userHint);
	//void OnStateChanged(int userHint, bool isGrabbing);
	//void OnDeviceRemoved(int userHint);
	//void OnNodeUpdated(int userHint, GenApi::INode* pNode);

	// Slots for GUI signals
	void on_DiscoverCam_clicked();
	void on_CloseCamButton_clicked();
	void on_SingleShotButton_clicked();
	void on_ContinuousShotButton_clicked();
	void on_StopButton_clicked();

private slots:
	// Camera Operations
	void on_SaveImgBtn_clicked();

private:
	Ui::Basler_QTClass ui;

	// all activate cameras
	Pylon::DeviceInfoList_t m_devices;
	// max camera number
	static const int MaxCamera = 2;
	// CameraControl all activate camera
	CameraControl m_camera[MaxCamera];

private:
	QString m_image_folder_name = "D:\\Simon_workspace\\Basler_QT\\imgtmp";
	QString m_video_folder_name = "D:\\Simon_workspace\\Basler_QT\\videotmp";
	QString m_image_file_suffix = ".mp4";
	
};
