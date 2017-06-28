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

	// ��갴���϶��Ĳ���
	QPoint							m_startPos;
	QPoint							m_clickPos;
	QPoint							m_WndCurPos;

	typedef enum enum_LESSON_TYPE{
		EN_TODAY_LESSON = 0,                  //!< ����ֱ��
		EN_ALL_LESSON,                       //!< ȫ������
		EN_1V1_LESSON                       //!< 1��1ֱ��
	}EN_LESSON_TYPE;

	// ȫ����������
	QWidget*						m_mainAllView;
	QVBoxLayout*					m_VerAll;
	QSpacerItem*					m_spacerAll;

	// ���տγ̲���
	QWidget*						m_mainTodayView;
	QVBoxLayout*					m_VerToday;
	QSpacerItem*					m_spacerToday;
	QPoint							last;
	QMenu*							m_menu;
	UIMenu*							m_UIMenu;
	QString							m_strStudentName;

	// һ��һ��������
	QWidget*						m_oneToOneView;
	QVBoxLayout*					m_VerOneToOne;
	QSpacerItem*					m_spacerOneToOne;
private:
	Ui::UIAuxiliaryWnd ui;
	UIMainWindow*		m_parent;
	Worker*				m_pWorker;
	std::vector<UIAuxiliaryList*>	m_vecAuxiliaryList;				// �����༯��

	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryChatID;		// �Ự����ID��������
	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryCourseID;		// ������ID��  ������
	QMap<QString, UIAuxiliaryList*>		m_mapAuxiliaryCourse1v1ID;	// 1v1������ID��  ������
	QMap<QString, QLabel*>				m_mapUrl;					// url��   ������ͼƬ	

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
			QString teacherID, QString studentName, std::string AudioPath, QString status);				// ���ȫ��������
	void AddTodayAuxiliary(QString lessonName, QString courseID, QString courseName, QString time, QString status, bool b1v1);		// ��ӽ��տγ�
	void AddOneToOneAuxiliary(QString picUrl, QString courseName, QString grade, QString teacherName, QString chatID, QString courseID,
		QString teacherID, QString studentName, std::string AudioPath, QString status);					// ���һ��һ����
	void AddTodayNoLesson(EN_LESSON_TYPE type);																			// �����޿γ�
	QPixmap setStudentUrl(QString url);
	void ReceiverNumber(QString chatID);
	void LoadPic();
	void setStudentName(QString studentName);
	void setVersion(QString version);
};

#endif // UIAUXILIARYWND_H
