#include "UiWaitMessageDlg.h"

#include <cMzCommon.h>
using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_WaitMessageDlgWnd)

Ui_WaitMessageDlgWnd::Ui_WaitMessageDlgWnd(const TCHAR* content){
    m_messageText = NULL;
    if(content){
        C::newstrcpy(&m_messageText,content);
    }else{
        C::newstrcpy(&m_messageText,L"«Î…‘µ»...");
    }
    nRet = false;
}

Ui_WaitMessageDlgWnd::~Ui_WaitMessageDlgWnd(){
    if(m_messageText) delete m_messageText;
}

BOOL Ui_WaitMessageDlgWnd::OnInitDialog(){
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }
	m_progress.EnableCancelButton(FALSE);
    return TRUE;
}

int Ui_WaitMessageDlgWnd::DoModal(){
	m_progress.BeginWaitDialog(m_hWnd,m_messageText,TRUE);
    SetTimer(m_hWnd,0x1001,100,NULL);
    return CMzWndEx::DoModal();
}

void Ui_WaitMessageDlgWnd::OnTimer(UINT nIDEvent){
    switch(nIDEvent){
        case 0x1001:
            KillTimer(m_hWnd,0x1001);
            SetTimer(m_hWnd,0x1002,100,NULL);
            nRet = CallBackProcess();
            break;
        case 0x1002:
			m_progress.EndWaitDialog();
            EndModal(nRet ? ID_OK : ID_CANCEL);
            break;
    }
}

void Ui_WaitMessageDlgWnd::setMessage(LPTSTR msg){
    if(msg){
        C::newstrcpy(&m_messageText,msg);
    }else{
        C::newstrcpy(&m_messageText,L"«Î…‘µ»...");
    }
	m_progress.ResetTipText(m_messageText,TRUE);
}
