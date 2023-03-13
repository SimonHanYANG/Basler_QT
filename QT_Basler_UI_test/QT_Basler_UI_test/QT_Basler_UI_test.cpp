#include "QT_Basler_UI_test.h"

QT_Basler_UI_test::QT_Basler_UI_test(QWidget *parent)
    : QMainWindow(parent)
{
    ui->setupUi(this);

	bool detectedCam = true;
	QString items[MAXCAMNUM] = { "Camera 1", "Camera 2"};

	qDebug() << "items num: " << items->size() << "\n";

	if (detectedCam)
	{
		for (int i = 0; i < MAXCAMNUM; i++)
		{
			ui->CameraListComboBox->addItem(items[i]);
		}
	}
	
	// Camera list ComboBox 连接信号槽
	connect(ui->CameraListComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CameraListComboBox_currentIndexChanged(const QString &)));

}

QT_Basler_UI_test::~QT_Basler_UI_test()
{}
 
// CameraListComboBox Function
void QT_Basler_UI_test::on_CameraListComboBox_currentIndexChanged(int index)
{
	QString selectedName = ui->CameraListComboBox->currentText();
	qDebug() << "Selected name is:" << selectedName;
}

// SingleShotButton Function
void QT_Basler_UI_test::on_OpenSelectedButton_clicked()
{
	qDebug() << "Open Camera!\n";
	// QLable show text
	ui->CamOpenStatusLabel->setText("Camera has been opened!");
}

// CloseCamButton Function
void QT_Basler_UI_test::on_CloseCamButton_clicked()
{
	qDebug() << "Close Camera!\n";
	// QLable show text
	ui->CamOpenStatusLabel->setText("Camera has been closed!");
}

// SingleShotButton Function
void QT_Basler_UI_test::on_SingleShotButton_clicked()
{
	qDebug() << "Single Shot!\n";
}

// ContinuousShotButton Function
void QT_Basler_UI_test::on_ContinuousShotButton_clicked()
{
	qDebug() << "Continuous Shot!\n";
}

// StopButton Function
void QT_Basler_UI_test::on_StopButton_clicked()
{
	qDebug() << "Stop Grabbing!\n";
}

// ExposureTimeSlider Function
// 将相机的曝光时间增加以后，相机的帧率就下降
void QT_Basler_UI_test::on_ExposureTimeSlider_valueChanged(int value)
{
	qDebug() << "Slider value (Exposure Time) is:" << value;
}


