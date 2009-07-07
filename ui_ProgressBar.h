#pragma once
#include "refreshRecord.h"

void initUiCallbackRefreshContact();
void initUiCallbackRefreshSms();
void initUiCallbackDeleteSms();

bool uiCallbackRefreshSIMContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackRefreshContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackRefreshSms(SmsData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackDeleteSms(SmsSimpleData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
