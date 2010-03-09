#include "logout.h"

#define DISABLE_LOG 1

FILE* BeginLogout(const wchar_t* f){
    return _wfopen(f,L"a+");
}

void EndLogout(FILE* h){
    if(h == NULL) return;
    fclose(h);
}

FILE* flog = NULL;

bool logout(const wchar_t* _Format,...){
#if DISABLE_LOG
    return true;
#endif
    if(flog == NULL){
        flog = BeginLogout(L"\\Disk\\smsbackup_log.txt");
    }
    if(flog == NULL) return false;
    //获取log时间
    SYSTEMTIME tm;
    GetLocalTime(&tm);

    //记录时间
    fwprintf(flog,
        L"[%04d-%02d-%02d %02d:%02d:%02d] ",
        tm.wYear,tm.wMonth,tm.wDay,
        tm.wHour,tm.wMinute,tm.wSecond);

    wchar_t buf[1024];
    va_list ap;
    va_start(ap, _Format);
    vswprintf(buf,_Format,ap);
    fwprintf(flog,buf);
    va_end(ap);

    fwprintf(flog,L"\n");

    EndLogout(flog);
    flog = NULL;
    return true;
}

FILE* f_dblog = NULL;

bool db_logout(const char* _Format,...){
#if DISABLE_LOG
    return true;
#endif
    if(f_dblog == NULL){
        f_dblog = fopen("\\Disk\\smsbackup_db_log.txt","a+");
    }
    if(f_dblog == NULL) return false;
    //获取log时间
    SYSTEMTIME tm;
    GetLocalTime(&tm);

    //记录时间
    fprintf(f_dblog,
        "[%04d-%02d-%02d %02d:%02d:%02d] ",
        tm.wYear,tm.wMonth,tm.wDay,
        tm.wHour,tm.wMinute,tm.wSecond);

    char buf[1024];
    va_list ap;
    va_start(ap, _Format);
    vsprintf(buf,_Format,ap);
    fprintf(f_dblog,buf);
    va_end(ap);

    fprintf(f_dblog,"\n");

    fclose(f_dblog);
    f_dblog = NULL;
    return true;
}
