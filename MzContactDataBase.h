#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"
#include <simmgr.h>

typedef enum 
{
    Firstname = 0x3a06001f,//��
    WorkTel  = 0x3a08001f,//�����绰
    HomeTel  = 0x3a09001f,//סլ�绰
    Lastname  = 0x3a11001f,//��
    Company   = 0x3a16001f,//��˾/��λ
    JobTitle = 0x3a17001f,//ְ��
    Department = 0x3a18001f,//����
    OfficeLoc = 0x3a19001f,//�칫��
    MobileTel=  0x3a1c001f,//�ƶ��绰 
    RadioTel =  0x3a1d001f,//���ߵ绰
    CarTel   = 0x3a1e001f,//���ص绰
    WorkFax  = 0x3a24001f,//��������
    HomeFax  = 0x3a25001f,//סլ����
    HomeTel2 = 0x3a2f001f ,//סլ�绰2
    Birthday  = 0x40010040,//����
    Assistant = 0x4002001f,//����
    Anniversary = 0x40030040, //�������
    AssistantTel = 0x4004001f,//����绰
    Children = 0x4006001f,//��Ů
    WorkTel2 = 0x4007001f,//�����绰2
    HomePage = 0x4008001f,//��ҳ
    Pager = 0x4009001f,//Ѱ����
    Spouse = 0x400a001f,//��ż
    Name = 0x4013001f,//����
    Title = 0x4023001f,//�ƺ�(����ϵ��)
    HomeAddr = 0x4040001f,//��ͥ��ַ
    HomeCity = 0x4041001f,//��ͥ���ڳ���
    HomeState = 0x4042001f,//��ͥ���ڵ�ʡ/��
    HomeZip = 0x4043001f,//��ͥ�ʱ�
    HomeCountry = 0x4044001f,//��ͥ���ڹ���
    WorkAddr = 0x4045001f,//������ַ
    WorkCity = 0x4046001f,//�����ĳ���
    WorkState = 0x4047001f, //������ʡ/��
    WorkZip = 0x4048001f,//�����ص��ʱ�
    WorkCountry = 0x4049001f,//�����صĹ���
    OtherAddr = 0x404a001f,//������ַ
    OtherCity = 0x404b001f,//��������
    OtherState = 0x404c001f,//����ʡ/��
    OtherZip = 0x404d001f,//�����ʱ�
    OtherCountry = 0x404e001f,//�����Ĺ���
    Email = 0x4083001f,//��������
    Email2nd = 0x4093001f,//��������2
    Email3rd = 0x40a3001f//��������3
}Contacts_field; //Contacts Database���ֶ�,ϵͳ���岻�ɸ���

#define DEFAULT_CONTACT_DB  L"\\Documents and Settings\\default.vol"

class MzContactDataBase
{
private:
    BOOL bConnected;	//���ݿ��Ƿ�򿪳ɹ�
    HANDLE hdDB;        //���ݿ���
    CEGUID ceguid;      //�洢���ݿ��ļ����ʶ��GUID��
    CEOID ceoid;        //DWORD�� �洢���ݿ�����ʶ�ͼ�¼�����ʶ��Ωһ��ʶ���ݿ��ID
public:
    MzContactDataBase(void);
    ~MzContactDataBase(void);
public:
    //��ȡ��ϵ������
    WORD GetContactCount();
    //��ȡ��ϵ������
    BOOL GetContact(WORD dwRecordID,ContactData_t &contact);
protected:
    BOOL Open();
};

class MzSimContactDataBase
{
private:
    BOOL bConnected;	//���ݿ��Ƿ�򿪳ɹ�
    HSIM g_hSim;
	SIMCAPS g_simcaps;
    CEOID ceoid;        //DWORD�� �洢���ݿ�����ʶ�ͼ�¼�����ʶ��Ωһ��ʶ���ݿ��ID
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
    //��ȡ��������
    ULONG GetContactCount() { 
        if(!bConnected) return 0;//���ݿ�û�гɹ���ʱ����0
        return g_simcaps.dwMaxPBIndex - g_simcaps.dwMinPBIndex+1;
    }
    //��ȡSim��ϵ������
    BOOL GetContact(DWORD dwRecordID,ContactData_t &contact);
protected:
    BOOL Open();
};
