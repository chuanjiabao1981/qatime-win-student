/*
*	Author       hzzhuling15
*	Date         2015/09/11
*	Copyright    Hangzhou, Netease Inc.
*	Brief        
*   Notice       
*/
#include "UIWorkThread.h"
#include <QPixmap>
#include "quazip/JlCompress.h"


Worker::Worker() 
{
	workerThread.start();
	this->moveToThread(&workerThread);
	m_bClose = false;
	m_name = "";
}

Worker::~Worker()
{
	
}

// 不需要更新的文件
void Worker::initNoupdateFile()
{
	m_FilesName.append("imageformats/qdds.dll");
	m_FilesName.append("imageformats/qgif.dll");
	m_FilesName.append("imageformats/qicns.dll");
	m_FilesName.append("imageformats/qico.dll");
	m_FilesName.append("imageformats/qjpeg.dll");
	m_FilesName.append("imageformats/qsvg.dll");
	m_FilesName.append("imageformats/qtga.dll");
	m_FilesName.append("imageformats/qtiff.dll");
	m_FilesName.append("imageformats/qwbmp.dll");
	m_FilesName.append("imageformats/qwebp.dll");

	m_FilesName.append("platforms/qminimal.dll");
	m_FilesName.append("platforms/qminimald.dll");
	m_FilesName.append("platforms/qoffscreen.dll");
	m_FilesName.append("platforms/qoffscreend.dll");
	m_FilesName.append("platforms/qwindows.dll");
	m_FilesName.append("platforms/qwindowsd.dll");

	m_FilesName.append("plugins/NPSWF32.dll");
	m_FilesName.append("plugins/NPSWF32_11_9_900_170.dll");
	m_FilesName.append("plugins/pepflashplayer.dll");

	m_FilesName.append("cef.pak");
	m_FilesName.append("d3dcompiler_43.dll");
	m_FilesName.append("d3dcompiler_46.dll");
	m_FilesName.append("devtools_resources.pak");
	m_FilesName.append("icudt.dll");
	m_FilesName.append("lame_enc.dll");
	m_FilesName.append("libcef.dll");
	m_FilesName.append("libeay32.dll");
	m_FilesName.append("libEGL.dll");
	m_FilesName.append("libGLESv2.dll");
	m_FilesName.append("msvcp120.dll");
	m_FilesName.append("msvcr120.dll");
	m_FilesName.append("nim.dll");
	m_FilesName.append("nim_audio.dll");
	m_FilesName.append("nim_tools_http.dll");
	m_FilesName.append("nrtc.dll");
	m_FilesName.append("Qt5Core.dll");
	m_FilesName.append("Qt5Gui.dll");
	m_FilesName.append("Qt5Network.dll");
	m_FilesName.append("Qt5Widgets.dll");
	m_FilesName.append("ssleay32.dll");
	m_FilesName.append("QtUpdate.exe");
	m_FilesName.append("quazip.dll");
}

void Worker::slot_StartStep()
{
	initNoupdateFile();

	QStringList Files = JlCompress::getFileList(m_name);
	QList<QString>::Iterator it = Files.begin(), itend = Files.end();
	int iTotal = Files.size();
	int i = 0;
	for (; it != itend; it++, i++)
	{
		QString filename = *it;
		if (m_FilesName.indexOf(filename) == -1)
		{
			JlCompress::extractFile(m_name, filename, filename);
		}

		if (m_bClose)
			return;

		onStepProgress(i + 1, iTotal);
	}

	emit sig_ResultReady();
}

void Worker::slot_Close()
{
	m_bClose = true;
}

void Worker::onStepProgress(qint64 bytesSent, qint64 bytesTotal)
{
	double total = bytesTotal;
	double sent = bytesSent;
	QString strProgress;
	strProgress = QString("安装中(%1个/%2个)").arg(QString::number(sent)).arg(QString::number(total));
	emit sig_Progress(strProgress, bytesSent, bytesTotal);
}

void Worker::setCompressName(QString name)
{
	m_name = name;
}