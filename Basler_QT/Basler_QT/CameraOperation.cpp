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

	// 设置图片
	m_copy_img = m_raw_img;
	

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
	qDebug() << "Video file name is: " << image_file_name << "!\n";

	// image_saved_path: yyyy-MM-dd_hh_mm_ss.mp4 
	image_saved_path = m_video_folder_name + image_file_name + m_video_file_suffix;
	qDebug() << "Video file saved path is: " << image_saved_path << "!\n";

	// 创建 OpenCV 视频编码器对象并开始写入视频
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
			// 从相机中获取图像帧保存
			saveImage(m_copy_img);
		}

	}
	
}


// TODO: Save Frame in a video writter
// Private Function
void RecordImage::saveImage(QImage image)
{
	// 将 QImage 转换为 cv::Mat
	cv::Mat frame(image.height(), image.width(), CV_8UC4, image.bits(), image.bytesPerLine());

	// 将图像帧写入视频
	m_videoWriter.write(frame);
}


// TODO: Stop Recording
void RecordImage::stopRecording()
{
	qDebug() << "Recording Video Thread Stoped!\n";

	// 点击 Stop 按钮后停止录制
	m_record_stop = true;

	if (m_videoWriter.isOpened())
	{
		qDebug() << "m_videoWriter NOT Closed!\n";

	}
	else
	{
		// 关闭 OpenCV 视频编码器对象
		m_videoWriter.release();
		qDebug() << "m_videoWriter Released!\n";
		
	}

	
}