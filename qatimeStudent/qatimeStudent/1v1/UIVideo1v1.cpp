#include "UIVideo1v1.h"
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QTime>
#include <QDebug>
#include "windows.h"

QMutex UIVideo1v1::m_mutex;
ST_NLSS_VIDEO_SAMPLER UIVideo1v1::m_SvideoSampler;

void sleepTime(int secs)
{
	QTime dieTime = QTime::currentTime().addMSecs(secs);

	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
}

UIVideo1v1::UIVideo1v1(QWidget *parent)
	: QWidget(parent)
	, m_capturnTimer(NULL)
	, m_refreshTimer(NULL)
	, m_screen(NULL)
	, capture_width_(1920)
	, capture_height_(1080)
{
	ui.setupUi(this);

	m_refreshTimer = new QTimer(this);
	m_refreshTimer->start(1000 / 25);
	connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(slot_onRefreshTimeout()));

	m_SvideoSampler.iWidth = 0;
	m_SvideoSampler.iHeight = 0;
	m_SvideoSampler.puaData = NULL;//一次性分配一个最大的控件，不断填充不同的数据
}

UIVideo1v1::~UIVideo1v1()
{
	if (m_refreshTimer)
	{
		m_refreshTimer->stop();
		delete m_refreshTimer;
	}
}

void UIVideo1v1::slot_onCapturnTimeout()
{

}

void UIVideo1v1::paintEvent(QPaintEvent *)
{
	if (this->isVisible())
	{
		QPainter p(this);
		p.setPen(QColor(0x8099be));

		if (m_SvideoSampler.iDataSize > 0)
		{
			QImage qimage;
			m_mutex.lock();
			qimage = QImage((uchar*)m_SvideoSampler.puaData, m_SvideoSampler.iWidth, m_SvideoSampler.iHeight, QImage::Format_RGB32);
			QImage q = qimage.mirrored(false, true);

			QPixmap pixmap = QPixmap::fromImage(q);
			p.drawPixmap(rect(), pixmap.scaled(rect().size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
			m_mutex.unlock();
		}
	}
}

void UIVideo1v1::slot_onRefreshTimeout()
{
	update();
}

// 当前抓取的窗口宽度
int UIVideo1v1::ScreenWidth()
{
	return capture_width_;
}

// 当前抓取的窗口高度
int UIVideo1v1::ScreenHeight()
{
	return capture_height_;
}

void UIVideo1v1::VideoCapture(const char* data, unsigned int iwidth, unsigned int iheight, unsigned int iSize)
{
	m_mutex.lock();
	if (this->isVisible())
	{
		if (m_SvideoSampler.puaData)
			delete[]m_SvideoSampler.puaData;
		m_SvideoSampler.puaData = new unsigned char[iSize];

		m_SvideoSampler.iWidth = iwidth;
		m_SvideoSampler.iHeight = iheight;
		m_SvideoSampler.iDataSize = iSize;

		capture_width_ = iwidth;
		capture_height_ = iheight;

		memcpy(m_SvideoSampler.puaData, data, iSize);
	}
	m_mutex.unlock();
	delete data;
}

void UIVideo1v1::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (this->isFullScreen())
	{
		setWindowFlags(Qt::SubWindow);
		showNormal();
	}
	else
	{
		setWindowFlags(Qt::Window);
		showFullScreen();
	}
}