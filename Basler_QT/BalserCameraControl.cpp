#include "BalserCameraControl.h"
#include <QDateTime>
#include <QDebug>

BalserCameraControl::BalserCameraControl(QObject *parent) : QObject(parent)
{
}

BalserCameraControl::~BalserCameraControl()
{
}

/*
	Init cameras param
	Properties: public function
*/
void BalserCameraControl::initSome()
{
	qDebug() << "BalserCameraControl: PylonInitialize initSome";
	PylonInitialize();

	CTlFactory &TlFactory = CTlFactory::GetInstance();
	TlInfoList_t lstInfo;
	int n = TlFactory.EnumerateTls(lstInfo);

	TlInfoList_t::const_iterator it;
	for (it = lstInfo.begin(); it != lstInfo.end(); ++it) {
		qDebug() << "FriendlyName: " << it->GetFriendlyName() << "FullName: " << it->GetFullName();
		qDebug() << "VendorName: " << it->GetVendorName() << "DeviceClass: " << it->GetDeviceClass();
	}
	// UpdateCameraList();
	// emit sigCameraCount(n);
	qDebug() << "Basler Camera Count: " << n;

}

/*
	Delete all cameras
	Properties: public function
*/
void BalserCameraControl::deleteAll()
{
	// Stop Grabbing
	if (m_isOpenAcquire)
	{
		qDebug() << "BalserCameraControl: Stop Grabbing";
	}

	//停止采集
	if (m_isOpenAcquire) {
		StopAcquire();
	}

	// Close camera
	CloseCamera();

	// Close libary
	qDebug() << "SBaslerCameraControl deleteAll: PylonTerminate";
	PylonTerminate();
	qDebug() << "SBaslerCameraControl deleteAll: Close";
	
}

/*
	Return all cameras
	Properties: public function
*/
QStringList BalserCameraControl::cameras()
{
	return m_cameralist;
}

/*
	Update camera list
	Properties: private function
*/
void BalserCameraControl::UpdateCameraList()
{
	CTlFactory& TLFactory = CTlFactory::GetInstance();
	ITransportLayer * pTl = TLFactory.CreateTl("BaslerGigE");
	DeviceInfoList_t devices;
	int n = pTl->EnumerateDevices(devices);
	CInstantCameraArray cameraArray(devices.size());

	if (n == 0) {
		qDebug() << "Cannot find Any camera!";
		return;
	}
	
	for (int i = 0; i < cameraArray.GetSize(); i++) {
		cameraArray[i].Attach(TLFactory.CreateDevice(devices[i]));
		string sn = cameraArray[i].GetDeviceInfo().GetSerialNumber();
		m_cameralist << QString::fromStdString(sn);
	}

	// 将 m_cameralist 信号发射（emit）出去，slop 用于接收信号
	// emit sigCameraUpdate(m_cameralist);
}

/*
	Copy to image
	Properties: private function
*/
void BalserCameraControl::CopyToImage(CGrabResultPtr pInBuffer, QImage &OutImage)
{
	try {
		uchar* buff = (uchar*)pInBuffer->GetBuffer();
		int nHeight = pInBuffer->GetHeight();
		int nWidth = pInBuffer->GetWidth();
		if (m_size != QSize(nWidth, nHeight)) {
			m_size = QSize(nWidth, nHeight);
			// 将 m_size 信号发射（emit）出去
			// emit sigSizeChange(m_size);
		}
		QImage imgBuff(buff, nWidth, nHeight, QImage::Format_Indexed8);
		OutImage = imgBuff;
		if (pInBuffer->GetPixelType() == PixelType_Mono8) {
			uchar* pCursor = OutImage.bits();
			if (OutImage.bytesPerLine() != nWidth) {
				for (int y = 0; y < nHeight; ++y) {
					pCursor = OutImage.scanLine(y);
					for (int x = 0; x < nWidth; ++x) {
						*pCursor = *buff;
						++pCursor;
						++buff;
					}
				}
			}
			else {
				memcpy(OutImage.bits(), buff, nWidth * nHeight);
			}
		}
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(L"CopyToImage\n");
		qDebug() << "CopyToImage Error:" << QString(e.GetDescription());
		//        OutputDebugString(LPCWSTR(e.GetDescription()));
	}
}

