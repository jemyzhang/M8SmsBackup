#include <Mzfc/MzProgressDialog.h> 
#include "ui_ProgressBar.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

static bool brangeSet = false;
static bool bdlgshown = false;
static MzProgressDialog m_Progressdlg;

void SetProgressBarTitle(LPWSTR t){
	if(t == NULL){
		m_Progressdlg.SetShowTitle(false);
		return;
	}
	m_Progressdlg.SetTitle(t);
	m_Progressdlg.SetShowTitle(true);
}
void SetProgressBarRange(WORD rmin, WORD rmax){
	if(brangeSet) return;
	if(rmin > rmax){
		WORD temp = rmin;
		rmin = rmax;
		rmax = temp;
	}
	m_Progressdlg.SetRange(rmin,rmax);
	brangeSet = true;
}
void ShowProgressBar(){
	if(!bdlgshown){
		m_Progressdlg.BeginProgress(0);
		bdlgshown = true;
	}
}
void HideProgressBar(){
	m_Progressdlg.EndProgress();
	bdlgshown = false;
}

void initProgressBar(LPWSTR title = NULL, WORD rmin = 0, WORD rmax = 100){
	SetProgressBarTitle(title);
	SetProgressBarRange(rmin,rmax);
	m_Progressdlg.SetShowInfo(true);
	brangeSet = false;
	bdlgshown = false;
}

void initUiCallbackRefreshContact(){
	initProgressBar(L"更新联系人记录...");
}

void initUiCallbackRefreshSms(){
	initProgressBar(L"更新短信记录...");
}

bool uiCallbackRefreshContact(ContactData_ptr pcontact,WORD nCount,WORD nSize,WORD nSuccess){
	if(nCount+1 >= nSize){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	wchar_t infotext[256];
	wsprintf(infotext,L"更新联系人记录...已导入:%d",nSuccess);
	m_Progressdlg.SetTitle(infotext);
	wsprintf(infotext,L"%s (%d/%d)",pcontact->Name,nCount + 1,nSize);
	m_Progressdlg.SetInfo(infotext);
	m_Progressdlg.SetCurValue(nCount);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}
bool uiCallbackRefreshSms(SmsData_ptr psms,WORD nCount,WORD nSize,WORD nSuccess){
	if(nCount+1 >= nSize){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	wchar_t infotext[256];
	wsprintf(infotext,L"更新短信记录...已导入:%d",nSuccess);
	m_Progressdlg.SetTitle(infotext);
	wsprintf(infotext,L"%s %s (%d/%d)",psms->SendReceiveFlag == 1 ? L"[←]": L"[→]",psms->PNSort,nCount + 1,nSize);
	m_Progressdlg.SetInfo(infotext);
	m_Progressdlg.SetCurValue(nCount);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}
