#include "CameraControl.h"


CameraControl::CameraControl() :
	m_cntGrabbedImages(0)
	, m_cntSkippedImages(0)
	, m_cntGrabErrors(0)
	, m_cntLastGrabbedImages(0)
	, m_userHint(-1)
	, m_invertImage(false)
{
	// Register this object as an image event handler in order to get notified of new images.
	// See Pylon::CImageEventHandler for details.
	m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);

	// Register this object as a configuration event handler in order to get notified of camera state changes.
	// See Pylon::CConfigurationEventHandler for details.
	m_camera.RegisterConfiguration(new Pylon::CAcquireContinuousConfiguration(), Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
	m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_Append, Pylon::Cleanup_None);

	// Choose an output format. This has to match the format of m_image.
	m_formatConverter.OutputPixelFormat = Pylon::PixelType_BGRA8packed;
}

CameraControl::~CameraControl()
{
	Close();
}

// Attributes
void CameraControl::SetUserHint(int userHint)
{
	QMutexLocker lock(&m_MemberLock);
	m_userHint = userHint;
}

int CameraControl::GetUserHint() const
{
	return m_userHint;
}


/*
	QMutexLocker��Qt�е�һ���࣬�����ڶ��߳�Ӧ�ó����п��ƹ�����Դ�ķ��ʡ�
	���ṩ��һ�ַ���ķ�ʽ������һ��QMutex���󣬲������������ʱ�Զ��ͷ�����
	�Ӷ��������ֶ��������ĸ����Ժ�Ǳ�ڵľ�����������ʹ��QMutexLockerʱ��
	����ȷ��ֻ��һ���߳̿��Է��ʱ���������Դ���Ӷ����������ݾ������������߳�����
*/
// Turn our sample image processing on or off.
void CameraControl::SetInvertImage(bool enable)
{
	QMutexLocker lock(&m_MemberLock);
	m_invertImage = enable;
}

// Return the converted bitmap.
// Called by the GUI to display the image on the screen.
const QImage& CameraControl::GetImage() const
{
	// No need to protect this member as it will only be accessed from the GUI thread.
	return m_image;
}

// Returns true if the device has been removed/disconnected.
bool CameraControl::IsCameraDeviceRemoved() const
{
	return m_camera.IsCameraDeviceRemoved();
}

// Returns true if the device is currently opened.
bool CameraControl::IsOpen() const
{
	return m_camera.IsOpen();
}

// Returns true if the device is currently grabbing.
bool CameraControl::IsGrabbing() const
{
	return m_camera.IsGrabbing();
}

// Returns true if the device supports single shot acquisition.
bool CameraControl::IsSingleShotSupported() const
{
	if (!m_camera.IsOpen())
	{
		return false;
	}

	Pylon::StringList_t modeEntries;
	m_camera.AcquisitionMode.GetSettableValues(modeEntries);

	for (Pylon::StringList_t::iterator it = modeEntries.begin(), end = modeEntries.end(); it != end; ++it)
	{
		const Pylon::String_t& entry = *it;
		if (entry.compare("SingleFrame") == 0)
		{
			return true;
		}
	}

	return false;
}

// Return a camera parameter.
// This function is called by the GUI to update controls.
Pylon::IIntegerEx& CameraControl::GetExposureTime()
{
	return m_exposureTime;
}

// Returns statistical values for the GUI.
uint64_t CameraControl::GetGrabbedImages() const
{
	// We must protect this member as it will be accessed from the grab thread and the GUI thread.
	QMutexLocker lock(&m_MemberLock);
	return m_cntGrabbedImages;
}

// Returns the number of images grabbed since the last call to this function.
// This is used to calculate the FPS received.
uint64_t CameraControl::GetGrabbedImagesDiff()
{
	// We must protect these members as they will be accessed from the grab thread and the GUI thread.
	QMutexLocker lock(&m_MemberLock);
	uint64_t delta = m_cntGrabbedImages - m_cntLastGrabbedImages;
	m_cntLastGrabbedImages = m_cntGrabbedImages;
	return delta;
}


// Returns statistical values for the GUI.
uint64_t CameraControl::GetGrabErrors() const
{
	// We must protect this member as it will be accessed from the grab thread and the GUI thread.
	QMutexLocker lock(&m_MemberLock);
	return m_cntGrabErrors;
}


