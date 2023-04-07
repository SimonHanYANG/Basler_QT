#pragma once
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QImage>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>


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

public:
	cv::VideoWriter cvVideoCreater;

// Functions
public:
	//=========================================================
	//=========================================================
	// Image Saving Function
	bool ImageSave(QImage m_raw_img);


//=========================================================
//=========================================================
// Save Image Variables
private:
	QString m_image_folder_name = "D:\\Simon_workspace\\Basler_QT\\imgtmp\\";
	QString m_video_folder_name = "D:\\Simon_workspace\\Basler_QT\\videotmp\\";
	QString m_image_file_suffix = ".jpg";
	


};
