#pragma once
#include "refreshRecord.h"

void initProgressBar(LPWSTR title, WORD rmin, WORD rmax);
bool updateProgressBar(int nCount);

void initUiCallbackRefreshContact();
void initUiCallbackRefreshSms();
void initUiCallbackDeleteSms();
void initUiCallbackUpdateDatabase();

bool uiCallbackRefreshSIMContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackRefreshContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackRefreshSms(SmsData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackDeleteSms(SmsSimpleData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
void uiCallBackUpdateDatabase(LPWSTR title, LPWSTR msg, UINT progress);