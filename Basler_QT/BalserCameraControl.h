#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>

// Pylon6 API
#include <pylon/PylonIncludes.h>

// For �ع�ʱ�䣬���ֵ����Сֵ
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
		Type_Basler_ExposureTimeAbs,								// ����������ع�ʱ��
		Type_Basler_AcquisitionFrameRateAbs,						// ���������Ƶ��
		Type_Basler_Width,											// image width
		Type_Basler_Height,											// image height
		Type_Basler_Freerun,										// ����������ڴ���
		Type_Basler_Line1,											// ����������ⴥ��
		Type_Basler_GainRaw,										// �������������
		Type_Basler_LineSource,										// �ƵĴ����ź�
	};

	// functions		
	void initSome();												// Initialization
	void deleteAll();												// Delete/Close all cameras
	QStringList cameras();											// Return all cameras
	int OpenCamera(QString cameraSN);								// Open camera
	int CloseCamera();												// Close camera

	void setExposureTime(double time);								// �����ع�ʱ��
	int getExposureTime();											// ��ȡ�ع�ʱ��
	int getExposureTimeMin();										// ��С�ع�ʱ��
	int getExposureTimeMax();										// ����ع�ʱ��

	void setFeatureTriggerSourceType(QString type);					// ��������
	QString getFeatureTriggerSourceType();							// ��ȡ���ࣺ�������ⴥ���ȵ�

	void setFeatureTriggerModeType(bool on);						// ����ģʽ����
	bool getFeatureTriggerModeType();								// ��ȡģʽ����
	// ���ø��ֲ���
	void SetCamera(BalserCameraControl::SBaslerCameraControl_Type index, double tmpValue = 0.0); 
	// ��ȡ���ֲ���
	double GetCamera(BalserCameraControl::SBaslerCameraControl_Type index); 

	long StartAcquire();											// ��ʼ�ɼ�
	long StopAcquire();												// �����ɼ�
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

