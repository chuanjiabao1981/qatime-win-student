#include "course.h"

Course::Course()
{

}


void Course::readJson(const QJsonObject &json)
{
    mId = json["id"].toInt();
    mName = json["name"].toString();
	mStatus = json["status"].toString();
	mChatId = json["chat_team_id"].toString();
	mOwnerId = json["chat_team_owner"].toString();

	mGrade = json["grade"].toString();
	mTeacherName = json["teacher_name"].toString();
	mPic = json["publicize"].toString();

	// ����
	int preset_lesson = json["preset_lesson_count"].toInt();
	int completed_lesson = json["completed_lesson_count"].toInt();
	mProgress = QString::number(completed_lesson);
	mProgress += "/";
	mProgress += QString::number(preset_lesson);

	// �װ�������ַ
	mURL = json["board_pull_stream"].toString();

	// ����ͷ������ַ
	mCameraURL = json["camera_pull_stream"].toString();
}

QString Course::url()
{
	return mURL;
}

QString Course::CameraURL()
{
	return mCameraURL;
}

QString Course::id()
{
    return QString::number(mId);
}

QString Course::name() const
{
    return mName;
}

QString Course::progress() const
{
	return mProgress;
}

QString Course::status()
{
    return mStatus;
}

QString Course::ChatId()
{
	return mChatId;
}

QString Course::OwnerId()
{
	return mOwnerId;
}

QString Course::Grade()
{
	return mGrade;
}

QString Course::TeacherName()
{
	return mTeacherName;
}

QString Course::PicUrl()
{
	return mPic;
}
