#ifndef COURSE_H
#define COURSE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QList>

typedef struct ONETOONE_TEACHER
{
	int id;
	QString name;
	QString nick_name;
	QString avatar_url;
	QString ex_big_avatar_url;
	QString login_mobile;
	QString email;
	QString teaching_years;
	QString category;
	QString subject;
	QStringList grade_range;
	QString gender;
	QString birthday;
	int province;
	int city;
	int school;
	QString desc;
}OTO_TEACHER;

typedef struct ONEONONE_DATA
{
	int id;
	QString name;
	QString subject;
	QString grade;
	QString price;
	QString status;
	QString description;
	int lessons_count;
	int completed_lessons_count;
	int closed_lessons_count;
	QString live_start_time;
	QString live_end_time;
	QString objective;
	QString suit_crowd;
	QString publicize_url;
	QString publicize_info_url;
	QString publicize_list_url;
	QString publicize_app_url;
	QString chat_team_id;
	QList<OTO_TEACHER> teachers;
	int created_at;
	int preview_time;
	bool is_bought;
}OTO_DATA;

typedef struct ONETOONE_INFO
{
	int status;
	QList<OTO_DATA> data;
}OTO_INFO;

typedef struct STATUS_DATA
{
	int id;
	int board;
	int camera;
	int t;
	int timestamp;
	QString name;
	QString status;
	QString room_id;
	QStringList online_users;
}STATUS_DATA;

typedef struct ONETOONE_STATUS
{
	int status;
	STATUS_DATA data;
}OTO_STATUS;

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

	//获取1v1互动课程信息
	static OTO_INFO getOneToOneInfoFromJson(const QJsonObject &json);
	static OTO_DATA getOneToOneDataFromJson(const QJsonObject &json);
	static OTO_TEACHER getOneToOneTeacherFromJson(const QJsonObject &json);

	//获取1v1互动状态
	static OTO_STATUS getOneToOneStatusFromJson(const QJsonObject &json);
	static STATUS_DATA getStatusDataFromJson(const QJsonObject &json);
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
