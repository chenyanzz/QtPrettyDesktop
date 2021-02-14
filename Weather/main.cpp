#include "Weather.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Weather w;
	HWND desktopHwnd = ::GetDesktopWindow();

	// 设置桌面为父窗口
	if (desktopHwnd) {
		::SetParent((HWND)w.winId(), desktopHwnd);
	}

    w.show();
    return a.exec();
}
