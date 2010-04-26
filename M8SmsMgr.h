#include "ui_main.h"

#define APPNAME L"短信管理"
#ifdef _DEBUG
#define CONFIG_INI L"Program Files\\M8SmsMgr\\M8SmsMgr.ini"
#else
#define CONFIG_INI L"\\Disk\\Programs\\M8SmsMgr\\M8SmsMgr.ini"
#endif

// Application class derived from CMzApp
class M8SmsMgr : public CMzApp {
public:
	Ui_MainWnd m_MainWnd;
	// Initialization of the application
	virtual BOOL Init();
	HWND isRuning(){
		//正常启动程序
		//检测程序是否已经运行
		HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8SmsMgr");
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND pWnd = FindWindow(m_MainWnd.GetMzClassName(),NULL);
			return pWnd; 
		}
		return 0;
	}
	virtual int Done();
private:
    //载入资源
    void loadImageRes();
    void LoadLangRes();
    void LoadRes();
private:
	bool isExternLangres;
};
