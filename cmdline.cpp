#include "cmdline.h"
#include "LocalDataBase.h"
#include "appconfigini.h"
#include "ui_ProgressBar.h"
#include "ui_backup.h"
#include <list>

extern LocalDataBase *g_pldb;
extern SmsBackupConfig appconfig;
extern list<SYSTEMTIME> dirtimes;
extern wchar_t g_password[256];
extern int g_password_len ;

void cmdline_backup(int maxBackup){
    if(!Ui_BackupWnd::bbackup()) return;

    if(maxBackup > 0){
        Ui_BackupWnd::getbackuplist();
        if(dirtimes.size() > maxBackup){
            SYSTEMTIME tm = dirtimes.back();
            wchar_t dispname[128];
            wsprintf(dispname,L"%04d-%02d-%02d %02d:%02d:%02d",
                tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
            CMzString f = dispname;
            Ui_BackupWnd::bdelete(f);
        }
    }
}

void cmdline_import(LPWSTR password){
    g_pldb = new LocalDataBase;

    wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};
    if(password == NULL || lstrlen(password) == 0){
	    g_password_len = 6;
	    memcpy(g_password,defaultpwd,sizeof(wchar_t)*6);
    }else{
        g_password_len = lstrlen(password);
        memcpy(g_password,password,sizeof(wchar_t)*g_password_len);
    }

	if(!g_pldb->checkpwd(NULL,0)){
		if(!g_pldb->checkpwd(g_password,g_password_len)){
            delete g_pldb; g_pldb = 0;
            return;
        }
    }

    WORD n;
    if(appconfig.IniUseSimPhoneBook.Get()){
        initUiCallbackRefreshContact();
        n = refreshSIMContact(uiCallbackRefreshSIMContact);
    }
    initUiCallbackRefreshContact();
    n = refreshContact(uiCallbackRefreshContact);
    initUiCallbackRefreshSms();
    n = refreshSms(uiCallbackRefreshSms);
    delete g_pldb; g_pldb = 0;
    return;
}

//命令有效时返回true
bool cmdline_run(LPWSTR param){
    bool bRet = false;
    LPWSTR pParam = param;

    int szParam = lstrlen(pParam);
    if(szParam){
        wchar_t *pParam0 = pParam;
        wchar_t *pParam1 = 0;
        for(int i = 0; i < szParam; i++){
            if(pParam[i] == ' '){
                pParam[i] = '\0';
                pParam1 = pParam + i + 1;
                break;
            }
        }
        if(wcscmp(pParam0,L"-backup") == 0){  //自动备份
            int maxBackup = 0;
            if(pParam1 != NULL && lstrlen(pParam1) > 0){
                maxBackup = _wtoi(pParam1);
                cmdline_backup(maxBackup);
            }
            bRet = true;
        }else if(wcscmp(pParam0,L"-import") == 0){    //自动导入
            wchar_t *pPassword = NULL;
            if(pParam1 != NULL && lstrlen(pParam1) > 0){
                pPassword = pParam1;
            }
            cmdline_import(pPassword);
            bRet = true;
        }
    }
    return bRet;
}
