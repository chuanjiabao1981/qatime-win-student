#ifndef ANIMATEDTEXTBROWSER_H
#define ANIMATEDTEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>
#include <QTimer>
class AnimatedTextBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	AnimatedTextBrowser(QWidget *parent);
	void addAnimation(const QUrl& url, const QString& fileName);
	void autoHeight();
	void AutoLeftOrEnter();
private slots:
	void animate(int);
	void slot_onTimeout();
signals:
	void colseCalendar();
	void colseBrow();
	void sig_scrollDown();
protected:
	void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件 
	void paintEvent(QPaintEvent *e);
private:
	QList<QUrl> lstUrl;
	QList<QMovie *> lstMovie;
	QHash<QMovie*, QUrl> urls;
	QTimer* m_timer;
	bool	m_bAutoLeft;//当超过一行时，自动判断左对齐或居中对齐
};

#endif // ANIMATEDTEXTBROWSER_H
