#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"
#include <simmgr.h>

typedef enum 
{
    Firstname = 0x3a06001f,//姓
    WorkTel  = 0x3a08001f,//工作电话
    HomeTel  = 0x3a09001f,//住宅电话
    Lastname  = 0x3a11001f,//名
    Company   = 0x3a16001f,//公司/单位
    JobTitle = 0x3a17001f,//职务
    Department = 0x3a18001f,//部门
    OfficeLoc = 0x3a19001f,//办公室
    MobileTel=  0x3a1c001f,//移动电话 
    RadioTel =  0x3a1d001f,//无线电话
    CarTel   = 0x3a1e001f,//车载电话
    WorkFax  = 0x3a24001f,//工作传真
    HomeFax  = 0x3a25001f,//住宅传真
    HomeTel2 = 0x3a2f001f ,//住宅电话2
    Birthday  = 0x40010040,//生日
    Assistant = 0x4002001f,//助理
    Anniversary = 0x40030040, //周年纪念
    AssistantTel = 0x4004001f,//助理电话
    Children = 0x4006001f,//子女
    WorkTel2 = 0x4007001f,//工作电话2
    HomePage = 0x4008001f,//主页
    Pager = 0x4009001f,//寻呼机
    Spouse = 0x400a001f,//配偶
    Name = 0x4013001f,//姓名
    Title = 0x4023001f,//称呼(对联系人)
    HomeAddr = 0x4040001f,//家庭地址
    HomeCity = 0x4041001f,//家庭所在城市
    HomeState = 0x4042001f,//家庭所在的省/州
    HomeZip = 0x4043001f,//家庭邮编
    HomeCountry = 0x4044001f,//家庭所在国家
    WorkAddr = 0x4045001f,//工作地址
    WorkCity = 0x4046001f,//工作的城市
    WorkState = 0x4047001f, //工作的省/州
    WorkZip = 0x4048001f,//工作地的邮编
    WorkCountry = 0x4049001f,//工作地的国家
    OtherAddr = 0x404a001f,//其他地址
    OtherCity = 0x404b001f,//其他城市
    OtherState = 0x404c001f,//其他省/州
    OtherZip = 0x404d001f,//其他邮编
    OtherCountry = 0x404e001f,//其他的国家
    Email = 0x4083001f,//电子邮箱
    Email2nd = 0x4093001f,//电子邮箱2
    Email3rd = 0x40a3001f//电子邮箱3
}Contacts_field; //Contacts Database的字段,系统定义不可更改

#define DEFAULT_CONTACT_DB  L"\\Documents and Settings\\default.vol"

class MzContactDataBase
{
private:
    BOOL bConnected;	//数据库是否打开成功
    HANDLE hdDB;        //数据库句柄
    CEGUID ceguid;      //存储数据库文件卷标识（GUID）
    CEOID ceoid;        //DWORD型 存储数据库对象标识和记录对象标识，惟一标识数据库的ID
public:
    MzContactDataBase(void);
    ~MzContactDataBase(void);
public:
    //获取联系人总数
    WORD GetContactCount();
    //获取联系人内容
    BOOL GetContact(WORD dwRecordID,ContactData_t &contact);
protected:
    BOOL Open();
};

class MzSimContactDataBase
{
private:
    BOOL bConnected;	//数据库是否打开成功
    HSIM g_hSim;
	SIMCAPS g_simcaps;
    CEOID ceoid;        //DWORD型 存储数据库对象标识和记录对象标识，惟一标识数据库的ID
public:
	MzSimContactDataBase(void){
		g_hSim = NULL;
		bConnected = Open();
	}
	~MzSimContactDataBase(void){
		if(bConnected){
			SimDeinitialize(g_hSim);
			g_hSim = NULL; 
		}
	}
public:
    //获取短信条数
    ULONG GetContactCount() { 
        if(!bConnected) return 0;//数据库没有成功打开时返回0
        return g_simcaps.dwMaxPBIndex - g_simcaps.dwMinPBIndex+1;
    }
    //获取Sim联系人内容
    BOOL GetContact(DWORD dwRecordID,ContactData_t &contact);
protected:
    BOOL Open();
};
