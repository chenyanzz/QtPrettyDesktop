#include "Weather.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Weather w;
	HWND desktopHwnd = ::GetDesktopWindow();

	// ��������Ϊ������
	if (desktopHwnd) {
		::SetParent((HWND)w.winId(), desktopHwnd);
	}

    w.show();
    return a.exec();
}