/*
	Properties: private slots
*/
void BalserCameraControl::onTimerGrabImage()
{
	if (m_isOpenAcquire) {
		QImage image;
		GrabImage(image, 5);
		if (!image.isNull()) {
			// emit sigCurrentImage(image);
		}
		QTimer::singleShot(5, this, SLOT(onTimerGrabImage()));
	}
}


/*
	Open camera
	Properties: public function
*/
int BalserCameraControl::OpenCamera(QString cameraSN)
{
	try {
		CDeviceInfo cInfo;
		String_t str = String_t(cameraSN.toStdString().c_str());
		cInfo.SetSerialNumber(str);
		m_basler.Attach(CTlFactory::GetInstance().CreateDevice(cInfo));
		m_basler.Open();

		// 获取触发模式
		getFeatureTriggerSourceType();
		
		// open the camera
		m_isOpen = true;
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(L"OpenCamera Error\n");
		qDebug() << "OpenCamera Error" << QString(e.GetDescription());
		m_isOpen = false;
		return -2;
	}

	return 0;
}

/*
	Close camera
	Properties: public function
*/
int BalserCameraControl::CloseCamera()
{
	if (!m_isOpen) {
		return -1;
	}
	try {
		if (m_basler.IsOpen()) {
			m_basler.DetachDevice();
			m_basler.Close();
		}
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(LPCWSTR(e.GetDescription()));
		qDebug() << "CloseCamera Error:" << QString(e.GetDescription());
		return -2;
	}
	return 0;
}

/*
	设置种类
	Properties: public function
*/
void BalserCameraControl::setFeatureTriggerSourceType(QString type)
{
	//停止采集
	if (m_isOpenAcquire) {
		StopAcquire();
	}
	if (type == "Freerun") {
		SetCamera(Type_Basler_Freerun);
	}
	else if (type == "Line1") {
		SetCamera(Type_Basler_Line1);
	}
}

/*
	获取种类：软触发、外触发等等
	Properties: public function
*/
QString BalserCameraControl::getFeatureTriggerSourceType()
{
	INodeMap &cameraNodeMap = m_basler.GetNodeMap();
	CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
	ptrTriggerSel->FromString("FrameStart");
	CEnumerationPtr  ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
	ptrTrigger->SetIntValue(1);
	CEnumerationPtr  ptrTriggerSource = cameraNodeMap.GetNode("TriggerSource");

	String_t str = ptrTriggerSource->ToString();
	m_currentMode = QString::fromLocal8Bit(str.c_str());

	return m_currentMode;
}

/*
	设置曝光时间
	Properties: public function
*/
void BalserCameraControl::setExposureTime(double time)
{
	SetCamera(Type_Basler_ExposureTimeAbs, time);
}

/*
	获取曝光时间
	Properties: public function
*/
int BalserCameraControl::getExposureTime()
{
	return QString::number(GetCamera(Type_Basler_ExposureTimeAbs)).toInt();
}

/*
	最小曝光时间
	Properties: public function
*/
int BalserCameraControl::getExposureTimeMin()
{
	return DOUBLE_MIN;
}

/*
	最大曝光时间
	Properties: public function
*/
int BalserCameraControl::getExposureTimeMax()
{
	return DOUBLE_MAX;
}

/*
	设置模式触发
	Properties: public function
*/
void BalserCameraControl::setFeatureTriggerModeType(bool on)
{
	INodeMap &cameraNodeMap = m_basler.GetNodeMap();
	CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
	ptrTriggerSel->FromString("FrameStart");
	CEnumerationPtr  ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
	ptrTrigger->SetIntValue(on ? 1 : 0);
}

