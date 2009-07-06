#include "refreshRecord.h"
#include "LocalDataBase.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

WORD refreshContact(CallBackRefreshContact callback){
    WORD nRet = 0;
    MzContactDataBase dbcontact;
    WORD count = dbcontact.GetContactCount();
    LocalDataBase ldb;
    ldb.connect();
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
    ldb.connect();
    ldb.beginTrans();
	SmsData_t sms;
    for(int i = 0; i < count; i++){
	    db.GetSms(i,sms);
		if(sms.SmsType == 1){
			continue;	//MMS
		}else if(sms.SmsType == 12){	//group send
			wchar_t* token = C::_wcstok(sms.MobileNumber,L"©«");
			while(token){
				SmsData_t newsms;
				newsms.Content = sms.Content;
				newsms.NetTimeStamp = sms.NetTimeStamp;
				newsms.TimeStamp = sms.TimeStamp;
				newsms.SmsStatus = sms.SmsStatus;
				newsms.SmsType = sms.SmsType;
				newsms.ExtraInfo = sms.ExtraInfo;
				newsms.AutoIndex = sms.AutoIndex;
				newsms.SendReceiveFlag = sms.SendReceiveFlag;
				newsms.PNSort = token;
				newsms.MobileNumber = token;
				nRet += ldb.AppendSmsRecord(&newsms) ? 1:0;
				token = C::_wcstok(NULL,L"©«");
				newsms.Reset();
			}
		}else{
			nRet += ldb.AppendSmsRecord(&sms) ? 1:0;
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