#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

class MzSMSDataBaseV2
{
private:
	sqlite3_connection sqlconn;
public:
	MzSMSDataBaseV2(const TCHAR *szSmsFileName);
	~MzSMSDataBaseV2(void);
protected:
    BOOL Open(const TCHAR *szSmsFileName);   //打开数据库
    BOOL Close( void );      // 关闭数据库
    WORD GetRecordCount();
public:
	//获取短信条数
	ULONG GetSmsCount() {
		return GetRecordCount(); 
	}
	//获取短信内容
	BOOL GetSms(DWORD dwRecordID,SmsData_t &sms);
};
