#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

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
	void updateUi(){
		updateUseSimPhoneBook();
		updateBootUpdate();
	}
public:
    UiToolbar_Text m_Toolbar;	//ȷ��
	UiCaption  m_Caption;
	UiButtonEx m_BtnUseSimPhoneBook;		//ʹ��SIM��ϵ��
	UiButtonEx m_BtnUseSimPhoneBookSW;	//ʹ��SIM��ϵ�� ON/OFF
	UiButtonEx m_BtnBootUpdate;		//����ʱ�Զ�����
	UiButtonEx m_BtnBootUpdateSW;	//����ʱ�Զ����� ON/OFF
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};