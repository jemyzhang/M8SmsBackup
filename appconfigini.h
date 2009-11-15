#pragma once

#include <MzConfig.h>

//应用程序配置
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
	//是否启动时更新内容
	MzConfig IniRefreshWhileLoad;
	//是否使用SIM电话簿
	MzConfig IniUseSimPhoneBook;
    //刷新方式: 0: 完全刷新; 1: 快速刷新
    MzConfig IniUpdateMethod;
};
