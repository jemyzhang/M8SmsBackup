#pragma once

#include <MzConfig.h>

//Ӧ�ó�������
class SmsBackupConfig : public AppConfigIni
{
public:
    SmsBackupConfig():
      AppConfigIni(L"SmsBackup.ini"){
		InitIniKey();
	}
protected:
	void InitIniKey();
public:
	//�Ƿ�����ʱ��������
	MzConfig IniRefreshWhileLoad;
	//�Ƿ�ʹ��SIM�绰��
	MzConfig IniUseSimPhoneBook;
    //ˢ�·�ʽ: 0: ��ȫˢ��; 1: ����ˢ��
    MzConfig IniUpdateMethod;
};
