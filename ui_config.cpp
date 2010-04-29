#include "ui_config.h"
#include "resource.h"
#include "appconfigini.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_password.h"
#include "ui_backup.h"
#include "LocalDataBase.h"
#include "ui_ProgressBar.h"
#include "UiFileDialog.h"

#include <fstream>
using namespace std;

extern HINSTANCE LangresHandle;
extern SmsBackupConfig appconfig;
extern int g_password_len;
extern LocalDataBase *g_pldb;

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101

#define MZ_IDC_BTN_USE_SIM		102
#define MZ_IDC_BTN_BOOT_UPDATE		103
#define MZ_IDC_BTN_SETUP_PASSWORD   104
#define MZ_IDC_BTN_UPDATE_METHOD    105
//////

Ui_ConfigWnd::Ui_ConfigWnd(void)
{
}

Ui_ConfigWnd::~Ui_ConfigWnd(void)
{
}

BOOL Ui_ConfigWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

	SetWindowText(L"设置");
    // Then init the controls & other things in the window
    int y = 0;
	m_BtnUseSimPhoneBook.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnUseSimPhoneBook.SetText(LOADSTRING(IDS_STR_USE_SIM_PHONEBOOK).C_Str());
    m_BtnUseSimPhoneBook.SetID(MZ_IDC_BTN_USE_SIM);
    AddUiWin(&m_BtnUseSimPhoneBook);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnBootUpdate.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnBootUpdate.SetText(LOADSTRING(IDS_STR_BOOTUPDATE).C_Str());
    m_BtnBootUpdate.SetID(MZ_IDC_BTN_BOOT_UPDATE);
    AddUiWin(&m_BtnBootUpdate);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnUpdateMethod.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnUpdateMethod.SetText(LOADSTRING(IDS_STR_REFRESH_METHOD).C_Str());
    m_BtnUpdateMethod.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnUpdateMethod.SetTextMaxLen(0);
    m_BtnUpdateMethod.SetID(MZ_IDC_BTN_UPDATE_METHOD);
    AddUiWin(&m_BtnUpdateMethod);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnSetupPassword.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnSetupPassword.SetText(LOADSTRING(IDS_STR_PWD_SETUP).C_Str());
    m_BtnSetupPassword.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnSetupPassword.SetTextMaxLen(0);
    m_BtnSetupPassword.SetID(MZ_IDC_BTN_SETUP_PASSWORD);
    AddUiWin(&m_BtnSetupPassword);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);
	updateUi();
    return TRUE;
}

void Ui_ConfigWnd::updateBootUpdate(){
	if(!appconfig.IniRefreshWhileLoad.Get()){
		m_BtnBootUpdate.SetSwitchStatus(FALSE);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnBootUpdate.SetSwitchStatus(TRUE);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnBootUpdate.Invalidate();
}

void Ui_ConfigWnd::updateUseSimPhoneBook(){
	if(!appconfig.IniUseSimPhoneBook.Get()){
		m_BtnUseSimPhoneBook.SetSwitchStatus(FALSE);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnUseSimPhoneBook.SetSwitchStatus(TRUE);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnUseSimPhoneBook.Invalidate();
}

void Ui_ConfigWnd::updateUpdateMethod(){
	if(appconfig.IniUpdateMethod.Get() == 0){
		m_BtnUpdateMethod.SetText2(LOADSTRING(IDS_STR_REFRESH_WHOLE).C_Str());
	}else{
		m_BtnUpdateMethod.SetText2(LOADSTRING(IDS_STR_REFRESH_FAST).C_Str());
	}
	m_BtnUpdateMethod.Invalidate();
	//m_BtnUpdateMethod.Update();
}

void Ui_ConfigWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_USE_SIM:
            {
			if(m_BtnUseSimPhoneBook.GetSwitchStatus()){
				appconfig.IniUseSimPhoneBook.Set(0);
			}else{
				appconfig.IniUseSimPhoneBook.Set(1);
			}
			updateUseSimPhoneBook();
                break;
            }
		case MZ_IDC_BTN_USE_SIM + 0x100:
		{
			if(m_BtnUseSimPhoneBook.GetSwitchStatus()){
				appconfig.IniUseSimPhoneBook.Set(1);
			}else{
				appconfig.IniUseSimPhoneBook.Set(0);
			}
			updateUseSimPhoneBook();
			break;
		}
		case MZ_IDC_BTN_BOOT_UPDATE:
		{
			if(m_BtnBootUpdate.GetSwitchStatus()){
				appconfig.IniRefreshWhileLoad.Set(0);
			}else{
				appconfig.IniRefreshWhileLoad.Set(1);
			}
			updateBootUpdate();
			break;
		}
		case MZ_IDC_BTN_BOOT_UPDATE + 0x100:
		{
			if(m_BtnBootUpdate.GetSwitchStatus()){
				appconfig.IniRefreshWhileLoad.Set(1);
			}else{
				appconfig.IniRefreshWhileLoad.Set(0);
			}
			updateBootUpdate();
			break;
		}
        case MZ_IDC_BTN_UPDATE_METHOD:
        {
            if(appconfig.IniUpdateMethod.Get()){
				appconfig.IniUpdateMethod.Set(0);
			}else{
				appconfig.IniUpdateMethod.Set(1);
			}
			updateUpdateMethod();
			break;
        }
        case MZ_IDC_BTN_SETUP_PASSWORD:
        {
            Ui_PasswordWnd dlg;
            dlg.setMode(1);
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
                m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            int rc = dlg.DoModal();
            if(rc == ID_OK){
                //启动等待画面
                wchar_t* p = 0;
                int len = 0;
                dlg.getPassword(&p,&len);
                m_progress.EnableCancelButton(FALSE);
                wchar_t* msg;
                if(p == 0 || len == 0){
                    msg = L"清除密码中，请稍候。";
                }else{
                    msg = L"设定密码中，请稍候。";
                }
                m_progress.BeginWaitDialog(m_hWnd,msg,TRUE);
                LPCTSTR retmsg = SetPassword(p,len);
                m_progress.EndWaitDialog();
                MzMessageAutoBoxV2(m_hWnd,retmsg);
                //密码设置完成后退出
                EndModal(ID_OK);
            }
            break;
        }
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
			if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//取消
				EndModal(ID_OK);
				SetWindowText(L"短信备份");
				return;
			}
            break;
		}
	}
}

LRESULT Ui_ConfigWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

LPCTSTR Ui_ConfigWnd::SetPassword(wchar_t *pWd, int len){
    LPCTSTR retVal;
    wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};

    if(pWd == NULL || len == 0){
        if(g_pldb->encrypt(defaultpwd,6)){
            retVal = LOADSTRING(IDS_STR_PWD_CLEAR_S).C_Str();
            g_password_len = 0;
        }else{
            retVal = LOADSTRING(IDS_STR_PWD_CLEAR_F).C_Str();
        }
    }else{
        if(g_pldb->encrypt(pWd,len)){
            retVal = LOADSTRING(IDS_STR_PWD_SET_S).C_Str();
            g_password_len = 0;
        }else{
            retVal = LOADSTRING(IDS_STR_PWD_SET_F).C_Str();
        }
    }

    //销毁数据库连接
    delete g_pldb;
    g_pldb = 0;
    return retVal;
}