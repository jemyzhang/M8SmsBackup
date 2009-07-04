#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiNaviList.h"
#include "LocalDataBase.h"

// Main window derived from CMzWndEx
class Ui_ViewWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ViewWnd);
private:
    //0: main
    //1: contact list 2: phone number list 3: sms list
    //0x10: year list 0x11: month list, 0x12 day list, 0x13 sms list
    UINT viewStatus; 
    SmsViewListKey_ptr plistkey;
    UINT plistSize;
public:
	Ui_ViewWnd();
	~Ui_ViewWnd();
public:
    UiToolbar_Text m_Toolbar;
    UiNaviList m_Navibar;
    UiList m_List;
protected:
    //设置右边列表内容
    void SetupList();
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

