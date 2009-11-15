#include "MzContactDataBase.h"
#include <MzCommon.h>
using namespace MzCommon;

#pragma comment(lib,"cellcore.lib")

MzContactDataBase::MzContactDataBase(void)
{
    hdDB = 0;
    ceoid = 0;
    bConnected = Open();
}

MzContactDataBase::~MzContactDataBase(void)
{
    if(bConnected){
        CloseHandle(hdDB);
    }
}

BOOL MzContactDataBase::GetContact(WORD dwRecordID,ContactData_t &contact){
    BOOL nRet = false;
    if(bConnected){
        WORD iCount = GetContactCount();
        if(dwRecordID < iCount){
            DWORD dwIndex;

            PBYTE pBuff;
            WORD wProps;
            DWORD dwRecSize;
            PCEPROPVAL pRecord;
            CEOID CEoid =0;
            //移动记录指针
            CEoid = CeSeekDatabase(hdDB,CEDB_SEEK_BEGINNING,dwRecordID,&dwIndex);

            //ASSERT(CEoid !=0);
            pBuff = 0; 
            //读取所有字段值
            CEoid = CeReadRecordProps(hdDB,CEDB_ALLOWREALLOC,&wProps,NULL,&(LPBYTE)pBuff,&dwRecSize);

            //ASSERT(CEoid !=0);

            pRecord = (PCEPROPVAL)pBuff;

            for(int j = 0; j < wProps; ++j)
            {
                switch(pRecord->propid)
                {
                case MobileTel ://移动电话
                    {
                        LPWSTR Number = NULL;
                        C::newstrcpy(&Number,pRecord->val.lpwstr);
                        contact.MobileTels.push_back(Number);
                    }
                    break;
				case RadioTel://也是移动电话？
                    {
                        LPWSTR Number = NULL;
                        C::newstrcpy(&Number,pRecord->val.lpwstr);
                        contact.MobileTels.push_back(Number);
                    }
                    break;
                case WorkTel:
                    {
                        LPWSTR Number = NULL;
                        C::newstrcpy(&Number,pRecord->val.lpwstr);
                        contact.WorkTels.push_back(Number);
                    }
                    break;
                case HomeTel:
                    {
                        LPWSTR Number = NULL;
                        C::newstrcpy(&Number,pRecord->val.lpwstr);
                        contact.HomeTels.push_back(Number);
                    }
                    break;
                case HomeTel2:
                    {
                        LPWSTR Number = NULL;
                        C::newstrcpy(&Number,pRecord->val.lpwstr);
                        contact.HomeTel2s.push_back(Number);
                    }
                    break;
                case Name://姓名
                    {
                        C::newstrcpy(&contact.Name,pRecord->val.lpwstr);
                    }
					break;
                case Firstname://名
                    {
                        C::newstrcpy(&contact.FirstName ,pRecord->val.lpwstr);
                    }
					break;
                case Lastname://姓
                    {
                        C::newstrcpy(&contact.LastName ,pRecord->val.lpwstr);
                    }
					break;
                default:
                    break;
                }
                pRecord ++;
            }
            LocalFree(pBuff);
        }
    }
    return nRet;
}

//获取联系人总数
WORD MzContactDataBase::GetContactCount() {
    WORD iCount = 0;
    if(bConnected){
        CEOIDINFO oldinfo;
        if(CeOidGetInfoEx(&ceguid,ceoid,&oldinfo))
        {
            iCount = oldinfo.infDatabase.wNumRecords;
        }
    }
    //数据库没有成功打开时返回0
    return iCount; 
}

BOOL MzContactDataBase::Open(){
    ///////////////////////////////////////////////////////////////////////////
    //读数据库中(在内存中读写,只能用API不能用WINCE封装的类,后者只对用户自定义的数据库有用)
    //存储地址本的数据库是Contacts Database,手机中看不到,加载在内存中
    //http://www.cppblog.com/zhangyq/archive/2009/04/16/80200.html
    //////////////////////////////////////////////////////////////////////////
    BOOL nRet = false;
    TCHAR szVolName[128];
    CREATE_INVALIDGUID(&ceguid);
    while(::CeEnumDBVolumes(&ceguid,szVolName,MAX_PATH))
    {
        if(wcsnicmp(DEFAULT_CONTACT_DB,szVolName,lstrlen(szVolName))==0)
        {
            /////将当前数据库卷定位到default.vol上
            break;
        }
    }

    CENOTIFYREQUEST *pRequest = (CENOTIFYREQUEST *) LocalAlloc(LPTR,sizeof(CENOTIFYREQUEST));
    pRequest->dwSize = sizeof(CENOTIFYREQUEST);
    pRequest->hwnd = NULL;
    pRequest->hHeap = NULL; 
    pRequest->dwFlags = CEDB_EXNOTIFICATION;

    //打开联系人数据库,并注册事件
    hdDB = ::CeOpenDatabaseEx(&ceguid,&ceoid,L"Contacts Database",0,0,pRequest);

    if(hdDB != INVALID_HANDLE_VALUE)
    {
        nRet = true;
    }
    return nRet;
}
////////////////////////////////////////////////////////////
//Sim联系人读取
////////////////////////////////////////////////////////////
BOOL MzSimContactDataBase::Open(){
    if (FAILED(SimInitialize(SIM_INIT_NONE, NULL, 0, &g_hSim)))
        return FALSE;
    if (FAILED(SimGetDevCaps(g_hSim, SIM_CAPSTYPE_PBINDEXRANGE, &g_simcaps)))
        return FALSE;
	return TRUE;
}
BOOL MzSimContactDataBase::GetContact(DWORD dwCurRec,ContactData_t &contact)
{
	BOOL nRet = FALSE;
	if(!bConnected) return nRet;
	//Sim Card
	const DWORD g_dwLocation = SIM_PBSTORAGE_SIM;//SIM_PBSTORAGE_OWNNUMBERS;//SIM_PBSTORAGE_SIM;

	DWORD dwRecNum = g_simcaps.dwMaxPBIndex - g_simcaps.dwMinPBIndex+1;
	dwCurRec += g_simcaps.dwMinPBIndex;
	if(dwCurRec > g_simcaps.dwMaxPBIndex) return nRet;


	SIMPHONEBOOKENTRY simPhoneEntry;  
	memset(&simPhoneEntry,0,sizeof(SIMPHONEBOOKENTRY));
	if(SUCCEEDED(SimReadPhonebookEntry(g_hSim, g_dwLocation, dwCurRec, &simPhoneEntry)))
	{
		LPWSTR Mobile = NULL;
		LPWSTR NAME = NULL;
		Mobile = simPhoneEntry.lpszText;
		NAME = simPhoneEntry.lpszAddress;
		LPWSTR Number = NULL;
		C::newstrcpy(&Number,simPhoneEntry.lpszAddress);
		contact.MobileTels.push_back(Number);
		C::newstrcpy(&contact.Name,simPhoneEntry.lpszText);
		nRet = TRUE;
	} 
	return nRet;
}
