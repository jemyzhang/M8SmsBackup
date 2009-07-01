#include "MzContactDataBase.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include <simmgr.h>
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

#if 0
void MzSimContactDataBase::ReadSimPhoneBook()
{

    //Sim Card
    HSIM g_hSim = NULL;
    SIMCAPS g_simcaps;
    const DWORD g_dwLocation = SIM_PBSTORAGE_SIM;//SIM_PBSTORAGE_OWNNUMBERS;//SIM_PBSTORAGE_SIM;

    DWORD dwRecNum=0;
    DWORD dwCurRec = 0;
    if (FAILED(SimInitialize(SIM_INIT_NONE, NULL, 0, &g_hSim)))
        return;
    if (FAILED(SimGetDevCaps(g_hSim, SIM_CAPSTYPE_PBINDEXRANGE, &g_simcaps)))
        return;

    dwRecNum = g_simcaps.dwMaxPBIndex - g_simcaps.dwMinPBIndex+1;
    dwCurRec = g_simcaps.dwMinPBIndex;
    SIMPHONEBOOKENTRY simPhoneEntry;  
    while( dwCurRec <= dwRecNum )
    {
        memset(&simPhoneEntry,0,sizeof(SIMPHONEBOOKENTRY));
        if(SUCCEEDED(SimReadPhonebookEntry(g_hSim, g_dwLocation, dwCurRec, &simPhoneEntry)))
        {
            LPWSTR Mobile = NULL;
            LPWSTR NAME = NULL;
            Mobile = simPhoneEntry.lpszText;
            NAME = simPhoneEntry.lpszAddress;
            /*
            int iItem = lstCtrl.InsertItem(lstCtrl.GetItemCount(),simPhoneEntry.lpszText);
            lstCtrl.SetItemText(iItem,1,simPhoneEntry.lpszAddress);
            CString tmp;
            tmp.Format(_T("%ld"),dwCurRec);
            lstCtrl.SetItemText(iItem,2,tmp);
            */
        } 
        dwCurRec++;
    }

    SimDeinitialize(g_hSim);
    g_hSim = NULL; 
}
#endif