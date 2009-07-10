#include "ui_password.h"
#include "LocalDataBase.h"
#include "passwordDlg.h"

wchar_t g_password[256] = {0};
int g_password_len = 0;

bool CheckPassword(HWND m_hWnd){
    bool nRet = false;
	LocalDataBase ldb;
	wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};
    if(g_password_len == 0){
	    g_password_len = 6;
	    memcpy(g_password,defaultpwd,sizeof(wchar_t)*6);
    }

	if(!ldb.checkpwd(NULL,0)){
		if(!ldb.checkpwd(g_password,g_password_len)){
            nRet = ShowPasswordDlg(0,m_hWnd);
        }else{
            nRet = true;
        }
	}else{
        nRet = true;
        //ÃÜÂëÎª¿ÕÊ±¼ÓÃÜ
		ldb.encrypt(defaultpwd,6);
	}
    return nRet;
}

//0: enter password 1: set password
bool ShowPasswordDlg(int mode,HWND m_hWnd){
    if(mode > 1 || mode < 0) mode = 0;

    Ui_PasswordWnd dlg;
    dlg.setMode(mode);
    RECT rcWork = MzGetWorkArea();
    dlg.Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        m_hWnd, 0, WS_POPUP);
    // set the animation of the window
    dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
    dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
    int rc = dlg.DoModal();
    bool nRet = false;
    if(rc == ID_OK){
        nRet = true;
    }
    return nRet;
}