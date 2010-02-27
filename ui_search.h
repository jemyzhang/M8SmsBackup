#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"
#include "UiSmsList.h"
#include "UiWaitMessageDlg.h"

class SearchWaitDlg : public Ui_WaitMessageDlgWnd{
public:
    SearchWaitDlg()
      :Ui_WaitMessageDlgWnd(L"À—À˜÷–£¨«Î…‘∫Ú°£"){
          m_pSmsList = 0;
      }
    bool CallBackProcess();
    UiSmsList *m_pSmsList;
};

// Main window derived from CMzWndEx
class Ui_SearchWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_SearchWnd);
private:
    SmsViewListKey_ptr plistkey;
    UINT plistSize;
public:
	Ui_SearchWnd();
	~Ui_SearchWnd();
public:
    UiToolbar_Text m_Toolbar;
	UiSingleLineEdit m_SearchBox;
	UiButton_Image m_ButtonSearch;
    UiSmsList m_SmsList;
	ImageContainer m_imgContainer;
protected:
    //…Ë÷√”“±ﬂ¡–±Ìƒ⁄»›
    void SetupList();
	void SetupToolbar();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};

