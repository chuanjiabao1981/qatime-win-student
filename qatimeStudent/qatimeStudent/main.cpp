#include "loginwindow.h"
#include <QtWidgets/QApplication>
#include <QIcon>
#include <Windows.h>
#include <gdiplus.h>
#include "UIMessageBox.h"
#include "UIWindowSet.h"

using namespace Gdiplus;

int main(int argc, char *argv[])
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, TEXT("QtStudentLive_Mutex"));
	//已存在互斥量说明在运行  
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hMutex);
		hMutex = NULL;
		return 0;
	}

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	QApplication a(argc, argv);
	QIcon qIcon("./images/favicon.png");
	a.setWindowIcon(qIcon);

	//初始化GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	LoginWindow w;
	w.setWindowFlags(Qt::FramelessWindowHint);
	w.ReadSetting();

	if (w.IsAutoLogin())
		w.hide();
	else
		w.show();

// 	UIWindowSet w;
// 	w.setWindowFlags(Qt::FramelessWindowHint);
// 	w.show();

	return a.exec();
}
