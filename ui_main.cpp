#include "ui_main.h"
#include "ui_ProgressBar.h"

#define MZ_IDC_TOOLBAR_MAIN 101

#define MZ_IDC_BUTTON_VIEW 103
#define MZ_IDC_BUTTON_REFRESH 104
#define MZ_IDC_BUTTON_CONFIG 105

MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

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
	/*left for logo
	wchar_t name[128];
	wsprintf(name,L"%s v%s",LOADSTRING(IDS_STR_APPNAME).C_Str(),VER_STRING);
	m_lblTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION/2);
	m_lblTitle.SetText(name);
	m_lblTitle.SetTextSize(m_lblTitle.GetTextSize()/2);
	m_lblTitle.SetTextWeight(FW_BOLD);
	m_lblTitle.SetID(MZ_IDC_CAPTION_TITLE);
	m_lblTitle.EnableNotifyMessage(true);
	AddUiWin(&m_lblTitle);
	*/

	y+=MZM_HEIGHT_CAPTION*2;

	m_BtnView.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnView.SetID(MZ_IDC_BUTTON_VIEW);
	m_BtnView.SetText(L"查看短信");
    m_BtnView.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	AddUiWin(&m_BtnView);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnRefresh.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnRefresh.SetID(MZ_IDC_BUTTON_REFRESH);
	m_BtnRefresh.SetText(L"立即刷新");
    m_BtnRefresh.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	AddUiWin(&m_BtnRefresh);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnConfig.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnConfig.SetID(MZ_IDC_BUTTON_CONFIG);
	m_BtnConfig.SetText(L"选项");
    m_BtnConfig.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	AddUiWin(&m_BtnConfig);

	y+=MZM_HEIGHT_BUTTONEX*2;
	m_TextAbout.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - y);
	m_TextAbout.SetEnable(false);
	m_TextAbout.SetText(L"作者：JEMYZHANG\n更新日期：2009-07-03");
	m_TextAbout.SetTextColor(RGB(128,128,128));
	m_TextAbout.SetDrawTextFormat(DT_RIGHT);
	m_TextAbout.SetTextSize(20);
	AddUiWin(&m_TextAbout);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetButton(1,true,true,L"退出");
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
	AddUiWin(&m_Toolbar);

	return TRUE;
}



void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	int nRet;
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_VIEW:
			{
				break;
			}
		case MZ_IDC_BUTTON_REFRESH:
			{
				WORD n;
				initUiCallbackRefreshContact();
				n = refreshContact(uiCallbackRefreshContact);
				initUiCallbackRefreshSms();
				n = refreshSms(uiCallbackRefreshSms);
				break;
			}
		case MZ_IDC_BUTTON_CONFIG:
			{
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
