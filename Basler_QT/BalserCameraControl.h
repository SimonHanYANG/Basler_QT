#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>

// Pylon6 API
#include <pylon/PylonIncludes.h>

// For 曝光时间，最大值与最小值
#define DOUBLE_MAX 100000
#define DOUBLE_MIN 0

// namespace
using namespace std;
using namespace Pylon;
using namespace GenApi;

class BalserCameraControl :
	public QObject
{
// public functions
public:
	explicit BalserCameraControl(QObject *parent = 0);
	~BalserCameraControl();

	enum SBaslerCameraControl_Type {
		Type_Basler_ExposureTimeAbs,								// 设置相机的曝光时间
		Type_Basler_AcquisitionFrameRateAbs,						// 设置相机的频率
		Type_Basler_Width,											// image width
		Type_Basler_Height,											// image height
		Type_Basler_Freerun,										// 设置相机的内触发
		Type_Basler_Line1,											// 设置相机的外触发
		Type_Basler_GainRaw,										// 设置相机的增益
		Type_Basler_LineSource,										// 灯的触发信号
	};

	// functions		
	void initSome();												// Initialization
	void deleteAll();												// Delete/Close all cameras
	QStringList cameras();											// Return all cameras
	int OpenCamera(QString cameraSN);								// Open camera
	int CloseCamera();												// Close camera

	void setExposureTime(double time);								// 设置曝光时间
	int getExposureTime();											// 获取曝光时间
	int getExposureTimeMin();										// 最小曝光时间
	int getExposureTimeMax();										// 最大曝光时间

	void setFeatureTriggerSourceType(QString type);					// 设置种类
	QString getFeatureTriggerSourceType();							// 获取种类：软触发、外触发等等

	void setFeatureTriggerModeType(bool on);						// 设置模式触发
	bool getFeatureTriggerModeType();								// 获取模式触发
	// 设置各种参数
	void SetCamera(BalserCameraControl::SBaslerCameraControl_Type index, double tmpValue = 0.0); 
	// 获取各种参数
	double GetCamera(BalserCameraControl::SBaslerCameraControl_Type index); 

	long StartAcquire();											// 开始采集
	long StopAcquire();												// 结束采集
	long GrabImage(QImage& image, int timeout = 2000);

signals:
	void sigCameraUpdate(QStringList list);
	void sigSizeChange(QSize size);
	void sigCameraCount(int count);
	void sigCurrentImage(QImage img);


// private functions
private:
	void UpdateCameraList();										// Update camera list
	void CopyToImage(CGrabResultPtr pInBuffer, QImage &OutImage);

private slots:
	void onTimerGrabImage();

// private variables
private:		
	CInstantCamera m_basler;										// Basler camera
	QStringList m_cameralist;										// Camera list
	QString m_currentMode;											// Current trigger type

	bool m_isOpenAcquire = false;									// Start Grabbing or not: init not
	bool m_isOpen = false;											// open camera or not: init not

	QSize m_size;
};

