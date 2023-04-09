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
	QString m_image_file_suffix = ".jpg";
	
};


class RecordImage : public QObject
{
	Q_OBJECT

public:
	RecordImage::RecordImage(QObject *parent = nullptr)
		: QObject(parent), m_record_stop(false)
	{
		// 创建 OpenCV 视频编码器对象
		m_fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
		m_fps = 30;
	}

	virtual ~RecordImage()
	{
		// 关闭 OpenCV 视频编码器对象
		m_videoWriter.release();
	}

public:
	QImage m_raw_img;
	QImage m_copy_img;

public slots:
	void setFrame(QImage frame);
	void startRecording();
	void stopRecording();


private:
	bool m_record_stop;
	cv::VideoWriter m_videoWriter;
	int m_fourcc;
	double m_fps;

	QString m_video_folder_name = "D:\\Simon_workspace\\Basler_QT\\videotmp\\";
	QString m_video_file_suffix = ".avi";

private:
	void saveImage(QImage image);


};