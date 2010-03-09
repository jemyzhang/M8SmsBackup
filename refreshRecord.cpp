#include "refreshRecord.h"
#include "LocalDataBase.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "appconfigini.h"

#include "logout.h"

extern SmsBackupConfig appconfig;
extern LocalDataBase *g_pldb;

/*
	去除联系人电话号码前的国家代码+86
*/
void getridCountryCode(ContactData_ptr pcontact){
    if(pcontact == NULL) return;

    int i;
    int size = pcontact->MobileTels.size();
    if(size > 0){
        for( i = 0; i < size; i++){
            C::removeSpecStr(pcontact->MobileTels.at(i),L"+86");
        }
    }

    size = pcontact->WorkTels.size();
    if(size > 0){
        for( i = 0; i < size; i++){
			C::removeSpecStr(pcontact->WorkTels.at(i),L"+86");            
        }
    }

    size = pcontact->HomeTels.size();
    if(size > 0){
        for(i = 0; i < size; i++){
			C::removeSpecStr(pcontact->HomeTels.at(i),L"+86");       
        }
    }

    size = pcontact->HomeTel2s.size();
    if(size > 0){
        for(i = 0; i < size; i++){
			C::removeSpecStr(pcontact->HomeTel2s.at(i),L"+86");            
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
	BOOL bContactDatabaseV2 = MzSystem::requireVersion(0,9,6,0);

    ::logout(L"Begin Refresh Contacts[%s]....",
        bContactDatabaseV2 ? L"v2" : L"v1");

	MzContactDataBase *pdb_v1 = NULL;
	MzContactDataBaseV2 *pdb_v2 = NULL;
    File::DirectoryExists_New(L"\\Disk\\Temp\\");   //检查文件夹是否存在
	if(bContactDatabaseV2){
		l.push_back(L"contacts.db");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\Temp\\",l);
		pdb_v2 = new MzContactDataBaseV2(L"\\Disk\\Temp\\contacts.db");
	}else{
		pdb_v1 = new MzContactDataBase;
	}

	ULONG count = bContactDatabaseV2 ? 
		pdb_v2->GetContactCount() : pdb_v1->GetContactCount();

    ::logout(L"Contact size = %d",
        count);
	
    g_pldb->beginTrans();
    ContactData_t contact;

    for(WORD i = 0; i < count; i++){
 		if(bContactDatabaseV2){
			pdb_v2->GetContact(i,contact);
		}else{
			pdb_v1->GetContact(i,contact);
		}
        ::logout(L"Got Contact#%d",i+1);
		getridCountryCode(&contact);
        ::logout(L"Appending Contact...");
        nRet += g_pldb->AppendContactRecord(&contact);
        ::logout(L"Append contact finished.");
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
	BOOL bSmsDatabaseV2 = MzSystem::requireVersion(0,9,6,0);

	MzSMSDataBase *pdb_v1 = NULL;
	MzSMSDataBaseV2 *pdb_v2 = NULL;

    File::DirectoryExists_New(L"\\Disk\\Temp\\");   //检查文件夹是否存在
	if(bSmsDatabaseV2){
		l.push_back(L"sms.db");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\Temp\\",l);
		pdb_v2 = new MzSMSDataBaseV2(L"\\Disk\\Temp\\sms.db");
	}else{
		l.push_back(L"sms.edb");
		File::BackupFiles(L"Documents and Settings\\",L"\\Disk\\Temp\\",l);
		pdb_v1 = new MzSMSDataBase(L"\\Disk\\Temp\\sms.edb");
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