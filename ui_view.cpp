#include "ui_view.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "resource.h"

extern HINSTANCE LangresHandle;
#define MZ_IDC_TOOLBAR_MAIN 101

#define MZ_IDC_BUTTON_MAIN_VIEW 102
#define MZ_IDC_BUTTON_VIEW_CONTACT 103
#define MZ_IDC_BUTTON_VIEW_DATE 104

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
    }else if(viewStatus == 1){   //联系人列表
        if(plistkey && plistSize > 0){
            for(UINT i = 0; i < plistSize; i++){
                plistkey[i].Reset();
            }
            delete plistkey;
            plistkey = 0;
        }
        MzBeginWaitDlg(m_hWnd);
        DateTime::waitms(0);
        plistSize = ldb.GetSmsContactList();
        if(plistSize > 0){
            plistkey = new SmsViewListKey_t[plistSize];
            plistSize = ldb.GetSmsContactList(plistkey);
            for(UINT i = 0; i < plistSize; i++){
                wchar_t strkey[128];
                wsprintf(strkey,L"%s (%d/%d)",plistkey[i].key,plistkey[i].nReceive,plistkey[i].nSend);
                li.Text = strkey;
                m_List.AddItem(li);
            }
        }
        MzEndWaitDlg();
    }
    m_List.Invalidate();
    m_List.Update();
}

void Ui_ViewWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BUTTON_MAIN_VIEW:
            {
                m_Navibar.popUntil(MZ_IDC_BUTTON_MAIN_VIEW);
                viewStatus = 0;
                SetupList();
                break;
            }
        case MZ_IDC_BUTTON_VIEW_CONTACT:
            {
                break;
            }
        case MZ_IDC_BUTTON_VIEW_DATE:
            {
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
                                total = ldb.GetSmsCount(&received,&sent);
                                wchar_t strcount[128];
                                wsprintf(strcount,L"(%d/%d)",received,sent);
                                m_Navibar.push(new UiNaviButton(MZ_IDC_BUTTON_VIEW_CONTACT + nIndex,m_List.GetItem(nIndex)->Text.C_Str(),strcount));
                                viewStatus = nIndex == 0 ? 1 : 0x10;
                                SetupList();
                            }
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