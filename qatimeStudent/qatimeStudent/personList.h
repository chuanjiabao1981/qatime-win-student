#ifndef PERSONLIST_H
#define PERSONLIST_H
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "personlistbuddy.h"
#include <QTimer>
//�Զ���QListWidget
class personList : public QListWidget 
{
    Q_OBJECT
public:
    explicit personList(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);		//������¼�
    void contextMenuEvent(QContextMenuEvent*);		//�˵��¼���Ϊ����ʾ�˵�
    void initMenu();//��ʼ���˵�
    QMenu *blankMenu;//����հ��ϵĲ˵�
    QMenu *groupMenu;//������ϵĲ˵�
    QMenu *personMenu;//������ϵĲ˵�
	QMap<QListWidgetItem*, personListBuddy*> groupMap;   // ������ - key:�� value:��
	QMap<QString, QListWidgetItem*> allStudents;
	QMap<QString, QListWidgetItem*> IdStudents;
    QMap<QListWidgetItem*,bool> isHideMap;//�����жϸ����Ƿ�������
    QLineEdit *groupNameEdit;//������֣���������ʱ����Ҫ�õ�
    QListWidgetItem *currentItem;//��ǰ����
	std::vector<personListBuddy*>	m_vecBuddy;	// ��Ա�б�
	QLabel* addStrdent(QString imagePath, QString stuName, QString ID, int iWidth);
	void setStrdentNumbers(int num);
	void initFronUi();
	void initSecUi(int iWidth);
	void setOlineNum(int olineNum, int allNum);//�������������Ľӿ�
	void cleanStudents(int iWidth);		//����б��е�����
	void setAllWidth(int iWidth);
	void setZoomWidth(int iWidth);
	void setBuddyWidth(int iWidth);
	std::vector<personListBuddy*> GetBuddy();
signals:
	void signalChickChage(int,QString, QString);
public slots:
    void slotAddGroup();   //�����
    void slotDelGroup();   //ɾ����
    void slotAddBuddy();   //��Ӻ���
    void slotDelBuddy();   //ɾ������
    void slotRename();     //��������
	void findName(const QString name);//���Һ���
	personListBuddy* findID(const QString id);	  //����ID
    void slotRenameEditFshed();//�������

	void chickChage(int b,QString ID, QString name);	//���Ե�״̬�����ı�
	void stopAllTalk(bool, QString ID); //�������е�ѧ��
	void timeOutSet();
private:
	personListBuddy *buddyFirst;
	personListBuddy *buddySec;
	personListBuddy *buddyLast;
	QTimer *timer;//������ʾ����ʱ��
	QLabel* backImage;
};
#endif // PERSONLIST_H