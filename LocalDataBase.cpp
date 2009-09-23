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
	bconnected = false;
	db = NULL;
}

LocalDataBase::~LocalDataBase() {
    if(bconnected){
        disconnectDatabase();
    }
}

bool LocalDataBase::connect(){
	if(bconnected) return bconnected;

	bconnected = connectDatabase(db_path);
    return bconnected;
}

bool LocalDataBase::checkpwd(wchar_t* pwd,int len){
	bool nRet = false;
	if(!bconnected) connect();
	if(!bconnected) return nRet;

	if(pwd && len != 0){
		decrypt(pwd,len);
	}
    wsprintf(sqlcmdw,L"select count(*) from sqlite_master");
	int rc;
    if ((rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail)) == SQLITE_OK) {
        nRet = (sqlite3_step(pStmt) == SQLITE_ROW);
    }
    sqlite3_finalize(pStmt);
    if(!nRet){
	    disconnect();
	}
	return nRet;
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

bool LocalDataBase::connectDatabase(const wchar_t * dbfile) {
    const wchar_t* f = dbfile;
    if (f == NULL) {
        f = DEFAULT_DB;
    }
    bool bfileNotExists = false;
    if(!File::FileExists((TCHAR*)f)){
        bfileNotExists = true;
    }
    int rc = sqlite3_open16(f, &db);
    //创建拼音搜索库
    sqlite3_create_collation16(db, (const char*)zName, SQLITE_UTF16, 0, pinyin_cmp);
    //文件新建时创建默认表
    if(bfileNotExists) createDefaultDatabase();

    return (rc == SQLITE_OK);
}

bool LocalDataBase::disconnectDatabase() {
    bconnected = !(sqlite3_close(db) == SQLITE_OK);
    return !bconnected;
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
	LPWSTR pname = getContactName(psms->PNSort);
    wsprintf(sqlcmdw,INSERT_SMS,TABLE_SMS,
        pname, psms->PNSort,psms->MobileNumber,psms->Content,
        smstm.wYear,smstm.wMonth,smstm.wDay,smstm.wHour,smstm.wMinute,smstm.wSecond,
        psms->SendReceiveFlag, smstm.wYear,smstm.wMonth,smstm.wDay);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        nRet = (sqlite3_step(pStmt) == SQLITE_DONE);
    }
    sqlite3_finalize(pStmt);
	delete [] pname;
    return nRet;
}


bool LocalDataBase::isDuplicateSms(SmsData_ptr psms){
    bool nRet = false;
    if(psms == NULL) return nRet;
    SYSTEMTIME smstm = psms->TimeStamp;
    wsprintf(sqlcmdw,COUNT_SELECT_SMS,TABLE_SMS,
        psms->MobileNumber,
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

	//更新短信名称
    wsprintf(sqlcmdw,L"update '%s' set Name='%s' where PN='%s'",TABLE_SMS,
        name,number);
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
	}else{
		//更新短信名称
		wsprintf(sqlcmdw,L"update '%s' set Name='%s' where PN='%s'",TABLE_SMS,
			name,number);
		if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
			nRet = (sqlite3_step(pStmt) == SQLITE_DONE);
		}
		sqlite3_finalize(pStmt);
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

LPWSTR LocalDataBase::getContactName(LPWSTR phonenumber){
	LPWSTR pname = NULL;

	if(phonenumber == NULL) return pname;

	bool c = false;	//如果为连接数据库，则打开数据后执行关闭，恢复初始状态
	if(!bconnected) {
		connect();
		c = true;
	}
	if(!bconnected) return pname;

    wsprintf(sqlcmdw,L"select Name from '%s' where PhoneNumber='%s'",TABLE_CONTACT,phonenumber);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		if(sqlite3_step(pStmt) == SQLITE_ROW){
			C::newstrcpy(&pname,(LPWSTR)sqlite3_column_text16(pStmt, 0));
		}
    }
    sqlite3_finalize(pStmt);
    if(c){
	    disconnect();
    }
	if(pname == NULL) 	C::newstrcpy(&pname,phonenumber);
	return pname;
}

