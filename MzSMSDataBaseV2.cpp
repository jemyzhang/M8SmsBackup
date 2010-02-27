#include "MzSMSDataBaseV2.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#define SMS_TABLE_NAME	L"sms"

MzSMSDataBaseV2::MzSMSDataBaseV2(const TCHAR *szSmsFileName)
{
	Open(szSmsFileName);
}

MzSMSDataBaseV2::~MzSMSDataBaseV2(void)
{
	Close();
}

//////////////////////////////////////////////////////////////////////////

BOOL MzSMSDataBaseV2::Close( void )
{
	BOOL bRet = TRUE;

	TRY{
		sqlconn.close();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = FALSE;
	}
	
	return bRet; 
}

//////////////////////////////////////////////////////////////////////////
BOOL MzSMSDataBaseV2::Open( const TCHAR *szSmsFileName )
{
	BOOL bRet = TRUE;

	TRY{
		sqlconn.open(szSmsFileName);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = FALSE;
	}
	
	return bRet; 
}

//////////////////////////////////////////////////////////////////////////
WORD MzSMSDataBaseV2::GetRecordCount()
{
    WORD nRet = 0;
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select count(*) from '"
		SMS_TABLE_NAME
		L"'");
		nRet = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
		nRet = 0;
	}

    return nRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL MzSMSDataBaseV2::GetSms(DWORD dwRecordID,SmsData_t &sms){
    BOOL bRet = TRUE;
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select address,date,status,msg_type,type,body from '"
		SMS_TABLE_NAME
		L"' limit 1 offset ?");
		cmd.bind(1,(int)dwRecordID);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&sms.MobileNumber,reader.getstring16(0).c_str());
			int szMobileNumber = lstrlen(sms.MobileNumber);
			if(szMobileNumber > 11){
				C::newstrcpy(&sms.PNSort,&sms.MobileNumber[szMobileNumber - 11]);
			}else{
				C::newstrcpy(&sms.PNSort,sms.MobileNumber);
			}
			DateTime::TimetToLocalTime(static_cast<time_t>(reader.getint(1)),&sms.TimeStamp);
			sms.SmsStatus = reader.getint(2);
			sms.SmsType = reader.getint(3);
			sms.SendReceiveFlag = (reader.getint(4) == 2);
			C::newstrcpy(&sms.Content,reader.getstring16(5).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = FALSE;
	}

    return bRet;
}