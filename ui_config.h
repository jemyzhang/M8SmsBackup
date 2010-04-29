#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiSwitchOption.h"

//过滤器设置界面
class Ui_ConfigWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_ConfigWnd);
public:
	Ui_ConfigWnd(void);
	~Ui_ConfigWnd(void);
private:
	void updateUseSimPhoneBook();
	void updateBootUpdate();
    void updateUpdateMethod();
	void updateUi(){
		updateUseSimPhoneBook();
		updateBootUpdate();
        updateUpdateMethod();
	}
    LPCTSTR SetPassword(wchar_t *pWd, int len);
public:
    UiToolBarPro m_Toolbar;	//确定
	UiSwitchOption m_BtnUseSimPhoneBook;		//使用SIM联系人
	UiSwitchOption m_BtnBootUpdate;		//启动时自动更新
	UiButtonEx m_BtnUpdateMethod;	//更新方式
    UiButtonEx m_BtnSetupPassword;

    CMzPopupWaitDialog m_progress;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