UINT LocalDataBase::GetSmsCount(UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;
    int rc = 0;

    wsprintf(sqlcmdw,L"select count(*) from %s",TABLE_SMS);
    if ((rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail)) == SQLITE_OK) {
        if ((rc = sqlite3_step(pStmt)) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);

    if(total != 0){
        wsprintf(sqlcmdw,L"select count(*) from %s where SendReceive=%d",
            TABLE_SMS,
            SMS_RECEIVE);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            if (sqlite3_step(pStmt) == SQLITE_ROW){
                received = sqlite3_column_int(pStmt, 0);
            }
        }
        sqlite3_finalize(pStmt);
        sent = total - received;
    }
    return total;
}

UINT LocalDataBase::GetSmsContactList(SmsViewListKey_ptr plist){
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct name) from '%s'",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct name from '%s' order by name collate pinyin",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    return nSize;
}

UINT LocalDataBase::GetSmsYearList(SmsViewListKey_ptr plist){
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct year) from '%s'",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct year from '%s'",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    return nSize;
}

UINT LocalDataBase::GetSmsMonthList(WORD year, SmsViewListKey_ptr plist){
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct month) from '%s' where year='%04d'",
        TABLE_SMS,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct month from '%s' where year='%04d'",
        TABLE_SMS,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    return nSize;
}

UINT LocalDataBase::GetSmsDayList(WORD year, WORD month,SmsViewListKey_ptr plist){
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct day) from '%s' where year='%04d' and month='%02d'",
        TABLE_SMS,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct day from '%s' where year='%04d'and month='%02d'",
        TABLE_SMS,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    return nSize;
}
////////////////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsYearCount(WORD year, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*),sum(sendreceive) from '%s' where year='%04d'",
        TABLE_SMS,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
			sent = sqlite3_column_int(pStmt, 1);
        }
    }
    sqlite3_finalize(pStmt);
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsMonthCount(WORD year, WORD month, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*),sum(sendreceive) from '%s' where year='%04d' and month='%02d'",
        TABLE_SMS,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
			sent = sqlite3_column_int(pStmt, 1);
        }
    }
    sqlite3_finalize(pStmt);
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsDayCount(WORD year, WORD month,WORD day, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*),sum(sendreceive) from '%s' where year='%04d' and month='%02d' and day='%02d'",
        TABLE_SMS,year,month,day);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
			sent = sqlite3_column_int(pStmt, 1);
        }
    }
    sqlite3_finalize(pStmt);
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsContactCount(LPWSTR name,UINT &received, UINT &sent){
    if(name == NULL) return 0;

    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*),sum(sendreceive) from '%s' where name='%s'",
        TABLE_SMS,name);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
			sent = sqlite3_column_int(pStmt, 1);
        }
    }
    sqlite3_finalize(pStmt);
    received = total - sent;
    return total;
}
//////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsByDate(WORD year, WORD month, WORD day,SmsSimpleData_ptr plist){
    if(year == 0) return 0; //年份错误

	CMzString strsql = L"select %s from '%s' where year='%04d'";
    if(month != 0) strsql = strsql + L" and month='%02d'";
    if(day != 0) strsql = strsql + L" and day='%02d'";

    UINT total = 0;
    wsprintf(sqlcmdw,strsql.C_Str(),L"count(*)",TABLE_SMS,year,month,day);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total == 0 || plist == NULL) return total;

    strsql = strsql + L" order by timestamps DESC";
    SmsSimpleData_ptr pi = plist;
    wsprintf(sqlcmdw,strsql.C_Str(),
        L"name,phonenumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive",
        TABLE_SMS,year,month,day);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&pi->ContactName,(LPWSTR) sqlite3_column_text16(pStmt, 0));
            C::newstrcpy(&pi->MobileNumber,(LPWSTR) sqlite3_column_text16(pStmt, 1));
            C::newstrcpy(&pi->Content,(LPWSTR) sqlite3_column_text16(pStmt, 2));
            C::newstrcpy(&pi->TimeStamp,(LPWSTR) sqlite3_column_text16(pStmt, 3));
            pi->SendReceiveFlag = (BOOL)sqlite3_column_int(pStmt, 4);
            pi++;
        }
    }
    sqlite3_finalize(pStmt);
    return total;
}

