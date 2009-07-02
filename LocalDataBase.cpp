#include "LocalDataBase.h"
#include "pinyin_sort.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

LocalDataBase::LocalDataBase() {
	wchar_t currpath[MAX_PATH];
	bool ret = true;
	if(File::GetCurrentPath(currpath)){
		wsprintf(db_path,L"%s\\sms.db",currpath);
	}else{
		wsprintf(db_path,DEFAULT_DB);
	}
}

LocalDataBase::~LocalDataBase() {
    disconnectDatabase();
}

bool LocalDataBase::connect(){
	bool rc = false;
	if(File::FileExists(db_path)){	//file exists
		rc = true;
	}
	connectDatabase(db_path);
	createDefaultDatabase();
	return rc;
}

bool LocalDataBase::decrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
	bool ret = decrytpDatabase(temp,bytecnt);
	delete temp;
	return ret;
}

bool LocalDataBase::encrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
	bool ret =  setDatabasePassword(temp,bytecnt);
	delete temp;
	return ret;
}

//sqlite database operation
unsigned short zName[] = {'p', 'i', 'n', 'y', 'i', 'n', 0};

void LocalDataBase::connectDatabase(const wchar_t * dbfile) {
    const wchar_t* f = dbfile;
    if (f == NULL) {
        f = DEFAULT_DB;
    }
    int rc = sqlite3_open16(f, &db);
	//创建拼音搜索库
	sqlite3_create_collation16(db, (const char*)zName, SQLITE_UTF16, 0, pinyin_cmp);

    return;
}

void LocalDataBase::disconnectDatabase() {
    sqlite3_close(db);
    return;
}

void LocalDataBase::createDefaultDatabase() {
    int rc = 0;
    //what ever create db
    //UNIQUE solved duplication problem
    //create account table
    wsprintf(sqlcmdw, CREATE_CONTACT_TBL, TABLE_CONTACT);

    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw, CREATE_SMS_TBL, TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);
    return;
}

bool LocalDataBase::decrytpDatabase(const char* pwd,int len){
	return (sqlite3_key(db,pwd,len) == SQLITE_OK);
}

bool LocalDataBase::setDatabasePassword(const char* pwd,int len){
	return (sqlite3_rekey(db,pwd,len) == SQLITE_OK);
}

//////////////////////////////////////
//sms操作
bool LocalDataBase::AppendSmsRecord(SmsData_ptr psms){
    bool nRet = false;
    if(psms == NULL) return nRet;
    if(isDuplicateSms(psms)) return nRet;

    SYSTEMTIME smstm = psms->TimeStamp;
    wsprintf(sqlcmdw,INSERT_SMS,TABLE_SMS,
        psms->PNSort,psms->MobileNumber,psms->Content,
        smstm.wYear,smstm.wMonth,smstm.wDay,smstm.wHour,smstm.wMinute,smstm.wSecond,
        psms->SendReceiveFlag);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		nRet = (sqlite3_step(pStmt) == SQLITE_DONE);
    }
	sqlite3_finalize(pStmt);
    return nRet;
}


bool LocalDataBase::isDuplicateSms(SmsData_ptr psms){
    bool nRet = false;
    if(psms == NULL) return nRet;
    SYSTEMTIME smstm = psms->TimeStamp;
    wsprintf(sqlcmdw,COUNT_SELECT_SMS,TABLE_SMS,
        psms->MobileNumber,psms->Content,
        smstm.wYear,smstm.wMonth,smstm.wDay,smstm.wHour,smstm.wMinute,smstm.wSecond,
        psms->SendReceiveFlag);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nRet = (sqlite3_column_int(pStmt, 0) != 0);
        }
    }
	sqlite3_finalize(pStmt);
    return nRet;
}
//////////////////////////////////////
//contact操作
bool LocalDataBase::isDuplicateContact(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool nRet = false;
    if(number == NULL || name == NULL) return nRet;

	wsprintf(sqlcmdw,COUNT_SELECT_CONTACT,TABLE_CONTACT,
        number,name,label);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nRet = (sqlite3_column_int(pStmt, 0) != 0);
        }
    }
	sqlite3_finalize(pStmt);
    return nRet;
}

bool LocalDataBase::updateContact(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool nRet = false;
    if(number == NULL || name == NULL) return nRet;

	wsprintf(sqlcmdw,UPDATE_SELECT_CONTACT,TABLE_CONTACT,
        name,label,number);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        nRet = (sqlite3_step(pStmt) == SQLITE_DONE);
    }
	sqlite3_finalize(pStmt);
    return nRet;
}

bool LocalDataBase::addContactRecord(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool nRet = false;
    if(number == NULL || name == NULL) return nRet;
	//检查记录是否已存在
	if(isDuplicateContact(number,name,label)) return nRet;

    wsprintf(sqlcmdw,INSERT_CONTACT,TABLE_CONTACT,
        number,name,label);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		nRet = (sqlite3_step(pStmt) == SQLITE_DONE);
    }
	sqlite3_finalize(pStmt);
	//添加记录不成功时尝试更新记录
	if(nRet == false){
		nRet = updateContact(number,name,label);
	}
    return nRet;
}

UINT LocalDataBase::AppendContactRecord(ContactData_ptr pcontact){
    UINT nRet = 0;
    if(pcontact == NULL) return nRet;

    TelNumbers_t::iterator i;
    if(pcontact->MobileTels.size()){
        for( i = pcontact->MobileTels.begin(); i != pcontact->MobileTels.end(); i++){
            if(addContactRecord(*i,pcontact->Name,MOBILETEL)) nRet++;
        }
    }
    if(pcontact->WorkTels.size()){
        for( i = pcontact->WorkTels.begin(); i != pcontact->WorkTels.end(); i++){
            if(addContactRecord(*i,pcontact->Name,WORKTEL)) nRet++;
        }
    }
    if(pcontact->HomeTels.size()){
        for( i = pcontact->HomeTels.begin(); i != pcontact->HomeTels.end(); i++){
            if(addContactRecord(*i,pcontact->Name,HOMETEL)) nRet++;
        }
    }
    if(pcontact->HomeTel2s.size()){
        for( i = pcontact->HomeTel2s.begin(); i != pcontact->HomeTel2s.end(); i++){
            if(addContactRecord(*i,pcontact->Name,HOMETEL2)) nRet++;
        }
    }
    return nRet;
}