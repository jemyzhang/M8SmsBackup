#include "appconfigini.h"

void SmsBackupConfig::InitIniKey(){
	IniRefreshWhileLoad.InitKey(L"Global",L"RefreshWhileLoad");
	IniUseSimPhoneBook.InitKey(L"Global",L"UseSimPhoneBook");
    IniUpdateMethod.InitKey(L"Global",L"UpdateMethod");
}