#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiNaviList.h"
#include "LocalDataBase.h"
#include "UiSmsList.h"

class UiKeyList : public UiList {
public:
	UiKeyList(){
		plistkey = 0; plist_size = 0; smode = 0;
		syear = 0; smonth = 0;
		pldb = 0;
        EnableScrollBarV(true);
	}
    // override the DrawItem member function to do your own drawing of the list
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
public:
	void SetListMode(UINT mode) {
        smode = mode;
	}
	void SetListYear(WORD year){
		syear = year;
	}
	void SetListMonth(WORD year, WORD month){
		syear = year;
		smonth = month;
	}
	void reqUpdate(){
        ClearList();
        SetupList();
 	}
    void SetupDB(LocalDataBase *pl){
        pldb = pl;
    }
	SmsViewListKey_ptr GetListItem(int nIndex){
		if(plistkey == NULL || nIndex >= plist_size){
			return NULL;
		}
		return (plistkey+nIndex);
	}
protected:
	void ClearList();
	void SetupList();
private:
    SmsViewListKey_ptr plistkey;
    UINT plist_size;
	UINT smode; //0: main 1: contact 2: year 3: month 4: day 
	UINT syear;	//��ѯʱ��Ҫyear
	UINT smonth;
	LocalDataBase *pldb;
};

// Main window derived from CMzWndEx
class Ui_ViewWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ViewWnd);
private:
    //0: main
    //1: contact list 2: phone number list 3: sms list
    //0x10: year list 0x11: month list, 0x12 day list, 0x13 sms list
    UINT viewStatus;
	UINT viewStatusSavedBeforeView;	//�鿴��ť����ʱ����ǰһֵ
    WORD selectedYear;
    WORD selectedMonth;
    WORD selectedDay;
public:
	Ui_ViewWnd();
	~Ui_ViewWnd();
    void OnTimer(UINT nIDEvent);
public:
    UiToolbar_Text m_Toolbar;
    UiNaviList m_Navibar;
    UiKeyList m_List;
    UiSmsList m_SmsList;
protected:
    //�����ұ��б�����
    void SetupList();
	//���ù���������
	void SetupToolbar();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};

