#pragma once

#include "MzSMSDataBase.h"
#include "MzContactDataBase.h"

typedef bool (*CallBackRefreshContact)(ContactData_ptr,WORD nCount,WORD nSize);
typedef bool (*CallBackRefreshSms)(SmsData_ptr,WORD nCount,WORD nSize);

WORD refreshContact(CallBackRefreshContact = NULL);
WORD refreshSms(CallBackRefreshSms = NULL);