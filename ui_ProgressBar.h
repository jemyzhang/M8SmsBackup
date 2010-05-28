#pragma once
#include "refreshRecord.h"

extern void initProgressBar(HWND parent,LPWSTR title, WORD rmin, WORD rmax);
extern bool updateProgressBar(int nCount);

extern void initUiCallbackRefreshContact(HWND parent = NULL);
extern void initUiCallbackRefreshSms(HWND parent = NULL);
extern void initUiCallbackDeleteSms(HWND parent = NULL);
extern void initUiCallbackUpdateDatabase(HWND parent = NULL);
extern void initUiCallbackExportSms(HWND parent = NULL);
extern void initUiCallbackExportContact(HWND parent = NULL);

extern bool uiCallbackRefreshSIMContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
extern bool uiCallbackRefreshContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
extern bool uiCallbackRefreshSms(SmsData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
extern bool uiCallbackDeleteSms(SmsSimpleData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
extern void uiCallBackUpdateDatabase(LPWSTR title, LPWSTR msg, UINT progress);
extern bool uiCallbackExportSms(SmsSimpleData_ptr psms,WORD nCount,WORD nSize);
extern bool uiCallbackExportContact(ContactData_ptr pcontact,WORD nCount,WORD nSize);
