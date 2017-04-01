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

private:
    int mId;
    QString mName;			// �γ�����
	QString mStatus;		// �γ�״̬
	QString mStartTime;		// �Ͽ�ʱ��
	QString mCourseID;		// ������ID
	QString mCourseName;	// ����������
	QString	mDate;			// ���� ��2017-2-28��
};

#endif // LESSON_H
