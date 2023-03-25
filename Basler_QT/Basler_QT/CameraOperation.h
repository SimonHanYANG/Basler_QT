#pragma once
#include <QObject>
#include <QMutex>
#include <QImage>
#include <QDebug>
#include <QThread>
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

// Image Result
public:
	QImage m_raw_img;			// raw image data; QImage format: Format_RGB32 --> Get from Camera Grab Thread
	cv::Mat m_copy_img;			// copied image for image processing

// Functions
public:
	//=========================================================
	//=========================================================
	// Image Saving Function
	void ImageSave();


//=========================================================
//=========================================================
// Save Image Variables
private:
	QString m_image_folder_name = "D:\\Simon_workspace\\Basler_QT\\imgtmp\\";
	QString m_video_folder_name = "D:\\Simon_workspace\\Basler_QT\\videotmp\\";
	QString m_image_file_suffix = ".jpg";
	


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

private:
	CameraOperation* This;
};