#ifndef YXMember_H
#define YXMember_H

#include <QJsonObject>
#include <QJsonArray>

class YXMember
{
public:
    YXMember();

	void		readJsonToMember(const QJsonObject &json);
	QString		accid();
    QString		name();
	QString		url();

	void		readJsonToAnnouncement(const QJsonObject &json);
	QString		announcement();
	QString		announcementTime();

private:
	QString		mAccid;				// ��Ա�û�ID
    QString		mName;				// ��Ա����
	QString		mIconUrl;			// ��Աͷ��url
	
	QString		mAnnouncement;		// ��������
	QString		mTime;				// ����ʱ��
};

#endif // YXMember_H
