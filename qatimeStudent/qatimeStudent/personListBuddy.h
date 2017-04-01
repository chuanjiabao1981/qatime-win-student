#ifndef PERSONLISTBUDDY_H  
#define PERSONLISTBUDDY_H  
#include <QWidget>  
#include <QLabel>  
#include <QEvent>  
#include <QCheckBox>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLineEdit>
#include <QPushButton>

//�Զ�����ϢItem��  
class personListBuddy : public QWidget  
{  
    Q_OBJECT  
public:  
    explicit personListBuddy(QWidget *parent = 0);  
	~personListBuddy();
	void initFirst();			//��ʼ����һ�� 
	void initFindPeople(int iWidth);		//��ʼ���ڶ���
	void initNotFind(int iWidth);			//��ʼ��û���ҵ�������ʾ
	QLabel *peopleNum;			//��һ�е�label
	QCheckBox* firstButton;		//��һ�еİ�ť
	QLineEdit* secLinEdit;		//�ڶ��е������
	QPushButton* secFindButton;	//�ڶ��еĲ��Ұ�ť
	QLabel *notFind;			//���һ�С�û���ҵ����ѵ�ʱ����ʾ��
	QLabel* initUi(const QString &szUrl, QString strName, QString ID, int iWidth);//��ʼ��Ui
	QLabel *head;  //ͷ��label  
    QLabel *name;  //�û���  
	bool isOline;	//�Ƿ�����
	QString m_ID;		//�û�ID
	QPixmap headPath;//��Ҫ���ص�ͷ��
	QCheckBox* button; //��ť  
	bool	m_bZoom;
//  QLabel *sign;  //����ǩ��  
	void setOlineNum(int olineNum, int allNum);	
	void setCheckBox(bool bCheck);
	bool IsZoom();
	void SetZoom(bool bZoom);
	void setMute(bool bMute);
protected:
	virtual bool eventFilter(QObject *target, QEvent *event);
signals:  
	void emitRadioChange(int,QString,QString);
	void signalRadioChange(bool);
	void signalStopAllTalk(bool,QString);	//��������ѧ���ź�
 	void signalFindName(const QString name);		//����ѧ�������ź�
	void signalFindNameNULL(const QString name);
	// 	void signalOnlyOnline(bool);
public slots :
	void radioChange(int b);			//��ֹĳ��ѧ��������	
	void stopAllTalk(bool b);			//��ֹ����ѧ��������
	void onlyOnline(bool b);			//ֻ��ʾ������Ա
	void findName();					//����ѧ������
	void findName(const QString);		//������ѧ���༭��ΪNULLʱ����ʾ����ѧ������
private:
	void setNetworkPic(const QString &szUrl);
//	bool eventFilter(QObject *obj, QEvent *event);//�¼������� 
};  
#endif // PERSONLISTBUDDY_H  