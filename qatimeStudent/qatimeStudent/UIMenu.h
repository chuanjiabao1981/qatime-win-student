#ifndef UIMENU_H
#define UIMENU_H

#include <QWidget>
#include "ui_UIMenu.h"
#include "UIAuxiliarywnd.h"

class UIAuxiliaryWnd;
class UIMenu : public QWidget
{
	Q_OBJECT

public:
	UIMenu(QWidget *parent = 0);
	~UIMenu();

	UIAuxiliaryWnd*			m_parent;
private:
	Ui::UIMenu ui;
	QString							m_version;
	QNetworkReply*					reply;
	QNetworkAccessManager			manager;
	QString							m_token;
private slots:
	void clickPic();
	void clickReturn();
	void clickCheck();
	void returnVersion();
protected:
	virtual void focusOutEvent(QFocusEvent* e);
	virtual void paintEvent(QPaintEvent *event);

public:
	void SetMainParent(UIAuxiliaryWnd* pWnd);
	void setPic(QPixmap pix);
	void setName(QString name);
	void setVersion(QString version);
	void setToken(QString token);
	void StartCheck(QString enforce, QString version, QString downpath);
	void checkVersion();
};

#endif // UIMENU_H
