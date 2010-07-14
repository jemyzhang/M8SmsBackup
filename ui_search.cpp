#include "ui_search.h"
#include "UiSmsList.h"
#include "UiContactFiltList.h"

#include <cMzCommon.h>
using namespace cMzCommon;
#include "resource.h"

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
extern HINSTANCE LangresHandle;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_BUTTON_SEARCH 102
#define MZ_IDC_EDIT_CONTACT 103
#define MZ_IDC_EDIT_KEY 104

#define MZ_IDC_SMS_LIST 121
#define MZ_IDC_CONTACT_LIST 122

extern LocalDataBase *g_pldb;

MZ_IMPLEMENT_DYNAMIC(Ui_SearchWnd)

Ui_SearchWnd::Ui_SearchWnd(){
	plistkey = 0;
	plistSize = 0;
    m_pSmsList = NULL;
    m_pContactList = NULL;
}

Ui_SearchWnd::~Ui_SearchWnd(){
    if(m_pSmsList) delete m_pSmsList;
    if(m_pContactList) delete m_pContactList;
}

BOOL Ui_SearchWnd::OnInitDialog() {
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	int y = 0;
    m_TitleBar.SetPos(0, y, GetWidth(), MZM_HEIGHT_HEADINGBAR);
    m_TitleBar.SetText(L"短信搜索");
    AddUiWin(&m_TitleBar);

    y += MZM_HEIGHT_HEADINGBAR;

	m_SearchBox.SetPos(0, y, 250, MZM_HEIGHT_SINGLELINE_EDIT);
	m_SearchBox.SetTip(LOADSTRING(IDS_STR_KEYWORDS).C_Str());
	m_SearchBox.SetSipMode(IM_SIP_MODE_GEL_PY,MZM_HEIGHT_TOOLBARPRO);
    m_SearchBox.SetID(MZ_IDC_EDIT_KEY);
    //m_SearchBox.SetEditStyle(SINGLELINE_EDIT_STYLE_DEFAULT_BG);
	AddUiWin(&m_SearchBox);

    m_ContactBox.SetPos(250, y, 180, MZM_HEIGHT_SINGLELINE_EDIT);
	m_ContactBox.SetTip(LOADSTRING(IDS_STR_KEYCONTACT).C_Str());
	m_ContactBox.SetSipMode(IM_SIP_MODE_GEL_PY,MZM_HEIGHT_TOOLBARPRO);
    //m_ContactBox.SetEditStyle(SINGLELINE_EDIT_STYLE_DEFAULT_BG);
    m_ContactBox.SetID(MZ_IDC_EDIT_CONTACT);
	AddUiWin(&m_ContactBox);

	m_ButtonSearch.SetPos(431, y, 48,MZM_HEIGHT_SINGLELINE_EDIT);
	m_ButtonSearch.SetID(MZ_IDC_BUTTON_SEARCH);
	m_ButtonSearch.SetButtonType(MZC_BUTTON_NONE);
	m_ButtonSearch.SetImage_Normal(
		m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEARCH_BTN, true));
	m_ButtonSearch.SetImage_Pressed(
		m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEARCH_BTN_PRESSED, true));
	AddUiWin(&m_ButtonSearch);


    y += MZM_HEIGHT_SINGLELINE_EDIT;
    m_pSmsList = new UiSmsList;
	m_pSmsList->SetPos(0, y, GetWidth(), GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_pSmsList->SetID(MZ_IDC_SMS_LIST);
	m_pSmsList->EnableNotifyMessage(true);
	m_pSmsList->SetupDB(g_pldb);
	m_pSmsList->SetupMode(3);
	m_pSmsList->SetItemHeight(80);
	AddUiWin(m_pSmsList);

    m_pContactList = new UiContactFiltList;
	m_pContactList->SetPos(0, y, GetWidth(), GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_pContactList->SetID(MZ_IDC_CONTACT_LIST);
	m_pContactList->SetDB(g_pldb);
    m_pContactList->SetVisible(false);
	AddUiWin(m_pContactList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
	m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	return TRUE;
}

void Ui_SearchWnd::SetupToolbar(){
	if(m_pSmsList->GetSelectionMode()){
				m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_REVERSE_SELECT).C_Str());
				m_Toolbar.SetMiddleButton(true,m_pSmsList->GetSelectedCount() > 0,LOADSTRING(IDS_STR_DELETE).C_Str(),NULL,NULL,NULL);
				m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_FINISHED).C_Str());
	}else{
		if(m_pSmsList->GetItemCount() == 0){
				m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
				m_Toolbar.ShowButton(TOOLBARPRO_RIGHT_TEXTBUTTON,false);
		}else{
				m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_SELECT).C_Str());
				m_Toolbar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
		}
	}
	m_Toolbar.Invalidate();
	//m_Toolbar.Update();
}

void Ui_SearchWnd::SetupList(){
	m_pSmsList->SetSelectedIndex(-1);
	m_pSmsList->RemoveAll();
	m_pSmsList->ScrollTo();

    CMzPopupWaitDialog m_progress;
    m_progress.EnableCancelButton(FALSE);
    m_progress.BeginWaitDialog(m_hWnd,L"搜索中，请稍候。",TRUE);
    m_pSmsList->reqUpdate();
    m_progress.EndWaitDialog();
    if(m_pSmsList->GetItemCount() == 0){
        MzMessageAutoBoxV2(m_hWnd,L"没有找到符合条件的短信。");
    }
	m_pSmsList->Invalidate();
	//m_pSmsList->Update();
	SetupToolbar();
	return;
}

