#include "Basler_QT.h"

Basler_QT::Basler_QT(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	for (int i = 0; i < MaxCamera; i++)
	{
		m_camera[i].SetUserHint(i);

		// Connect signals from CGuiCamera class to this dialog.
		QObject::connect(&(m_camera[i]), &CameraControl::NewGrabResult, this, &Basler_QT::OnNewGrabResult);
		// QObject::connect(&(m_camera[i]), &CameraControl::StateChanged, this, &Basler_QT::OnStateChanged);
		// QObject::connect(&(m_camera[i]), &CameraControl::DeviceRemoved, this, &Basler_QT::OnDeviceRemoved);
		// QObject::connect(&(m_camera[i]), &CameraControl::NodeUpdated, this, &Basler_QT::OnNodeUpdated);
	}

	int cameraNum;
	cameraNum = EnumerateDevices();
	qDebug() << "Detected Camera Number is: " << cameraNum;

	// Remove question mark from the title bar.
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Basler_QT::~Basler_QT()
{
	
}

// Helper function to get a list of all attached devices and store it in m_devices.
int Basler_QT::EnumerateDevices()
{
	// Before using any pylon methods, the pylon runtime must be initialized.
	Pylon::PylonInitialize();

	Pylon::DeviceInfoList_t devices;
	try
	{
		// Get the transport layer factory.
		Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();

		// Get all attached cameras.
		TlFactory.EnumerateDevices(devices);
	}
	catch (const Pylon::GenericException& e)
	{
		PYLON_UNUSED(e);
		devices.clear();

		qDebug() << e.GetDescription();
	}

	m_devices = devices;

	// When calling this function, make sure to update the device list control
	// because its items store pointers to elements in the m_devices list.
	return (int)m_devices.size(); 
}

// Show a warning dialog.
void Basler_QT::ShowWarning(QString warningText)
{
	QMessageBox::warning(this, "GUI Sample", warningText, QMessageBox::Ok);
}


// This will be called in response to the NewGrabResult signal posted by
// CGuiCamera when a new grab result has been received.
// This function is called in the GUI thread so you can access GUI elements.
void Basler_QT::OnNewGrabResult(int userHint)
{
	if ((userHint == 0) && m_camera[0].IsOpen())
	{
		// Make sure to repaint the image control.
		// The actual drawing is done in paintEvent.
		ui.ImageShowWidget->repaint();
	}

	if ((userHint == 1) && m_camera[1].IsOpen())
	{
		// Make sure to repaint the image control.
		// The actual drawing is done in paintEvent.
		ui.ImageShowWidget->repaint();
	}
}


// DiscoverCam Function -- Discover cameras
void Basler_QT::on_DiscoverCam_clicked()
{
	qDebug() << "Discover Cameras!\n";

	// Remove all items from the combo box.
	ui.CameraListComboBox->clear();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	// Enumerate devices.
	int deviceCount = EnumerateDevices();
	QApplication::restoreOverrideCursor();

	if (deviceCount == 0)
	{
		ShowWarning("No camera found.");
		return;
	}

	// Fill the combo box.
	for (Pylon::DeviceInfoList_t::const_iterator it = m_devices.begin(); it != m_devices.end(); ++it)
	{
		// Get the pointer to the current device info.
		const Pylon::CDeviceInfo* const pDeviceInfo = &(*it);

		// Add the friendly name to the list.
		Pylon::String_t friendlyName = pDeviceInfo->GetFriendlyName();
		// Add a pointer to CDeviceInfo as item data so we can use it later.
		ui.CameraListComboBox->addItem(friendlyName.c_str(), QVariant::fromValue((void *)pDeviceInfo));
	}

	// Select first item.
	ui.CameraListComboBox->setCurrentIndex(0);

	// Enable/disable controls.
	// on_cameraList_currentIndexChanged(-1);
}

// SingleShotButton Function -- Open Selected Btn
void Basler_QT::on_OpenSelectedButton_clicked()
{
	qDebug() << "Open Camera!\n";
	// QLable show text
	ui.CamOpenStatusLabel->setText("Camera has been opened!");
}

// CloseCamButton Function -- Close Camera Btn
void Basler_QT::on_CloseCamButton_clicked()
{
	qDebug() << "Close Camera!\n";
	// QLable show text
	ui.CamOpenStatusLabel->setText("Camera has been closed!");
}

// SingleShotButton Function -- Single Shot Btn
void Basler_QT::on_SingleShotButton_clicked()
{
	qDebug() << "Single Shot!\n";
}

// ContinuousShotButton Function -- Continuous Shot Btn
void Basler_QT::on_ContinuousShotButton_clicked()
{
	qDebug() << "Continuous Shot!\n";
}

// StopButton Function -- Stop Btn
void Basler_QT::on_StopButton_clicked()
{
	qDebug() << "Stop Grabbing!\n";
}

// ExposureTimeSlider Function -- Exposure Time SlideBar
// 将相机的曝光时间增加以后，相机的帧率就下降
void Basler_QT::on_ExposureTimeSlider_valueChanged(int value)
{
	qDebug() << "Slider value (Exposure Time) is:" << value;
}