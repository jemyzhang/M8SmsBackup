#pragma once
#include "refreshRecord.h"

void initUiCallbackRefreshContact();
void initUiCallbackRefreshSms();

bool uiCallbackRefreshContact(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
bool uiCallbackRefreshSms(SmsData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