// Creates and opens the camera specified in deviceInfo.
// Initializes the member variables to access camera features.
// Registers camera event handler for camera features so we'll get notfied when a feature changes.
// Registers an image event handler so we'll get notifed when a new image has been grabbed.
// Registers a configuration event so we can configure the camrea when a grab is being started.
void CameraControl::Open(const Pylon::CDeviceInfo& deviceInfo)
{
	QMutexLocker lock(&m_MemberLock);

	try
	{
		// Add the AutoPacketSizeConfiguration and let pylon delete it when not needed anymore.
		// m_camera.RegisterConfiguration(new CAutoPacketSizeConfiguration(), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

		// Create the device and attach it to CInstantCamera.
		// Let CInstantCamera take care of destroying the device.
		Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateDevice(deviceInfo);
		m_camera.Attach(pDevice, Pylon::Cleanup_Delete);

		// Open camera.
		m_camera.Open();

		// Get the ExposureTime feature.
		// On GigE cameras, the feature is called ExposureTimeRaw.
		// On USB cameras, it is called ExposureTime.
		if (m_camera.ExposureTime.IsValid())
		{
			// We need the integer representation because the GUI controls can only use integer values.
			// If it doesn't exist, return an empty parameter.
			m_camera.ExposureTime.GetAlternativeIntegerRepresentation(m_exposureTime);
		}
		else if (m_camera.ExposureTimeRaw.IsValid())
		{
			m_exposureTime.Attach(m_camera.ExposureTimeRaw.GetNode());
		}

		// Get the Gain feature.
		// On GigE cameras, the feature is called GainRaw.
		// On USB cameras, it is called Gain.
		if (m_camera.Gain.IsValid())
		{
			// We need the integer representation for this sample.
			// If it doesn't exist, return an empty parameter.
			m_camera.Gain.GetAlternativeIntegerRepresentation(m_gain);
		}
		else if (m_camera.GainRaw.IsValid())
		{
			m_gain.Attach(m_camera.GainRaw.GetNode());
		}

		// Add the event handlers that will be called when the feature changes, e.g., its state or value.

		if (m_exposureTime.IsValid())
		{
			// If we must use the alternative integer representation, we don't know the name of the node as it is defined by the camera.
			m_camera.RegisterCameraEventHandler(this, m_exposureTime.GetNode()->GetName(), 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional);
		}

		if (m_gain.IsValid())
		{
			// If we must use the alternative integer representation, we don't know the name of the node as it is defined by the camera.
			m_camera.RegisterCameraEventHandler(this, m_gain.GetNode()->GetName(), 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional);
		}

		m_camera.RegisterCameraEventHandler(this, "PixelFormat", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional);

		m_camera.RegisterCameraEventHandler(this, "TriggerMode", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional);

		m_camera.RegisterCameraEventHandler(this, "TriggerSource", 0, Pylon::RegistrationMode_Append, Pylon::Cleanup_None, Pylon::CameraEventAvailability_Optional);
	}
	catch (const Pylon::GenericException& e)
	{
		PYLON_UNUSED(e);

		qDebug() << e.GetDescription();

		// Undo everything.
		lock.unlock();
		Close();

		throw;
	}
}

// Perform cleanup and undo everything we did in Open():
// - Stop the grab.
// - Release all images.
// - Deregister event handlers.
// - Free the camera.
void CameraControl::Close()
{
	QMutexLocker lock(&m_MemberLock);

	// Stop the grab so the grab thread will not set new m_ptrGrabResult.
	StopGrab();

	// Free the grab result if present.
	m_ptrGrabResult.Release();

	// Free the image by swapping it with a dummy (null) image.
	QImage dummy;
	m_image.swap(dummy);

	// Remove the event handlers that will be called when the feature changes, e.g., its state or value.
	m_camera.DeregisterCameraEventHandler(this, "TriggerSource");
	m_camera.DeregisterCameraEventHandler(this, "TriggerMode");
	m_camera.DeregisterCameraEventHandler(this, "PixelFormat");

	if (m_gain.IsValid())
	{
		// If we must use the alternative integer representation, we don't know the name of the node as it is defined by the camera.
		m_camera.DeregisterCameraEventHandler(this, m_gain.GetNode()->GetName());
	}

	if (m_exposureTime.IsValid())
	{
		// If we must use the alternative integer representation, we don't know the name of the node as it is defined by the camera.
		m_camera.DeregisterCameraEventHandler(this, m_exposureTime.GetNode()->GetName());
	}

	// Clear the pointers to the features we set manually in Open().
	m_exposureTime.Release();
	m_gain.Release();

	// Close the camera and free all resources.
	m_camera.DestroyDevice();
}

// Stop the continuous grab on the camera.
void CameraControl::StopGrab()
{
	m_camera.StopGrabbing();
}

