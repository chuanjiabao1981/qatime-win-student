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

	typedef enum enum_LESSON_TYPE{
		EN_TODAY_LESSON = 0,                  //!< 今日直播
		EN_ALL_LESSON,                       //!< 全部辅导
		EN_1V1_LESSON                       //!< 1对1直播
	}EN_LESSON_TYPE;

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

	// 一对一互动布局
	QWidget*						m_oneToOneView;
	QVBoxLayout*					m_VerOneToOne;
	QSpacerItem*					m_spacerOneToOne;
private:
	Ui::UIAuxiliaryWnd ui;
	UIMainWindow*		m_parent;
	Worker*				m_pWorker;
	std::vector<UIAuxiliaryList*>	m_vecAuxiliaryList;				// 辅导班集合

	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryChatID;		// 会话窗口ID、辅导班
	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryCourseID;		// 辅导班ID、  辅导班
	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryCourse1v1ID;	// 1v1辅导班ID、  辅导班
	QMap<QString, QLabel*>				m_mapUrl;					// url、   辅导班图片	

private:
	void style(QScrollArea *style);

signals:
	void sig_StartLoading();
	void sig_Close();

public slots :
	void	clickAuxiliary(UIAuxiliaryList*);
	void    clickAuxiliaryOneToOne(UIAuxiliaryList*);
	void    clickAuxiliaryToday(UIAuxiliaryToday*);
	void    clickToday();
	void    clickAll();
	void    clickOneToOne();
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
			QString teacherID, QString studentName, std::string AudioPath, QString status);				// 添加全部辅导班
	void AddTodayAuxiliary(QString lessonName, QString courseID, QString courseName, QString time, QString status, bool b1v1);		// 添加今日课程
	void AddOneToOneAuxiliary(QString picUrl, QString courseName, QString grade, QString teacherName, QString chatID, QString courseID,
		QString teacherID, QString studentName, std::string AudioPath, QString status);					// 添加一对一互动
	void AddTodayNoLesson(EN_LESSON_TYPE type);																			// 今日无课程
	QPixmap setStudentUrl(QString url);
	void ReceiverNumber(QString chatID);
	void LoadPic();
	void setStudentName(QString studentName);
	void setVersion(QString version);
};

#endif // UIAUXILIARYWND_H
