#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <MzCommon.h>
#include "UiControlBackground.h"

class Ui_PasswordWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_PasswordWnd);
public:
	Ui_PasswordWnd(void);
	~Ui_PasswordWnd(void);
public:
    UiToolbar_Text m_Toolbar;
    UiSingleLineEdit m_EdtPassword;
	UiControlBackground m_Background;
public:
	void getPassword(wchar_t** p,int* plen);
	void setMode(int m) { _uiMode = m; }
	virtual int DoModal(){
		_isModal = true;
		return CMzWndEx::DoModal();
	}
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
//    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	int _uiMode;	//0: enter password, 1: setup password
	bool _isModal;
};
