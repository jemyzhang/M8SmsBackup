#include "ui_search.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "resource.h"

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
extern HINSTANCE LangresHandle;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_BUTTON_SEARCH 102

#define MZ_IDC_SMS_LIST 121
extern LocalDataBase *g_pldb;

MZ_IMPLEMENT_DYNAMIC(Ui_SearchWnd)

Ui_SearchWnd::Ui_SearchWnd(){
	plistkey = 0;
	plistSize = 0;
}

Ui_SearchWnd::~Ui_SearchWnd(){
}

BOOL Ui_SearchWnd::OnInitDialog() {
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	int y = 5;
	m_SearchBox.SetPos(5, y, GetWidth() - 80, MZM_HEIGHT_SINGLELINE_EDIT);
	m_SearchBox.SetTip(LOADSTRING(IDS_STR_KEYWORDS).C_Str());
	m_SearchBox.SetSipMode(IM_SIP_MODE_GEL_PY,MZM_HEIGHT_TOOLBARPRO);
	AddUiWin(&m_SearchBox);

	m_ButtonSearch.SetPos(GetWidth() - 75, y, 70,MZM_HEIGHT_SINGLELINE_EDIT);
	m_ButtonSearch.SetID(MZ_IDC_BUTTON_SEARCH);
	m_ButtonSearch.SetButtonType(MZC_BUTTON_NONE);
	m_ButtonSearch.SetImage_Normal(
		m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEARCH_BTN, true));
	m_ButtonSearch.SetImage_Pressed(
		m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEARCH_BTN_PRESSED, true));
	AddUiWin(&m_ButtonSearch);

	y += MZM_HEIGHT_SINGLELINE_EDIT;
	m_SmsList.SetPos(5, y, GetWidth() - 10, GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_SmsList.SetID(MZ_IDC_SMS_LIST);
	m_SmsList.EnableNotifyMessage(true);
	m_SmsList.SetupDB(g_pldb);
	m_SmsList.SetupMode(2);
	m_SmsList.SetItemHeight(80);
	AddUiWin(&m_SmsList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
	m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	return TRUE;
}

void Ui_SearchWnd::SetupToolbar(){
	if(m_SmsList.GetSelectionMode()){
				m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_REVERSE_SELECT).C_Str());
				m_Toolbar.SetButton(1,true,m_SmsList.GetSelectedCount() > 0,LOADSTRING(IDS_STR_DELETE).C_Str());
				m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_FINISHED).C_Str());
	}else{
		if(m_SmsList.GetItemCount() == 0){
				m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.SetButton(1,false,false,NULL);
				m_Toolbar.SetButton(2,false,false,NULL);
		}else{
				m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_SELECT).C_Str());
				m_Toolbar.SetButton(1,false,false,NULL);
		}
	}
	m_Toolbar.Invalidate();
	//m_Toolbar.Update();
}

void Ui_SearchWnd::SetupList(){
	m_SmsList.SetSelectedIndex(-1);
	m_SmsList.RemoveAll();
	m_SmsList.ScrollTo();

    CMzPopupWaitDialog m_progress;
    m_progress.EnableCancelButton(FALSE);
    m_progress.BeginWaitDialog(m_hWnd,L"搜索中，请稍候。",TRUE);
    m_SmsList.reqUpdate();
    m_progress.EndWaitDialog();
    if(m_SmsList.GetItemCount() == 0){
        MzMessageAutoBoxV2(m_hWnd,L"没有找到符合条件的短信。");
    }
	m_SmsList.Invalidate();
	//m_SmsList.Update();
	SetupToolbar();
	return;
}

void Ui_SearchWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_SEARCH:
			{
				CMzString key = m_SearchBox.GetText();
				if(!key.IsEmpty()){
					if(MzIsSipOpen()){
						MzCloseSip();
					}
					m_SearchBox.SetFocus(false);
					m_SearchBox.Invalidate();
					//m_SearchBox.Update();
					m_SmsList.SetupListContent(key.C_Str());
					SetupList();
				}
				break;
			}
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == 0) {
					if(m_SmsList.GetSelectionMode()){
						m_SmsList.ReverseSelect();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
					}else{
						EndModal(ID_OK);
						return;
					}
				}
				if(nIndex == 2){
					m_SmsList.SetSelectionMode();
					m_SmsList.Invalidate();
					//m_SmsList.Update();
					SetupToolbar();
				}
				if(nIndex == 1){
					if(MzMessageBoxV2(m_hWnd,
						LOADSTRING(IDS_STR_DELETE_CONFIRM).C_Str(),
						MZV2_MB_YESNO,TRUE) == 1){
							m_SmsList.DeleteSelectedItems();
							m_SmsList.reqUpdate();
							m_SmsList.Invalidate();
							//m_SmsList.Update();
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
					if (!m_SmsList.IsMouseDownAtScrolling() && !m_SmsList.IsMouseMoved()) {
						int nIndex = m_SmsList.CalcIndexOfPos(x, y);
						m_SmsList.SetSelectedIndex(nIndex);
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
						return 0;
					}
				}
				if (nID == MZ_IDC_SMS_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_SmsList.SetSelectedIndex(-1);
					m_SmsList.Invalidate();
					//m_SmsList.Update();
					return 0;
				}
			}
	}
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

