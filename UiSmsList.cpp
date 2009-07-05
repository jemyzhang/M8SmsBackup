#include "UiSmsList.h"

void UiSmsList::SetupList() {
    if(pldb == NULL) return;
    switch(smode){
        case 0:         //datetime
            plist_size = pldb->GetSmsByDate(syear,smonth,sday);
            if(plist_size > 0){
                plist_record = new SmsSimpleData_t[plist_size];
                plist_size = pldb->GetSmsByDate(syear,smonth,sday,plist_record);
            }
            break;
        case 1:
            plist_size = pldb->GetSmsByContact(sname);
            if(plist_size > 0){
                plist_record = new SmsSimpleData_t[plist_size];
                plist_size = pldb->GetSmsByContact(sname,plist_record);
            }
            break;
        case 2:
            plist_size = pldb->GetSmsByContent(scontent);
            if(plist_size > 0){
                plist_record = new SmsSimpleData_t[plist_size];
                plist_size = pldb->GetSmsByContent(scontent,plist_record);
            }
            break;
        default:
            break;
    }
    for(UINT i = 0; i < plist_size; i++){
        ListItem li;
        AddItem(li);
    }
}
void UiSmsList::ClearList(){
    if(plist_record && plist_size > 0){
        for(UINT i = 0; i < plist_size; i++){
            plist_record[i].Reset();
        }
        delete plist_record;
        plist_record = 0;
        plist_size = 0;
    }
}


void UiSmsList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the text
	if(plist_size == 0) return;

	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

	SmsSimpleData_t rec = plist_record[nIndex];

    HFONT hf;
    COLORREF cr;
	//收/发标记
    hf = FontHelper::GetFont( 30 );
    SelectObject( hdcDst , hf );
    RECT rcSendReceive = {prcItem->left,prcItem->top,prcItem->left + 40,prcItem->top + 40};
    if(rec.SendReceiveFlag){
        cr = RGB(42,235,235);
    }else{
        cr = RGB(235,53,51);
    }
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , rec.SendReceiveFlag ? L"发" : L"收", &rcSendReceive , DT_CENTER|DT_SINGLELINE|DT_BOTTOM );
	DeleteObject(hf);
    
    //姓名
    hf = FontHelper::GetFont( 25 );
    SelectObject( hdcDst , hf );
    RECT rcName = {rcSendReceive.right,prcItem->top,rcSendReceive.right + 180,rcSendReceive.bottom};
    cr = RGB(0,0,0);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , rec.ContactName, &rcName , DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

    //号码
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst , hf );
    RECT rcNumber = {prcItem->right - 160,prcItem->top,prcItem->right - 5,rcName.bottom - 20};
    cr = RGB(128,128,128);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , rec.MobileNumber, &rcNumber , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

    //日期
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst , hf );
    RECT rcDate = {prcItem->right - 160,rcNumber.bottom,prcItem->right - 5,rcNumber.bottom + 20};
    cr = RGB(128,128,128);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , rec.TimeStamp, &rcDate , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);
    //内容
    hf = FontHelper::GetFont( 20 );
    SelectObject( hdcDst , hf );
    RECT rcContent = {prcItem->left + 40,prcItem->top + 45,prcItem->right - 10,prcItem->bottom};
    cr = RGB(0,0,0);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , rec.Content, &rcContent , DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);
}