UINT LocalDataBase::GetSmsByContact(LPWSTR pname,SmsSimpleData_ptr plist){
    if(pname == NULL) return 0; //姓名错误

    UINT total = 0;
    wsprintf(sqlcmdw,L"select count(*) from '%s' where name='%s'",TABLE_SMS,pname);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total == 0 || plist == NULL) return total;

    SmsSimpleData_ptr pi = plist;
    wsprintf(sqlcmdw,L"select name,phonenumber,content,strftime('%%Y-%%m-%%d %%H:%%M:%%S',timestamps),sendreceive from '%s' "
                    L"where name='%s' order by timestamps DESC",
                    TABLE_SMS,pname);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&pi->ContactName,(LPWSTR) sqlite3_column_text16(pStmt, 0));
            C::newstrcpy(&pi->MobileNumber,(LPWSTR) sqlite3_column_text16(pStmt, 1));
            C::newstrcpy(&pi->Content,(LPWSTR) sqlite3_column_text16(pStmt, 2));
            C::newstrcpy(&pi->TimeStamp,(LPWSTR) sqlite3_column_text16(pStmt, 3));
            pi->SendReceiveFlag = (BOOL)sqlite3_column_int(pStmt, 4);
            pi++;
        }
    }
    sqlite3_finalize(pStmt);
    return total;
}

UINT LocalDataBase::GetSmsByContent(LPWSTR pcontent,SmsSimpleData_ptr plist){
    if(pcontent == NULL) return 0; //内容错误

    UINT total = 0;
    wsprintf(sqlcmdw,L"select count(*) from '%s' where content like '%%%s%%'",TABLE_SMS,pcontent);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total == 0 || plist == NULL) return total;

    SmsSimpleData_ptr pi = plist;
    wsprintf(sqlcmdw,L"select name,phonenumber,content,strftime('%%Y-%%m-%%d %%H:%%M:%%S',timestamps),sendreceive from '%s' "
                    L"where content like '%%%s%%' order by timestamps DESC",
                    TABLE_SMS,pcontent);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&pi->ContactName,(LPWSTR) sqlite3_column_text16(pStmt, 0));
            C::newstrcpy(&pi->MobileNumber,(LPWSTR) sqlite3_column_text16(pStmt, 1));
            C::newstrcpy(&pi->Content,(LPWSTR) sqlite3_column_text16(pStmt, 2));
            C::newstrcpy(&pi->TimeStamp,(LPWSTR) sqlite3_column_text16(pStmt, 3));
            pi->SendReceiveFlag = (BOOL)sqlite3_column_int(pStmt, 4);
            pi++;
        }
    }
    sqlite3_finalize(pStmt);
    return total;
}

///////////////////////////////////////
bool LocalDataBase::RemoveSmsRecord(SmsSimpleData_ptr psms){
	bool nRet = false;
	if(psms == NULL) return nRet;
	//从短信表中删除
	wsprintf(sqlcmdw,
		L"delete from '%s' where PhoneNumber='%s' and TimeStamps='%s' and SendReceive=%d",
		TABLE_SMS,
		psms->MobileNumber,psms->TimeStamp,psms->SendReceiveFlag);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_DONE){
            nRet = true;
        }
    }
    sqlite3_finalize(pStmt);
	return nRet;
}

