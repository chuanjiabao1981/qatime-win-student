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

	// 进度
	int preset_lesson = json["preset_lesson_count"].toInt();
	int completed_lesson = json["completed_lesson_count"].toInt();
	mProgress = QString::number(completed_lesson);
	mProgress += "/";
	mProgress += QString::number(preset_lesson);

	// 白板拉流地址
	mURL = json["board_pull_stream"].toString();

	// 摄像头拉流地址
	mCameraURL = json["camera_pull_stream"].toString();
}

OTO_INFO Course::getOneToOneInfoFromJson(const QJsonObject &json)
{
	OTO_INFO info;
	info.status = json["status"].toInt();
	QJsonArray data = json["data"].toArray();
	foreach(const QJsonValue &value, data) {
		QJsonObject obj = value.toObject();
		OTO_DATA otoData = getOneToOneDataFromJson(obj);
		info.data.append(otoData);
	}

	return info;
}

OTO_DATA Course::getOneToOneDataFromJson(const QJsonObject &json)
{
	OTO_DATA data;
	data.id							= json["id"].toInt();
	data.name						= json["name"].toString();
	data.subject					= json["subject"].toString();
	data.grade						= json["grade"].toString();
	data.price						= json["price"].toString();
	data.status						= json["status"].toString();
	data.description				= json["description"].toString();
	data.lessons_count				= json["lessons_count"].toInt();
	data.completed_lessons_count	= json["completed_lessons_count"].toInt();
	data.closed_lessons_count		= json["closed_lessons_count"].toInt();
	data.live_start_time			= json["live_start_time"].toString();
	data.live_end_time				= json["live_end_time"].toString();
	data.objective					= json["objective"].toString();
	data.suit_crowd					= json["suit_crowd"].toString();
	data.publicize_url				= json["publicize_url"].toString();
	data.publicize_info_url			= json["publicize_info_url"].toString();
	data.publicize_list_url			= json["publicize_list_url"].toString();
	data.publicize_app_url			= json["publicize_app_url"].toString();
	data.chat_team_id				= json["chat_team_id"].toString();
	data.created_at					= json["created_at"].toInt();
	data.preview_time				= json["preview_time"].toInt();
	data.is_bought					= json["is_bought"].toBool();
	QJsonArray teachers				= json["teachers"].toArray();
	foreach(const QJsonValue &value, teachers) {
		QJsonObject obj = value.toObject();
		OTO_TEACHER otoTeacher = getOneToOneTeacherFromJson(obj);
		data.teachers.append(otoTeacher);
	}

	return data;
}

OTO_TEACHER Course::getOneToOneTeacherFromJson(const QJsonObject &json)
{
	OTO_TEACHER teacher;
	teacher.id					= json["id"].toInt();
	teacher.name				= json["name"].toString();
	teacher.nick_name			= json["nick_name"].toString();
	teacher.avatar_url			= json["avatar_url"].toString();
	teacher.ex_big_avatar_url	= json["ex_big_avatar_url"].toString();
	teacher.login_mobile		= json["login_mobile"].toString();
	teacher.email				= json["email"].toString();
	teacher.teaching_years		= json["teaching_years"].toString();
	teacher.category			= json["category"].toString();
	teacher.subject				= json["subject"].toString();
	teacher.gender				= json["gender"].toString();
	teacher.birthday			= json["birthday"].toString();
	teacher.province			= json["province"].toInt();
	teacher.city				= json["city"].toInt();
	teacher.school				= json["school"].toInt();
	teacher.desc				= json["desc"].toString();
	QJsonArray array			= json["grade_range"].toArray();
	foreach(const QJsonValue &value, array) {
		teacher.grade_range.append(value.toString());
	}

	return teacher;
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
