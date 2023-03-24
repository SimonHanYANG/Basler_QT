#pragma once
#include <QObject>
#include <QMutex>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QDateTime>

// QT UI
#include "ui_Basler_QT.h"

// Camera Control
#include "CameraControl.h"

// Opencv API
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Thread Worker Classes
class ImageSaveWorker;

class CameraOperation :
	public QObject
{
	Q_OBJECT

public:
	CameraOperation();
	virtual ~CameraOperation();

	//=========================================================
	//=========================================================
	// Image Saving Function
	void ImageSave();
	// Return image is saved successfully
	bool ImageSavedSatate();

//=========================================================
//=========================================================
// QT UI
private:
	Ui::Basler_QTClass ui;

//=========================================================
//=========================================================
// Camera Control variables
private:
	// all activate cameras
	Pylon::DeviceInfoList_t m_devices;
	// max camera number
	static const int MaxCamera = 2;
	// CameraControl all activate camera
	CameraControl m_camera[MaxCamera];

//=========================================================
//=========================================================
// Save Image Variables
private:
	QString m_image_folder_name = "D:\\Simon_workspace\\Basler_QT\\imgtmp";
	QString m_video_folder_name = "D:\\Simon_workspace\\Basler_QT\\videotmp";
	QString m_image_file_suffix = ".mp4";
	bool m_image_saved = false;


};

/************************************************************************/
/* Worker class for image saving                                        */
/************************************************************************/
class ImageSaveWorker : public QObject
{
	Q_OBJECT
public:
	ImageSaveWorker(void* pParameter);
	~ImageSaveWorker();

public slots:
	void ImageSave();
	bool ImageSavedSatate();

private:
	CameraOperation* This;
};