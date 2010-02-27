#include <Mzfc/MzProgressDialog.h> 
#include "ui_ProgressBar.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "resource.h"
extern HINSTANCE LangresHandle;
#include "appconfigini.h"

extern SmsBackupConfig appconfig;
static bool brangeSet = false;
static bool bdlgshown = false;
static MzPopupProgress m_Progressdlg;

void SetProgressBarTitle(LPWSTR t){
	if(t == NULL){
		//m_Progressdlg.SetShowTitle(false);
		return;
	}
	m_Progressdlg.SetTitleText(t);
	//m_Progressdlg.SetShowTitle(true);
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
		m_Progressdlg.StartProgress(0,FALSE,FALSE,TRUE);
		bdlgshown = true;
	}
}
void HideProgressBar(){
	m_Progressdlg.KillProgress();
	bdlgshown = false;
}

void initProgressBar(LPWSTR title = NULL, WORD rmin = 0, WORD rmax = 100){
	SetProgressBarTitle(title);
	SetProgressBarRange(rmin,rmax);
	//m_Progressdlg.SetShowInfo(true);
	brangeSet = false;
	bdlgshown = false;
}

bool updateProgressBar(int nCount){
	if(nCount == 0){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	m_Progressdlg.SetCurrentValue(nCount);
	m_Progressdlg.UpdateProgress();
	return true;
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
	m_Progressdlg.SetTitleText(sImported.C_Str());
	wsprintf(infotext,L"%s (%d/%d)",pcontact->Name,nCount + 1,nSize);
	m_Progressdlg.SetNoteText(infotext);
	m_Progressdlg.SetCurrentValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	//DateTime::waitms(0);
	return true;
}

bool uiCallbackRefreshSIMContact(ContactData_ptr pcontact,WORD nCount,WORD nSize,WORD nSuccess){
	if(pcontact == NULL){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	CMzString sImported = LOADSTRING(IDS_STR_UPDATE_SIM_CONTACT);
	wchar_t infotext[256];
	wsprintf(infotext,LOADSTRING(IDS_STR_IMPORTED).C_Str(),nSuccess);
	sImported = sImported + infotext;
	m_Progressdlg.SetTitleText(sImported.C_Str());
	wsprintf(infotext,L"%s (%d/%d)",pcontact->Name,nCount + 1,nSize);
	m_Progressdlg.SetNoteText(infotext);
	m_Progressdlg.SetCurrentValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	//DateTime::waitms(0);
	return true;
}

bool uiCallbackRefreshSms(SmsData_ptr psms,WORD nCount,WORD nSize,WORD nSuccess){
	if(psms == NULL){
		HideProgressBar();
		return false;
	}
    if(appconfig.IniUpdateMethod.Get() && nSize > 20){
        if(nCount%7){
            return true;
        }
    }
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	CMzString sImported = LOADSTRING(IDS_STR_UPDATE_SMS);
	wchar_t infotext[256];
	wsprintf(infotext,LOADSTRING(IDS_STR_IMPORTED).C_Str(),nSuccess);
	sImported = sImported + infotext;
	m_Progressdlg.SetTitleText(sImported.C_Str());
	wchar_t* pnumber = 0;
	C::newstrcpy(&pnumber,psms->MobileNumber,30);
	wsprintf(infotext,L"%s %s (%d/%d)",psms->SendReceiveFlag == 1 ? L"[←]": L"[→]",pnumber,nCount + 1,nSize);
	m_Progressdlg.SetNoteText(infotext);
	m_Progressdlg.SetCurrentValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	//DateTime::waitms(0);
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
	m_Progressdlg.SetTitleText(sImported.C_Str());
	wsprintf(infotext,L"%s %s (%d/%d)",psms->SendReceiveFlag == 1 ? L"[←]": L"[→]",psms->MobileNumber,nCount + 1,nSize);
	m_Progressdlg.SetNoteText(infotext);
	m_Progressdlg.SetCurrentValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	//DateTime::waitms(0);
	return true;
}

void initUiCallbackUpdateDatabase(){
	initProgressBar(L"数据库升级中...");
}
void uiCallBackUpdateDatabase(LPWSTR title = NULL,LPWSTR msg = NULL, UINT progress = 0){
	if(progress == 0){
		HideProgressBar();
		return;
	}
	ShowProgressBar();
	SetProgressBarRange(0,100);
	if(title){
		m_Progressdlg.SetTitleText(title);
	}
	if(msg){
		m_Progressdlg.SetNoteText(msg);
	}
	m_Progressdlg.SetCurrentValue(progress);
	m_Progressdlg.UpdateProgress();
	return;
}