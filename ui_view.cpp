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

Ui_ViewWnd::Ui_ViewWnd(){
	viewStatusSavedBeforeView = 0;
	viewStatus = 0;
	plistkey = 0;
	plistSize = 0;
	ldb.connect();
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
	MzBeginWaitDlg(m_hWnd);
	DateTime::waitms(0);
	ldb.CreateTempSmsTable();
	MzEndWaitDlg();
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
		m_List.SetVisible(false);
		m_List.Invalidate();
		m_List.Update();

		m_SmsList.SetSelectedIndex(-1);
		m_SmsList.RemoveAll();
		m_SmsList.SetVisible(true);
		m_SmsList.ScrollTo();
		m_SmsList.Invalidate();
		m_SmsList.reqUpdate();
		m_SmsList.Update();
		SetupToolbar();
		return;
	}else{
		m_SmsList.SetVisible(false);
		m_List.SetVisible(true);
	}
	if(viewStatus == 0){
		m_List.SetItemHeight(80);
	}else{
		m_List.SetItemHeight(50);
	}
	m_List.RemoveAll();
	m_List.Invalidate();
	m_List.Update();
	ListItem li;
	if(viewStatus == 0){
		li.Text = LOADSTRING(IDS_STR_VIEW_BY_CONTACT);//L"按联系人分类";
		m_List.AddItem(li);

		li.Text = LOADSTRING(IDS_STR_VIEW_BY_DATE);;
		m_List.AddItem(li);
	}else{
		if(plistkey && plistSize > 0){
			for(UINT i = 0; i < plistSize; i++){
				plistkey[i].Reset();
			}
			delete plistkey;
			plistkey = 0;
		}
		MzBeginWaitDlg(m_hWnd);
		DateTime::waitms(0);
		if(viewStatus == 1){
			plistSize = ldb.GetSmsContactList();
		}else if(viewStatus == 0x10){
			plistSize = ldb.GetSmsYearList();
		}else if(viewStatus == 0x11){
			plistSize = ldb.GetSmsMonthList(selectedYear);
		}else if(viewStatus == 0x12){
			plistSize = ldb.GetSmsDayList(selectedYear,selectedMonth);
		}else{
			plistSize = 0;
		}
		if(plistSize > 0){
			plistkey = new SmsViewListKey_t[plistSize];
			if(viewStatus == 1){
				plistSize = ldb.GetSmsContactList(plistkey);
			}else if(viewStatus == 0x10){
				plistSize = ldb.GetSmsYearList(plistkey);
			}else if(viewStatus == 0x11){
				plistSize = ldb.GetSmsMonthList(selectedYear,plistkey);
			}else if(viewStatus == 0x12){
				plistSize = ldb.GetSmsDayList(selectedYear,selectedMonth,plistkey);
			}
			for(UINT i = 0; i < plistSize; i++){
				wchar_t strkey[128];
				if(viewStatus == 0x11){
					wsprintf(strkey,L"%04d-%s‖%d+%d=%d",selectedYear,
						plistkey[i].key,plistkey[i].nReceive,plistkey[i].nSend,
						plistkey[i].nReceive+plistkey[i].nSend);
				}else if(viewStatus == 0x12){
					wsprintf(strkey,L"%04d-%02d-%s‖%d+%d=%d",selectedYear,selectedMonth,
						plistkey[i].key,plistkey[i].nReceive,plistkey[i].nSend,
						plistkey[i].nReceive+plistkey[i].nSend);
				}else{
					wsprintf(strkey,L"%s‖%d+%d=%d",
						plistkey[i].key,plistkey[i].nReceive,plistkey[i].nSend,
						plistkey[i].nReceive+plistkey[i].nSend);
				}
				li.Text = strkey;
				if(viewStatus >= 0x10 && viewStatus < 0x13){
					WORD num;
					swscanf(plistkey[i].key,L"%d",&num);
					li.Data = (void *)num;
				}
				m_List.AddItem(li);
			}
		}
		MzEndWaitDlg();
	}
	m_List.ScrollTo();
	m_List.Invalidate();
	m_List.Update();
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
							if(viewStatus == 0){
								UINT received, sent, total;
								total = ldb.GetSmsCount(received,sent);
								wchar_t strcount2[128];
								wsprintf(strcount2,L"%d/%d",received,sent,total);
								wchar_t strcount[128];
								wsprintf(strcount,L"%d",total);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT + nIndex,nIndex == 0 ? L"联系人":L"日期",strcount,strcount2));
								viewStatus = nIndex == 0 ? 1 : 0x10;
							}else if(viewStatus == 0x1){
								UINT received, sent, total;
								wchar_t *name = C::_wcstok(m_List.GetItem(nIndex)->Text.C_Str(),L"‖");
								total = ldb.GetSmsContactCount(name,received,sent);
								wchar_t strcount2[128];
								wsprintf(strcount2,L"%d/%d",received,sent,total);
								wchar_t strcount[128];
								wsprintf(strcount,L"%d",total);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT_NAME,name,strcount,strcount2));
								viewStatus = 3;
								m_SmsList.SetupMode(1);
								m_SmsList.SetupListName(name);
							}else if(viewStatus == 0x10){
								UINT received, sent, total;
								selectedYear = (WORD)m_List.GetItem(nIndex)->Data;
								total = ldb.GetSmsYearCount(selectedYear,received,sent);
								wchar_t strcount2[128];
								wsprintf(strcount2,L"%d/%d",received,sent,total);
								wchar_t strcount[128];
								wsprintf(strcount,L"%d",total);
								wchar_t strLabel[16];
								wsprintf(strLabel,L"%04d",selectedYear);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_YEAR,strLabel,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x11){
								UINT received, sent, total;
								selectedMonth = (WORD)m_List.GetItem(nIndex)->Data;
								total = ldb.GetSmsMonthCount(selectedYear,selectedMonth,received,sent);
								wchar_t strcount2[128];
								wsprintf(strcount2,L"%d/%d",received,sent,total);
								wchar_t strcount[128];
								wsprintf(strcount,L"%d",total);
								wchar_t strLabel[16];
								wsprintf(strLabel,L"%02d",selectedMonth);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_MONTH,strLabel,strcount,strcount2));
								viewStatus += 1;
							}else if(viewStatus == 0x12){
								UINT received, sent, total;
								selectedDay = (WORD)m_List.GetItem(nIndex)->Data;
								total = ldb.GetSmsDayCount(selectedYear,selectedMonth,selectedDay,received,sent);
								wchar_t strcount2[128];
								wsprintf(strcount2,L"%d/%d",received,sent,total);
								wchar_t strcount[128];
								wsprintf(strcount,L"%d",total);
								wchar_t strLabel[16];
								wsprintf(strLabel,L"%02d",selectedDay);
								m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_DATE_DAY,strLabel,strcount,strcount2));
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
void UiSmsAmountList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {

	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

	wchar_t *s = NULL;
	C::newstrcpy(&s,GetItem(nIndex)->Text.C_Str());

    HFONT hf;
    COLORREF cr;
    
	wchar_t *str = C::_wcstok(s,L"‖");
	wchar_t *str2 = C::_wcstok(NULL,L"‖");
	if(str){
		//栏目1
		hf = FontHelper::GetFont( str2 == NULL ? 30 : 25 );
		SelectObject( hdcDst , hf );
		RECT rc1 = {prcItem->left + 10,prcItem->top,prcItem->right - (str2 == NULL ? 10 : 150),prcItem->bottom};
		cr = RGB(0,0,0);
		::SetTextColor( hdcDst , cr );
		MzDrawText( hdcDst , str, &rc1 , DT_VCENTER|(str2 == NULL ? DT_CENTER : DT_LEFT)|DT_SINGLELINE|DT_WORD_ELLIPSIS );
		DeleteObject(hf);
	}

	if(str2){
		//栏目2
		hf = FontHelper::GetFont( 20 );
		SelectObject( hdcDst , hf );
		RECT rc2 = {prcItem->right - 150,prcItem->top,prcItem->right - 5,prcItem->bottom};
		cr = RGB(128,128,128);
		::SetTextColor( hdcDst , cr );
		MzDrawText( hdcDst, str2, &rc2 , DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
		DeleteObject(hf);
	}
	delete [] s;
}
