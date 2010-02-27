#include "ui_password.h"
#include "LocalDataBase.h"
#include "passwordDlg.h"
#include "Ui_ProgressBar.h"
#include "resource.h"

wchar_t g_password[256] = {0};
int g_password_len = 0;
extern HINSTANCE LangresHandle;

LocalDataBase *g_pldb = 0;

//仅在数据库连接未创建的时候检查
bool CheckPassword(HWND m_hWnd){
    bool nRet = false;
    if(g_pldb) return true;
    
    g_pldb = new LocalDataBase;

	wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};
    if(g_password_len == 0){
	    g_password_len = 6;
	    memcpy(g_password,defaultpwd,sizeof(wchar_t)*6);
    }

	if(!g_pldb->checkpwd(NULL,0)){
		if(!g_pldb->checkpwd(g_password,g_password_len)){
            Ui_PasswordWnd dlg;
            dlg.setMode(0);
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
                m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            nRet = false;
            while(dlg.DoModal() == ID_OK){
                wchar_t *p = 0; int len = 0;
                dlg.getPassword(&p,&len);
                if(p == NULL || len == 0){
                    p = defaultpwd; len = 6;
                }
                if(g_pldb->checkpwd(p,len)){
                    nRet = true;
                    memcpy(g_password,p,sizeof(wchar_t)*len);
                    g_password_len = len;
                    break;
                }else{
                    MzMessageBoxV2(m_hWnd,
						LOADSTRING(IDS_STR_PWD_INCORRECT).C_Str(),MZV2_MB_OK,TRUE);
                }
            }
        }else{
            nRet = true;
        }
        if(nRet){
	        //检查是否要升级数据库
	        initUiCallbackUpdateDatabase();
	        g_pldb->updateV2(uiCallBackUpdateDatabase);
        }
	}else{
        nRet = true;
        //密码为空时加密
		g_pldb->encrypt(defaultpwd,6);
	}

    if(!nRet){  //解密失败时，销毁数据库连接
        delete g_pldb;
        g_pldb = 0;
    }
    return nRet;
}
