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
    QString		mName;				// 辅导班名称
	QString		mStatus;
	QString		mProgress;			// 进度
	QString		mURL;				// 课程推流的URL
	QString		mCameraURL;			// 摄像头推流地址		
	QString		mChatId;			// 云信会话ID
	QString		mOwnerId;			// 自身ID
	QString     mGrade;				// 科目
	QString     mTeacherName;		// 老师名字
	QString		mPic;				// 辅导班图片地址
};

#endif // COURSE_H
