#include "ui_SmsViewer.h"
#include <MzCommon.h>
using namespace MzCommon;
#include "resource.h"

extern HINSTANCE LangresHandle;
extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];

#define MZ_IDC_TOOLBAR_MAIN 101

#define MZ_IDC_BUTTON_FORWARD 102
#define MZ_IDC_BUTTON_REPLY 103
#define MZ_IDC_BUTTON_REPORT 104


void UiSmsTitle::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	if(precord == NULL) return;
	HFONT hf;
	COLORREF cr;
	//收/发标记
	UINT SmsResID;
	if(precord->SendReceiveFlag){
		SmsResID = IDB_PNG_SMS_SEND - IDB_PNG_BEGIN;
	}else{
		SmsResID = IDB_PNG_SMS_RECV - IDB_PNG_BEGIN;
	}
	SetBkMode(hdcDst,TRANSPARENT);
	RECT rcSendReceive = {prcWin->left,prcWin->top + 20,prcWin->left + 40,prcWin->top + 60};
	pimg[SmsResID]->Draw(hdcDst,&rcSendReceive);

	//姓名
	hf = FontHelper::GetFont( 30 );
	SelectObject( hdcDst , hf );
	RECT rcName = {rcSendReceive.right,prcWin->top,rcSendReceive.right + 200,rcSendReceive.bottom};
	cr = RGB(255,255,255);
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , precord->ContactName, &rcName , DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

	//号码
	hf = FontHelper::GetFont( 20 );
	SelectObject( hdcDst , hf );
	RECT rcNumber = {prcWin->right - 200,prcWin->top,prcWin->right - 5,rcName.bottom - 20};
	cr = RGB(128,128,128);
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , precord->MobileNumber, &rcNumber , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

	//日期
	hf = FontHelper::GetFont( 20 );
	SelectObject( hdcDst , hf );
	RECT rcDate = {prcWin->right - 200,rcNumber.bottom,prcWin->right - 5,rcNumber.bottom + 20};
	cr = RGB(128,128,128);
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , precord->TimeStamp, &rcDate , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);
}

MZ_IMPLEMENT_DYNAMIC(Ui_SmsViewerWnd)

Ui_SmsViewerWnd::Ui_SmsViewerWnd(){
	precord = NULL;
	viewIndex = 0;
}

Ui_SmsViewerWnd::~Ui_SmsViewerWnd(){
}

BOOL Ui_SmsViewerWnd::OnInitDialog() {
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	m_Background.SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	AddUiWin(&m_Background);

	int y = 10;
	m_Title.SetPos(10, y, GetWidth() - 20, 60);
	m_Background.AddChild(&m_Title);

	y+=65;
	m_Content.SetPos(10, y, GetWidth() - 20 - 48, GetHeight() - y - MZM_HEIGHT_TEXT_TOOLBAR);
	m_Content.SetReadOnly(true);
#ifdef USE_903SDK
	m_Content.SetInsideScroll(true);
#endif
	m_Content.SetTextColor( RGB(255,255,255));
	m_Background.AddChild(&m_Content);

	int btnX = GetWidth() - 10 - 48;
	int btnY = y + 5;
	m_ForwardSms.SetPos(btnX, btnY, 55, 55);
	m_ForwardSms.SetButtonType(MZC_BUTTON_NONE);
	m_ForwardSms.SetID(MZ_IDC_BUTTON_FORWARD);
	m_ForwardSms.SetImage_Normal(pimg[IDB_PNG_FORWARD_SMS - IDB_PNG_BEGIN]);
	m_ForwardSms.SetImage_Pressed(pimg[IDB_PNG_FORWARD_SMS - IDB_PNG_BEGIN]);
	m_Background.AddChild(&m_ForwardSms);

	btnY += 60;
	m_ReplySms.SetPos(btnX, btnY, 55, 55);
	m_ReplySms.SetButtonType(MZC_BUTTON_NONE);
	m_ReplySms.SetID(MZ_IDC_BUTTON_REPLY);
	m_ReplySms.SetImage_Normal(pimg[IDB_PNG_REPLY_SMS - IDB_PNG_BEGIN]);
	m_ReplySms.SetImage_Pressed(pimg[IDB_PNG_REPLY_SMS - IDB_PNG_BEGIN]);
	m_Background.AddChild(&m_ReplySms);

	btnY += 60;
	m_ReportSms.SetPos(btnX, btnY, 55, 55);
	m_ReportSms.SetButtonType(MZC_BUTTON_NONE);
	m_ReportSms.SetID(MZ_IDC_BUTTON_REPORT);
	m_ReportSms.SetImage_Normal(pimg[IDB_PNG_REPORT_SMS - IDB_PNG_BEGIN]);
	m_ReportSms.SetImage_Pressed(pimg[IDB_PNG_REPORT_SMS - IDB_PNG_BEGIN]);
	m_Background.AddChild(&m_ReportSms);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetButton(0,true,true,LOADSTRING(IDS_STR_PREVIOUS).C_Str());
	m_Toolbar.SetButton(1,true,true,LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_NEXT).C_Str());
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	UpdateSms();
	return TRUE;
}

void Ui_SmsViewerWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_FORWARD:	//转发
			{
				MzBeginWaitDlg(m_hWnd);
				SmsSimpleData_ptr pr = precord + viewIndex;
				PROCESS_INFORMATION pi;
				CMzString s = L"-n ";
				s = s + L"▓";
				s = s + pr->Content;
				CreateProcess(L"\\windows\\sms.exe", 
					s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
				MzEndWaitDlg();
				break;
			}
		case MZ_IDC_BUTTON_REPLY:	//回复
			{
				MzBeginWaitDlg(m_hWnd);
				SmsSimpleData_ptr pr = precord + viewIndex;
				PROCESS_INFORMATION pi;
				CMzString s = L"-n ";
				s = s + pr->MobileNumber;
				s = s + L"▓";
				CreateProcess(L"\\windows\\sms.exe", 
					s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
				MzEndWaitDlg();
				break;
			}
		case MZ_IDC_BUTTON_REPORT:	//举报
			{
				MzBeginWaitDlg(m_hWnd);
				SmsSimpleData_ptr pr = precord + viewIndex;
				PROCESS_INFORMATION pi;
				CMzString s = L"-n ";
				s = s + L"10086999";
				s = s + L" ▓";
				s = s + pr->MobileNumber;
				s = s + L" ";
				s = s + pr->Content;
				CreateProcess(L"\\windows\\sms.exe", 
					s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
				MzEndWaitDlg();
				break;
			}
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == 1) {
					EndModal(ID_OK);
					return;
				}
				if (nIndex == 0) {
					viewIndex--;
					UpdateSms();
					return;
				}
				if (nIndex == 2) {
					viewIndex++;
					UpdateSms();
					return;
				}
			}
			break;
	}
}
