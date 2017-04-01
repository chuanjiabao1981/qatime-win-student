#ifndef UIAUXILIARYWND_H
#define UIAUXILIARYWND_H

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include "ui_UIAuxiliarywnd.h"
#include "UIMainwindow.h"
#include "UIAuxiliarylist.h"
#include "UIAuxiliarytoday.h"
#include "UIWorkThread.h"
#include "UIMenu.h"

class UIMainWindow;
class UIAuxiliaryList;
class UIAuxiliaryToday;
class UIMenu;
class UIAuxiliaryWnd : public QWidget
{
	Q_OBJECT

public:
	UIAuxiliaryWnd(QWidget *parent = 0);
	~UIAuxiliaryWnd();

	// 鼠标按下拖动的操作
	QPoint							m_startPos;
	QPoint							m_clickPos;
	QPoint							m_WndCurPos;

	// 全部辅导布局
	QWidget*						m_mainAllView;
	QVBoxLayout*					m_VerAll;
	QSpacerItem*					m_spacerAll;

	// 今日课程布局
	QWidget*						m_mainTodayView;
	QVBoxLayout*					m_VerToday;
	QSpacerItem*					m_spacerToday;
	QPoint							last;
	QMenu*							m_menu;
	UIMenu*							m_UIMenu;
	QString							m_strStudentName;
private:
	Ui::UIAuxiliaryWnd ui;
	UIMainWindow*		m_parent;
	Worker*				m_pWorker;
	std::vector<UIAuxiliaryList*>	m_vecAuxiliaryList;			// 辅导班集合

	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryChatID;	// 会话窗口ID、辅导班
	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryCourseID;	// 辅导班ID、  辅导班
	QMap<QString, QLabel*>				m_mapUrl;				// url、   辅导班图片	

private:
	void style(QScrollArea *style);

signals:
	void sig_StartLoading();
	void sig_Close();

public slots :
	void	clickAuxiliary(UIAuxiliaryList*);
	void    clickAuxiliaryToday(UIAuxiliaryToday*);
	void    clickToday();
	void    clickAll();
	void	MinDialog();
	void	CloseDialog();
	void    clickPic();
	void    ReturnAccount();

protected:
	virtual bool eventFilter(QObject *target, QEvent *event);
	virtual void paintEvent(QPaintEvent *event);
//	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

public:
	void setMainWindow(UIMainWindow* parent);
	void AddAuxiliary(QString picUrl, QString courseName, QString grade, QString teacherName,QString chatID, QString courseID, 
			QString teacherID, QString token, QString studentName, std::string AudioPath, QString status);				// 添加全部辅导班
	void AddTodayAuxiliary(QString lessonName, QString courseID, QString courseName, QString time, QString status);		// 添加今日课程
	void AddTodayNoLesson();																			// 今日无课程
	QPixmap setStudentUrl(QString url);
	void ReceiverNumber(QString chatID);
	void LoadPic();
	void setStudentName(QString studentName);
	void setVersion(QString version);
	void SetEnvironmental(bool m_EnvironmentalTyle);
	void SetToken(QString mRemeberToken);
};

#endif // UIAUXILIARYWND_H
