#include "ui_view.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "resource.h"

extern HINSTANCE LangresHandle;
#define MZ_IDC_TOOLBAR_MAIN 101

#define MZ_IDC_BUTTON_MAIN_VIEW 102
#define MZ_IDC_BUTTON_VIEW_CONTACT 103
#define MZ_IDC_BUTTON_VIEW_DATE 104
#define MZ_IDC_BUTTON_VIEW_DATE_YEAR 105
#define MZ_IDC_BUTTON_VIEW_DATE_MONTH 106
#define MZ_IDC_BUTTON_VIEW_DATE_DAY 107
#define MZ_IDC_BUTTON_VIEW_CONTACT_NAME 108

#define MZ_IDC_LIST 120
#define MZ_IDC_SMS_LIST 121

MZ_IMPLEMENT_DYNAMIC(Ui_ViewWnd)
extern LocalDataBase *g_pldb;
extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];

Ui_ViewWnd::Ui_ViewWnd(){
	viewStatusSavedBeforeView = 0;
	viewStatus = 0;
}

Ui_ViewWnd::~Ui_ViewWnd(){
}

BOOL Ui_ViewWnd::OnInitDialog() {
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	int y = 0;
	m_Navibar.SetPos(0, y, 108, GetHeight() - MZM_HEIGHT_TOOLBARPRO);
	m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_MAIN_VIEW,LOADSTRING(IDS_STR_VIEW_SMS).C_Str()));
	AddUiWin(&m_Navibar);

	m_List.SetPos(108, y, GetWidth() - 108, GetHeight() - MZM_HEIGHT_TOOLBARPRO);
	m_List.SetID(MZ_IDC_LIST);
	m_List.SetupDB(g_pldb);
	m_List.EnableNotifyMessage(true);
	m_List.SetItemHeight(50);
	AddUiWin(&m_List);

	m_SmsList.SetPos(108, y, GetWidth() - 108, GetHeight() - MZM_HEIGHT_TOOLBARPRO);
	m_SmsList.SetID(MZ_IDC_SMS_LIST);
	m_SmsList.EnableNotifyMessage(true);
	m_SmsList.SetVisible(false);
	m_SmsList.SetupDB(g_pldb);
	m_SmsList.SetItemHeight(80);
	AddUiWin(&m_SmsList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

    SetTimer(m_hWnd,0x1010,100,NULL);
	return TRUE;
}

void Ui_ViewWnd::OnTimer(UINT nIDEvent){
    switch(nIDEvent){
        case 0x1010:
            KillTimer(m_hWnd,0x1010);
            SetupList();
            break;
    }
}

void Ui_ViewWnd::SetupToolbar(){
	if((viewStatus & 0x0f) == 3 && m_SmsList.GetSelectionMode()){
		m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,false,false,NULL);
		m_Toolbar.SetMiddleButton(true,m_SmsList.GetSelectedCount() > 0,LOADSTRING(IDS_STR_DELETE).C_Str(),NULL,NULL,NULL);
		m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_FINISHED).C_Str());
	}else{
		//选择模式时，取消选择模式
		if(m_SmsList.GetSelectionMode()) m_SmsList.SetSelectionMode();
		if(viewStatus == 0 || 
			viewStatus == 1 ||
			viewStatus == 0x10){
				m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
				m_Toolbar.ShowButton(TOOLBARPRO_RIGHT_TEXTBUTTON,false);
		}else if((viewStatus & 0x0f) == 3){
			m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
			m_Toolbar.ShowButton(TOOLBARPRO_MIDDLE_TEXTBUTTON,false);
			m_Toolbar.SetButton(TOOLBARPRO_RIGHT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_SELECT).C_Str());
		}else{
			m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
			m_Toolbar.SetMiddleButton(true,true,LOADSTRING(IDS_STR_VIEW_SMS).C_Str(),NULL,NULL,NULL);
			m_Toolbar.ShowButton(TOOLBARPRO_RIGHT_TEXTBUTTON,false);
		}
	}
	m_Toolbar.Invalidate();
	//m_Toolbar.Update();
}
void Ui_ViewWnd::SetupList(){
	if((viewStatus & 0x0f) == 3){    //smslist
		if(m_List.IsVisible()){
			m_List.SetVisible(false);
			m_List.Invalidate();
			//m_List.Update();
		}

		m_SmsList.SetVisible(true);
		m_SmsList.SetSelectedIndex(-1);
		m_SmsList.RemoveAll();
		MzBeginWaitDlg(m_hWnd,NULL,TRUE);
		m_SmsList.reqUpdate();
		MzEndWaitDlg();
		//m_SmsList.Invalidate();
		//m_SmsList.Update();
		m_SmsList.ScrollTo();
	}else{
		if(m_SmsList.IsVisible()){
			m_SmsList.SetVisible(false);
			m_SmsList.Invalidate();
			//m_SmsList.Update();
		}

		m_List.SetVisible(true);
		m_List.RemoveAll();
		m_List.Invalidate();
		//m_List.Update();
		if(viewStatus == 0){
			m_List.SetListMode(0);
			m_List.SetItemHeight(80);
		}else{
			m_List.SetItemHeight(50);
			if(viewStatus == 1){
				m_List.SetListMode(1);
			}else if(viewStatus == 0x10){
				m_List.SetListMode(2);
			}else if(viewStatus == 0x11){
				m_List.SetListYear(selectedYear);
				m_List.SetListMode(3);
			}else if(viewStatus == 0x12){
				m_List.SetListMonth(selectedYear,selectedMonth);
				m_List.SetListMode(4);
			}
		}
		MzBeginWaitDlg(m_hWnd,NULL,TRUE);
		//DateTime::waitms(2);
		m_List.reqUpdate();
		MzEndWaitDlg();

        m_List.SetSelectedIndex(0);
		m_List.ScrollTo();
		//m_List.Invalidate();
		//m_List.Update();
	}
	SetupToolbar();
}

