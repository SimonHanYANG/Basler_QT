#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QDateTime>
#include <QThreadPool>

#include "ui_Basler_QT.h"

// Include files to use the pylon API.
#include "CameraControl.h"

// Include files to camera operation
#include "CameraOperation.h"

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

public:
	// all activate cameras
	Pylon::DeviceInfoList_t m_devices;
	// max camera number
	static const int MaxCamera = 2;
	// CameraControl all activate camera
	CameraControl m_camera[MaxCamera];

public:
	// image for all operations
	QImage m_raw_img;			// raw image data; QImage format: Format_RGB32 --> Get from Camera Grab Thread
	QImage m_display_img;		// display image on the screen 


protected:
	virtual void paintEvent(QPaintEvent *) override;

signals:
	void frameReady(QImage frame);

	// video start signal
	void startVideoRecordProcess();
	void stopVideoRecordProcess();


private slots:
	// Slots for GuiCamera signals
	void OnNewGrabResult(int userHint);
	//void OnStateChanged(int userHint, bool isGrabbing);
	void OnDeviceRemoved(int userHint);
	//void OnNodeUpdated(int userHint, GenApi::INode* pNode);

private slots:
	// Slots for GUI signals
	// Camera Control
	void on_DiscoverCam_clicked();
	void on_CloseCamButton_clicked();
	void on_SingleShotButton_clicked();
	void on_ContinuousShotButton_clicked();
	void on_StopButton_clicked();

private slots:
	// Camera Operation
	void on_SaveImgBtn_clicked();
	void on_RecordVideoStart_clicked();
	void on_RecordVideoStop_clicked();
	void on_ContactDetection_clicked();

private:
	void StartVideoRecordThread();

private:
	Ui::Basler_QTClass ui;

private:
	CameraOperation* m_cameraOperation;
	// image recording thread
	RecordImage* m_recordVideo_worker;

// Threads
private:
	QThread* m_record_video_thread;
	bool m_record_video_thread_is_start = false;
	
};
