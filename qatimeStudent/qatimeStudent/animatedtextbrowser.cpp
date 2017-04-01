#include "animatedtextbrowser.h"
#include <QFile>
#include <QMovie>
#include <QScrollBar>
AnimatedTextBrowser::AnimatedTextBrowser(QWidget *parent)
	:QTextBrowser(parent)
{
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_onTimeout()));
}

void AnimatedTextBrowser::addAnimation(const QUrl& url, const QString& fileName)
{
	QFile *file = new QFile(fileName);
	if (!file->open(QIODevice::ReadOnly)){

	}

	if (lstUrl.contains(url)){ //ͬһ��gif ʹ��ͬһ��movie
		return;
	}
	else{
		lstUrl.append(url);
	}

	QMovie* movie = new QMovie(this);
	movie->setFileName(fileName);
	movie->setScaledSize(QSize(24, 24));
	movie->setCacheMode(QMovie::CacheNone);


	lstMovie.append(movie);   //��ȡ��movie,�Ա�ɾ��
	urls.insert(movie, url);

	//��֡ʱˢ��
	connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
	movie->start();
	file->close();
	delete file;
}

void AnimatedTextBrowser::animate(int)
{
	// qDebug()<<"hash count is "<<urls.count();
	if (QMovie* movie = qobject_cast<QMovie*>(sender()))
	{

		// qDebug()<<"No."<<lstMovie.indexOf(movie)<<a<<"time is"<<QTime::currentTime();
		document()->addResource(QTextDocument::ImageResource,   //�滻ͼƬΪ��ǰ֡
			urls.value(movie), movie->currentPixmap());

		setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // ..ˢ����ʾ
	}

}

void AnimatedTextBrowser::mousePressEvent(QMouseEvent *e)
{
	if (Qt::LeftButton == e->button())
	{
		emit colseCalendar();
		emit colseBrow();
	}
}

void AnimatedTextBrowser::autoHeight()
{
	m_timer->start(200);
}

void AnimatedTextBrowser::slot_onTimeout()
{
	m_timer->stop();

	if (this->verticalScrollBar()->isVisible())
	{
		this->setFixedHeight(height() + 100);
		m_timer->start(50);
		this->moveCursor(QTextCursor::End);
	}
	else
	{
		int h = this->document()->size().rheight();
		this->setFixedHeight(h);
	}

	emit sig_scrollDown();
}