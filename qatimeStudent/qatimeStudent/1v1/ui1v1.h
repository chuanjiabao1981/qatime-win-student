#ifndef UI1V1_H
#define UI1V1_H

#include <QWidget>
#include "ui_ui1v1.h"

//互动直播
class Palette;
class UICamera1v1;
class UICameraS1v1;
class UIVideoChange1v1;
class UIAudioChange1v1;
class UIAudioOutChange1v1;
class UIWhiteBoardTool;
class UIVideo1v1;
class UIAppWnd;
class UIAppWndTool;

class UI1v1 : public QWidget
{
	Q_OBJECT

public:
	UI1v1(QWidget *parent = 0);
	~UI1v1();

	void	createRtsRoom(const QString &roomName, const QString &roomInfo = "");// 创建白板链接
	void	InitSetParamWnds();										  // 打开摄像头参数、麦克风等窗口
	void	initWhiteBoardWidget();									  // 初始化白板
	void	initConnection();										  // 初始化白板接口
	void	show1v1ErrorTip(QString sError);
	QLayout* chat1v1Widget();
	void	initDevice();

private slots:
	/*互动直播*/
	void joinRtsRoom(const std::string &);					// 加入白板房间
	void joinRoomSuccessfully(const std::string &, __int64, const std::string &);// 加入白板房间成功
	//void joinVChatRoom();									// 加入音视频
	void joinVChatSuccessfully();							// 加入音视频房间成功
	void errorInfo(const QString &);						// 加入失败错误信息
	void PicData(QString);									// 白板数据
	void setDeviceInfos(int);								// 设备参数
	void clickVideo1v1Param();								// 摄像头参数
	void clickAudio1v1Param();								// 麦克风参数
	void clickAudioOut1v1Param();							// 扬声器参数
	void Audio1v1Status(int iStatus);						// 开启关闭麦克风
	void Video1v1Status(int iStatus);						// 开启关闭摄像头
	void AudioOut1v1Status(int);							// 开启关闭扬声器
	void selectColor(QColor&);								// 颜色器
	void returnClick();										// 撤销上一步
	void rtsDataReceived(const std::string& data);			// 接收白板数据

	void setVideoChange1v1(QString path);				    // 设置一对一摄像头
	void setAudioChange1v1(QString path);				    // 设置一对一麦克风
	void setAudioOutChange1v1(QString path);				// 设置一对一扬声器
	void setValueChange1v1(int iVolumn, bool capturn);		// 设置麦克风、扬声器音量
private:
	Ui::UI1v1 ui;

	/***************************互动直播*****************************/
	bool							m_bLiving1v1;			// 1对1直播标识
	Palette*						mWhiteBoard;			// 画板
	UICamera1v1*					m_Camera1v1Info;		// 1对1摄像头
	UICameraS1v1*					m_CameraS1v1Info;		// 1对1教师摄像头
	UIVideoChange1v1*				m_VideoChangeInfo1v1;	// 摄像头选择窗口
	UIAudioChange1v1*				m_AudioChangeInfo1v1;	// 麦克风选择窗口
	UIAudioOutChange1v1*			m_AudioOutChangeInfo1v1;// 扬声器选择窗口
	UIWhiteBoardTool*				m_WhiteBoardTool;		// 白板操作
	UIVideo1v1*						m_VideoInfo1v1;			// 1v1全屏桌面

	std::string						m_RoomName;				//
};

#endif // UI1V1_H
