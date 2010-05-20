#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

class UiContactFiltList;
class UiSmsList;

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
    UiHeadingBar m_TitleBar;
    UiToolBarPro m_Toolbar;
	UiSingleLineEdit m_SearchBox;
	UiSingleLineEdit m_ContactBox;
	UiButton_Image m_ButtonSearch;
    UiSmsList *m_pSmsList;
    UiContactFiltList *m_pContactList;
	ImageContainer m_imgContainer;
protected:
    //设置右边列表内容
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

