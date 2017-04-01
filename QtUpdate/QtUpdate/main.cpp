#include "qtupdate.h"
#include <QtWidgets/QApplication>
#include "windows.h"
#include "string.h"
#include <TCHAR.H>
#include <QLatin1String>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex mutex;
	mutex.lock();

	QString text;
	switch (type)
	{
	case QtDebugMsg:
		text = QString("Debug:");
		break;

	case QtWarningMsg:
	{
		text = QString("Warning:");
		mutex.unlock();
		return;
		break;
	}
	case QtCriticalMsg:
		text = QString("Critical:");
		break;

	case QtFatalMsg:
		text = QString("Fatal:");
	}

	QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
	QString current_date = QString("(%1)").arg(current_date_time);
	QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

	QFile file("log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << message << "\r\n";
	file.flush();
	file.close();

	mutex.unlock();
}

static BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}

static std::string WTOMB(const std::wstring & astr)
{
	std::string lRet;
	int liLen = astr.size() * 2 + 10;
	char * buff = new char[liLen];
	if (buff == 0)
		return "";
	memset(buff, 0, liLen);
	WCharToMByte(astr.c_str(), buff, liLen - 1);
	lRet = buff;
	delete[] buff;
	return lRet;
}

int main(int argc, char *argv[])
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, TEXT("QtStudentUpdate_Mutex"));
	//已存在互斥量说明在运行  
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hMutex);
		hMutex = NULL;
		return 0;
	}

	//注册MessageHandler
	qInstallMessageHandler(outputMessage);

	// 获取命令行参数
	LPWSTR lpCmdLine = GetCommandLine();
	TCHAR szCmdLine[1024] = { 0 };
	if (lpCmdLine != NULL)
		lstrcpy(szCmdLine, lpCmdLine);

//	lstrcpy(szCmdLine, L" -downpath http://qatime-production.oss-cn-beijing.aliyuncs.com/download/software/windows/qatime-student-release-0.3.3-win.zip -version 0.3.3 -update 1");
	qDebug() << WTOMB(lpCmdLine).c_str();
	QString downPath;
	QString version;
	QString update;
	if (szCmdLine != NULL )
	{
		TCHAR strDownLoadPth[_MAX_PATH] = { 0 };
		TCHAR strVersion[_MAX_PATH] = { 0 };
		TCHAR strUpdate[_MAX_PATH] = { 0 };
		TCHAR *token = wcstok(szCmdLine, L" ");
		TCHAR seps[] = L" ";
		while (token != NULL)
		{
			//命令行参数值获取
			if (wcscmp(token, L"-downpath") == 0)
				wcscpy_s(strDownLoadPth, wcstok(NULL, L" "));
			if (wcscmp(token, L"-version") == 0)
				wcscpy_s(strVersion, wcstok(NULL, L" "));
			if (wcscmp(token, L"-update") == 0)
				wcscpy_s(strUpdate, wcstok(NULL, L" "));
			
			token = wcstok(NULL, seps);
		}

		downPath = QString::fromStdString((WTOMB)(strDownLoadPth));
		version = QString::fromStdString((WTOMB)(strVersion));
		update = QString::fromStdString((WTOMB)(strUpdate));
	}

	QApplication a(argc, argv);
	QtUpdate w;
	w.setWindowFlags(Qt::FramelessWindowHint);
	w.setLoadPath(downPath);
	w.setVersion(version);
	w.show();
	w.setUpdate(update);
	SetWindowPos((HWND)w.winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	return a.exec();
}
