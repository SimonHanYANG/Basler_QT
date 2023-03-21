#pragma once
#include <QObject>
#include <QMutex>
#include <QImage>
#include <QDebug>

// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

class CameraControl : public QObject
	, public Pylon::CImageEventHandler             // Allows you to get notified about images received and grab errors.
	, public Pylon::CConfigurationEventHandler     // Allows you to get notified about device removal.
	, public Pylon::CCameraEventHandler            // Allows you to get notified about camera events and GenICam node changes.
{
	Q_OBJECT

public:
	CameraControl();
	virtual ~CameraControl();

	// Attributes
	void SetUserHint(int userHint);
	int GetUserHint() const;

	// Turn our sample image processing on or off.
	void SetInvertImage(bool enable);

	// Return the converted bitmap.
	// Called by the GUI to display the image on the screen.
	const QImage& GetImage() const;

public:
	// Returns true if the device has been removed/disconnected.
	bool IsCameraDeviceRemoved() const;
	// Returns true if the device is currently opened.
	bool IsOpen() const;
	// Returns true if the device is currently grabbing.
	bool IsGrabbing() const;
	// Returns true if the device supports single shot acquisition.
	bool IsSingleShotSupported() const;

public:
	// For GUI -- get the exposure time
	Pylon::IIntegerEx& GetExposureTime();

public:
	// Statistics
	uint64_t GetGrabbedImages() const;
	uint64_t GetGrabbedImagesDiff();
	uint64_t GetGrabErrors() const;

public:
	// Operations
	void Open(const Pylon::CDeviceInfo& deviceInfo);
	void Close();

	void SingleGrab();
	void ContinuousGrab();
	void StopGrab();

public:
	// Pylon::CImageEventHandler functions
	virtual void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages);
	virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult);


signals:
	void NewGrabResult(int userHint);
	void StateChanged(int userHint, bool isGrabbing);
	void DeviceRemoved(int userHint);
	void NodeUpdated(int userHint, GenApi::INode* pNode);

private:
	// Used to store the index of the camera.
	int m_userHint;
	// Will be set to toggle image processing on or off.
	bool m_invertImage;
	// Protects members.
	mutable QMutex m_MemberLock;
	// Protects the converted image.
	mutable QMutex m_imageLock;

private:
	// Statistical values
	uint64_t m_cntGrabbedImages;
	uint64_t m_cntSkippedImages;
	uint64_t m_cntGrabErrors;
	uint64_t m_cntLastGrabbedImages;

private:
	// The pylon camera object.
	Pylon::CBaslerUniversalInstantCamera m_camera;
	// The grab result retrieved from the camera.
	Pylon::CGrabResultPtr m_ptrGrabResult;
	// The grab result as a QImage to be displayed on the screen.
	QImage m_image;
	// The format converter to convert a grab result into a QImage.
	Pylon::CImageFormatConverter m_formatConverter;

	// Configurations to apply when starting single or continuous grab.
	Pylon::CAcquireSingleFrameConfiguration m_singleConfiguration;
	Pylon::CAcquireContinuousConfiguration m_continuousConfiguration;

	// Smart pointer to camera features
	Pylon::CIntegerParameter m_exposureTime;
	Pylon::CIntegerParameter m_gain;
};