void Ui_SearchWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_SEARCH:
			{
                if(m_pContactList->IsVisible()){
                    m_pContactList->SetVisible(false);
                    m_pContactList->Invalidate();
                }
				CMzString key = m_SearchBox.GetText();
                CMzString contact = m_ContactBox.GetText();
                if(!key.IsEmpty() || !contact.IsEmpty()){
					m_SearchBox.SetFocus(false);
                    m_ContactBox.SetFocus(false);
					m_SearchBox.Invalidate();
                    m_ContactBox.Invalidate();
					//m_SearchBox.Update();
                    if(!key.IsEmpty()){
					    m_pSmsList->SetupListContent(key.C_Str());
                    }else{
                        m_pSmsList->SetupListContent(NULL);
                    }
                    if(!contact.IsEmpty()){
                        m_pSmsList->SetupListName(contact.C_Str());
                    }else{
                        m_pSmsList->SetupListName(NULL);
                    }
					SetupList();
				}
				break;
			}
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == TOOLBARPRO_LEFT_TEXTBUTTON) {
					if(m_pSmsList->GetSelectionMode()){
						m_pSmsList->ReverseSelect();
						m_pSmsList->Invalidate();
						//m_pSmsList->Update();
						SetupToolbar();
					}else{
						EndModal(ID_OK);
						return;
					}
				}
				if(nIndex == TOOLBARPRO_RIGHT_TEXTBUTTON){
					m_pSmsList->SetSelectionMode();
					m_pSmsList->Invalidate();
					//m_pSmsList->Update();
					SetupToolbar();
				}
				if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){
					if(MzMessageBoxV2(m_hWnd,
						LOADSTRING(IDS_STR_DELETE_CONFIRM).C_Str(),
						MZV2_MB_YESNO,TRUE) == 1){
							m_pSmsList->DeleteSelectedItems();
							m_pSmsList->reqUpdate();
							m_pSmsList->Invalidate();
							//m_pSmsList->Update();
					}
				}
			}
			break;
	}
}

LRESULT Ui_SearchWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case MZ_WM_MOUSE_NOTIFY:
			{
				int nID = LOWORD(wParam);
				int nNotify = HIWORD(wParam);
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);

				if (nID == MZ_IDC_SMS_LIST && nNotify == MZ_MN_LBUTTONUP) {
					if (!m_pSmsList->IsMouseDownAtScrolling() && !m_pSmsList->IsMouseMoved()) {
						int nIndex = m_pSmsList->CalcIndexOfPos(x, y);
						m_pSmsList->SetSelectedIndex(nIndex);
						m_pSmsList->Invalidate();
						//m_pSmsList->Update();
						SetupToolbar();
						return 0;
					}
				}
				if (nID == MZ_IDC_SMS_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_pSmsList->SetSelectedIndex(-1);
					m_pSmsList->Invalidate();
					//m_pSmsList->Update();
					return 0;
				}

				if (nID == MZ_IDC_CONTACT_LIST && nNotify == MZ_MN_LBUTTONUP) {
					if (!m_pContactList->IsMouseDownAtScrolling() && !m_pContactList->IsMouseMoved()) {
						int nIndex = m_pContactList->CalcIndexOfPos(x, y);
                        if(nIndex != -1){
                            m_pContactList->SetSelectedIndex(nIndex);
                            m_ContactBox.SetText(m_pContactList->GetSelectName());
                            m_ContactBox.Invalidate();
                            m_pContactList->SetSelectedIndex(-1);
                            m_pContactList->SetVisible(false);
                        }
						m_pContactList->Invalidate();
						return 0;
					}
				}
				if (nID == MZ_IDC_CONTACT_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_pContactList->SetSelectedIndex(-1);
					m_pContactList->Invalidate();
					return 0;
				}
                break;
            }
        case MZ_WM_UISINGLELINEEDIT:
            {
                int nID = LOWORD(wParam);
                int op = LOWORD(lParam);
                if(nID == MZ_IDC_EDIT_CONTACT){
                    if(op == WM_EDIT_OP_FOCUS && !m_ContactBox.IsFocused()){
                    }else{
                        m_pContactList->SetFilterKeyword(m_ContactBox.GetText().C_Str());
                    }
                }
                break;
            }
        case MZ_WM_UI_FOCUS:
            {
                int nID = LOWORD(wParam);
                if(nID != MZ_IDC_EDIT_CONTACT){
                    if(nID == MZ_IDC_EDIT_KEY){
                        if(m_pContactList->IsVisible()){
                            m_pContactList->SetVisible(false);
                            m_pContactList->Invalidate();
                        }
                    }else{
                        if(MzIsSipOpen()){
                            MzCloseSip();
                        }
                    }
                }
                if(MzIsSipOpen()){
                    RECT rcVisible = MzGetVisibleDesktopRect();
                    m_pContactList->SetPos(0,0,GetWidth(),RECT_HEIGHT(rcVisible) - m_pContactList->GetPosY(),SP_NOMOVE);
                }else{
                    m_pContactList->SetPos(0,0,GetWidth(),m_pSmsList->GetHeight(),SP_NOMOVE);
                }
                m_pContactList->Invalidate();
                break;
            }
        case MZ_WM_UILISTEX_VISIBLE_NOTIFY:
            {
                int nID = LOWORD(wParam);
                bool visible = LOWORD(lParam) > 0;
                if(nID == MZ_IDC_CONTACT_LIST){
                    if(visible){
                        if(m_pSmsList->IsVisible()){
                            m_pSmsList->SetVisible(false);
                            m_pSmsList->Invalidate();
                        }
                    }else{
                        if(!m_pSmsList->IsVisible()){
                            m_pSmsList->SetVisible(true);
                            m_pSmsList->Invalidate();
                        }
                    }
                }
            }
            break;
	}
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
