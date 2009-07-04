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

#define MZ_IDC_LIST 120

MZ_IMPLEMENT_DYNAMIC(Ui_ViewWnd)

Ui_ViewWnd::Ui_ViewWnd(){
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
    m_Navibar.SetPos(0, y, 100, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_MAIN_VIEW,L"查看"));
    AddUiWin(&m_Navibar);

    m_List.SetPos(100, y, GetWidth() - 100, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_List.SetID(MZ_IDC_LIST);
    m_List.EnableNotifyMessage(true);
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

    SetupList();
    ldb.CreateTempSmsTable();
    return TRUE;
}

void Ui_ViewWnd::SetupList(){
    m_List.RemoveAll();
    ListItem li;
    if(viewStatus == 0){
        li.Text = L"按联系人分类";
        m_List.AddItem(li);

        li.Text = L"按时间分类";
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
                wsprintf(strkey,L"%s (%d+%d=%d)",
                    plistkey[i].key,plistkey[i].nReceive,plistkey[i].nSend,
                    plistkey[i].nReceive+plistkey[i].nSend);
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
        case MZ_IDC_BUTTON_VIEW_DATE:
            {
                if(viewStatus != 0x10){
                    m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE);
                    viewStatus = 0x10;
                    SetupList();
                }
                break;
            }
        case MZ_IDC_BUTTON_VIEW_DATE_YEAR:
            {
                if(viewStatus != 0x11){
                    m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE_YEAR);
                    viewStatus = 0x11;
                    SetupList();
                }
                break;
            }
        case MZ_IDC_BUTTON_VIEW_DATE_MONTH:
            {
                if(viewStatus != 0x12){
                    m_Navibar.popUntil(MZ_IDC_BUTTON_VIEW_DATE_MONTH);
                    viewStatus = 0x12;
                    SetupList();
                }
                break;
            }
        case MZ_IDC_TOOLBAR_MAIN:
            {
                int nIndex = lParam;
                if (nIndex == 0) {
                    EndModal(ID_OK);
                    return;
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
                            }
                            SetupList();
                        }
#if 0
                        m_List.SetSelectedIndex(nIndex);
                        m_List.Invalidate();
                        m_List.Update();
#endif
                    }
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