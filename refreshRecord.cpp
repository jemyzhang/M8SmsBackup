#include "refreshRecord.h"
#include "LocalDataBase.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "appconfigini.h"

extern AppConfigIni appconfig;
extern wchar_t g_password[256];
extern int g_password_len;

WORD refreshSIMContact(CallBackRefreshSIMContact callback){
    WORD nRet = 0;
	MzSimContactDataBase dbSimContact;
	WORD simcount = dbSimContact.GetContactCount();
    LocalDataBase ldb;
	if(!ldb.checkpwd(g_password,g_password_len)){
		return 0;
	}
    ldb.beginTrans();
    ContactData_t contact;
    for(WORD i = 0; i < simcount; i++){
		if(dbSimContact.GetContact(i,contact)){
			nRet += ldb.AppendContactRecord(&contact);
		}else{
			LPWSTR Number = NULL;
			C::newstrcpy(&Number,L"0");
			contact.MobileTels.push_back(Number);
			C::newstrcpy(&contact.Name,L"SIM");
		}
		if(callback){
			if(!(*callback)(&contact,i,simcount,nRet)){
				break;
			}
		}
        contact.Reset();
    }
    ldb.commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
    return nRet;
}

WORD refreshContact(CallBackRefreshContact callback){
    WORD nRet = 0;
    MzContactDataBase dbcontact;
    WORD count = dbcontact.GetContactCount();
    LocalDataBase ldb;
	if(!ldb.checkpwd(g_password,g_password_len)){
		return 0;
	}
    ldb.beginTrans();
    ContactData_t contact;
    for(WORD i = 0; i < count; i++){
        dbcontact.GetContact(i,contact);
        nRet += ldb.AppendContactRecord(&contact);
        if(callback){
            if(!(*callback)(&contact,i,count,nRet)){
                break;
            }
        }
        contact.Reset();
    }
    ldb.commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
    return nRet;
}

WORD refreshSms(CallBackRefreshSms callback){
    WORD nRet = 0;
	list<CMzString> l;
	l.push_back(L"sms.edb");
	File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\",l);
	MzSMSDataBase db(L"\\Disk\\sms.edb");
	ULONG count = db.GetSmsCount();

    LocalDataBase ldb;
	if(!ldb.checkpwd(g_password,g_password_len)){
		return 0;
	}
    ldb.beginTrans();
	SmsData_t sms;
    bool bfast = false;
    SYSTEMTIME edt;
    if(appconfig.IniUpdateMethod.Get()){
        bfast = true;
        edt = ldb.GetSmsLatestDateTime();
    }
    for(int i = 0; i < count; i++){
        bool bomit = false; //是否跳过
	    db.GetSms(i,sms);
        if(bfast){
            if(DateTime::compareDateTime(sms.TimeStamp,edt) <= 0 ){
                bomit = true;
            }
        }
        if(!bomit){
		    if(sms.SmsType == 1){
			    continue;	//MMS
		    }else if(sms.SmsType == 12){	//group send
			    wchar_t* token = C::_wcstok(sms.MobileNumber,L"┇");
                SmsData_t groupsms;   //对应群发
			    while(token){
				    groupsms.Content = sms.Content;
				    groupsms.NetTimeStamp = sms.NetTimeStamp;
				    groupsms.TimeStamp = sms.TimeStamp;
				    groupsms.SmsStatus = sms.SmsStatus;
				    groupsms.SmsType = sms.SmsType;
				    groupsms.ExtraInfo = sms.ExtraInfo;
				    groupsms.AutoIndex = sms.AutoIndex;
				    groupsms.SendReceiveFlag = sms.SendReceiveFlag;
				    groupsms.PNSort = token;
				    groupsms.MobileNumber = token;
				    nRet += ldb.AppendSmsRecord(&groupsms) ? 1:0;
				    token = C::_wcstok(NULL,L"┇");
				    groupsms.Reset();
			    }
		    }else{
			    nRet += ldb.AppendSmsRecord(&sms) ? 1:0;
		    }
        }
        if(callback){
            if(!(*callback)(&sms,i,count,nRet)){
                break;
            }
        }
        sms.Reset();
    }
    ldb.commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
    return nRet;
}