void Ui_ViewWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_MAIN_VIEW:
			{
				if(viewStatus != 0){
					m_Navibar.popUntil(MZ_IDC_BUTTON_MAIN_VIEW);
					viewStatus = 0;
					SetupList();
				}
				break;
			}
		case MZ_IDC_BUTTON_VIEW_CONTACT:
			{
				if(viewStatus != 1){
					m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_CONTACT);
					viewStatus = 1;
					SetupList();
				}
				break;
			}
		case MZ_IDC_BUTTON_VIEW_CONTACT_NAME:
			{
				if((viewStatus & 0x0f) == 3){
					if(m_SmsList.GetSelectionMode()){
						m_SmsList.ReverseSelect();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
					}
				}
			}
			break;
		case MZ_IDC_BUTTON_VIEW_DATE:
			{
				if(viewStatus != 0x10){
					m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE);
					viewStatus = 0x10;
					SetupList();
					selectedYear = 0;
					selectedMonth = 0;
					selectedDay = 0;
				}
				break;
			}
		case MZ_IDC_BUTTON_VIEW_DATE_YEAR:
			{
				if((viewStatus & 0x0f) == 3 && 
					viewStatusSavedBeforeView == 0x11 &&
					m_SmsList.GetSelectionMode()){
						m_SmsList.ReverseSelect();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
						break;;
				}
				if(viewStatus != 0x11){
					m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE_YEAR);
					viewStatus = 0x11;
					SetupList();
					selectedMonth = 0;
					selectedDay = 0;
				}
				break;
			}
		case MZ_IDC_BUTTON_VIEW_DATE_MONTH:
			{
				if((viewStatus & 0x0f) == 3 && 
					viewStatusSavedBeforeView == 0x12 &&
					m_SmsList.GetSelectionMode()){
						m_SmsList.ReverseSelect();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
						break;;
				}
				if(viewStatus != 0x12){
					m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE_MONTH);
					viewStatus = 0x12;
					SetupList();
					selectedDay = 0;
				}
				break;
			}
		case MZ_IDC_BUTTON_VIEW_DATE_DAY:
			{
				if((viewStatus & 0x0f) == 3){
					if(m_SmsList.GetSelectionMode()){
						m_SmsList.ReverseSelect();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
						SetupToolbar();
					}
				}
			}
			break;
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == TOOLBARPRO_LEFT_TEXTBUTTON) {
					EndModal(ID_OK);
					return;
				}
				if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){
					if((viewStatus & 0x0f) == 3){	//删除按钮
						if(m_SmsList.GetSelectionMode()){//sms list选中模式
							if(MzMessageBoxV2(m_hWnd,
								LOADSTRING(IDS_STR_DELETE_CONFIRM).C_Str(),
								MZV2_MB_YESNO,TRUE) == 1){
									m_SmsList.DeleteSelectedItems();
									m_SmsList.reqUpdate();
									m_SmsList.Invalidate();
									//m_SmsList.Update();
							}

						}
					}else{//查看按钮
						viewStatusSavedBeforeView = viewStatus;
						viewStatus = 0x13;
						m_SmsList.SetupMode(0);
						m_SmsList.SetupListDateTime(selectedYear,selectedMonth,selectedDay);
						SetupList();
					}
				}
				if(nIndex == TOOLBARPRO_RIGHT_TEXTBUTTON){
					if((viewStatus & 0x0f) == 3){
						m_SmsList.SetSelectionMode();
						m_SmsList.Invalidate();
						//m_SmsList.Update();
					}
					SetupToolbar();
				}
			}
			break;
	}
}

