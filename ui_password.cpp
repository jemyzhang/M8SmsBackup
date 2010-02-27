#include "ui_password.h"
#include "LocalDataBase.h"
#include "resource.h"

using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_PasswordWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104

extern HINSTANCE LangresHandle;

bool g_pwddlgshow;

Ui_PasswordWnd::Ui_PasswordWnd(void)
{
	_uiMode = 0;
	_isModal = false;
}

Ui_PasswordWnd::~Ui_PasswordWnd(void)
{
	g_pwddlgshow = false;
}

void Ui_PasswordWnd::getPassword(wchar_t** p,int* plen){
	if(this->m_EdtPassword.GetText().IsEmpty()){
		*p = 0;
		*plen = 0;
        return;
	}
    C::newstrcpy(p,this->m_EdtPassword.GetText().C_Str());
	*plen = m_EdtPassword.GetText().Length();
}

BOOL Ui_PasswordWnd::OnInitDialog() {
	g_pwddlgshow = true;
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 10;
	m_Background.SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	AddUiWin(&m_Background);

	m_EdtPassword.SetPos(5, y, GetWidth() - 10, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtPassword.SetMaxChars(32);
    m_EdtPassword.SetSipMode(IM_SIP_MODE_GEL_LETTER, MZM_HEIGHT_TEXT_TOOLBAR);
	if(!_uiMode){
		m_EdtPassword.EnablePassWord();
	}
	if(_uiMode){
	    m_EdtPassword.SetTip(LOADSTRING(IDS_STR_PWD_TIP).C_Str());
	}else{
	    m_EdtPassword.SetTip(LOADSTRING(IDS_STR_PWD_LOCKED).C_Str());
	}
    m_Background.AddChild(&m_EdtPassword);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
    m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);

	m_EdtPassword.SetFocus(true);
    return TRUE;
}

void Ui_PasswordWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//取消
				if(_uiMode){
					if(_isModal){
						EndModal(ID_CANCEL);
					}else{
						m_EdtPassword.SetText(L"\0");
						Show(false);
						g_pwddlgshow = false;
					}
				}else{
					m_EdtPassword.SetFocus(false);
					EndModal(ID_CANCEL);
				}
				return;
			}
			if(nIndex == 2){	//确定
				if(_isModal){
					EndModal(ID_OK);
				}else{
					g_pwddlgshow = false;
					m_EdtPassword.SetText(L"\0");
					Show(false);
				}
				return;
			}
		}
	}
}

