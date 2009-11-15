#include "ui_main.h"
#include "ui_ProgressBar.h"
#include <MzCommon.h>
using namespace MzCommon;
#include "resource.h"
#include "ui_view.h"
#include "ui_search.h"
#include "appconfigini.h"
#include "ui_config.h"
#include "passwordDlg.h"

#ifdef DEBUG
#define BUILD_CONFIG	L"D"
#else
#define BUILD_CONFIG	L"R"
#endif
#ifdef MZFC_STATIC
#define BUILD_METHOD L"s"
#else
#define BUILD_METHOD L"d"
#endif
#define VER_STRING L"1.61"BUILD_METHOD
#define BUILD_STRING L"20091115"BUILD_CONFIG

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
extern HINSTANCE LangresHandle;
extern SmsBackupConfig appconfig;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_BUTTON_VIEW 103
#define MZ_IDC_BUTTON_REFRESH 104
#define MZ_IDC_BUTTON_CONFIG 105
#define MZ_IDC_BUTTON_SEARCH 106

MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

extern wchar_t g_password[256];
extern int g_password_len;

Ui_MainWnd::Ui_MainWnd(){
}

Ui_MainWnd::~Ui_MainWnd(){
}

BOOL Ui_MainWnd::OnInitDialog() {
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	int y = 0;
	m_Logo.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION*3);
	m_Logo.setupImage(pimg[IDB_PNG_LOGO - IDB_PNG_BEGIN]);
	AddUiWin(&m_Logo);

	y+=MZM_HEIGHT_CAPTION*3;

	m_BtnView.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnView.SetID(MZ_IDC_BUTTON_VIEW);
	m_BtnView.SetText(LOADSTRING(IDS_STR_VIEW_SMS).C_Str());
	m_BtnView.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnView.SetText2(LOADSTRING(IDS_STR_MENU_VIEW_TIP).C_Str());
	m_BtnView.SetTextMaxLen2(0);
	AddUiWin(&m_BtnView);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnSearch.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnSearch.SetID(MZ_IDC_BUTTON_SEARCH);
	m_BtnSearch.SetText(LOADSTRING(IDS_STR_SEARCH).C_Str());
	m_BtnSearch.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnSearch.SetText2(LOADSTRING(IDS_STR_MENU_SEARCH_TIP).C_Str());
	m_BtnSearch.SetTextMaxLen2(0);
	AddUiWin(&m_BtnSearch);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnRefresh.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnRefresh.SetID(MZ_IDC_BUTTON_REFRESH);
	m_BtnRefresh.SetText(LOADSTRING(IDS_STR_REFRESH_DB).C_Str());
	m_BtnRefresh.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnRefresh.SetText2(LOADSTRING(IDS_STR_MENU_UPDATE_TIP).C_Str());
	m_BtnRefresh.SetTextMaxLen2(0);
	AddUiWin(&m_BtnRefresh);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnConfig.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnConfig.SetID(MZ_IDC_BUTTON_CONFIG);
	m_BtnConfig.SetText(LOADSTRING(IDS_STR_CONFIGURATIONS).C_Str());
	m_BtnConfig.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnConfig.SetText2(LOADSTRING(IDS_STR_MENU_CONFIG_TIP).C_Str());
	m_BtnConfig.SetTextMaxLen2(0);
	AddUiWin(&m_BtnConfig);

	y=GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_BUTTONEX*2;
	m_TextAbout.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - y);
	m_TextAbout.SetEnable(false);
	m_TextAbout.SetTextColor(RGB(128,128,128));
	m_TextAbout.SetDrawTextFormat(DT_RIGHT);
	m_TextAbout.SetTextSize(20);
	CMzString sAbout;
	wchar_t sa[256];
	wsprintf(sa,LOADSTRING(IDS_STR_APPAUTHOR).C_Str(),L"JEMYZHANG");
	sAbout = sa;
	sAbout = sAbout + L"\n";
	wsprintf(sa,LOADSTRING(IDS_STR_APPVERSION).C_Str(),VER_STRING,BUILD_STRING);
	sAbout = sAbout + sa;
	sAbout = sAbout + L"\n";
	sAbout = sAbout + L"Email: jemy.zhang@gmail.com\n";
	sAbout = sAbout + L"(C)2009 JEMYZHANG  保留所有权利";
	m_TextAbout.SetText(sAbout.C_Str());
	AddUiWin(&m_TextAbout);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetButton(1,true,true,LOADSTRING(IDS_STR_EXIT).C_Str());
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	return TRUE;
}

void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
    if(id >= MZ_IDC_BUTTON_VIEW && id <= MZ_IDC_BUTTON_SEARCH){
        if(!CheckPassword(m_hWnd)){
            return;
        }
    }
	switch (id) {
		case MZ_IDC_BUTTON_VIEW:
			{
				Ui_ViewWnd dlg;
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				dlg.DoModal();
				break;
			}
		case MZ_IDC_BUTTON_SEARCH:
			{
				Ui_SearchWnd dlg;
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				dlg.DoModal();
				break;
			}
		case MZ_IDC_BUTTON_REFRESH:
			{
				WORD n;
				if(appconfig.IniUseSimPhoneBook.Get()){
					initUiCallbackRefreshContact();
					n = refreshSIMContact(uiCallbackRefreshSIMContact);
				}
				initUiCallbackRefreshContact();
				n = refreshContact(uiCallbackRefreshContact);
				initUiCallbackRefreshSms();
				n = refreshSms(uiCallbackRefreshSms);
				break;
			}
		case MZ_IDC_BUTTON_CONFIG:
			{
				Ui_ConfigWnd dlg;
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				dlg.DoModal();
				break;
			}
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if (nIndex == 1) {
					PostQuitMessage(0);
					return;
				}
			}
			break;
	}
}
