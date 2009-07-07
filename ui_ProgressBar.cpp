#include <Mzfc/MzProgressDialog.h> 
#include "ui_ProgressBar.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "resource.h"
extern HINSTANCE LangresHandle;

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
	initProgressBar(LOADSTRING(IDS_STR_UPDATE_CONTACT).C_Str());//L"更新联系人记录...");
}

void initUiCallbackRefreshSms(){
	initProgressBar(LOADSTRING(IDS_STR_UPDATE_SMS).C_Str());//L"更新短信记录...");
}

void initUiCallbackDeleteSms(){
	initProgressBar(LOADSTRING(IDS_STR_DELETE_SMS).C_Str());//L"更新短信记录...");
}

bool uiCallbackRefreshContact(ContactData_ptr pcontact,WORD nCount,WORD nSize,WORD nSuccess){
	if(pcontact == NULL){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	CMzString sImported = LOADSTRING(IDS_STR_UPDATE_CONTACT);
	wchar_t infotext[256];
	wsprintf(infotext,LOADSTRING(IDS_STR_IMPORTED).C_Str(),nSuccess);
	sImported = sImported + infotext;
	m_Progressdlg.SetTitle(sImported.C_Str());
	wsprintf(infotext,L"%s (%d/%d)",pcontact->Name,nCount + 1,nSize);
	m_Progressdlg.SetInfo(infotext);
	m_Progressdlg.SetCurValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}
bool uiCallbackRefreshSms(SmsData_ptr psms,WORD nCount,WORD nSize,WORD nSuccess){
	if(psms == NULL){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	CMzString sImported = LOADSTRING(IDS_STR_UPDATE_SMS);
	wchar_t infotext[256];
	wsprintf(infotext,LOADSTRING(IDS_STR_IMPORTED).C_Str(),nSuccess);
	sImported = sImported + infotext;
	m_Progressdlg.SetTitle(sImported.C_Str());
	wsprintf(infotext,L"%s %s (%d/%d)",psms->SendReceiveFlag == 1 ? L"[←]": L"[→]",psms->PNSort,nCount + 1,nSize);
	m_Progressdlg.SetInfo(infotext);
	m_Progressdlg.SetCurValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}

bool uiCallbackDeleteSms(SmsSimpleData_ptr psms,WORD nCount,WORD nSize,WORD nSuccess){
	if(psms == NULL){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	CMzString sImported = LOADSTRING(IDS_STR_DELETE_SMS);
	wchar_t infotext[256];
	wsprintf(infotext,L"%d",nSuccess);
	sImported = sImported + infotext;
	m_Progressdlg.SetTitle(sImported.C_Str());
	wsprintf(infotext,L"%s %s (%d/%d)",psms->SendReceiveFlag == 1 ? L"[←]": L"[→]",psms->MobileNumber,nCount + 1,nSize);
	m_Progressdlg.SetInfo(infotext);
	m_Progressdlg.SetCurValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}