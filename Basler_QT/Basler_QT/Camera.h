#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QDebug>

// Pylon API
#include <pylon/PylonIncludes.h>

// opencv API
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>

// namespace
using namespace std;
using namespace Pylon;
using namespace GenApi;
using namespace cv;

class Camera :
	public QObject
{

};

