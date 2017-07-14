#include "UIAudioBar.h"
#include <QToolTip>

UIAudioBar::UIAudioBar(QWidget *parent)
	: QWidget(parent)
	, m_parent(NULL)
	, m_timer(NULL)
	, m_bSend(false)
	, m_FailTimer(false)
	, m_bCapturing(false)
{
	ui.setupUi(this);
	
	connect(ui.send_pushButton, SIGNAL(clicked()), this, SLOT(clickSend()));
	connect(ui.cancel_pushButton, SIGNAL(clicked()), this, SLOT(clickCancel()));

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_onTimeout()));

	m_FailTimer = new QTimer(this);
	connect(m_FailTimer, SIGNAL(timeout()), this, SLOT(slot_onFailTimeout()));
}

UIAudioBar::~UIAudioBar()
{

}

void UIAudioBar::setMainWindow(UIChatRoom* parent)
{
	m_parent = parent;
}

void UIAudioBar::clickSend()
{
	ui.time_label->setText("");
	ui.progressBar->setValue(0);
	m_bSend = true;
	hide();
	m_timer->stop();
	if (m_parent)
		m_parent->finishAudio();

	m_bCapturing = false;
}

void UIAudioBar::clickCancel()
{
	ui.time_label->setText("");
	ui.progressBar->setValue(0);
	m_bSend = false;
	hide();
	m_timer->stop();
	if (m_parent)
		m_parent->finishAudio();

	m_bCapturing = false;
}

void UIAudioBar::slot_onTimeout()
{
	QDateTime tNowTime = QDateTime::currentDateTime();
	int sec = m_StartTime.secsTo(tNowTime);
	ui.progressBar->setValue(sec);
	
	QString sTime = QString::number(sec);
	sTime += "\"";
	ui.time_label->setText(sTime);

	// 60秒直接发送
	if (sec == 60)
	{
		m_timer->stop();
		m_bCapturing = false;
		emit ui.send_pushButton->clicked();
		return;
	}
}

// 语音条开始执行动作
void UIAudioBar::CaptureAudio()
{
	m_bCapturing = true;
	m_FailTimer->stop();
	ui.time_label->setText("");
	ui.progressBar->setValue(0);
	m_timer->start(100);

	m_StartTime = QDateTime::currentDateTime();
}

bool UIAudioBar::IsSend()
{
	return m_bSend;
}

void UIAudioBar::slot_onFailTimeout()
{
	m_bCapturing = false;
	m_FailTimer->stop();
	QString time = ui.time_label->text();
	hide();
	if (time == "")
		QToolTip::showText(QCursor::pos(), "录制语音失败！");
}

void UIAudioBar::MonitorFail()
{
	m_bCapturing = true;
	m_FailTimer->stop();
	m_FailTimer->start(5000);
}

bool UIAudioBar::IsCapturing()
{
	return m_bCapturing;
}