#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiNaviList.h"
#include "LocalDataBase.h"
#include "UiSmsList.h"

class UiSmsAmountList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
};

// Main window derived from CMzWndEx
class Ui_ViewWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ViewWnd);
private:
    //0: main
    //1: contact list 2: phone number list 3: sms list
    //0x10: year list 0x11: month list, 0x12 day list, 0x13 sms list
    UINT viewStatus;
	UINT viewStatusSavedBeforeView;	//查看按钮按下时保持前一值
    SmsViewListKey_ptr plistkey;
    UINT plistSize;
    WORD selectedYear;
    WORD selectedMonth;
    WORD selectedDay;
public:
	Ui_ViewWnd();
	~Ui_ViewWnd();
public:
    UiToolbar_Text m_Toolbar;
    UiNaviList m_Navibar;
    UiSmsAmountList m_List;
    UiSmsList m_SmsList;
protected:
    //设置右边列表内容
    void SetupList();
	//设置工具栏内容
	void SetupToolbar();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    LocalDataBase ldb;
};

