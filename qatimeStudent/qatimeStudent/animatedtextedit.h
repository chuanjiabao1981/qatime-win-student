#ifndef ANIMATEDTEXTEDIT_H
#define ANIMATEDTEXTEDIT_H

#include <QTextEdit>
#include <QMouseEvent>
class AnimatedTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	AnimatedTextEdit(QWidget * p = 0);
	// QTextEdit *textEdit;

	void addAnimation(const QUrl& url, const QString& fileName);


	private slots:
	void animate(int);

signals:
	void colseBrow();  //�رձ���
protected:
//	void mousePressEvent(QMouseEvent *e);       //--��갴���¼� 
private:
	QList<QUrl> lstUrl;
	QList<QMovie *> lstMovie;
	QHash<QMovie*, QUrl> urls;
	
};

#endif // ANIMATEDTEXTEDIT_H
