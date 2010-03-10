#include "LocalDataBase.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#include "logout.h"

#ifndef _DEBUG
#pragma warning(disable:4101)
#endif

LocalDataBase::LocalDataBase() {
    wchar_t currpath[MAX_PATH];
    bool ret = true;
    if(File::GetCurrentPath(currpath)){
        wsprintf(db_path,L"%s\\sms.db",currpath);
    }else{
        wsprintf(db_path,DEFAULT_DB);
    }

    connect();
}

LocalDataBase::~LocalDataBase() {
    disconnect();
}

bool LocalDataBase::connect(){
	bool bRet = true;
	TRY{
		bool bfileNotExists = false;
		if(!File::FileExists(db_path)){
			bfileNotExists = true;
		}
		sqlconn.open(db_path);
		sqlconn.createPinyinSearchEngine();
		//文件新建时创建默认表
		if(bfileNotExists) createDefaultDatabase();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool LocalDataBase::disconnect(){
	bool bRet = true;
	TRY{
		sqlconn.close();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool LocalDataBase::checkpwd(wchar_t* pwd,int len){
	bool bRet = false;

	if(pwd && len != 0){
		decrypt(pwd,len);
	}
	TRY{
		bRet = sqlconn.executeint(L"select count(*) from sqlite_master;") > 0;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    if(!bRet){  //如果解密失败，则需要断开数据库后再连接
        disconnect();
        connect();
    }
    return bRet;
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
    bool bRet = true;
    TRY{
	    sqlconn.decypt(temp,bytecnt);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	delete temp;
    return bRet;
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
    bool bRet = true;
    TRY{
	    sqlconn.encypt(temp,bytecnt);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	delete temp;
	return bRet;
}

bool LocalDataBase::beginTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"begin;");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool LocalDataBase::commitTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"commit;");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool LocalDataBase::indexDatabase(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(
            L"create index idx_sms on '"
            TABLE_SMS
            L"'(Name,PN,Year,Month,Day);");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool LocalDataBase::reorgDatebase(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"VACUUM");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

void LocalDataBase::createDefaultDatabase() {
	TRY{
		//create sms table
		int count = sqlconn.executeint(
			L"select count(*) from sqlite_master where name='"
			TABLE_SMS
			L"';");
		if(count == 0){
			sqlconn.executenonquery(
				L"create table if not exists '"
				TABLE_SMS
				L"' (Name text not null, PN text not null, PhoneNumber text not null, Content text not null,"
				L"TimeStamps datetime not null, SendReceive numeric not null,"
				L"Year text, Month text, Day text);");
		}
		//create contact table
		count = sqlconn.executeint(
			L"select count(*) from sqlite_master where name='"
			TABLE_CONTACT
			L"';");
		if(count == 0){
			sqlconn.executenonquery(
				L"create table if not exists '"
				TABLE_CONTACT
				L"' (PhoneNumber text primary key,Name text NOT NULL,Label numeric NOT NULL);");
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return;
}

//////////////////////////////////////
//sms操作
bool LocalDataBase::AppendSmsRecord(SmsData_ptr psms){
    if(psms == NULL) return false;
    if(isDuplicateSms(psms)) return false;

    SYSTEMTIME smstm = psms->TimeStamp;
    LPWSTR pname = NULL;
    if(!getContactName(psms->PNSort,&pname)){   //当依靠号码没找到名称时，使用收件人号码
        if(!getContactName(psms->MobileNumber,&pname)){
            C::newstrcpy(&pname,psms->PNSort);
        }
    }

    bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"insert into '"
		TABLE_SMS
		L"' (Name,PN,PhoneNumber,Content,TimeStamps,SendReceive,Year,Month,Day)"
		L" values(?,?,?,?,?,?, ?,?,?);");

		cmd.bind(1,pname,lstrlen(pname)*2);
		cmd.bind(2,psms->PNSort,lstrlen(psms->PNSort)*2);
		cmd.bind(3,psms->MobileNumber,lstrlen(psms->MobileNumber)*2);
		cmd.bind(4,psms->Content,lstrlen(psms->Content)*2);
		wchar_t timestamps[20];
		wsprintf(timestamps,L"%04d-%02d-%02d %02d:%02d:%02d",
			smstm.wYear,smstm.wMonth,smstm.wDay,
			smstm.wHour,smstm.wMinute,smstm.wSecond);
		cmd.bind(5,timestamps,lstrlen(timestamps)*2);

		cmd.bind(6,(int)psms->SendReceiveFlag);

		wchar_t sDate[8];
		wsprintf(sDate,L"%04d",smstm.wYear);
		cmd.bind(7,sDate,lstrlen(sDate)*2);

		wsprintf(sDate,L"%02d",smstm.wMonth);
		cmd.bind(8,sDate,lstrlen(sDate)*2);

		wsprintf(sDate,L"%02d",smstm.wDay);
		cmd.bind(9,sDate,lstrlen(sDate)*2);

		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	delete [] pname;
    return bRet;
}


bool LocalDataBase::isDuplicateSms(SmsData_ptr psms){
    bool bRet = true;
    if(psms == NULL) return false;
    SYSTEMTIME smstm = psms->TimeStamp;

	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select count(*) from '"
		TABLE_SMS
		L"' where PhoneNumber=? and TimeStamps=? and SendReceive=?");

		cmd.bind(1,psms->MobileNumber,lstrlen(psms->MobileNumber)*2);

		wchar_t timestamps[20];
		wsprintf(timestamps,L"%04d-%02d-%02d %02d:%02d:%02d",
			smstm.wYear,smstm.wMonth,smstm.wDay,
			smstm.wHour,smstm.wMinute,smstm.wSecond);
		cmd.bind(2,timestamps,lstrlen(timestamps)*2);
		cmd.bind(3,(int)psms->SendReceiveFlag);
		bRet = cmd.executeint() != 0;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}
//////////////////////////////////////
//contact操作
void LocalDataBase::query_clear(){
	for(int i = 0; i < query_contact_list.size(); i++){
		delete query_contact_list.at(i);
	}
	query_contact_list.clear();
}

bool LocalDataBase::query_contacts(){
    bool bRet = true;

	query_clear();

	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select * from '"
		TABLE_CONTACT
		L"' order by NAME collate pinyin;");
//PhoneNumber text primary key,Name text NOT NULL,Label numeric NOT NULL
		ContactData_ptr pcontact = 0;
		sqlite3_reader reader=cmd.executereader();
		TelLabel_t t;
		while(reader.read()){
			bool samecontact = false;
			wchar_t* pname = 0;
			//getstring后结果会销毁，所以必须保存
			C::newstrcpy(&pname, reader.getstring16(1).c_str());	
			if(pcontact == 0){
				pcontact = new ContactData;
				C::newstrcpy(&pcontact->Name,pname);
			}else{
				if(wcscmp(pcontact->Name,pname) == 0){
					samecontact = true;
				}else{
					pcontact = new ContactData;
					C::newstrcpy(&pcontact->Name,pname);
				}
			}
			delete pname;
			t = (TelLabel_t)reader.getint(2);
			LPWSTR pn = 0;
			C::newstrcpy(&pn,reader.getstring16(0).c_str());
			switch(t){
				case MOBILETEL:
					pcontact->MobileTels.push_back(pn);
					break;
				case WORKTEL:
					pcontact->WorkTels.push_back(pn);
					break;
				case HOMETEL:
					pcontact->HomeTels.push_back(pn);
					break;
				case HOMETEL2:
					pcontact->HomeTel2s.push_back(pn);
					break;
				default:
					break;
			}
			if(!samecontact){
				query_contact_list.push_back(pcontact);
			}
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}

bool LocalDataBase::isDuplicateContact(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool bRet = true;
    if(number == NULL || name == NULL) return false;

	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select count(*) from '"
		TABLE_CONTACT
		L"' where PhoneNumber=? and Name=? and Label=?");

		cmd.bind(1,number,lstrlen(number)*2);
		cmd.bind(2,name,lstrlen(name)*2);
		cmd.bind(3,(int)label);
		bRet = cmd.executeint() != 0;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    ::logout(L"Contact %s: %s %s",name,number,bRet ? L"exists" : L"not exsits");

    return bRet;
}

bool LocalDataBase::updateContact(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool bRet = true;
    if(number == NULL || name == NULL) return false;

	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"update '"
		TABLE_CONTACT
		L"' set Name=?,Label=? where PhoneNumber=?");

		cmd.bind(1,name,lstrlen(name)*2);
		cmd.bind(2,(int)label);
		cmd.bind(3,number,lstrlen(number)*2);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}

bool LocalDataBase::addContactRecord(LPWSTR number,LPWSTR name,TelLabel_t label){
    bool bRet = true;
    if(number == NULL || name == NULL) return false;
    //检查记录是否已存在
    if(isDuplicateContact(number,name,label)) return false;

    ::logout(L"Inserting Contact %s: %s",name,number);
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"insert into '"
		TABLE_CONTACT
		L"' (PhoneNumber,Name,Label) values(?,?,?)");

		cmd.bind(1,number,lstrlen(number)*2);
		cmd.bind(2,name,lstrlen(name)*2);
		cmd.bind(3,(int)label);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	//添加记录不成功时尝试更新记录
    if(bRet == false){
        bRet = updateContact(number,name,label);
        ::logout(L"Update Contact %s: %s=%d",name,number,bRet);
	}
    
    if(bRet){
		//更新短信名称
        int numberlen = lstrlen(number);
		wchar_t sqlcmdw[255];

		if(numberlen >= 10){  //当号码长度超过10时，取后面十位，针对国际号码
			wsprintf(sqlcmdw,L"update '"
				TABLE_SMS
				L"' set Name='%s' where PN like '%%%s'",
				name,number + numberlen - 10);
		}else{
			if(number[0] == '+'){   //当联系人号码中仍然有+号时，使用PhoneNumber更新
				wsprintf(sqlcmdw,L"update '"
					TABLE_SMS
					L"' set Name='%s' where PhoneNumber='%s'",
					name,number);
			}else{
				wsprintf(sqlcmdw,L"update '"
					TABLE_SMS
					L"' set Name='%s' where PN='%s'",
					name,number);
			}
		}
		TRY{
			sqlite3_command updatecmd(this->sqlconn,sqlcmdw);
			updatecmd.executenonquery();
            ::logout(L"SmsTable Updated");
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
	}
    return bRet;
}

UINT LocalDataBase::AppendContactRecord(ContactData_ptr pcontact){
    UINT nRet = 0;
    if(pcontact == NULL) return nRet;

    int size = pcontact->MobileTels.size();
    int i = 0;

    if(size > 0){
        for( i = 0; i < size; i++){
            ::logout(L"Append MobileTels#%d : %s",i+1,pcontact->MobileTels.at(i));
            if(addContactRecord(pcontact->MobileTels.at(i),pcontact->Name,MOBILETEL)) nRet++;
        }
    }
    
    size = pcontact->WorkTels.size();
    if(size > 0){
        for( i = 0; i < size; i++){
            ::logout(L"Append WorkTels#%d : %s",i+1,pcontact->WorkTels.at(i));
            if(addContactRecord(pcontact->WorkTels.at(i),pcontact->Name,WORKTEL)) nRet++;
        }
    }

    size = pcontact->HomeTels.size();
    if(size > 0){
        for( i = 0; i < size; i++){
            ::logout(L"Append HomeTels#%d : %s",i+1,pcontact->HomeTels.at(i));
            if(addContactRecord(pcontact->HomeTels.at(i),pcontact->Name,HOMETEL)) nRet++;
        }
    }

    size = pcontact->HomeTel2s.size();
    if(size > 0){
        for( i = 0; i < size; i++){
            ::logout(L"Append OtherTels#%d : %s",i+1,pcontact->HomeTel2s.at(i));
            if(addContactRecord(pcontact->HomeTel2s.at(i),pcontact->Name,HOMETEL2)) nRet++;
        }
    }
    return nRet;
}

bool LocalDataBase::getContactName(LPWSTR phonenumber,LPWSTR *ppname){
    bool bRet = true;;

	if(phonenumber == NULL) return false;

    int numberlen = lstrlen(phonenumber);
	wchar_t sqlcmdw[255];
    if(numberlen >= 10){
        wsprintf(sqlcmdw,L"select Name from '"
			TABLE_CONTACT
			L"' where PhoneNumber like '%%%s'",phonenumber + numberlen - 10);        
    }else{
        wsprintf(sqlcmdw,L"select Name from '"
			TABLE_CONTACT
			L"' where PhoneNumber='%s'",phonenumber);
    }
	TRY{
		sqlite3_command cmd(this->sqlconn,sqlcmdw);
		C::newstrcpy(ppname,cmd.executestring16().c_str());
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

UINT LocalDataBase::GetSmsCount(UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;

	bool bRet = true;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*),sum(sendreceive) from '"
			TABLE_SMS
			L"';");
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			total = reader.getint(0);
			sent = reader.getint(1);
			received = total - sent;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
    return total;
}

UINT LocalDataBase::GetSmsContactList(SmsViewListKey_ptr plist){
    UINT nSize = 0;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(distinct name) from '"
			TABLE_SMS
			L"';");
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
	TRY{	//获取列表
		sqlite3_command cmd(this->sqlconn,
			L"select distinct name from '"
			TABLE_SMS
			L"' order by name collate pinyin");
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&(pkey++)->key,reader.getstring16(0).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

UINT LocalDataBase::GetSmsYearList(SmsViewListKey_ptr plist){
    UINT nSize = 0;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(distinct year) from '"
			TABLE_SMS
			L"';");
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
	TRY{	//获取列表
		sqlite3_command cmd(this->sqlconn,
			L"select distinct year from '"
			TABLE_SMS
			L"' order by year DESC");
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&(pkey++)->key,reader.getstring16(0).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

UINT LocalDataBase::GetSmsMonthList(WORD year, SmsViewListKey_ptr plist){
    UINT nSize = 0;

	wchar_t sYear[8];
	wsprintf(sYear,L"%04d",year);
	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(distinct month) from '"
			TABLE_SMS
			L"' where year=?;");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
	TRY{	//获取列表
		sqlite3_command cmd(this->sqlconn,
			L"select distinct month from '"
			TABLE_SMS
			L"' where year=? order by month DESC");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&(pkey++)->key,reader.getstring16(0).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

UINT LocalDataBase::GetSmsDayList(WORD year, WORD month,SmsViewListKey_ptr plist){
    UINT nSize = 0;

	wchar_t sYear[8];
	wchar_t sMonth[6];
	wsprintf(sYear,L"%04d",year);
	wsprintf(sMonth,L"%02d",month);
	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(distinct day) from '"
			TABLE_SMS
			L"' where year=? and month=?;");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		cmd.bind(2,sMonth,lstrlen(sMonth)*2);
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(plist == NULL || nSize == 0) return nSize;

    SmsViewListKey_ptr pkey = plist;
	TRY{	//获取列表
		sqlite3_command cmd(this->sqlconn,
			L"select distinct day from '"
			TABLE_SMS
			L"' where year=? and month=? order by day DESC");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		cmd.bind(2,sMonth,lstrlen(sMonth)*2);
		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&(pkey++)->key,reader.getstring16(0).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}
////////////////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsYearCount(WORD year, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;
	wchar_t sYear[8];
	wsprintf(sYear,L"%04d",year);
	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*),sum(sendreceive) from '"
			TABLE_SMS
			L"' where year=?;");
		cmd.bind(1,sYear,lstrlen(sYear)*2);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			total = reader.getint(0);
			sent = reader.getint(1);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	received = total - sent;

    return total;
}

UINT LocalDataBase::GetSmsMonthCount(WORD year, WORD month, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;

	wchar_t sYear[8];
	wchar_t sMonth[6];
	wsprintf(sYear,L"%04d",year);
	wsprintf(sMonth,L"%02d",month);

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*),sum(sendreceive) from '"
			TABLE_SMS
			L"' where year=? and month=?;");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		cmd.bind(2,sMonth,lstrlen(sMonth)*2);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			total = reader.getint(0);
			sent = reader.getint(1);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
    received = total - sent;

    return total;
}

UINT LocalDataBase::GetSmsDayCount(WORD year, WORD month,WORD day, UINT &received, UINT &sent){
    UINT total = 0;
    received = 0;
    sent = 0;

	wchar_t sYear[8];
	wchar_t sMonth[6];
	wchar_t sDay[6];
	wsprintf(sYear,L"%04d",year);
	wsprintf(sMonth,L"%02d",month);
	wsprintf(sDay,L"%02d",day);

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*),sum(sendreceive) from '"
			TABLE_SMS
			L"' where year=? and month=? and day=?;");
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		cmd.bind(2,sMonth,lstrlen(sMonth)*2);
		cmd.bind(3,sDay,lstrlen(sDay)*2);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			total = reader.getint(0);
			sent = reader.getint(1);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
    received = total - sent;
    return total;
}

