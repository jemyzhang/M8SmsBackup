#pragma once
#include "refreshRecord.h"

void initUiCallbackRefreshContact();
void initUiCallbackRefreshSms();

bool uiCallbackRefreshContact(ContactData_ptr,WORD nCount,WORD nSize);
bool uiCallbackRefreshSms(SmsData_ptr,WORD nCount,WORD nSize);
