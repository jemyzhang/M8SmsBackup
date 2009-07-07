#pragma once

#include "MzSMSDataBase.h"
#include "MzContactDataBase.h"

typedef bool (*CallBackRefreshSIMContact)(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
typedef bool (*CallBackRefreshContact)(ContactData_ptr,WORD nCount,WORD nSize,WORD nSuccess);
typedef bool (*CallBackRefreshSms)(SmsData_ptr,WORD nCount,WORD nSize,WORD nSuccess);

WORD refreshContact(CallBackRefreshContact = NULL);
WORD refreshSms(CallBackRefreshSms = NULL);
WORD refreshSIMContact(CallBackRefreshSIMContact = NULL);