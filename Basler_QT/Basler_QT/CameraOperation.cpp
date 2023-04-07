#include "CameraOperation.h"

CameraOperation::CameraOperation()
{

}

CameraOperation::~CameraOperation()
{
	
}

// TODO: Image save function
bool CameraOperation::ImageSave(QImage m_raw_img)
{
	qDebug() << "Save Image Button Clicked!\n";

	// set image saved name
	QString image_file_name;
	QString image_saved_path;

	// 获取当前日期和时间
	QDateTime currentDateTime = QDateTime::currentDateTime();

	// 获取当前日期
	QDate currentDate = currentDateTime.date();
	// qDebug() << "Current date: " << currentDate.toString("yyyy-MM-dd");

	// 获取当前时间
	QTime currentTime = currentDateTime.time();
	// qDebug() << "Current time: " << currentTime.toString("hh_mm_ss");

	// image_file_name: yyyy-MM-dd_hh:mm:ss
	image_file_name = currentDate.toString("yyyy-MM-dd") + "_" + currentTime.toString("hh_mm_ss");
	qDebug() << "Image file name is: " << image_file_name << "!\n";

	// image_saved_path: yyyy-MM-dd_hh_mm_ss.mp4 
	image_saved_path = m_image_folder_name + image_file_name + m_image_file_suffix;
	qDebug() << "Image file saved path is: " << image_saved_path << "!\n";

	// Check the raw image
	if (m_raw_img.isNull())
	{
		qDebug() << "m_raw_image is NULL!\n";
	}
	else
	{
		qDebug() << "m_raw_image is Successed!\n";
	}

	// Save image as JPG format
	if (m_raw_img.save(image_saved_path))
	{
		qDebug() << "Image saved to file: " << image_saved_path << "\n";
		return true;
	}
	else
	{
		qDebug() << "Failed to save image to file.\n";
		return false;
	}
}