// Grab a single image.
void CameraControl::SingleGrab()
{
	// Camera may have been disconnected.
	if (!m_camera.IsOpen() || m_camera.IsGrabbing())
	{
		return;
	}

	// Try setting the single frame mode, if available.
	m_camera.AcquisitionMode.TrySetValue(Basler_UniversalCameraParams::AcquisitionMode_SingleFrame);

	// Grab one image.
	// When the image is received, pylon will call the OnImageGrab() handler.
	m_camera.StartGrabbing(1, Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}


// Start a continuous grab on the camera.
void CameraControl::ContinuousGrab()
{
	// Camera may have been disconnected.
	if (!m_camera.IsOpen() || m_camera.IsGrabbing())
	{
		return;
	}

	// Try setting the continuous frame mode, if available.
	m_camera.AcquisitionMode.TrySetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);

	// Start grabbing until StopGrabbing() is called.
	m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}


// Pylon::CImageEventHandler functions
void CameraControl::OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
{
	qDebug() << __FUNCTION__;

	QMutexLocker lock(&m_MemberLock);
	m_cntSkippedImages += countOfSkippedImages;

	// Prevent unused variable warning.
	PYLON_UNUSED(camera);
}

// This function is called from the CInstantCamera grab thread when a new image is available.
// Perform your image processing here.
// After this, we'll convert the image to an RGB bitmap and inform the GUI about the new image.
//
// NOTE: You must not access any UI objects, since this function is not called from the GUI thread.
//       To update the GUI, we will post a message to the main window at the end of this function.
//       The GUI thread will process the message and update the GUI.
void CameraControl::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult)
{
	// The following line is commented out as this function will be called frequently
	// filling up the debug output.
	//qDebug() << __FUNCTION__;

	// When overwriting the current CGrabResultPtr, the previous result will automatically be
	// released and reused by CInstantCamera.
	QMutexLocker lockGrabResult(&m_MemberLock);
	m_ptrGrabResult = grabResult;
	lockGrabResult.unlock();

	// First, check whether the smart pointer is valid.
	// Then, call GrabSucceeded() on the CGrabResultData to test whether the grab result contains
	// a sucessfully grabbed image.
	// In case of problems, e.g., transmission errors, the result may be invalid.
	if (grabResult.IsValid() && grabResult->GrabSucceeded())
	{
		// This is where you would do image processing and other tasks.
		// --- Start of sample image processing --- (only works for 8-bit formats)
		if (m_invertImage && Pylon::BitDepth(grabResult->GetPixelType()) == 8)
		{
			size_t imageSize = Pylon::ComputeBufferSize(grabResult->GetPixelType(), grabResult->GetWidth(), grabResult->GetHeight(), grabResult->GetPaddingX());

			uint8_t* p = reinterpret_cast<uint8_t*>(grabResult->GetBuffer());
			uint8_t* const pEnd = p + (imageSize / sizeof(uint8_t));
			for (; p < pEnd; ++p)
			{
				*p = 255 - *p; //For demonstration purposes only, invert the image.
			}
		}
		//--- End of sample image processing ---

		// Convert the processed image to a QImage so we can display it on the screen.
		// We must lock the image so we don't modify the pixels while the GUI thread is painting.
		QMutexLocker lockImage(&m_imageLock);

		// Reallocate the image if the size is a mismatch.
		if ((static_cast<uint32_t>(m_image.width()) != grabResult->GetWidth())
			|| (static_cast<uint32_t>(m_image.height()) != grabResult->GetHeight()))
		{
			m_image = QImage(grabResult->GetWidth(), grabResult->GetHeight(), QImage::Format_RGB32);
		}

		// Convert the image.
		void *pBuffer = m_image.bits();
		size_t bufferSize = m_image.sizeInBytes();
		m_formatConverter.Convert(pBuffer, bufferSize, grabResult);

		lockImage.unlock();

		QMutexLocker lockImageCount(&m_MemberLock);
		++m_cntGrabbedImages;
		lockImageCount.unlock();
	}
	else
	{
		// If the grab result is invalid, we also mark the bitmap as invalid.
		QMutexLocker lockImage(&m_imageLock);
		m_image.fill(0);
		lockImage.unlock();

		// Some transport layers provide an error code why the grab failed.
		qDebug() << __FUNCTION__ << " Grab failed. Error code: " << grabResult->GetErrorCode();

		QMutexLocker lockErrorCount(&m_MemberLock);
		++m_cntGrabErrors;
		lockErrorCount.unlock();
	}

	// Tell everyone that there is a new image available.
	emit NewGrabResult(m_userHint);

	// Prevent unused variable warning.
	PYLON_UNUSED(camera);
}

