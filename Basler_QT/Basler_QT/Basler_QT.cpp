#include "Basler_QT.h"

Basler_QT::Basler_QT(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	for (int i = 0; i < MaxCamera; i++)
	{
		m_camera[i].SetUserHint(i);

		// Connect signals from CGuiCamera class to this QTMainWindow.
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
	for (int i = 0; i < MaxCamera; i++)
	{
		m_camera[i].Close();
	}

	// delete ui;
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

// Helper function to open a CGuiCamera and update controls.
// After the camera has been opened, we adjust the controls to configure
// the camera features. Slider ranges are set and drop-down lists are filled
// with enumeration entries.
bool Basler_QT::InternalOpenCamera(const Pylon::CDeviceInfo& devInfo, int cameraId)
{
	try
	{
		// Open() may throw exceptions.
		m_camera[cameraId].Open(devInfo);
		return true;
	}
	catch (const Pylon::GenericException& e)
	{
		ShowWarning(QString("Could not open camera!\n") + QString(e.GetDescription()));

		return false;
	}



}

// Helper function to close a CGuiCamera and update controls.
// After the camera has been closed, we disable controls to configure the camera.
// Slider ranges are reset and drop-down lists are cleared.
void Basler_QT::InternalCloseCamera(int cameraId)
{
	try
	{
		m_camera[cameraId].Close();

		// Enable/disable controls.
		// UpdateCameraDialog(cameraId);
	}
	catch (const Pylon::GenericException& e)
	{
		PYLON_UNUSED(e);
	}
}

// This overrides the paintEvent of the dialog to paint the images.
// For better performance and easy maintenance a custom control should be used.
void Basler_QT::paintEvent(QPaintEvent *ev)
{
	QMainWindow::paintEvent(ev);
	
	// Repaint image of the camera 0
	if (m_camera[0].IsOpen())
	{
		QPainter painter(this);
		QRect target = ui.ImageShowWidget->geometry();

		QMutexLocker locker(m_camera[0].GetBmpLock());
		QImage image = m_camera[0].GetImage();
		QRect source = QRect(0, 0, image.width(), image.height());

		// painte the image
		painter.drawImage(target, image, source);
	}

	// Repaint image of the camera 1
	if (m_camera[1].IsOpen())
	{
		QPainter painter(this);
		QRect target = ui.ImageShowWidget->geometry();

		QMutexLocker locker(m_camera[1].GetBmpLock());
		QImage image = m_camera[1].GetImage();
		QRect source = QRect(0, 0, image.width(), image.height());

		// painte the image
		painter.drawImage(target, image, source);
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

// SelectCamButton Function -- Open Selected Btn
void Basler_QT::on_OpenSelectedButton_clicked()
{
	// QLable show text
	ui.CamOpenStatusLabel->setText("Camera has been opened!");

	int index = ui.CameraListComboBox->currentIndex();
	qDebug() << "Open Camera " << index << "!\n";

	if (index < 0)
	{
		// if there is no camera opened
		return;
	}

	// Get the pointer to Pylon::CDeviceInfo selected
	const Pylon::CDeviceInfo* pDeviceInfo = (const Pylon::CDeviceInfo*)ui.CameraListComboBox->itemData(index).value<void *>();

	// Open the camera
	InternalOpenCamera(*pDeviceInfo, index);
}

// CloseCamButton Function -- Close Camera Btn
void Basler_QT::on_CloseCamButton_clicked()
{
	// QLable show text
	ui.CamOpenStatusLabel->setText("Camera has been closed!");

	// Find the opened camera index
	int index = ui.CameraListComboBox->currentIndex();
	qDebug() << "Close Camera " << index << "!\n";

	if (index < 0)
	{
		// if there is no camera opened
		return;
	}

	InternalCloseCamera(index);

	// Make sure to repaint the image control.
	// The actual drawing is done in paintEvent.
	ui.ImageShowWidget->repaint();
}

// SingleShotButton Function -- Single Shot Btn
void Basler_QT::on_SingleShotButton_clicked()
{
	// Find the opened camera index
	int index = ui.CameraListComboBox->currentIndex();
	qDebug() << "Camera " << index << " Started Single Shot!\n";

	// Start camera single grab
	try
	{
		m_camera[index].SingleGrab();
	}
	catch (const Pylon::GenericException& e)
	{
		ShowWarning(QString("Could not start grab!\n") + QString(e.GetDescription()));
	}

}

// ContinuousShotButton Function -- Continuous Shot Btn
void Basler_QT::on_ContinuousShotButton_clicked()
{
	// Find the opened camera index
	int index = ui.CameraListComboBox->currentIndex();
	qDebug() << "Camera " << index << " Started Continuous Shot!\n";

	try
	{
		m_camera[index].ContinuousGrab();
	}
	catch (const Pylon::GenericException& e)
	{
		ShowWarning(QString("Could not start grab!\n") + QString(e.GetDescription()));
	}

}

// StopButton Function -- Stop Btn
void Basler_QT::on_StopButton_clicked()
{
	// Find the opened camera index
	int index = ui.CameraListComboBox->currentIndex();
	qDebug() << "Camera " << index << " Stop Shotting!\n";

	try
	{
		m_camera[index].StopGrab();
	}
	catch (const Pylon::GenericException& e)
	{
		ShowWarning(QString("Could not stop grab!\n") + QString(e.GetDescription()));
	}

}

// ExposureTimeSlider Function -- Exposure Time SlideBar
// ��������ع�ʱ�������Ժ������֡�ʾ��½�
void Basler_QT::on_ExposureTimeSlider_valueChanged(int value)
{
	qDebug() << "Slider value (Exposure Time) is:" << value;
}