#include "refreshRecord.h"
#include "LocalDataBase.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "appconfigini.h"

extern SmsBackupConfig appconfig;
extern LocalDataBase *g_pldb;

/*
	去除联系人电话号码前的国家代码+86
*/
void getridCountryCode(ContactData_ptr pcontact){
    if(pcontact == NULL) return;

    TelNumbers_t::iterator i;
    if(pcontact->MobileTels.size()){
        for( i = pcontact->MobileTels.begin(); i != pcontact->MobileTels.end(); i++){
			C::removeSpecStr(*i,L"+86");
        }
    }
    if(pcontact->WorkTels.size()){
        for( i = pcontact->WorkTels.begin(); i != pcontact->WorkTels.end(); i++){
			C::removeSpecStr(*i,L"+86");            
        }
    }
    if(pcontact->HomeTels.size()){
        for( i = pcontact->HomeTels.begin(); i != pcontact->HomeTels.end(); i++){
			C::removeSpecStr(*i,L"+86");       
        }
    }
    if(pcontact->HomeTel2s.size()){
        for( i = pcontact->HomeTel2s.begin(); i != pcontact->HomeTel2s.end(); i++){
			C::removeSpecStr(*i,L"+86");            
        }
    }
    return;
}

WORD refreshSIMContact(CallBackRefreshSIMContact callback){
    WORD nRet = 0;
	MzSimContactDataBase dbSimContact;
	WORD simcount = dbSimContact.GetContactCount();
    g_pldb->beginTrans();
    ContactData_t contact;
    for(WORD i = 0; i < simcount; i++){
		if(dbSimContact.GetContact(i,contact)){
			getridCountryCode(&contact);
			nRet += g_pldb->AppendContactRecord(&contact);
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
    g_pldb->commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
    return nRet;
}

WORD refreshContact(CallBackRefreshContact callback){
    WORD nRet = 0;
	list<CMzString> l;
	BOOL bContactDatabaseV2 = MzSystem::requireVersion(0,9,5,0);

	MzContactDataBase *pdb_v1 = NULL;
	MzContactDataBaseV2 *pdb_v2 = NULL;
	if(bContactDatabaseV2){
		l.push_back(L"contacts.db");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\",l);
		pdb_v2 = new MzContactDataBaseV2(L"\\Disk\\contacts.db");
	}else{
		pdb_v1 = new MzContactDataBase;
	}

	ULONG count = bContactDatabaseV2 ? 
		pdb_v2->GetContactCount() : pdb_v1->GetContactCount();
	
    g_pldb->beginTrans();
    ContactData_t contact;
    for(WORD i = 0; i < count; i++){
 		if(bContactDatabaseV2){
			pdb_v2->GetContact(i,contact);
		}else{
			pdb_v1->GetContact(i,contact);
		}
		getridCountryCode(&contact);
        nRet += g_pldb->AppendContactRecord(&contact);
        if(callback){
            if(!(*callback)(&contact,i,count,nRet)){
                break;
            }
        }
        contact.Reset();
    }
    g_pldb->commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
	if(bContactDatabaseV2){
		delete pdb_v2;
	}else{
		delete pdb_v1;
	}
    return nRet;
}

WORD refreshSms(CallBackRefreshSms callback){
    WORD nRet = 0;
	list<CMzString> l;
	BOOL bSmsDatabaseV2 = MzSystem::requireVersion(0,9,5,0);

	MzSMSDataBase *pdb_v1 = NULL;
	MzSMSDataBaseV2 *pdb_v2 = NULL;
	if(bSmsDatabaseV2){
		l.push_back(L"sms.db");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\",l);
		pdb_v2 = new MzSMSDataBaseV2(L"\\Disk\\sms.db");
	}else{
		l.push_back(L"sms.edb");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\",l);
		pdb_v1 = new MzSMSDataBase(L"\\Disk\\sms.edb");
	}

	ULONG count = bSmsDatabaseV2 ? 
		pdb_v2->GetSmsCount() : pdb_v1->GetSmsCount();

	SmsData_t sms;
    bool bfast = false;
    SYSTEMTIME edt;
    if(appconfig.IniUpdateMethod.Get()){
        bfast = true;
        edt = g_pldb->GetSmsLatestDateTime();
    }
    g_pldb->beginTrans();
    for(int i = 0; i < count; i++){
        bool bomit = false; //是否跳过
		if(bSmsDatabaseV2){
			pdb_v2->GetSms(i,sms);
		}else{
			pdb_v1->GetSms(i,sms);
		}
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
				    nRet += g_pldb->AppendSmsRecord(&groupsms) ? 1:0;
				    token = C::_wcstok(NULL,L"┇");
				    groupsms.Reset();
			    }
		    }else{
			    nRet += g_pldb->AppendSmsRecord(&sms) ? 1:0;
		    }
        }
        if(callback){
            if(!(*callback)(&sms,i,count,nRet)){
                break;
            }
        }
        sms.Reset();
    }
    g_pldb->commitTrans();
    if(callback){
        (*callback)(NULL,0,0,0);	//indicate end
    }
	if(bSmsDatabaseV2){
		delete pdb_v2;
	}else{
		delete pdb_v1;
	}
    return nRet;
}