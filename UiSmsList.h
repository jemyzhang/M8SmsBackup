#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"
#include "ui_SmsViewer.h"

class UiSmsList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	UiSmsList() { 
        syear = 0; smonth = 0; sday = 0; 
        sname = NULL; scontent = NULL;
        plist_record = NULL; plist_size = 0;
        smode = 0;
        pldb = NULL;
        seletedidx = -1;
		bSelectionMode = false;
    }
    ~UiSmsList(){
        ClearList();
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
        ClearList();
        SetupList();
    }
    void SetSelectedIndex(int index);
	void SetSelectionMode(){
		bSelectionMode = !bSelectionMode;
		for(UINT i = 0; i < plist_size; i++){
			plist_record[i].SelectionFlag = FALSE;
		}
	}
	bool GetSelectionMode(){
		return bSelectionMode;
	}

	void ReverseSelect(){
		for(UINT i = 0; i < plist_size; i++){
			plist_record[i].SelectionFlag = !plist_record[i].SelectionFlag;
		}
	}
	//获取已选中条目个数
	UINT GetSelectedCount(){
		UINT nCount = 0;
		for(UINT i = 0; i < plist_size; i++){
			if(plist_record[i].SelectionFlag) nCount++;
		}
		return nCount;
	}
	void DeleteSelectedItems();
	//获取列表短信内容
	SmsSimpleData_ptr GetSmsItem(int nIndex){
		if(plist_record == NULL || nIndex >= plist_size){
			return NULL;
		}
		return (plist_record+nIndex);
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
	SmsSimpleData_ptr plist_record;	//缓存
    UINT plist_size;
    UINT smode; //0: datetime, 1: name, 2: content
    LocalDataBase *pldb;
    int seletedidx;
	bool bSelectionMode;	//是否为选择模式
};

