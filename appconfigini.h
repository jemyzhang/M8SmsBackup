#pragma once

#include <cMzConfig.h>

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
	cMzConfig IniRefreshWhileLoad;
	//�Ƿ�ʹ��SIM�绰��
	cMzConfig IniUseSimPhoneBook;
    //ˢ�·�ʽ: 0: ��ȫˢ��; 1: ����ˢ��
    cMzConfig IniUpdateMethod;
};