/*
	获取模式触发
	Properties: public function
*/
bool BalserCameraControl::getFeatureTriggerModeType()
{
	INodeMap &cameraNodeMap = m_basler.GetNodeMap();
	CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
	ptrTriggerSel->FromString("FrameStart");
	CEnumerationPtr  ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
	return ptrTrigger->GetIntValue() == 1;
}

/*
	设置各种参数
	Properties: public function
*/
void BalserCameraControl::SetCamera(BalserCameraControl::SBaslerCameraControl_Type index, double tmpValue)
{
	INodeMap &cameraNodeMap = m_basler.GetNodeMap();
	switch (index) {
	case Type_Basler_Freerun: {
		CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
		ptrTriggerSel->FromString("FrameStart");
		CEnumerationPtr  ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
#ifdef Real_Freerun
		ptrTrigger->SetIntValue(0);
#else //Software
		ptrTrigger->SetIntValue(1);
		CEnumerationPtr  ptrTriggerSource = cameraNodeMap.GetNode("TriggerSource");
		ptrTriggerSource->FromString("Software");
#endif
	} break;
	case Type_Basler_Line1: {
		CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
		ptrTriggerSel->FromString("FrameStart");
		CEnumerationPtr  ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
		ptrTrigger->SetIntValue(1);
		CEnumerationPtr  ptrTriggerSource = cameraNodeMap.GetNode("TriggerSource");
		ptrTriggerSource->FromString("Line1");
	} break;
	case Type_Basler_ExposureTimeAbs: {
		const CFloatPtr exposureTime = cameraNodeMap.GetNode("ExposureTimeAbs");
		exposureTime->SetValue(tmpValue);
	} break;
	case Type_Basler_GainRaw: {
		const CIntegerPtr cameraGen = cameraNodeMap.GetNode("GainRaw");
		cameraGen->SetValue(tmpValue);
	} break;
	case Type_Basler_AcquisitionFrameRateAbs: {
		const CBooleanPtr frameRate = cameraNodeMap.GetNode("AcquisitionFrameRateEnable");
		frameRate->SetValue(TRUE);
		const CFloatPtr frameRateABS = cameraNodeMap.GetNode("AcquisitionFrameRateAbs");
		frameRateABS->SetValue(tmpValue);
	} break;
	case Type_Basler_Width: {
		const CIntegerPtr widthPic = cameraNodeMap.GetNode("Width");
		widthPic->SetValue(tmpValue);
	} break;
	case Type_Basler_Height: {
		const CIntegerPtr heightPic = cameraNodeMap.GetNode("Height");
		heightPic->SetValue(tmpValue);
	} break;
	case Type_Basler_LineSource: {
		CEnumerationPtr  ptrLineSource = cameraNodeMap.GetNode("LineSource");
		ptrLineSource->SetIntValue(2);
	} break;
	default:
		break;
	}
}

/*
	获取各种参数
	Properties: public function
*/
double BalserCameraControl::GetCamera(BalserCameraControl::SBaslerCameraControl_Type index)
{
	INodeMap &cameraNodeMap = m_basler.GetNodeMap();
	switch (index) {
	case Type_Basler_ExposureTimeAbs: {
		const CFloatPtr exposureTime = cameraNodeMap.GetNode("ExposureTimeAbs");
		return exposureTime->GetValue();
	} break;
	case Type_Basler_GainRaw: {
		const CIntegerPtr cameraGen = cameraNodeMap.GetNode("GainRaw");
		return cameraGen->GetValue();
	} break;
	case Type_Basler_AcquisitionFrameRateAbs: {
		const CBooleanPtr frameRate = cameraNodeMap.GetNode("AcquisitionFrameRateEnable");
		frameRate->SetValue(TRUE);
		const CFloatPtr frameRateABS = cameraNodeMap.GetNode("AcquisitionFrameRateAbs");
		return frameRateABS->GetValue();
	} break;
	case Type_Basler_Width: {
		const CIntegerPtr widthPic = cameraNodeMap.GetNode("Width");
		return widthPic->GetValue();
	} break;
	case Type_Basler_Height: {
		const CIntegerPtr heightPic = cameraNodeMap.GetNode("Height");
		return heightPic->GetValue();
	} break;
	default:
		return -1;
		break;
	}
}

