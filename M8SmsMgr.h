#define VER_STRING L"1.00"
#ifdef MZFC_STATIC
#define BUILD_STRING L"20090625(S)"
#else
#define BUILD_STRING L"20090625(D)"
#endif
#define APPNAME L"���Ź���"
#ifdef _DEBUG
#define CONFIG_INI L"Program Files\\M8SmsMgr\\M8SmsMgr.ini"
#else
#define CONFIG_INI L"\\Disk\\Programs\\M8SmsMgr\\M8SmsMgr.ini"
#endif
// Application class derived from CMzApp
class M8SmsMgr : public CMzApp {
public:
    // Initialization of the application
    virtual BOOL Init();
	HWND isRuning(){
		//������������
		//�������Ƿ��Ѿ�����
		HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8SmsMgr");
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND pWnd=0;//FindWindow(m_MainWnd.GetMzClassName(),NULL);
			return pWnd; 
		}
		return 0;
	}
};
