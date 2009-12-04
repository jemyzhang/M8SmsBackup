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
    BOOL Open(const TCHAR *szContactFileName);   //�����ݿ�
    BOOL Close( void );      // �ر����ݿ�
    WORD GetRecordCount();
public:
	//��ȡ��ϵ������
	ULONG GetContactCount() {
		return GetRecordCount(); 
	}
	//��ȡ��ϵ������
	BOOL GetContact(WORD dwRecordID,ContactData_t &contact);
};
