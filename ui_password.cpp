#include "ui_password.h"
#include "LocalDataBase.h"
#include "resource.h"

MZ_IMPLEMENT_DYNAMIC(Ui_PasswordWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104

extern HINSTANCE LangresHandle;

bool g_pwddlgshow;
bool g_bencypt;
extern wchar_t g_password[256];
extern int g_password_len;

Ui_PasswordWnd::Ui_PasswordWnd(void)
{
	_uiMode = 0;
	_isModal = false;
}

Ui_PasswordWnd::~Ui_PasswordWnd(void)
{
	g_pwddlgshow = false;
}

void Ui_PasswordWnd::getPassword(wchar_t* p,int* plen){
	if(this->m_EdtPassword.GetText().IsEmpty()){
		*p = '\0';
		plen = 0;
	}
	wsprintf(p,L"%s",this->m_EdtPassword.GetText().C_Str());
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
				CMzStringW s = m_EdtPassword.GetText();
				LocalDataBase ldb;
				wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};
				wchar_t* spwd = s.IsEmpty() ? defaultpwd : s.C_Str();
				int len = s.IsEmpty() ? 6 : s.Length();

				if(_uiMode){
                    ldb.connect();
                    ldb.decrypt(g_password,g_password_len);
					if(s.IsEmpty()){
						g_bencypt = false;
                        if(ldb.encrypt(defaultpwd,6)){
						    MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_CLEAR_S).C_Str(),2000);
        				    g_password_len = 0;
                        }else{
                            MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_CLEAR_F).C_Str(),2000);
                        }
					}else{
						g_bencypt = true;
						if(ldb.encrypt(s.C_Str(),s.Length())){
						    MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_SET_S).C_Str(),2000);
                            g_password_len = 0;
                        }else{
                            MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_SET_F).C_Str(),2000);
                        }
					}
                    ldb.disconnect();
				}else{
					if(!ldb.checkpwd(spwd,len)){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_INCORRECT).C_Str(),2000);
						m_EdtPassword.SetFocus(true);
						m_EdtPassword.Invalidate();
						m_EdtPassword.Update();
						return;
					}
				    g_password_len = len;
				    memcpy(g_password,spwd,sizeof(wchar_t)*s.Length());
				}
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
