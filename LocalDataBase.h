#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <windows.h>

#include <sqlite3x.hpp>
using namespace sqlite3x;

#include <vector>
using namespace std;

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8SmsBackup\\sms.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8SmsBackup\\sms.db"
#endif

#define TABLE_CONTACT	L"contacts_v1"
#define TABLE_SMS	L"sms_v2"


#define TABLE_SMS_OLD	L"sms_v1"


#ifdef _DEBUG
#define db_out(s) printf("%s:%d: %s\n",__FUNCTION__, __LINE__, s)
#else
#define db_out(s) db_logout(s)
#endif

#define TRY try
#define CATCH catch

/* ������������smsʱ
//������ʱ�������ݿ�
create temp table if not exists exec (name text,telnumber text, content text,timestamps datetime,sendreceive numeric)
//������ʱ�������ݿ⣬������������룬����ϵ���д��ڸú���ʱ
insert into exec (name,telnumber,content,timestamps,sendreceive) 
	select contacts_v1.Name as name, sms_v1.PN, sms_v1.content as content ,sms_v1.timestamps as timestamps,sms_v1.sendreceive as sendreceive 
		from contacts_v1,sms_v1 where (contacts_v1.PhoneNumber =  sms_v1.PN)
//������ʱ�������ݿ⣬�Ժ�����Ϊ����������ϵ���в����ڸú���ʱ
insert into exec (name,telnumber,content,timestamps,sendreceive)
    select PN as name,PN, content,timestamps,sendreceive 
		from sms_v1 where (select count(*) from contacts_v1 where contacts_v1.PhoneNumber ==  sms_v1.PN)=0
//sms��ϵ���б�
select distinct name from exec
*/

/*�����б����ɷ���
//ref:http://www.sqlite.org/lang_datefunc.html
����
select distinct date(timestamps) from exec
//��
select distinct strftime('%Y',timestamps) from exec
//��
select distinct strftime('%m',timestamps) from exec where strftime('%Y',timestamps)='2009'
//��
select distinct strftime('%d',timestamps) from exec where strftime('%Y%m',timestamps)='200912'
*/

typedef vector<LPWSTR> TelNumbers_t,*TelNumbers_ptr;
typedef enum TelLabelType{
	MOBILETEL	=	0,
	WORKTEL		=	1,
	HOMETEL		=	2,
	HOMETEL2	=	3,
}TelLabel_t;

typedef struct ContactData{
	LPWSTR	Name;
	LPWSTR	FirstName;
	LPWSTR	LastName;
	TelNumbers_t	MobileTels;
    TelNumbers_t    WorkTels;
    TelNumbers_t    HomeTels;
    TelNumbers_t    HomeTel2s;
    void Reset(){    //��գ��Ա��´�ʹ��
		if(Name){
			delete [] Name;
			Name = NULL;
		}
		if(FirstName){
			delete [] FirstName;
			FirstName = NULL;
		}
		if(LastName){
			delete [] LastName;
			LastName = NULL;
		}

		for(int i = 0; i < MobileTels.size(); i ++){
			LPWSTR strNum = MobileTels.at(i);
			if(strNum) delete [] strNum;
		}
		MobileTels.clear();

		for(int i = 0; i < WorkTels.size(); i ++){
			LPWSTR strNum = WorkTels.at(i);
			if(strNum) delete [] strNum;
		}
		WorkTels.clear();

		for(int i = 0; i < HomeTels.size(); i ++){
			LPWSTR strNum = HomeTels.at(i);
			if(strNum) delete [] strNum;
		}
		HomeTels.clear();

		for(int i = 0; i < HomeTel2s.size(); i ++){
			LPWSTR strNum = HomeTel2s.at(i);
			if(strNum) delete [] strNum;
		}
		HomeTel2s.clear();
    }
	ContactData(){
        Name = NULL;
		FirstName = NULL;
		LastName = NULL;
	}
    ~ContactData(){
        Reset();
    }
}ContactData_t, *ContactData_ptr;

#define SAFE_RELEASE_LPWSTR(s) { \
    if((s)) {\
        delete [] (s); \
        (s) = NULL; \
        }\
    }

typedef enum SmsSendReceive{
	SMS_RECEIVE	=	0,
	SMS_SEND	=	1,
}SmsSendReceive_t;

typedef struct SmsData{
	LPWSTR MobileNumber;
	ULONG	SmsStatus;
	BOOL	SendReceiveFlag;
	ULONG	SmsType;
	SYSTEMTIME	TimeStamp;
	LPWSTR	Content;
	SYSTEMTIME	NetTimeStamp;
	LPWSTR	ExtraInfo;
	LPWSTR	PNSort;
	ULONG	AutoIndex;
    void Reset(){
        SAFE_RELEASE_LPWSTR(MobileNumber);
        SAFE_RELEASE_LPWSTR(Content);
        SAFE_RELEASE_LPWSTR(ExtraInfo);
        SAFE_RELEASE_LPWSTR(PNSort);
    }
	SmsData(){
		MobileNumber = NULL;
		SmsStatus = 0;
		SendReceiveFlag = false;
		SmsType = 0;
		Content = NULL;
		ExtraInfo = NULL;
		PNSort = NULL;
		AutoIndex = 0;
	}
    ~SmsData(){
        Reset();
    }
}SmsData_t, *SmsData_ptr;

