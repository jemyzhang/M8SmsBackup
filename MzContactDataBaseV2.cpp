#include "MzContactDataBaseV2.h"
#include <MzCommon.h>
using namespace MzCommon;

#define CONTACT_TABLE_NAME	L"ABPerson"
#define CONTACT_TABLE_PHONE	L"ABPhones"

MzContactDataBaseV2::MzContactDataBaseV2(const TCHAR *szContactFileName)
{
	Open(szContactFileName);
}

MzContactDataBaseV2::~MzContactDataBaseV2(void)
{
	Close();
}

//////////////////////////////////////////////////////////////////////////

BOOL MzContactDataBaseV2::Close( void )
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
BOOL MzContactDataBaseV2::Open( const TCHAR *szContactFileName )
{
	BOOL bRet = TRUE;

	TRY{
		sqlconn.open(szContactFileName);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = FALSE;
	}
	
	return bRet; 
}

//////////////////////////////////////////////////////////////////////////
WORD MzContactDataBaseV2::GetRecordCount()
{
    WORD nRet = 0;
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select count(*) from '"
		CONTACT_TABLE_PHONE
		L"'");
		nRet = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
		nRet = 0;
	}

    return nRet;
}

//////////////////////////////////////////////////////////////////////////
BOOL MzContactDataBaseV2::GetContact(WORD dwRecordID,ContactData_t &contact){
    BOOL bRet = TRUE;
	TRY{
		sqlite3_command cmd(this->sqlconn,
		L"select b.name,a.label,a.number,b.first,b.last from '"
		CONTACT_TABLE_PHONE
		L"' as a,'"
		CONTACT_TABLE_NAME
		L"' as b where a.record_id=b.rowid"
		L" limit 1 offset ?");

		cmd.bind(1,(int)dwRecordID);

		sqlite3_reader reader=cmd.executereader();
		while(reader.read()){
			C::newstrcpy(&contact.Name,reader.getstring16(0).c_str());
			int type = reader.getint(1);

			LPWSTR Number = NULL;
            C::newstrcpy(&Number,reader.getstring16(2).c_str());
			switch(type){
				case 0:
					contact.MobileTels.push_back(Number);
					break;
				case 1:
					contact.HomeTels.push_back(Number);
					break;
				case 2:
					contact.WorkTels.push_back(Number);
					break;
				default:
					contact.HomeTel2s.push_back(Number);
					break;
			}
			C::newstrcpy(&contact.FirstName,reader.getstring16(3).c_str());
			C::newstrcpy(&contact.LastName,reader.getstring16(4).c_str());
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = FALSE;
	}

    return bRet;
}