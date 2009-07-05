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
    bTempTableCreated = false;
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

UINT LocalDataBase::GetSmsCount(UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;

    wsprintf(sqlcmdw,L"select count(*) from %s",TABLE_SMS);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
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

////////////////////////////////////////////////////////////////////
bool LocalDataBase::CreateTempSmsTable(){
    if(bTempTableCreated) return bTempTableCreated;

    wsprintf(sqlcmdw,L"create temp table if not exists '%s' "
        L"(name text,telnumber text, content text,timestamps datetime,sendreceive numeric, year text, month text, day text)",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_DONE){
            bTempTableCreated = true;
        }
    }
    sqlite3_finalize(pStmt);
    if(!bTempTableCreated) return bTempTableCreated;

    wsprintf(sqlcmdw,L"insert into '%s' (name,telnumber,content,timestamps,sendreceive,year,month,day) "
        L"select contacts_v1.Name, sms_v1.PN, sms_v1.content,sms_v1.timestamps,sms_v1.sendreceive,"
        L"strftime('%%Y',sms_v1.timestamps),strftime('%%m',sms_v1.timestamps),strftime('%%d',sms_v1.timestamps)"
        L"from contacts_v1,sms_v1 where (contacts_v1.PhoneNumber =  sms_v1.PN)",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw,L"insert into '%s' (name,telnumber,content,timestamps,sendreceive,year,month,day) "
        L"select PN as name,PN, content,timestamps,sendreceive,"
        L"strftime('%%Y',sms_v1.timestamps),strftime('%%m',sms_v1.timestamps),strftime('%%d',sms_v1.timestamps)"
        L"from sms_v1 where (select count(*) from contacts_v1 where contacts_v1.PhoneNumber ==  sms_v1.PN)=0",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);
    return bTempTableCreated;
}

UINT LocalDataBase::GetSmsContactList(SmsViewListKey_ptr plist){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct name) from '%s'",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct name from '%s' order by name collate pinyin",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    for(UINT i = 0; i < nSize; i++){
        plist[i].nSend = 0;
        plist[i].nReceive = 0;
        //获取已发送条数
        wsprintf(sqlcmdw,
            L"select sendreceive from '%s' where name='%s'",
            TABLE_TEMP,plist[i].key);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            while (sqlite3_step(pStmt) == SQLITE_ROW){
                SmsSendReceive_t sr = (SmsSendReceive_t)sqlite3_column_int(pStmt, 0);
                if(sr == SMS_SEND){
                    plist[i].nSend++;
                }else{
                    plist[i].nReceive++;
                }
            }
        }
        sqlite3_finalize(pStmt);
    }
    return nSize;
}

UINT LocalDataBase::GetSmsYearList(SmsViewListKey_ptr plist){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct year) from '%s'",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct year from '%s'",TABLE_TEMP);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    for(UINT i = 0; i < nSize; i++){
        plist[i].nSend = 0;
        plist[i].nReceive = 0;
        //获取已发送条数
        wsprintf(sqlcmdw,
            L"select sendreceive from '%s' where year='%s'",
            TABLE_TEMP,plist[i].key);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            while (sqlite3_step(pStmt) == SQLITE_ROW){
                SmsSendReceive_t sr = (SmsSendReceive_t)sqlite3_column_int(pStmt, 0);
                if(sr == SMS_SEND){
                    plist[i].nSend++;
                }else{
                    plist[i].nReceive++;
                }
            }
        }
        sqlite3_finalize(pStmt);
    }
    return nSize;
}

UINT LocalDataBase::GetSmsMonthList(WORD year, SmsViewListKey_ptr plist){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct month) from '%s' where year='%04d'",
        TABLE_TEMP,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct month from '%s' where year='%04d'",
        TABLE_TEMP,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    for(UINT i = 0; i < nSize; i++){
        plist[i].nSend = 0;
        plist[i].nReceive = 0;
        //获取已发送条数
        wsprintf(sqlcmdw,
            L"select sendreceive from '%s' where month='%s' and year='%04d'",
            TABLE_TEMP,plist[i].key,year);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            while (sqlite3_step(pStmt) == SQLITE_ROW){
                SmsSendReceive_t sr = (SmsSendReceive_t)sqlite3_column_int(pStmt, 0);
                if(sr == SMS_SEND){
                    plist[i].nSend++;
                }else{
                    plist[i].nReceive++;
                }
            }
        }
        sqlite3_finalize(pStmt);
    }
    return nSize;
}