//used for sms display list
typedef struct SmsSimpleData{
    LPWSTR ContactName;
	LPWSTR MobileNumber;
	BOOL	SendReceiveFlag;
	LPWSTR	TimeStamp;
	LPWSTR	Content;
	BOOL	SelectionFlag;
    void Reset(){
        SAFE_RELEASE_LPWSTR(ContactName);
        SAFE_RELEASE_LPWSTR(MobileNumber);
        SAFE_RELEASE_LPWSTR(Content);
        SAFE_RELEASE_LPWSTR(TimeStamp);
		SelectionFlag = false;
		SendReceiveFlag = false;
    }
	SmsSimpleData(){
        ContactName = NULL;
		MobileNumber = NULL;
		SendReceiveFlag = false;
		Content = NULL;
        TimeStamp = NULL;
		SelectionFlag = false;
	}
    ~SmsSimpleData(){
        Reset();
    }
}SmsSimpleData_t, *SmsSimpleData_ptr;

typedef struct SmsViewListKey{
	LPWSTR key;			//�б���Ŀ����
	UINT	nReceive;	//��������
	UINT	nSend;		//��������
	SmsViewListKey(){
		key = NULL;
		nReceive = 0;
		nSend = 0;
	}
	~SmsViewListKey(){
		Reset();
	}
    void Reset(){
        SAFE_RELEASE_LPWSTR(key);
		nReceive = 0;
		nSend = 0;
    }
}SmsViewListKey_t,*SmsViewListKey_ptr;

typedef void (*CallBackDatabaseUpdate)(LPWSTR title,LPWSTR msg, UINT progress);

class LocalDataBase {
public:
    LocalDataBase();
    ~LocalDataBase();
public:
	bool connect();
	bool disconnect();
	bool decrypt(wchar_t* pwd,int len);
	bool encrypt(wchar_t* pwd,int len);

	//check if pwd is correct, db will disconnected automatically after this operation
	bool checkpwd(wchar_t* pwd,int len);	
public:
//sms��ز���
	bool AppendSmsRecord(SmsData_ptr);
	bool RemoveSmsRecord(SmsData_ptr);
	bool RemoveSmsRecord(SmsSimpleData_ptr);
public:// get title list
    bool GetMainList();
	bool GetSmsContactList();
	bool GetSmsYearList();
	bool GetSmsMonthList(WORD year);
	bool GetSmsDayList(WORD year, WORD month);
    int query_list_size() { return keys.size(); }
    SmsViewListKey_ptr query_list_item(int idx) {
        if(idx >= keys.size()) return 0;
        return keys.at(idx);
    }
protected:
    void ClearSmsKeyList();
    bool GetSmsKeyList(sqlite3_command &cmd);
private:
    vector<SmsViewListKey_ptr> keys;
public:
    //��ȡ��������
    bool GetSmsCount(UINT &received, UINT &sent);
    //��ȡ������Ŀ
    //�������ڻ�ȡ������Ŀ��0����ʾ����
    UINT GetSmsByDate(WORD year, WORD month, WORD day,SmsSimpleData_ptr = NULL);
    //������ϵ�˻�ȡ������Ŀ
    UINT GetSmsByContact(LPWSTR,SmsSimpleData_ptr = NULL);
    //���ݶ�����������������Ŀ
    UINT GetSmsByContent(LPWSTR,SmsSimpleData_ptr = NULL);
    //��ȡ����ʱ��
    SYSTEMTIME GetSmsLatestDateTime();
private:
    //�����¼ǰ����Ƿ����ظ�sms
    bool isDuplicateSms(SmsData_ptr);
public:
	void query_sms_clear();
	bool query_sms();
	int query_sms_size() { return query_sms_list.size(); }
	SmsSimpleData_ptr query_sms_at(int i) { return query_sms_list.at(i); }
private:
	vector<SmsSimpleData_ptr> query_sms_list;
public:
	//contact��ز���
	UINT AppendContactRecord(ContactData_ptr);
	bool RemoveContactRecord(ContactData_ptr);
    void ClearContactTable();

	void query_clear();
	bool query_contacts();
	int query_contact_size() { return query_contact_list.size(); }
	ContactData_ptr query_contact_at(int i) { return query_contact_list.at(i); }
private:
	vector<ContactData_ptr> query_contact_list;
private:
    bool addContactRecord(LPWSTR number,LPWSTR name,TelLabel_t label);
	bool isDuplicateContact(LPWSTR number,LPWSTR name,TelLabel_t label);
	bool updateContact(LPWSTR number,LPWSTR name,TelLabel_t label);
	bool getContactName(LPWSTR phonenumber,LPWSTR *ppname);	//ʹ�ú���Ҫ���ٷ���ֵ
private:
	sqlite3_connection sqlconn;
    wchar_t db_path[MAX_PATH];
protected:
    bool connectDatabase(const wchar_t*);
    void createDefaultDatabase();
public:
	bool beginTrans();
	bool commitTrans();
	bool reorgDatebase();
	bool indexDatabase();
//���ݿ�����
private:
	bool TableExists(wchar_t* tablename);	//�����Ƿ����
	void MigrateSmsData();	//Ǩ�ƶ������ݱ�
	void DropOldSmsTable(); //ɾ���ϵĶ������ݱ�
public:
	void updateV2(CallBackDatabaseUpdate callback = NULL);	//�������ݿ⵽v2
};

