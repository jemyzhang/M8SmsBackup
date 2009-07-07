#include "UiSmsList.h"
#include "resource.h"
#include "ui_SmsViewer.h"
#include "ui_ProgressBar.h"

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];

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
	RemoveAll();
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

	SmsSimpleData_ptr prec = plist_record+nIndex;

    HFONT hf;
    COLORREF cr;
	//收/发标记
    UINT SmsResID;
    if(prec->SendReceiveFlag){
        SmsResID = IDB_PNG_SMS_SEND - IDB_PNG_BEGIN;
    }else{
        SmsResID = IDB_PNG_SMS_RECV - IDB_PNG_BEGIN;
    }

    RECT rcSendReceive = {prcItem->left,prcItem->top,prcItem->left + 40,prcItem->top + 40};
    pimg[SmsResID]->Draw(hdcDst,&rcSendReceive);
    
    //姓名
    hf = FontHelper::GetFont( 25 );
    SelectObject( hdcDst , hf );
    RECT rcName = {rcSendReceive.right,prcItem->top,rcSendReceive.right + 180,rcSendReceive.bottom};
    cr = RGB(0,0,0);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , prec->ContactName, &rcName , DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

    //号码
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst , hf );
    RECT rcNumber = {prcItem->right - 160,prcItem->top,prcItem->right - 5,rcName.bottom - 20};
    cr = RGB(128,128,128);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , prec->MobileNumber, &rcNumber , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

    //日期
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst , hf );
    RECT rcDate = {prcItem->right - 160,rcNumber.bottom,prcItem->right - 5,rcNumber.bottom + 20};
    cr = RGB(128,128,128);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , prec->TimeStamp, &rcDate , DT_BOTTOM|DT_RIGHT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);
    //内容
    hf = FontHelper::GetFont( 20 );
    SelectObject( hdcDst , hf );
    RECT rcContent = {prcItem->left + 40,prcItem->top + 45,prcItem->right - 10,prcItem->bottom};
    cr = RGB(0,0,0);
    ::SetTextColor( hdcDst , cr );
    MzDrawText( hdcDst , prec->Content, &rcContent , DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORD_ELLIPSIS );
	DeleteObject(hf);

	if(bSelectionMode){
		RECT rcSelection = {prcItem->right - 40,prcItem->top,prcItem->right,prcItem->bottom};
		MzDrawControl(hdcDst, &rcSelection,  prec->SelectionFlag ? MZC_SELECTED : MZC_UNSELECTED, 0);
	}

}

void UiSmsList::SetSelectedIndex(int nIndex){
	if(bSelectionMode){
		plist_record[nIndex].SelectionFlag = !plist_record[nIndex].SelectionFlag;
	}else{
		if(nIndex != -1){
			if(nIndex == seletedidx){   //show sms
				Ui_SmsViewerWnd dlg;
				dlg.SetViewRecord(plist_record,nIndex,plist_size);
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/2, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)/2,
					GetParentWnd(), 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
				dlg.DoModal();
			}
		}
		seletedidx = nIndex;
	}
	return UiList::SetSelectedIndex(nIndex);
}

void UiSmsList::DeleteSelectedItems(){
	initUiCallbackDeleteSms();
	UINT nDeleting = GetSelectedCount();
	UINT nDeleted = 0;
	UINT nCount = 0;
	pldb->beginTrans();
	for(UINT i = 0; i < plist_size; i++){
		if(plist_record[i].SelectionFlag){
			nDeleted += (pldb->RemoveSmsRecord(plist_record+i) ? 1 : 0);
			uiCallbackDeleteSms(plist_record+i,nCount,nDeleting,nDeleted);
			nCount++;
		}
	}
	pldb->commitTrans();
	uiCallbackDeleteSms(NULL,0,0,0);
}