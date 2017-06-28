#ifndef UIVIDEO1V1_H
#define UIVIDEO1V1_H

#include <QWidget>
#include "ui_UIVideo1v1.h"
#include "nlss_type.h"

class UIVideo1v1 : public QWidget
{
	Q_OBJECT

public:
	UIVideo1v1(QWidget *parent = 0);
	~UIVideo1v1();

	enum FrameType
	{
		Ft_I420 = 0,
		Ft_ARGB,
		Ft_ARGB_r,
	};
private:
	Ui::UIVideo1v1 ui;
	QTimer*							m_capturnTimer;	//图像抓取定时器
	QTimer*							m_refreshTimer;	//刷新
	QPixmap							m_pBkImage;		//背景
	QScreen*						m_screen;
	
	static ST_NLSS_VIDEO_SAMPLER	m_SvideoSampler;
	static QMutex					m_mutex;

	char*							capture_data_;
	int								capture_width_;
	int								capture_height_;
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual	void mouseDoubleClickEvent(QMouseEvent* e);
signals:
	void sig_refreshWnd();
	void sig_CustomVideoData(__int64, const char*, int, int, int);
private slots:
	void slot_onCapturnTimeout();
	void slot_onRefreshTimeout();
	void VideoCapture(const char* data, unsigned int iwidth, unsigned int iheight, unsigned int iSize);
public:
	int	 ScreenWidth();
	int  ScreenHeight();
};

#endif // UIVIDEO1V1_H
