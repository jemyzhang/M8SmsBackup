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
    BOOL Open(const TCHAR *szSmsFileName);   //�����ݿ�
    BOOL Close( void );      // �ر����ݿ�
    WORD GetRecordCount();
public:
	//��ȡ��������
	ULONG GetSmsCount() {
		return GetRecordCount(); 
	}
	//��ȡ��������
	BOOL GetSms(DWORD dwRecordID,SmsData_t &sms);
};
