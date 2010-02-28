#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

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
public:
    UiToolbar_Text m_Toolbar;	//确定
	UiButtonEx m_BtnUseSimPhoneBook;		//使用SIM联系人
	UiButtonEx m_BtnUseSimPhoneBookSW;	//使用SIM联系人 ON/OFF
	UiButtonEx m_BtnBootUpdate;		//启动时自动更新
	UiButtonEx m_BtnBootUpdateSW;	//启动时自动更新 ON/OFF
	UiButtonEx m_BtnUpdateMethod;	//更新方式
    UiButtonEx m_BtnSetupPassword;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
