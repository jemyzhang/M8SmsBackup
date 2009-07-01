#include "MzSMSDataBase.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include <winbase.h>
extern "C"
{
#include <Windbase_edb.h>
};

#define SMSDB_TABLE	L"smsDB"
//�ֶ����ͣ���˳��
static WORD SmsFieldType[16]=
{
    CEVT_LPWSTR,    //0
    CEVT_UI4,       //1
    CEVT_BOOL,      //2
    CEVT_UI4,       //3
    CEVT_FILETIME,  //4
    CEVT_LPWSTR,    //5
    CEVT_FILETIME,  //6
    CEVT_LPWSTR,    //7
    CEVT_LPWSTR,    //8
    CEVT_AUTO_I4,   //9
    CEVT_UNKNOW,
    CEVT_UNKNOW,
    CEVT_UNKNOW,
    CEVT_UNKNOW
};

MzSMSDataBase::MzSMSDataBase(const TCHAR *szSmsFileName)
{
    pFieldType = SmsFieldType;
    MAX_COL = 10;
    m_TableName = 0;
    m_hDB=INVALID_HANDLE_VALUE;
    m_ceOid = NULL;
    m_hSession=NULL;
    CREATE_SYSTEMGUID(&this->m_ceGuid);
	SetDataBaseTable(SMSDB_TABLE);
	bConnected = Open(szSmsFileName);
}

MzSMSDataBase::~MzSMSDataBase(void)
{
    SAFE_RELEASE_LPWSTR(m_TableName);
	if(bConnected){
		Close();
	}
}

//////////////////////////////////////////////////////////////////////////
void MzSMSDataBase::SetDataBaseTable( const TCHAR *szTableName )
{
    C::newstrcpy(&m_TableName,szTableName);
}

BOOL MzSMSDataBase::Close( void )
{
    //1���ر����ݿ�
    if (!CloseHandle(m_hDB))
    {
        return FALSE;
    }
    //2�������ݿ������ݻ��嵽���ô洢������
    if (!CeFlushDBVol(&m_ceGuid))
    {
        return FALSE;
    }
    //3��ж�����ݿ��
    if (!CeUnmountDBVol(&m_ceGuid))
    {
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL MzSMSDataBase::Open( const TCHAR *szSmsFileName )
{
    //�������ݿ�����������򿪣������ڣ����½�һ��
    if (!CeMountDBVolEx(&m_ceGuid,(LPWSTR)szSmsFileName,NULL,OPEN_EXISTING))
    {
        return FALSE;
    }

    //�����ݿ�
    m_hDB = CeOpenDatabaseInSession(NULL,&m_ceGuid,&m_ceOid,m_TableName,NULL,CEDB_AUTOINCREMENT,NULL);
    if (m_hDB == INVALID_HANDLE_VALUE)
    {
        //������ݿⲻ����
        CeUnmountDBVol(&m_ceGuid);
        return FALSE;
    }
    return TRUE; 
}

//////////////////////////////////////////////////////////////////////////
WORD MzSMSDataBase::GetRecordCount()
{
    DWORD iCount=0;

    CEOID         oid=0,ceoidFindRecord = 0;
    WORD          dwColCount = 0;
    DWORD         dwSizeOfBuffer = 1024;
    LPBYTE        lpRecordBuffer = NULL; 
    PCEPROPVAL    pCePropVal = NULL;
    
    //��λ��¼
    // Seek( 0 );
    oid = CeSeekDatabaseEx(m_hDB, CEDB_SEEK_BEGINNING, 0, 0, NULL);
    if(oid==NULL)
    {
        return 0;
    }

    while(ceoidFindRecord = CeReadRecordPropsEx(m_hDB, CEDB_ALLOWREALLOC, 
        &dwColCount, NULL, &lpRecordBuffer, &dwSizeOfBuffer, NULL))
    {
        if(GetLastError() != ERROR_NO_MORE_ITEMS)
        { 
            iCount++;
        }
    }
    //�ͷ���Դ
    if(lpRecordBuffer)
    {
        LocalFree(lpRecordBuffer);
        lpRecordBuffer = NULL;
    }
    return (int)iCount;
}

//////////////////////////////////////////////////////////////////////////
BOOL MzSMSDataBase::GetSms(DWORD dwRecordID,SmsData_t &sms){
    CEOID         oid=0,ceoidFindRecord = 0;
    WORD          dwColCount = 0;
    DWORD         dwSizeOfBuffer = 1024;
    LPBYTE        lpRecordBuffer = NULL; 
    PCEPROPVAL    pCePropVal = NULL;
    DWORD dwFieldsCount;

    //��λ��¼
    oid = CeSeekDatabaseEx(m_hDB, CEDB_SEEK_BEGINNING, dwRecordID, 0, NULL);
    if(oid==NULL)
    {
        return FALSE;
    }
    //��ȡ����
    ceoidFindRecord = CeReadRecordPropsEx(m_hDB, CEDB_ALLOWREALLOC, 
        &dwColCount, NULL, &lpRecordBuffer, &dwSizeOfBuffer, NULL);
    if(ceoidFindRecord==NULL)
    {
        return FALSE;
    }
    dwFieldsCount=dwColCount; //ͬʱ�����ֶθ���

    if(GetLastError() != ERROR_NO_MORE_ITEMS)
    {
        pCePropVal = (PCEPROPVAL)lpRecordBuffer;
        for(int  i=0;i<dwColCount;i++)
        {
            //���propid�����⣺����32λ����������16λΪ�Զ����ID����16λΪ�ֶ����ͣ�
            //ȡ��16λ�жϼ��ɡ�
            WORD dwFieldType=(WORD)(pCePropVal[i].propid&0x0000ffff);
            //��ȡ���ݵ�bufffer
            switch(dwFieldType)
            {
            case CEVT_LPWSTR:
                if(i == 0){
                    C::newstrcpy(&sms.MobileNumber,pCePropVal[i].val.lpwstr);
                }
                if(i == 5){
                    C::newstrcpy(&sms.Content,pCePropVal[i].val.lpwstr);
                }
                if(i == 7){
                    C::newstrcpy(&sms.ExtraInfo,pCePropVal[i].val.lpwstr);
                }
                if(i == 8){
                    C::newstrcpy(&sms.PNSort,pCePropVal[i].val.lpwstr);
                }
                break;
            case CEVT_FILETIME:
                {
                    FILETIME ft=pCePropVal[i].val.filetime;
                    SYSTEMTIME st;
                    ::FileTimeToSystemTime(&ft,&st);
                    if(i == 4){
                        sms.TimeStamp = st;
                    }
                    if(i == 6){
                        sms.NetTimeStamp = st;
                    }
                }break;
            case CEVT_BOOL:
                if(i == 2){
                    sms.SendReceiveFlag = pCePropVal[i].val.boolVal;
                }
                break;
            case CEVT_UI4:
                if( i == 1){
                    sms.SmsStatus = pCePropVal[i].val.ulVal;
                }
                if( i == 3){
                    sms.SmsType = pCePropVal[i].val.ulVal;
                }
                break;
            case CEVT_I2:
                break;
            case CEVT_UI2:
                break;
            case CEVT_I4:
                break;
            case CEVT_AUTO_I4://������
                if(i == 9){
                    sms.AutoIndex = pCePropVal[i].val.ulVal;
                }
                break;
            default:
                break;
            }//switch
        }//for
    } //if
    else
    {
        return FALSE;
    }
    //�ͷ���Դ
    if(lpRecordBuffer)
    {
        LocalFree(lpRecordBuffer);
        lpRecordBuffer = NULL;
    }
    return TRUE;
}