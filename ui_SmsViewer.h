#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"
#include "UiImage.h"
#include "UiControlBackground.h"

class UiSmsTitle : public UiStatic {
public:
	UiSmsTitle() { precord = NULL; }
	void SetRecord(SmsSimpleData_ptr p){
		precord = p;
	}
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
private:
	SmsSimpleData_ptr precord;
};

// Main window derived from CMzWndEx
class Ui_SmsViewerWnd : public CMzWndEx {
	MZ_DECLARE_DYNAMIC(Ui_SmsViewerWnd);
private:
	SmsSimpleData_ptr precord;
	int viewIndex;
	int viewSize;
public:
	Ui_SmsViewerWnd();
	~Ui_SmsViewerWnd();
public:
	UiToolbar_Text m_Toolbar;
#ifdef USE_903SDK
	UiMultiLineEdit m_Content;
#else
	UiEdit m_Content;
#endif
	UiButton_Image m_ReportSms;
	UiButton_Image m_ReplySms;
	UiButton_Image m_ForwardSms;
	UiSmsTitle m_Title;
	UiControlBackground m_Background;
public:
	//设置右边列表内容
	void SetViewRecord(SmsSimpleData_ptr p,int idx, int nsize){
		precord = p;
		viewIndex = idx;
		viewSize = nsize;
	}
private:
	void UpdateSms(){
		if(precord){
			if(viewIndex >= viewSize){
				viewIndex = 0;
			}
			if(viewIndex < 0){
				viewIndex = viewSize - 1;
			}
			SmsSimpleData_ptr pr = precord + viewIndex;
			m_Content.SetText(pr->Content);
			m_Content.Invalidate();
			//m_Content.Update();
			m_Title.SetRecord(pr);
			m_Title.Invalidate();
			//m_Title.Update();
		}
	}
protected:
	// Initialization of the window (dialog)
	virtual BOOL OnInitDialog();

	// override the MZFC command handler
	virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};

