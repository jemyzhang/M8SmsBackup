#include <MzCommon.h>
using namespace MzCommon;

#include "M8SmsMgr.h"
#include "resource.h"
#include "appconfigini.h"
#include "ui_ProgressBar.h"
#include "cmdline.h"

// The global variable of the application.
M8SmsMgr theApp;
HINSTANCE LangresHandle;
HINSTANCE ImgresHandle;
ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
ImagingHelper *imgArrow;
SmsBackupConfig appconfig;

extern wchar_t g_password[256];
extern int g_password_len;

void M8SmsMgr::loadImageRes(){
	ImgresHandle = MzGetInstanceHandle();
	for(int i = 0; i < sizeof(pimg) / sizeof(pimg[0]); i++){
		pimg[i] = LOADIMAGE(IDB_PNG_BEGIN + i);
	}
	HINSTANCE MzresHandle = GetMzResModuleHandle();
	imgArrow = ImagingHelper::GetImageObject(MzresHandle, MZRES_IDR_PNG_ARROW_RIGHT, true);
}

void M8SmsMgr::LoadLangRes(){
    //������Դ
    LangresHandle = LoadLibrary(L"language.dll");
    if(LangresHandle){
        isExternLangres = true;
    }else{
        LangresHandle = MzGetInstanceHandle();
        isExternLangres = false;
    }
}

void M8SmsMgr::LoadRes(){
    //����ͼ����Դ
    loadImageRes();
    //����������Դ
    LoadLangRes();
}

BOOL M8SmsMgr::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);

    //������Դ
    LoadRes();

    //��ʼ��default����
	wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};
    g_password_len = 6;
    memcpy(g_password,defaultpwd,sizeof(wchar_t)*6);

    //�������Ƿ��Ѿ�����
    HWND pWnd = isRuning();
    if(pWnd)
    {
        SetForegroundWindow(pWnd);
        PostMessage(pWnd,WM_NULL,NULL,NULL);
        PostQuitMessage(0);
        return true; 
    }

    //���������
    if(cmdline_run(GetCommandLine())){
        PostQuitMessage(0);
        return TRUE;
    }

    // Create the main window
    RECT rcWork = MzGetWorkArea();
    m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
    m_MainWnd.Show();

	if(appconfig.IniRefreshWhileLoad.Get()){
        cmdline_import(0);
	}
    // return TRUE means init success.
    return TRUE;
}

int M8SmsMgr::Done(){
    FreeMzResModule();
    if(isExternLangres) FreeLibrary(LangresHandle);
    return CMzApp::Done();
}