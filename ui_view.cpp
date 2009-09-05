#include "ui_view.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
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
extern wchar_t g_password[256];
extern int g_password_len;

Ui_ViewWnd::Ui_ViewWnd(){
	viewStatusSavedBeforeView = 0;
	viewStatus = 0;
	if(!ldb.checkpwd(g_password,g_password_len)){
		return;
	}
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
	m_Navibar.SetPos(0, y, 108, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_MAIN_VIEW,LOADSTRING(IDS_STR_VIEW_SMS).C_Str()));
	AddUiWin(&m_Navibar);

	m_List.SetPos(108, y, GetWidth() - 108, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	m_List.SetID(MZ_IDC_LIST);
	m_List.SetupDB(&ldb);
	m_List.EnableNotifyMessage(true);
	m_List.SetItemHeight(50);
	AddUiWin(&m_List);

	m_SmsList.SetPos(108, y, GetWidth() - 108, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	m_SmsList.SetID(MZ_IDC_SMS_LIST);
	m_SmsList.EnableNotifyMessage(true);
	m_SmsList.SetVisible(false);
	m_SmsList.SetupDB(&ldb);
	m_SmsList.SetItemHeight(80);
	AddUiWin(&m_SmsList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	SetupList();
	return TRUE;
}

void Ui_ViewWnd::SetupToolbar(){
	if((viewStatus & 0x0f) == 3 && m_SmsList.GetSelectionMode()){
		m_Toolbar.SetButton(0,false,false,NULL);
		m_Toolbar.SetButton(1,true,m_SmsList.GetSelectedCount() > 0,LOADSTRING(IDS_STR_DELETE).C_Str());
		m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_FINISHED).C_Str());
	}else{
		//选择模式时，取消选择模式
		if(m_SmsList.GetSelectionMode()) m_SmsList.SetSelectionMode();
		if(viewStatus == 0 || 
			viewStatus == 1 ||
			viewStatus == 0x10){
				m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
				m_Toolbar.SetButton(1,false,false,NULL);
				m_Toolbar.SetButton(2,false,false,NULL);
		}else if((viewStatus & 0x0f) == 3){
			m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
			m_Toolbar.SetButton(1,false,false,NULL);
			m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_SELECT).C_Str());
		}else{
			m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
			m_Toolbar.SetButton(1,true,true,LOADSTRING(IDS_STR_VIEW_SMS).C_Str());
			m_Toolbar.SetButton(2,false,false,NULL);
		}
	}
	m_Toolbar.Invalidate();
	m_Toolbar.Update();
}
void Ui_ViewWnd::SetupList(){
	if((viewStatus & 0x0f) == 3){    //smslist
		if(m_List.IsVisible()){
			m_List.SetVisible(false);
			m_List.Invalidate();
			m_List.Update();
		}

		m_SmsList.SetVisible(true);
		m_SmsList.SetSelectedIndex(-1);
		m_SmsList.RemoveAll();
		MzBeginWaitDlg(m_hWnd);
		DateTime::waitms(2);
		m_SmsList.reqUpdate();
		MzEndWaitDlg();
		m_SmsList.Invalidate();
		m_SmsList.Update();
		m_SmsList.ScrollTo();
	}else{
		if(m_SmsList.IsVisible()){
			m_SmsList.SetVisible(false);
			m_SmsList.Invalidate();
			m_SmsList.Update();
		}

		m_List.SetVisible(true);
		m_List.RemoveAll();
		m_List.Invalidate();
		m_List.Update();
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
		MzBeginWaitDlg(m_hWnd);
		DateTime::waitms(2);
		m_List.reqUpdate();
		MzEndWaitDlg();

		m_List.Invalidate();
		m_List.Update();
		m_List.ScrollTo();
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
						m_SmsList.Update();
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
						m_SmsList.Update();
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
						m_SmsList.Update();
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
						m_SmsList.Update();
						SetupToolbar();
					}
				}
			}
			break;
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == 0) {
					EndModal(ID_OK);
					return;
				}
				if(nIndex == 1){
					if((viewStatus & 0x0f) == 3){	//删除按钮
						if(m_SmsList.GetSelectionMode()){//sms list选中模式
							if(MzMessageBoxEx(m_hWnd,
								LOADSTRING(IDS_STR_DELETE_CONFIRM).C_Str(),
								LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
									m_SmsList.DeleteSelectedItems();
									m_SmsList.reqUpdate();
									m_SmsList.Invalidate();
									m_SmsList.Update();
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
				if(nIndex == 2){
					if((viewStatus & 0x0f) == 3){
						m_SmsList.SetSelectionMode();
						m_SmsList.Invalidate();
						m_SmsList.Update();
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
						m_SmsList.Update();
						SetupToolbar();
						return 0;
					}
				}
				//列表
				if (nID == MZ_IDC_LIST && nNotify == MZ_MN_LBUTTONUP) {
					if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
						int nIndex = m_List.CalcIndexOfPos(x, y);
						if(nIndex != -1){
							SmsViewListKey_ptr pkey = m_List.GetListItem(nIndex);
							UINT received = pkey->nReceive;
							UINT sent = pkey->nSend;
							UINT total = sent + received;
							wchar_t strcount[128];
							wchar_t strcount2[128];
							wsprintf(strcount2,L"%d/%d",sent,received);
							wsprintf(strcount,L"%d",total);
							if(viewStatus == 0){
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT + nIndex,nIndex == 0 ? L"联系人":L"日期",strcount,strcount2));
								viewStatus = nIndex == 0 ? 1 : 0x10;
							}else if(viewStatus == 0x1){
								ldb.GetSmsContactCount(pkey->key ,pkey->nReceive,pkey->nSend);
								received = pkey->nReceive;
								sent = pkey->nSend;
								total = sent + received;
								wsprintf(strcount2,L"%d/%d",sent,received);
								wsprintf(strcount,L"%d",total);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT_NAME,pkey->key,strcount,strcount2));
								viewStatus = 3;
								m_SmsList.SetupMode(1);
								m_SmsList.SetupListName(pkey->key);
							}else if(viewStatus == 0x10){
								selectedYear = _wtoi(pkey->key);
								ldb.GetSmsYearCount(selectedYear ,pkey->nReceive,pkey->nSend);
								received = pkey->nReceive;
								sent = pkey->nSend;
								total = sent + received;
								wsprintf(strcount2,L"%d/%d",sent,received);
								wsprintf(strcount,L"%d",total);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_YEAR,pkey->key,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x11){
								selectedMonth = _wtoi(pkey->key);
								ldb.GetSmsMonthCount(selectedYear, selectedMonth, pkey->nReceive,pkey->nSend);
								received = pkey->nReceive;
								sent = pkey->nSend;
								total = sent + received;
								wsprintf(strcount2,L"%d/%d",sent,received);
								wsprintf(strcount,L"%d",total);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_MONTH,pkey->key,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x12){
								selectedDay = _wtoi(pkey->key);
								ldb.GetSmsDayCount(selectedYear, selectedMonth, selectedDay, pkey->nReceive,pkey->nSend);
								received = pkey->nReceive;
								sent = pkey->nSend;
								total = sent + received;
								wsprintf(strcount2,L"%d/%d",sent,received);
								wsprintf(strcount,L"%d",total);
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
					m_SmsList.Update();
					return 0;
				}
				if (nID == MZ_IDC_LIST && nNotify == MZ_MN_MOUSEMOVE) {
					m_List.SetSelectedIndex(-1);
					m_List.Invalidate();
					m_List.Update();
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
			plist_size = pldb->GetSmsContactList();
            if(plist_size > 0){
                plistkey = new SmsViewListKey_t[plist_size];
                plist_size = pldb->GetSmsContactList(plistkey);
            }
            break;
        case 2:
			plist_size = pldb->GetSmsYearList();
            if(plist_size > 0){
                plistkey = new SmsViewListKey_t[plist_size];
                plist_size = pldb->GetSmsYearList(plistkey);
            }
            break;
        case 3:
            plist_size = pldb->GetSmsMonthList(syear);
            if(plist_size > 0){
                plistkey = new SmsViewListKey_t[plist_size];
                plist_size = pldb->GetSmsMonthList(syear,plistkey);
            }
            break;
		case 4:
            plist_size = pldb->GetSmsDayList(syear,smonth);
            if(plist_size > 0){
                plistkey = new SmsViewListKey_t[plist_size];
                plist_size = pldb->GetSmsDayList(syear,smonth,plistkey);
            }
			break;
		case 0:
			plist_size = 2;
			plistkey = new SmsViewListKey_t[plist_size];
			C::newstrcpy(&plistkey->key, LOADSTRING(IDS_STR_VIEW_BY_CONTACT));
			C::newstrcpy(&(plistkey+1)->key, LOADSTRING(IDS_STR_VIEW_BY_DATE));
			pldb->GetSmsCount(plistkey->nReceive,plistkey->nSend);
			(plistkey+1)->nReceive = plistkey->nReceive;
			(plistkey+1)->nSend = plistkey->nSend;
			break;
        default:
            break;
    }
    for(UINT i = 0; i < plist_size; i++){
        ListItem li;
        AddItem(li);
    }
}
void UiKeyList::ClearList(){
	RemoveAll();
    if(plistkey && plist_size > 0){
        for(UINT i = 0; i < plist_size; i++){
            plistkey[i].Reset();
        }
        delete plistkey;
        plistkey = 0;
        plist_size = 0;
    }
}

void UiKeyList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
	if(plistkey == NULL || plist_size == 0) return;
	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

	SmsViewListKey_ptr pkey = plistkey + nIndex;
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
#if 0
	if(smode != 0){
		wsprintf(str,L"%d+%d=%d",pkey->nSend,pkey->nReceive,pkey->nReceive+pkey->nSend);
		//栏目2
		hf = FontHelper::GetFont( 20 );
		SelectObject( hdcDst , hf );
		RECT rc2 = {prcItem->right - 150,prcItem->top,prcItem->right - 5,prcItem->bottom};
		cr = RGB(128,128,128);
		::SetTextColor( hdcDst , cr );
		MzDrawText( hdcDst, str, &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
		DeleteObject(hf);
	}
#endif
}