/*
	Start Grabbing/Acquire
	Properties: public function
*/
long BalserCameraControl::StartAcquire()
{
	m_isOpenAcquire = true;
	try {
		qDebug() << "BalserCameraControl StartAcquire" << m_currentMode;
		if (m_currentMode == "Freerun") {
			m_basler.StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);
		}
		else if (m_currentMode == "Software") {
			m_basler.StartGrabbing(GrabStrategy_LatestImageOnly);
			// private slots: slot 接收信号
			onTimerGrabImage();
		}
		else if (m_currentMode == "Line1") {
			m_basler.StartGrabbing(GrabStrategy_OneByOne);
			onTimerGrabImage();
		}
		else if (m_currentMode == "Line2") {
			m_basler.StartGrabbing(GrabStrategy_OneByOne);
		}
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(L"StartAcquire error:");
		qDebug() << "StartAcquire Error:" << QString(e.GetDescription());
		return -2;
	}
	return 0;
}

/*
	结束采集
	Properties: public function
*/
long BalserCameraControl::StopAcquire()
{
	m_isOpenAcquire = false;
	qDebug() << "BalserCameraControl StopAcquire";
	try {
		if (m_basler.IsGrabbing()) {
			m_basler.StopGrabbing();
		}
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(LPCWSTR(e.GetDescription()));
		qDebug() << "StopAcquire Error:" << QString(e.GetDescription());
		return -2;
	}
	return 0;
}

/*
	Grab Image
	Properties: public function
*/
long BalserCameraControl::GrabImage(QImage &image, int timeout)
{
	try {
		if (!m_basler.IsGrabbing()) {
			StartAcquire();
		}
		CGrabResultPtr ptrGrabResult;
		if (m_currentMode == "Freerun") {
		}
		else if (m_currentMode == "Software") {
			if (m_basler.WaitForFrameTriggerReady(1000, TimeoutHandling_Return)) {
				m_basler.ExecuteSoftwareTrigger();
				m_basler.RetrieveResult(timeout, ptrGrabResult, TimeoutHandling_Return);
			}
		}
		else if (m_currentMode == "Line1") {
			m_basler.RetrieveResult(timeout, ptrGrabResult, TimeoutHandling_Return);
		}
		else if (m_currentMode == "Line2") {
			m_basler.RetrieveResult(timeout, ptrGrabResult, TimeoutHandling_Return);
		}
		if (ptrGrabResult == NULL) {
			return -5;
		}
		if (ptrGrabResult->GrabSucceeded()) {
			qDebug() << "what: ptrGrabResult GrabSucceeded";
			if (!ptrGrabResult.IsValid()) { OutputDebugString(L"GrabResult not Valid Error\n"); return -1; }
			EPixelType pixelType = ptrGrabResult->GetPixelType();
			switch (pixelType) {
			case PixelType_Mono8: {
				CopyToImage(ptrGrabResult, image);
			} break;
			case PixelType_BayerRG8: { qDebug() << "what: PixelType_BayerRG8"; }  break;
			default:  qDebug() << "what: default"; break;
			}
		}
		else {
			qDebug() << "Grab Error!!!";
			//            OutputDebugString(L"Grab Error!!!");
			return -3;
		}
	}
	catch (GenICam::GenericException &e) {
		//        OutputDebugString(L"GrabImage Error\n");
		qDebug() << "GrabImage Error:" << QString(e.GetDescription());
		//        OutputDebugString((e.GetDescription()));
		return -2;
	}
	catch (...) {
		OutputDebugString(L"ZP 11 Shot GetParam Try 12 No know Error\n");
		return -1;
	}
	return 0;
}
