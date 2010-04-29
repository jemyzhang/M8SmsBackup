#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

//���������ý���
class Ui_ToolWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_ToolWnd);
public:
	Ui_ToolWnd(void);
	~Ui_ToolWnd(void);
public:
    UiToolBarPro m_Toolbar;	//ȷ��
	UiButtonEx m_BtnBackup;	//�������ݿ�
	UiButtonEx m_BtnOptimize;	//�Ż����ݿ⣬��������
	UiButtonEx m_BtnClearContact;	//���µ�����ϵ��
	UiButtonEx m_BtnExpContact;	//������ϵ��Ϊ�ı�
	UiButtonEx m_BtnExpSms;	//��������Ϊ�ı�
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