UINT LocalDataBase::GetSmsDayList(WORD year, WORD month,SmsViewListKey_ptr plist){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT nSize = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(distinct day) from '%s' where year='%04d' and month='%02d'",
        TABLE_TEMP,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            nSize = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
    wsprintf(sqlcmdw,L"select distinct day from '%s' where year='%04d'and month='%02d'",
        TABLE_TEMP,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW){
            C::newstrcpy(&(pkey++)->key,(LPWSTR) sqlite3_column_text16(pStmt, 0));
        }
    }
    sqlite3_finalize(pStmt);

    for(UINT i = 0; i < nSize; i++){
        plist[i].nSend = 0;
        plist[i].nReceive = 0;
        //获取已发送条数
        wsprintf(sqlcmdw,
            L"select sendreceive from '%s' where day='%s' and month='%02d' and year='%04d'",
            TABLE_TEMP,plist[i].key,month,year);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            while (sqlite3_step(pStmt) == SQLITE_ROW){
                SmsSendReceive_t sr = (SmsSendReceive_t)sqlite3_column_int(pStmt, 0);
                if(sr == SMS_SEND){
                    plist[i].nSend++;
                }else{
                    plist[i].nReceive++;
                }
            }
        }
        sqlite3_finalize(pStmt);
    }
    return nSize;
}
////////////////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsYearCount(WORD year, UINT &received, UINT &sent){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d'",
        TABLE_TEMP,year);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total > 0){
        wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d' and sendreceive=%d",
            TABLE_TEMP,year,SMS_SEND);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            if (sqlite3_step(pStmt) == SQLITE_ROW){
                sent = sqlite3_column_int(pStmt, 0);
            }
        }
        sqlite3_finalize(pStmt);
    }
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsMonthCount(WORD year, WORD month, UINT &received, UINT &sent){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d' and month='%02d'",
        TABLE_TEMP,year,month);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total > 0){
        wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d' and month='%02d' and sendreceive=%d",
            TABLE_TEMP,year,month,SMS_SEND);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            if (sqlite3_step(pStmt) == SQLITE_ROW){
                sent = sqlite3_column_int(pStmt, 0);
            }
        }
        sqlite3_finalize(pStmt);
    }
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsDayCount(WORD year, WORD month,WORD day, UINT &received, UINT &sent){
    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d' and month='%02d' and day='%02d'",
        TABLE_TEMP,year,month,day);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total > 0){
        wsprintf(sqlcmdw,L"select count(*) from '%s' where year='%04d' and month='%02d' and day='%02d' and sendreceive=%d",
            TABLE_TEMP,year,month,day,SMS_SEND);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            if (sqlite3_step(pStmt) == SQLITE_ROW){
                sent = sqlite3_column_int(pStmt, 0);
            }
        }
        sqlite3_finalize(pStmt);
    }
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsContactCount(LPWSTR name,UINT &received, UINT &sent){
    if(name == NULL) return 0;

    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    received = 0;
    sent = 0;
    //GetSize
    wsprintf(sqlcmdw,L"select count(*) from '%s' where name='%s'",
        TABLE_TEMP,name);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total > 0){
        wsprintf(sqlcmdw,L"select count(*) from '%s' where name='%s' and sendreceive=%d",
            TABLE_TEMP,name,SMS_SEND);
        if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
            if (sqlite3_step(pStmt) == SQLITE_ROW){
                sent = sqlite3_column_int(pStmt, 0);
            }
        }
        sqlite3_finalize(pStmt);
    }
    received = total - sent;
    return total;
}
//////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsByDate(WORD year, WORD month, WORD day,SmsSimpleData_ptr plist){
    if(year == 0) return 0; //年份错误

    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    CMzString strsql = L"select %s from '%s' where year='%04d'";
    if(month != 0) strsql = strsql + L" and month='%02d'";
    if(day != 0) strsql = strsql + L" and day='%02d'";

    UINT total = 0;
    wsprintf(sqlcmdw,strsql.C_Str(),L"count(*)",TABLE_TEMP,year,month,day);
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
        L"name,telnumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive",
        TABLE_TEMP,year,month,day);
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
    return total;
}

UINT LocalDataBase::GetSmsByContact(LPWSTR pname,SmsSimpleData_ptr plist){
    if(pname == NULL) return 0; //姓名错误

    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    wsprintf(sqlcmdw,L"select count(*) from '%s' where name='%s'",TABLE_TEMP,pname);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total == 0 || plist == NULL) return total;

    SmsSimpleData_ptr pi = plist;
    wsprintf(sqlcmdw,L"select name,telnumber,content,strftime('%%Y-%%m-%%d %%H:%%M:%%S',timestamps),sendreceive from '%s' "
                    L"where name='%s' order by timestamps DESC",
                    TABLE_TEMP,pname);
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
    return total;
}

UINT LocalDataBase::GetSmsByContent(LPWSTR pcontent,SmsSimpleData_ptr plist){
    if(pcontent == NULL) return 0; //内容错误

    if(!bTempTableCreated){
        if(!CreateTempSmsTable()){  //创建不成功
            return 0;
        }
    }
    UINT total = 0;
    wsprintf(sqlcmdw,L"select count(*) from '%s' where content like '%%%s%%'",TABLE_TEMP,pcontent);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
        if (sqlite3_step(pStmt) == SQLITE_ROW){
            total = sqlite3_column_int(pStmt, 0);
        }
    }
    sqlite3_finalize(pStmt);
    if(total == 0 || plist == NULL) return total;

    SmsSimpleData_ptr pi = plist;
    wsprintf(sqlcmdw,L"select name,telnumber,content,strftime('%%Y-%%m-%%d %%H:%%M:%%S',timestamps),sendreceive from '%s' "
                    L"where content like '%%%s%%' order by timestamps DESC",
                    TABLE_TEMP,pcontent);
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
    return total;
}