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
            if(!(*callback)(&contact,i,count)){
                break;
            }
        }
        contact.Reset();
    }
    ldb.commitTrans();
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
        nRet += ldb.AppendSmsRecord(&sms) ? 1:0;
        if(callback){
            if(!(*callback)(&sms,i,count)){
                break;
            }
        }
        sms.Reset();
    }
    ldb.commitTrans();
    return nRet;
}