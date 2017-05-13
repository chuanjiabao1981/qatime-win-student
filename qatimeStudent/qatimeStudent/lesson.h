#ifndef LESSON_H
#define LESSON_H

#include <QJsonObject>

#pragma execution_character_set("utf-8")

class Lesson
{
public:
    Lesson();
    void readJson(const QJsonObject &json);
    QString name();
	QString LessonTime();
	QString LessonStatus();
	QString ChinaLessonStatus();
	QString LessonID();
	QString CourseID();
	QString CourseName();
	QString Date();
	bool	Is1v1();

private:
    int mId;
    QString mName;			// 课程名称
	QString mStatus;		// 课程状态
	QString mStartTime;		// 上课时间
	QString mCourseID;		// 辅导班ID
	QString mCourseName;	// 辅导班名字
	QString	mDate;			// 日期 （2017-2-28）
	bool	mb1v1;			// 是否是一对一直播课
};

#endif // LESSON_H
