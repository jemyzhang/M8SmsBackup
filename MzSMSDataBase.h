#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

#ifndef EDB
#define EDB 1
#endif

// EDB 专用类型，来自 windbase.h
#ifndef CEVT_AUTO_I4
#define CEVT_AUTO_I4 (102)
#endif

// 自定义未知类型
#ifndef CEVT_UNKNOW
#define CEVT_UNKNOW (0)
#endif

class MzSMSDataBase
{
private:
    CEGUID   m_ceGuid;   //存储数据库文件卷标识（GUID）PCEGUID m_pCeGuid;
    CEOID    m_ceOid;     //DWORD型 存储数据库对象标识和记录对象标识，惟一标识数据库的ID，CeCreateDatabaseEx()返回
    HANDLE   m_hDB;      //存储数据库句柄，CeOpenDatabaseEx()返回
    HANDLE   m_hSession; //
	BOOL bConnected;	//数据库是否打开成功

    WORD	*pFieldType;	//字段类型，灵活变换
	UINT	MAX_COL;			//字段数量

    TCHAR   *m_TableName;      //操作的数据表名称
public:
	MzSMSDataBase(const TCHAR *szSmsFileName);
	~MzSMSDataBase(void);
protected:
    BOOL Open(const TCHAR *szSmsFileName);   //打开数据库
    BOOL MzSMSDataBase::Close( void );      // 关闭数据库
    void SetDataBaseTable(const TCHAR *szTableName); //设置要操作的表名
    WORD GetRecordCount();
public:
	//获取短信条数
	ULONG GetSmsCount() { 
		if(!bConnected) return 0;//数据库没有成功打开时返回0
		return GetRecordCount(); 
	}
	//获取短信内容
	BOOL GetSms(DWORD dwRecordID,SmsData_t &sms);
};
