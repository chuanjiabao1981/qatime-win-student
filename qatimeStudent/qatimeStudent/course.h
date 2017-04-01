#ifndef COURSE_H
#define COURSE_H

#include <QJsonObject>
#include <QJsonArray>

class Course
{
public:
    Course();

    QString		id();
    QString		name() const;
	QString		progress() const;
	QString		status();
    void		readJson(const QJsonObject &json);
	QString		url();
	QString		CameraURL();
	QString		ChatId();
	QString     OwnerId();
	QString		Grade();
	QString		TeacherName();
	QString		PicUrl();
private:
    int			mId;
    QString		mName;				// ����������
	QString		mStatus;
	QString		mProgress;			// ����
	QString		mURL;				// �γ�������URL
	QString		mCameraURL;			// ����ͷ������ַ		
	QString		mChatId;			// ���ŻỰID
	QString		mOwnerId;			// ����ID
	QString     mGrade;				// ��Ŀ
	QString     mTeacherName;		// ��ʦ����
	QString		mPic;				// ������ͼƬ��ַ
};

#endif // COURSE_H
