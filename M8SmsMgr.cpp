#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include "M8SmsMgr.h"
// The global variable of the application.
M8SmsMgr theApp;

BOOL M8SmsMgr::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);

	//检测程序是否已经运行
	HWND pWnd = isRuning();
	if(pWnd)
	{
		SetForegroundWindow(pWnd);
		PostMessage(pWnd,WM_NULL,NULL,NULL);
		PostQuitMessage(0);
		return true; 
	}

	// Create the main window
	RECT rcWork = MzGetWorkArea();
	m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
	m_MainWnd.Show();

    // return TRUE means init success.
    return TRUE;
}