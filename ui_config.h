#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiSwitchOption.h"

//���������ý���
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
    UiToolBarPro m_Toolbar;	//ȷ��
	UiSwitchOption m_BtnUseSimPhoneBook;		//ʹ��SIM��ϵ��
	UiSwitchOption m_BtnBootUpdate;		//����ʱ�Զ�����
	UiButtonEx m_BtnUpdateMethod;	//���·�ʽ
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
