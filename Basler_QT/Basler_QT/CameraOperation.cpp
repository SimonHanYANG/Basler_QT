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

	// ��ȡ��ǰ���ں�ʱ��
	QDateTime currentDateTime = QDateTime::currentDateTime();

	// ��ȡ��ǰ����
	QDate currentDate = currentDateTime.date();
	// qDebug() << "Current date: " << currentDate.toString("yyyy-MM-dd");

	// ��ȡ��ǰʱ��
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
	// �ڽ����߳�֮ǰ����Ҫȷ�����е���Դ�Ѿ����ͷţ��Ա����ڴ�й©����������ķ�����
	// ���磬�� MyObject �����������У����Ե��� QThread �� quit() ������ȷ���߳������˳���
	QThread::currentThread()->quit(); // �˳��߳��¼�ѭ��
}

void ImageSaveWorker::ImageSave()
{
	This->ImageSave();
}

bool ImageSaveWorker::ImageSavedSatate()
{
	return This->ImageSavedSatate();
}