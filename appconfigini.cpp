/*
* @filename appconfigini.h
* @note 应用程序配置文件读写类
* @author JEMYZHANG
* @date 2009.6.20
* @ver. 1.1
* @changelog
*  ver.1.0 初始化程序
*  ver.1.1 创建MzConfig类，简化键值读写
*/

#include "appconfigini.h"

#include "ReadWriteIni.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#ifdef _DEBUG
#define CONFIG_INI L"Program Files\\M8SmsBackup\\m8smsbackup.ini"
#else
#define CONFIG_INI L"\\Disk\\Programs\\M8SmsBackup\\m8smsbackup.ini"
#endif

DWORD MzConfig::Get(){
	DWORD val = _defaultValue;
	if(checkKey()){
		if(!IniReadInt(_mainKey,_subKey,&val,gs_inifile)){
			Set(val);
		}
	}
	return val;
}
void MzConfig::Set(DWORD val){
	if(!checkKey()) return;
	IniWriteInt(_mainKey,_subKey,val,gs_inifile);
}

///////////////////////////////////////////

void AppConfigIni::SetupIniPath(){
	//setup path
	wchar_t currpath[128];
	if(File::GetCurrentPath(currpath)){
		wsprintf(ini_path,L"%s\\m8smsbackup.ini",currpath);
	}else{
		wsprintf(ini_path,CONFIG_INI);
	}
	gs_inifile = ini_path;
}

void AppConfigIni::CreateIni(){
	if(!File::FileExists(ini_path)){
		IniCreateFile(ini_path);
	}
}

void AppConfigIni::InitIniKey(){
	IniRefreshWhileLoad.InitKey(L"Global",L"RefreshWhileLoad");
	IniUseSimPhoneBook.InitKey(L"Global",L"UseSimPhoneBook");
    IniUpdateMethod.InitKey(L"Global",L"UpdateMethod");
}