#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

class MzContactDataBaseV2
{
private:
	sqlite3_connection sqlconn;
public:
	MzContactDataBaseV2(const TCHAR *szContactFileName);
	~MzContactDataBaseV2(void);
protected:
    BOOL Open(const TCHAR *szContactFileName);   //打开数据库
    BOOL Close( void );      // 关闭数据库
    WORD GetRecordCount();
public:
	//获取联系人总数
	ULONG GetContactCount() {
		return GetRecordCount(); 
	}
	//获取联系人内容
	BOOL GetContact(WORD dwRecordID,ContactData_t &contact);
};
