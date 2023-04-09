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


//====================================================================================
//====================================================================================
//====================================================================================
// RecordImage Thread

void RecordImage::setFrame(QImage frame)
{
	m_raw_img = frame;

	if (m_raw_img.isNull())
	{
		qDebug() << "setFrame m_raw_image is NULL!\n";
	}
	else
	{
		qDebug() << "setFrame m_raw_image is Succeed!\n";
	}

}

// TODO: Start Recording
void RecordImage::startRecording()
{
	qDebug() << "Recording Video Thread Started!\n";

	m_record_stop = false;

	if (m_raw_img.isNull())
	{
		qDebug() << "startRecording m_raw_image is NULL!\n";
	}

	// ����ͼƬ
	m_copy_img = m_raw_img;
	

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
	qDebug() << "Video file name is: " << image_file_name << "!\n";

	// image_saved_path: yyyy-MM-dd_hh_mm_ss.mp4 
	image_saved_path = m_video_folder_name + image_file_name + m_video_file_suffix;
	qDebug() << "Video file saved path is: " << image_saved_path << "!\n";

	// ���� OpenCV ��Ƶ���������󲢿�ʼд����Ƶ
	m_videoWriter.open(image_saved_path.toStdString(), m_fourcc, m_fps, cv::Size(m_copy_img.width(), m_copy_img.height()));
	if (!m_videoWriter.isOpened())
	{
		qDebug() << "Failed to open video file";
		return;
	}

	if (m_copy_img.isNull())
	{
		qDebug() << "startRecording m_copy_image is NULL!\n";
		return;
	}
	else
	{
		qDebug() << "startRecording m_copy_image is Successed!\n";
		while (!m_record_stop)
		{
			// ������л�ȡͼ��֡����
			saveImage(m_copy_img);
		}

	}
	
}


// TODO: Save Frame in a video writter
// Private Function
void RecordImage::saveImage(QImage image)
{
	// �� QImage ת��Ϊ cv::Mat
	cv::Mat frame(image.height(), image.width(), CV_8UC4, image.bits(), image.bytesPerLine());

	// ��ͼ��֡д����Ƶ
	m_videoWriter.write(frame);
}


// TODO: Stop Recording
void RecordImage::stopRecording()
{
	qDebug() << "Recording Video Thread Stoped!\n";

	// ��� Stop ��ť��ֹͣ¼��
	m_record_stop = true;

	if (m_videoWriter.isOpened())
	{
		qDebug() << "m_videoWriter NOT Closed!\n";

	}
	else
	{
		// �ر� OpenCV ��Ƶ����������
		m_videoWriter.release();
		qDebug() << "m_videoWriter Released!\n";
		
	}

	
}