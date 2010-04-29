#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

//过滤器设置界面
class Ui_ToolWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_ToolWnd);
public:
	Ui_ToolWnd(void);
	~Ui_ToolWnd(void);
public:
    UiToolBarPro m_Toolbar;	//确定
	UiButtonEx m_BtnBackup;	//备份数据库
	UiButtonEx m_BtnOptimize;	//优化数据库，创建索引
	UiButtonEx m_BtnClearContact;	//重新导入联系人
	UiButtonEx m_BtnExpContact;	//导出联系人为文本
	UiButtonEx m_BtnExpSms;	//导出短信为文本
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
