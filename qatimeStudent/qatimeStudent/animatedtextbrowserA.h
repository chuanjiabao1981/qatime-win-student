#ifndef AnimatedTextBrowserAA_H
#define AnimatedTextBrowserAA_H

#include <QTextBrowser>
#include <QMouseEvent>
#include <QTimer>

class AnimatedTextBrowserA : public QTextBrowser
{
	Q_OBJECT

public:
	AnimatedTextBrowserA(QWidget *parent);
	AnimatedTextBrowserA(bool changed, QWidget *parent); //�Ƿ���Ҫ�ı�����߶�
	void addAnimation(const QUrl& url, const QString& fileName);
	void autoHeight();
private slots:
	void animate(int);
	void slot_onTimeout();
signals:
	void colseCalendar();
	void colseBrow();
	void sig_scrollDown();
protected:
	void mousePressEvent(QMouseEvent *e);       //--��갴���¼� 
	virtual void resizeEvent(QResizeEvent *);	//���ڸı�
private:
	QList<QUrl> lstUrl;
	QList<QMovie *> lstMovie;
	QHash<QMovie*, QUrl> urls;
private:
	QTimer* m_timer; // �ı�����߶�
	bool    m_bTimer;// �Ƿ���Ҫ�ı�����߶�
};

#endif // AnimatedTextBrowserA_H
