#include "lesson.h"
#include "define.h"
#include <QJsonObject>

Lesson::Lesson()
{

}

void Lesson::readJson(const QJsonObject &json)
{
	// 课程ID
    mId = json["id"].toInt();

	// 课程名字
    mName = json["name"].toString();

	// 课程状态
	mStatus = json["status"].toString();

	// 上课时间
	mStartTime = json["live_time"].toString();
	mStartTime = mStartTime.mid(0, 5);

	// 辅导班ID
	mCourseID = json["course_id"].toString();

	// 辅导班名字
	mCourseName = json["course_name"].toString();

	// 日期
	mDate = json["class_date"].toString();

	QJsonObject obj = json["product_interactive_course"].toObject();
	if (obj.empty())
		mb1v1 = false;
	else
		mb1v1 = true;
}

bool Lesson::Is1v1()
{
	return mb1v1;
}

QString Lesson::Date()
{
	return mDate;
}

QString Lesson::CourseName()
{
	return mCourseName;
}

QString Lesson::CourseID()
{
	return mCourseID;
}

QString Lesson::name()
{
    return mName;
}

QString Lesson::LessonTime()
{
	return mStartTime;
}

QString Lesson::LessonStatus()
{
	return mStatus;
}

QString Lesson::LessonID()
{
	return QString::number(mId);
}

QString Lesson::ChinaLessonStatus()
{
	QString strChinaStatus;
	if (mStatus == "init")
	{
		strChinaStatus = MSG_LESSON_STATUS_INIT;
	}
	else if (mStatus == "ready")
	{
		strChinaStatus = MSG_LESSON_STATUS_READY;
	}
	else if (mStatus == "teaching")
	{
		strChinaStatus = MSG_LESSON_STATUS_THEACHING;
	}
	else if (mStatus == "paused")
	{
		strChinaStatus = MSG_LESSON_STATUS_PAUSED;
	}
	else if (mStatus == "closed")
	{
		strChinaStatus = MSG_LESSON_STATUS_CLOSED;
	}
	else if (mStatus == "finished")
	{
		strChinaStatus = MSG_LESSON_STATUS_FINISHED;
	}
	else if (mStatus == "billing")
	{
		strChinaStatus = MSG_LESSON_STATUS_BILLING;
	}
	else if (mStatus == "completed")
	{
		strChinaStatus = MSG_LESSON_STATUS_COMPLETED;
	}
	else if (mStatus == "missed")
	{
		strChinaStatus = MSG_LESSON_STATUS_MISSED;
	}

	return strChinaStatus;
}