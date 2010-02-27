#pragma once
/*
 * @filename UiSingleOption.h
 * @note 等待对话框
 * @author JEMYZHANG
 * @date 2009.10.17
 * @ver. 1.0
 * @changelog
 *  ver.1.0 初始化程序
*/
// include the MZFC library header file
#include <mzfc_inc.h>

class UiBackground :
	public UiWin
{
public:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridDlgBG(hdcDst,prcWin);
	}
};

class Ui_WaitMessageDlgWnd : 
    public CMzWndEx{
    MZ_DECLARE_DYNAMIC(Ui_WaitMessageDlgWnd);
public:
    Ui_WaitMessageDlgWnd(const TCHAR* content = NULL);
    ~Ui_WaitMessageDlgWnd();
public:
    UiBackground m_Bg;
    UiStatic m_Message;
public:
    BOOL OnInitDialog();
    virtual bool CallBackProcess() = 0;
    int DoModal();
    void OnTimer(UINT nIDEvent);
    void setMessage(LPTSTR msg);
private:
    bool nRet;
    LPTSTR m_messageText;
};