UINT LocalDataBase::GetSmsContactCount(LPWSTR name,UINT &received, UINT &sent){
    if(name == NULL) return 0;

    UINT total = 0;
    received = 0;
    sent = 0;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*),sum(sendreceive) from '"
			TABLE_SMS
			L"' where name=?;");
		cmd.bind(1,name,lstrlen(name)*2);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			total = reader.getint(0);
			sent = reader.getint(1);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
    received = total - sent;
    return total;
}
//////////////////////////////////////////////////////
UINT LocalDataBase::GetSmsByDate(WORD year, WORD month, WORD day,SmsSimpleData_ptr plist){
    if(year == 0) return 0; //年份错误
	UINT nSize = 0;
	TRY{	//获取总数
		std::wstring sql = L"select count(*) from '"
			TABLE_SMS
			L"' where year=?";
		if(month != 0) sql = sql + L" and month=?";
		if(day != 0) sql = sql + L" and day=?";
		sqlite3_command cmd(this->sqlconn,sql);

		wchar_t sYear[8];
		wsprintf(sYear,L"%04d",year);
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		if(month != 0){
			wchar_t sMonth[6];
			wsprintf(sMonth,L"%02d",month);
			cmd.bind(2,sMonth,lstrlen(sMonth)*2);
		}
		if(day != 0){
			wchar_t sDay[6];
			wsprintf(sDay,L"%02d",day);
			cmd.bind(3,sDay,lstrlen(sDay)*2);
		}
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(nSize == 0 || plist == NULL) return nSize;

	TRY{	//获取总数
		std::wstring sql = L"select name,phonenumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive from '"
			TABLE_SMS
			L"' where year=?";
		if(month != 0) sql = sql + L" and month=?";
		if(day != 0) sql = sql + L" and day=?";

		sql = sql + L" order by timestamps DESC;";
		sqlite3_command cmd(this->sqlconn,sql);

		wchar_t sYear[8];
		wsprintf(sYear,L"%04d",year);
		cmd.bind(1,sYear,lstrlen(sYear)*2);
		if(month != 0){
			wchar_t sMonth[6];
			wsprintf(sMonth,L"%02d",month);
			cmd.bind(2,sMonth,lstrlen(sMonth)*2);
		}
		if(day != 0){
			wchar_t sDay[6];
			wsprintf(sDay,L"%02d",day);
			cmd.bind(3,sDay,lstrlen(sDay)*2);
		}
		sqlite3_reader reader=cmd.executereader();
	    SmsSimpleData_ptr pi = plist;
		while(reader.read()){
			C::newstrcpy(&pi->ContactName,reader.getstring16(0).c_str());
            C::newstrcpy(&pi->MobileNumber,reader.getstring16(1).c_str());
            C::newstrcpy(&pi->Content,reader.getstring16(2).c_str());
            C::newstrcpy(&pi->TimeStamp,reader.getstring16(3).c_str());
			pi->SendReceiveFlag = reader.getint(4);
            pi++;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

UINT LocalDataBase::GetSmsByContact(LPWSTR pname,SmsSimpleData_ptr plist){
    if(pname == NULL) return 0; //姓名错误
	UINT nSize = 0;
	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*) from '"
			TABLE_SMS
			L"' where name=?");

		cmd.bind(1,pname,lstrlen(pname)*2);
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(nSize == 0 || plist == NULL) return nSize;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select name,phonenumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive from '"
			TABLE_SMS
			L"' where name=? order by timestamps DESC;");

		cmd.bind(1,pname,lstrlen(pname)*2);

		sqlite3_reader reader=cmd.executereader();
	    SmsSimpleData_ptr pi = plist;
		while(reader.read()){
			C::newstrcpy(&pi->ContactName,reader.getstring16(0).c_str());
            C::newstrcpy(&pi->MobileNumber,reader.getstring16(1).c_str());
            C::newstrcpy(&pi->Content,reader.getstring16(2).c_str());
            C::newstrcpy(&pi->TimeStamp,reader.getstring16(3).c_str());
			pi->SendReceiveFlag = reader.getint(4);
            pi++;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

UINT LocalDataBase::GetSmsByContent(LPWSTR pcontent,SmsSimpleData_ptr plist){
    if(pcontent == NULL) return 0; //内容错误

	UINT nSize = 0;
	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select count(*) from '"
			TABLE_SMS
			L"' where content like ?");
		std::wstring likestr = L"%%";
		likestr = likestr + pcontent + L"%%";

		cmd.bind(1,likestr.c_str(),likestr.length()*2);
		nSize = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    if(nSize == 0 || plist == NULL) return nSize;

	TRY{	//获取总数
		sqlite3_command cmd(this->sqlconn,
			L"select name,phonenumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive from '"
			TABLE_SMS
			L"' where content like ? order by timestamps DESC;");

		std::wstring likestr = L"%%";
		likestr = likestr + pcontent + L"%%";
		cmd.bind(1,likestr.c_str(),likestr.length()*2);

		sqlite3_reader reader=cmd.executereader();
	    SmsSimpleData_ptr pi = plist;
		while(reader.read()){
			C::newstrcpy(&pi->ContactName,reader.getstring16(0).c_str());
            C::newstrcpy(&pi->MobileNumber,reader.getstring16(1).c_str());
            C::newstrcpy(&pi->Content,reader.getstring16(2).c_str());
            C::newstrcpy(&pi->TimeStamp,reader.getstring16(3).c_str());
			pi->SendReceiveFlag = reader.getint(4);
            pi++;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

    return nSize;
}

///////////////////////////////////////
bool LocalDataBase::RemoveSmsRecord(SmsSimpleData_ptr psms){
	bool bRet = true;
	if(psms == NULL) return false;
	//从短信表中删除
	TRY{	//获取最新短信时间
		sqlite3_command cmd(this->sqlconn,
			L"delete from '"
			TABLE_SMS
			L"' where PhoneNumber=? and TimeStamps=? and SendReceive=?;");
		cmd.bind(1,psms->MobileNumber,lstrlen(psms->MobileNumber)*2);
		cmd.bind(2,psms->TimeStamp,lstrlen(psms->TimeStamp)*2);
		cmd.bind(3,(int)psms->SendReceiveFlag);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

///////////////////////////////////////////////
SYSTEMTIME LocalDataBase::GetSmsLatestDateTime(){
    SYSTEMTIME dt = {0,0,0,0,0,0,0,0};
    UINT s,r;
    GetSmsCount(s,r);
    if(s+r == 0) return dt;

	TRY{	//获取最新短信时间
		sqlite3_command cmd(this->sqlconn,
			L"select max(TimeStamps) from '"
			TABLE_SMS
			L"';");
		swscanf(cmd.executestring16().c_str(),
                L"%04d-%02d-%02d %02d:%02d:%02d",
                &dt.wYear,&dt.wMonth,&dt.wDay,&dt.wHour,&dt.wMinute,&dt.wSecond);
	}CATCH(exception &ex){
		db_out(ex.what());
	}
    return dt;
}

bool LocalDataBase::TableExists(wchar_t* tablename){
	bool bRet = true;
	TRY{	//检查表是否存在
		sqlite3_command cmd(this->sqlconn,
			L"select count(*) from sqlite_master where type='table' and name=?;");
		cmd.bind(1,tablename,lstrlen(tablename)*2);
		bRet = cmd.executeint() > 0;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

void LocalDataBase::MigrateSmsData(){
	TRY{	//创建新版数据库
		sqlite3_command cmd(this->sqlconn,
			L"create table if not exists  '"
			TABLE_SMS
			L"' (Name text not null, PN text not null, PhoneNumber text not null, Content text not null, "
			L"TimeStamps datetime not null, SendReceive numeric not null, "
			L"Year text, Month text, Day text);");
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	TRY{	//插入有联系人名称的记录
		sqlite3_command cmd(this->sqlconn,
			L"insert into  '"
			TABLE_SMS
			L"' (name,PN,phonenumber,content,timestamps,sendreceive,year,month,day)"
			L" select contacts_v1.Name, sms_v1.PN, sms_v1.PhoneNumber, sms_v1.content,sms_v1.timestamps,sms_v1.sendreceive,"
			L" strftime('%Y',sms_v1.timestamps),strftime('%m',sms_v1.timestamps),strftime('%d',sms_v1.timestamps)"
			L" from contacts_v1,sms_v1 where (contacts_v1.PhoneNumber =  sms_v1.PN);");
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	TRY{	//插入陌生人记录
		sqlite3_command cmd(this->sqlconn,
			L"insert into  '"
			TABLE_SMS
			L"' (name,PN,phonenumber,content,timestamps,sendreceive,year,month,day)"
			L" select PN as name,PN, PhoneNumber, content,timestamps,sendreceive,"
			L" strftime('%Y',timestamps),strftime('%m',timestamps),strftime('%d',timestamps)"
			L" from sms_v1 where PN not in (select PhoneNumber from contacts_v1);");
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}
}

void LocalDataBase::DropOldSmsTable(){
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"drop table '"
			TABLE_SMS_OLD
			L"';");
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}
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

void LocalDataBase::ClearContactTable(){
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"delete from '"
			TABLE_CONTACT
			L"';");
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}
}


void LocalDataBase::query_sms_clear(){
	for(int i = 0; i < query_sms_list.size(); i++){
		delete query_sms_list.at(i);
	}
	query_sms_list.clear();
}

bool LocalDataBase::query_sms(){
    bool bRet = true;

	query_sms_clear();

	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select name,phonenumber,content,strftime('%Y-%m-%d %H:%M:%S',timestamps),sendreceive from '"
			TABLE_SMS
			L"' order by name collate pinyin,timestamps;");

		sqlite3_reader reader=cmd.executereader();
		SmsSimpleData_ptr pi;
		while(reader.read()){
			pi = new SmsSimpleData;
			C::newstrcpy(&pi->ContactName,reader.getstring16(0).c_str());
			C::newstrcpy(&pi->MobileNumber,reader.getstring16(1).c_str());
			C::newstrcpy(&pi->Content,reader.getstring16(2).c_str());
			C::newstrcpy(&pi->TimeStamp,reader.getstring16(3).c_str());
			pi->SendReceiveFlag = reader.getint(4);
			query_sms_list.push_back(pi);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}