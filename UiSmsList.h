#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

class UiSmsList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	UiSmsList() { 
        syear = 0; smonth = 0; sday = 0; 
        sname = NULL; scontent = NULL;
        plist_record = NULL; plist_size = 0;
        smode = 0;
        pldb = NULL;
        breq = false;
    }
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void SetupMode(UINT mode) {
        smode = mode;
	}
    void SetupListDateTime(WORD y,WORD m = 0, WORD d = 0){
        syear = y; smonth = m; sday = d;
    }
    void SetupListName(const wchar_t * ContactName){
        sname = ContactName;
    }
    void SetupListContent(const wchar_t * Content){
        scontent = Content;
    }
    void SetupDB(LocalDataBase *pl){
        pldb = pl;
    }
    void reqUpdate(){
        breq = true;
    }
	/// update the control
    virtual void Update(){
        if(breq){
            breq = false;
            ClearList();
            SetupList();
        }
        UiList::Update();
    }
private:
	void SetupList();
	void ClearList();
private:
	WORD syear;
    WORD smonth;
    WORD sday;
    CMzString sname;
    CMzString scontent;
	SmsSimpleData_ptr plist_record;	//»º´æ
    UINT plist_size;
    UINT smode; //0: datetime, 1: name, 2: content
    LocalDataBase *pldb;
    bool breq;
};

