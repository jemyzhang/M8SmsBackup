#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

#ifndef EDB
#define EDB 1
#endif

// EDB ר�����ͣ����� windbase.h
#ifndef CEVT_AUTO_I4
#define CEVT_AUTO_I4 (102)
#endif

// �Զ���δ֪����
#ifndef CEVT_UNKNOW
#define CEVT_UNKNOW (0)
#endif

class MzSMSDataBase
{
private:
    CEGUID   m_ceGuid;   //�洢���ݿ��ļ����ʶ��GUID��PCEGUID m_pCeGuid;
    CEOID    m_ceOid;     //DWORD�� �洢���ݿ�����ʶ�ͼ�¼�����ʶ��Ωһ��ʶ���ݿ��ID��CeCreateDatabaseEx()����
    HANDLE   m_hDB;      //�洢���ݿ�����CeOpenDatabaseEx()����
    HANDLE   m_hSession; //
	BOOL bConnected;	//���ݿ��Ƿ�򿪳ɹ�

    WORD	*pFieldType;	//�ֶ����ͣ����任
	UINT	MAX_COL;			//�ֶ�����

    TCHAR   *m_TableName;      //���������ݱ�����
public:
	MzSMSDataBase(const TCHAR *szSmsFileName);
	~MzSMSDataBase(void);
protected:
    BOOL Open(const TCHAR *szSmsFileName);   //�����ݿ�
    BOOL MzSMSDataBase::Close( void );      // �ر����ݿ�
    void SetDataBaseTable(const TCHAR *szTableName); //����Ҫ�����ı���
    WORD GetRecordCount();
public:
	//��ȡ��������
	ULONG GetSmsCount() { 
		if(!bConnected) return 0;//���ݿ�û�гɹ���ʱ����0
		return GetRecordCount(); 
	}
	//��ȡ��������
	BOOL GetSms(DWORD dwRecordID,SmsData_t &sms);
};