LRESULT Ui_ViewWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
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
				//列表
				if (nID == MZ_IDC_LIST && nNotify == MZ_MN_LBUTTONUP) {
					if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
						int nIndex = m_List.CalcIndexOfPos(x, y);
						if(nIndex != -1){
                            m_List.SetSelectedIndex(nIndex);
                            m_List.Invalidate();
                            DateTime::waitms(1);
                            SmsViewListKey_ptr pkey = g_pldb->query_list_item(nIndex);
							UINT received = 0;
							UINT sent = 0;
							UINT total = 0;
							wchar_t strcount[128];
							wchar_t strcount2[128];
                            received = pkey->nReceive; sent = pkey->nSend; total = sent + received;
						    wsprintf(strcount2,L"%d/%d",sent,received);
						    wsprintf(strcount,L"%d",total);

                            if(viewStatus == 0){
                                m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT + nIndex,pkey->key,strcount,strcount2));
								viewStatus = nIndex == 0 ? 1 : 0x10;
							}else if(viewStatus == 0x1){
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT_NAME,pkey->key,strcount,strcount2));
								viewStatus = 3;
								m_SmsList.SetupMode(1);
								m_SmsList.SetupListName(pkey->key);
							}else if(viewStatus == 0x10){
								selectedYear = _wtoi(pkey->key);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_YEAR,pkey->key,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x11){
								selectedMonth = _wtoi(pkey->key);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_MONTH,pkey->key,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x12){
								selectedDay = _wtoi(pkey->key);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_DAY,pkey->key,strcount,strcount2));
								viewStatus += 1;
								m_SmsList.SetupMode(0);
								m_SmsList.SetupListDateTime(selectedYear,selectedMonth,selectedDay);
							}
							SetupList();
						}
					}
					return 0;
				}
				if (nID == MZ_IDC_SMS_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_SmsList.SetSelectedIndex(-1);
					m_SmsList.Invalidate();
					//m_SmsList.Update();
					return 0;
				}
				if (nID == MZ_IDC_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_List.SetSelectedIndex(-1);
					m_List.Invalidate();
					//m_List.Update();
					return 0;
				}
			}
	}
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
void UiKeyList::SetupList() {
    if(pldb == NULL) return;

    switch(smode){
        case 1:         //contact
            pldb->GetSmsContactList();
            break;
        case 2:
			pldb->GetSmsYearList();
            break;
        case 3:
            pldb->GetSmsMonthList(syear);
            break;
		case 4:
            pldb->GetSmsDayList(syear,smonth);
			break;
		case 0:
            pldb->GetMainList();
			break;
        default:
            break;
    }
    for(UINT i = 0; i < pldb->query_list_size(); i++){
        ListItem li;
        AddItem(li);
    }
}
void UiKeyList::ClearList(){
	RemoveAll();
}

void UiKeyList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    SmsViewListKey_ptr pkey = pldb->query_list_item(nIndex);
    if(pkey == 0) return;

    HFONT hf;
    COLORREF cr;
    
	wchar_t str[256];
	switch(smode){
		case 0:
		case 1:
		case 2:
			wsprintf(str,pkey->key);
			break;
		case 3:
			wsprintf(str,L"%04d-%s",syear,pkey->key);
			break;
		case 4:
			wsprintf(str,L"%04d-%02d-%s",syear,smonth,pkey->key);
			break;
	}

	//栏目1
	hf = FontHelper::GetFont( smode == 0 ? 30 : 25 );
	SelectObject( hdcDst , hf );
	RECT rc1 = {prcItem->left + 10,prcItem->top,prcItem->right - (smode == 0 ? 10 : 150),prcItem->bottom};
	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , str, &rc1 , DT_VCENTER|(smode == 0 ? DT_CENTER : DT_LEFT)|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

    if(smode != 0){
        wsprintf(str,L"%d",pkey->nSend);
		//栏目2
		hf = FontHelper::GetFont( 18 );
		SelectObject( hdcDst , hf );
		RECT rc2 = {prcItem->right - 180,prcItem->top,prcItem->right - 140,prcItem->bottom};
		//cr = RGB(128,192,128);
		//::SetTextColor( hdcDst , cr );
		//MzDrawText( hdcDst, L"↑", &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
        pimg[IDB_PNG_SMS_SEND - IDB_PNG_BEGIN]->Draw(hdcDst,&rc2);

        rc2.left = rc2.right; rc2.right = prcItem->right - 90;
		cr = RGB(128,128,128);
		::SetTextColor( hdcDst , cr );
		MzDrawText( hdcDst, str, &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );

        wsprintf(str,L"%d",pkey->nReceive);
		//栏目3
        rc2.left = rc2.right; rc2.right = rc2.right + 40;
		//cr = RGB(192,128,128);
		//::SetTextColor( hdcDst , cr );
		//MzDrawText( hdcDst, L"↓", &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
        pimg[IDB_PNG_SMS_RECV - IDB_PNG_BEGIN]->Draw(hdcDst,&rc2);

        rc2.left = rc2.right; rc2.right = prcItem->right - 5;
		cr = RGB(128,128,128);
		::SetTextColor( hdcDst , cr );
		MzDrawText( hdcDst, str, &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
		DeleteObject(hf);
    }

}
