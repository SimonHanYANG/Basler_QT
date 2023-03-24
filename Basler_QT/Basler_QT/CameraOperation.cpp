#include "CameraOperation.h"

CameraOperation::CameraOperation()
{

}

CameraOperation::~CameraOperation()
{
	
}

// TODO: Image save function
void CameraOperation::ImageSave()
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

	
	m_image_saved = true;

}

bool CameraOperation::ImageSavedSatate()
{
	return m_image_saved;
}


//=========================================================
//=========================================================
//The ProcessImageWorker class
ImageSaveWorker::ImageSaveWorker(void* pParameter)
{
	//Pass the pointer so the worker can access the variables in the mainWindow
	This = (CameraOperation*)pParameter;
}

ImageSaveWorker::~ImageSaveWorker()
{
	// 在结束线程之前，需要确保所有的资源已经被释放，以避免内存泄漏和其他问题的发生。
	// 例如，在 MyObject 的析构函数中，可以调用 QThread 的 quit() 函数来确保线程正常退出。
	QThread::currentThread()->quit(); // 退出线程事件循环
}

void ImageSaveWorker::ImageSave()
{
	This->ImageSave();
}

bool ImageSaveWorker::ImageSavedSatate()
{
	return This->ImageSavedSatate();
}