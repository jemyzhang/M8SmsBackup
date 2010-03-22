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

/* 按照姓名检索sms时
//创建临时关联数据库
create temp table if not exists exec (name text,telnumber text, content text,timestamps datetime,sendreceive numeric)
//插入临时关联数据库，以姓名替代号码，当联系人中存在该号码时
insert into exec (name,telnumber,content,timestamps,sendreceive) 
	select contacts_v1.Name as name, sms_v1.PN, sms_v1.content as content ,sms_v1.timestamps as timestamps,sms_v1.sendreceive as sendreceive 
		from contacts_v1,sms_v1 where (contacts_v1.PhoneNumber =  sms_v1.PN)
//插入临时关联数据库，以号码作为姓名，当联系人中不存在该号码时
insert into exec (name,telnumber,content,timestamps,sendreceive)
    select PN as name,PN, content,timestamps,sendreceive 
		from sms_v1 where (select count(*) from contacts_v1 where contacts_v1.PhoneNumber ==  sms_v1.PN)=0
//sms联系人列表
select distinct name from exec
*/

/*日期列表生成方法
//ref:http://www.sqlite.org/lang_datefunc.html
日期
select distinct date(timestamps) from exec
//年
select distinct strftime('%Y',timestamps) from exec
//月
select distinct strftime('%m',timestamps) from exec where strftime('%Y',timestamps)='2009'
//日
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
    void Reset(){    //清空，以便下次使用
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
	LPWSTR key;			//列表醒目名称
	UINT	nReceive;	//发送条数
	UINT	nSend;		//接收条数
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
//sms相关操作
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
    //获取短信数量
    bool GetSmsCount(UINT &received, UINT &sent);
    //获取短信条目
    //根据日期获取短信条目，0：表示任意
    UINT GetSmsByDate(WORD year, WORD month, WORD day,SmsSimpleData_ptr = NULL);
    //根据联系人获取短信条目
    UINT GetSmsByContact(LPWSTR,SmsSimpleData_ptr = NULL);
    //根据短信内容搜索短信条目
    UINT GetSmsByContent(LPWSTR,SmsSimpleData_ptr = NULL);
    //获取短信时间
    SYSTEMTIME GetSmsLatestDateTime();
private:
    //插入记录前检查是否有重复sms
    bool isDuplicateSms(SmsData_ptr);
public:
	void query_sms_clear();
	bool query_sms();
	int query_sms_size() { return query_sms_list.size(); }
	SmsSimpleData_ptr query_sms_at(int i) { return query_sms_list.at(i); }
private:
	vector<SmsSimpleData_ptr> query_sms_list;
public:
	//contact相关操作
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
	bool getContactName(LPWSTR phonenumber,LPWSTR *ppname);	//使用后，需要销毁返回值
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
//数据库升级
private:
	bool TableExists(wchar_t* tablename);	//检测表是否存在
	void MigrateSmsData();	//迁移短信数据表
	void DropOldSmsTable(); //删除老的短信数据表
public:
	void updateV2(CallBackDatabaseUpdate callback = NULL);	//升级数据库到v2
};

