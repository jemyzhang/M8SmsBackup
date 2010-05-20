#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "LocalDataBase.h"

#define MZ_WM_UILISTEX_VISIBLE_NOTIFY 0xFFF2000 + 0x10 

class UiContactFiltList : public UiListEx {
public:
	UiContactFiltList() { 
        pldb = NULL;
        SetItemHeight(65);
        SetItemAttribute(UILISTEX_ITEMTYPE_BASE);
        EnableNotifyMessage(true);
        EnableScrollBarV(true);
    }
    void SetFilterKeyword(const wchar_t * key){
        scontent = key;
        SetVisible(refresh());
        ScrollTo(UI_SCROLLTO_TOP,0,false);
    }
    void SetDB(LocalDataBase *pl){
        pldb = pl;
    }
	//获取列表短信内容
	LPCTSTR GetSelectName();
    
    virtual void SetVisible(bool bVisible=true){
        SendMessage(MZ_WM_UILISTEX_VISIBLE_NOTIFY,
            GetID(),bVisible);
        UiListEx::SetVisible(bVisible);
    }
private:
	bool refresh();
private:
    wstring scontent;
    LocalDataBase *pldb;
};

