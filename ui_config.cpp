#include "ui_config.h"
#include "resource.h"
#include "appconfigini.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "passwordDlg.h"

extern HINSTANCE LangresHandle;
extern AppConfigIni appconfig;

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101

#define MZ_IDC_BTN_USE_SIM		102
#define MZ_IDC_BTN_BOOT_UPDATE		103
#define MZ_IDC_BTN_SETUP_PASSWORD   104

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

    // Then init the controls & other things in the window
    int y = 0;
	m_Caption.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption.SetText(LOADSTRING(IDS_STR_CONFIGURATIONS).C_Str());;
	AddUiWin(&m_Caption);

	y+=MZM_HEIGHT_CAPTION;
	m_BtnUseSimPhoneBook.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnUseSimPhoneBook.SetText(LOADSTRING(IDS_STR_USE_SIM_PHONEBOOK).C_Str());
    m_BtnUseSimPhoneBook.SetButtonType(MZC_BUTTON_LINE_NONE);
	m_BtnUseSimPhoneBook.SetTextMaxLen(0);
	m_BtnUseSimPhoneBook.SetEnable(false);
    AddUiWin(&m_BtnUseSimPhoneBook);

	m_BtnUseSimPhoneBookSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnUseSimPhoneBookSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnUseSimPhoneBookSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnUseSimPhoneBookSW.SetID(MZ_IDC_BTN_USE_SIM);
    AddUiWin(&m_BtnUseSimPhoneBookSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnBootUpdate.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnBootUpdate.SetText(LOADSTRING(IDS_STR_BOOTUPDATE).C_Str());
    m_BtnBootUpdate.SetButtonType(MZC_BUTTON_LINE_NONE);
	m_BtnBootUpdate.SetTextMaxLen(0);
	m_BtnBootUpdate.SetEnable(false);
    AddUiWin(&m_BtnBootUpdate);

	m_BtnBootUpdateSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnBootUpdateSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnBootUpdateSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnBootUpdateSW.SetID(MZ_IDC_BTN_BOOT_UPDATE);
    AddUiWin(&m_BtnBootUpdateSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnSetupPassword.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnSetupPassword.SetText(LOADSTRING(IDS_STR_PWD_SETUP).C_Str());
    m_BtnSetupPassword.SetButtonType(MZC_BUTTON_LINE_NONE);
	m_BtnSetupPassword.SetTextMaxLen(0);
    m_BtnSetupPassword.SetID(MZ_IDC_BTN_SETUP_PASSWORD);
    AddUiWin(&m_BtnSetupPassword);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);
	updateUi();
    return TRUE;
}

void Ui_ConfigWnd::updateBootUpdate(){
	if(!appconfig.IniRefreshWhileLoad.Get()){
		m_BtnBootUpdateSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnBootUpdateSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnBootUpdate.Invalidate();
	m_BtnBootUpdate.Update();
	m_BtnBootUpdateSW.Invalidate();
	m_BtnBootUpdateSW.Update();
}

void Ui_ConfigWnd::updateUseSimPhoneBook(){
	if(!appconfig.IniUseSimPhoneBook.Get()){
		m_BtnUseSimPhoneBookSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnUseSimPhoneBookSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnUseSimPhoneBook.Invalidate();
	m_BtnUseSimPhoneBook.Update();
	m_BtnUseSimPhoneBookSW.Invalidate();
	m_BtnUseSimPhoneBookSW.Update();
}

void Ui_ConfigWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_USE_SIM:
		{
			if(m_BtnUseSimPhoneBookSW.GetState() == MZCS_BUTTON_PRESSED){
				appconfig.IniUseSimPhoneBook.Set(1);
			}else{
				appconfig.IniUseSimPhoneBook.Set(0);
			}
			updateUseSimPhoneBook();
			break;
		}
		case MZ_IDC_BTN_BOOT_UPDATE:
		{
			if(m_BtnBootUpdateSW.GetState() == MZCS_BUTTON_PRESSED){
				appconfig.IniRefreshWhileLoad.Set(1);
			}else{
				appconfig.IniRefreshWhileLoad.Set(0);
			}
			updateBootUpdate();
			break;
		}
        case MZ_IDC_BTN_SETUP_PASSWORD:
        {
            if(ShowPasswordDlg(1,m_hWnd)){
                EndModal(ID_OK);
            }
            break;
        }
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//È¡Ïû
				EndModal(ID_OK);
				return;
			}
		}
	}
}

LRESULT Ui_ConfigWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
