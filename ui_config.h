#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <UiWaitMessageDlg.h>

class OptimizeWaitDlg : public Ui_WaitMessageDlgWnd{
public:
    OptimizeWaitDlg()
        :Ui_WaitMessageDlgWnd(L"���ݿ��Ż��У����Ժ�"){
    }
public:
    bool CallBackProcess();
};

class SetPasswordWaitDlg : public Ui_WaitMessageDlgWnd{
public:
    SetPasswordWaitDlg()
        :Ui_WaitMessageDlgWnd(L"�趨�����У����Ժ�"){
            pWd = 0;
            len = 0;
    }
    ~SetPasswordWaitDlg(){
        if(pWd) delete pWd;
    }
public:
    bool CallBackProcess();
    void setPassword(wchar_t* p, int sz);
private:
    wchar_t* pWd;
    int len;
};

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
public:
    UiToolbar_Text m_Toolbar;	//ȷ��
	UiCaption  m_Caption;
	UiButtonEx m_BtnUseSimPhoneBook;		//ʹ��SIM��ϵ��
	UiButtonEx m_BtnUseSimPhoneBookSW;	//ʹ��SIM��ϵ�� ON/OFF
	UiButtonEx m_BtnBootUpdate;		//����ʱ�Զ�����
	UiButtonEx m_BtnBootUpdateSW;	//����ʱ�Զ����� ON/OFF
	UiButtonEx m_BtnUpdateMethod;	//���·�ʽ
    UiButtonEx m_BtnSetupPassword;
	UiButtonEx m_BtnBackup;	//�������ݿ�
	UiButtonEx m_BtnOptimize;	//�Ż����ݿ⣬��������
	UiButtonEx m_BtnClearContact;	//�Ż����ݿ⣬��������
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