///////////////////////////////////////////////
SYSTEMTIME LocalDataBase::GetSmsLatestDateTime(){
    SYSTEMTIME dt = {0,0,0,0,0,0,0,0};
    UINT s,r;
    GetSmsCount(s,r);
    if(s+r == 0) return dt;
    wsprintf(sqlcmdw,
        L"SELECT max(TimeStamps) FROM %s",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            swscanf((LPWSTR) sqlite3_column_text16(pStmt, 0),
                L"%04d-%02d-%02d %02d:%02d:%02d",
                &dt.wYear,&dt.wMonth,&dt.wDay,&dt.wHour,&dt.wMinute,&dt.wSecond);
        }
    }
    sqlite3_finalize(pStmt);
    return dt;
}

bool LocalDataBase::TableExists(wchar_t* tablename){
	bool nRet = false;
	bool c = false;	//如果为连接数据库，则打开数据后执行关闭，恢复初始状态
	if(!bconnected) {
		connect();
		c = true;
	}
	if(!bconnected) return nRet;

    wsprintf(sqlcmdw,L"select count(*) from sqlite_master where type='table' and name = '%s'",tablename);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		if(sqlite3_step(pStmt) == SQLITE_ROW){
			nRet = (sqlite3_column_int(pStmt, 0) != 0);
		}
    }
    sqlite3_finalize(pStmt);
    if(c){
	    disconnect();
    }
	return nRet;
}

void LocalDataBase::MigrateSmsData(){
	bool c = false;	//如果为连接数据库，则打开数据后执行关闭，恢复初始状态
	if(!bconnected) {
		connect();
		c = true;
	}
	if(!bconnected) return;

    wsprintf(sqlcmdw,CREATE_SMS_TBL,TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw,L"insert into '%s' (name,PN,phonenumber,content,timestamps,sendreceive,year,month,day) "
        L"select contacts_v1.Name, sms_v1.PN, sms_v1.PhoneNumber, sms_v1.content,sms_v1.timestamps,sms_v1.sendreceive,"
        L"strftime('%%Y',sms_v1.timestamps),strftime('%%m',sms_v1.timestamps),strftime('%%d',sms_v1.timestamps)"
        L"from contacts_v1,sms_v1 where (contacts_v1.PhoneNumber =  sms_v1.PN)",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw,L"insert into '%s' (name,PN,phonenumber,content,timestamps,sendreceive,year,month,day) "
        L"select PN as name,PN, PhoneNumber, content,timestamps,sendreceive,"
        L"strftime('%%Y',timestamps),strftime('%%m',timestamps),strftime('%%d',timestamps)"
        L"from sms_v1 where PN not in (select PhoneNumber from contacts_v1)",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);
    if(c){
	    disconnect();
    }
}

void LocalDataBase::DropOldSmsTable(){
	bool c = false;	//如果为连接数据库，则打开数据后执行关闭，恢复初始状态
	if(!bconnected) {
		connect();
		c = true;
	}
	if(!bconnected) return;

    wsprintf(sqlcmdw,L"drop table '%s'",TABLE_SMS_OLD);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);
    if(c){
	    disconnect();
    }
	return;
}

void LocalDataBase::updateV2(CallBackDatabaseUpdate callback){
	if(TableExists(TABLE_SMS_OLD)){
		if(callback){
			(*callback)(L"升级数据库中。。。",L"转移数据",10);
		}
		beginTrans();
		MigrateSmsData();
		if(callback){
			(*callback)(L"升级数据库中。。。",L"删除老数据",50);
		}
		DropOldSmsTable();
		commitTrans();
		if(callback){
			(*callback)(L"升级数据库中。。。",L"优化数据",80);
		}
		reorgDatebase();
		if(callback){
			(*callback)(L"升级数据库中。。。",L"完成",100);
		}
		if(callback){
			(*callback)(NULL,NULL,0);
		}
	}
}