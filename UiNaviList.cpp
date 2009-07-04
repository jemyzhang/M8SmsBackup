#include "UiNaviList.h"


UiNaviButton::UiNaviButton(int id, LPCTSTR text, LPCTSTR text1, LPCTSTR text2)
{
    SetID(id);
    SetText(text);
    SetText1(text1);
    SetText2(text2);
    SetButtonType(MZC_BUTTON_NONE);
    SetButtonMode(MZC_BUTTON_MODE_HOLD);
}

UiNaviButton::~UiNaviButton(void)
{
}

void UiNaviButton::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    if(GetState() == MZCS_BUTTON_PRESSED){
        MzDrawSelectedBg(hdcDst,prcWin);
    }
    int ntext = 3;
    if(m_Text1.IsEmpty() && m_Text2.IsEmpty()) ntext = 1;

    LONG m_height = RECT_HEIGHT(*prcWin)/ntext;
    RECT rcText = {prcWin->left,prcWin->top,prcWin->right,prcWin->top + m_height};
    RECT rcText1 = {prcWin->left,rcText.bottom,prcWin->right,rcText.top + m_height};
    RECT rcText2 = {prcWin->left,rcText1.bottom,prcWin->right,rcText1.top + m_height};

    COLORREF cr = RGB(0,0,0); 
    HFONT hf;
    if(!m_Text.IsEmpty()){
        ::SetTextColor( hdcDst , cr );
        hf = FontHelper::GetFont( ntext > 1 ? 30 : 25 );
        SelectObject( hdcDst, hf );
        MzDrawText( hdcDst , m_Text.C_Str(), &rcText , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
        DeleteObject(hf);
    }
    cr = RGB(128,128,128);
    hf = FontHelper::GetFont( 20 );
    SelectObject( hdcDst, hf );
    ::SetTextColor( hdcDst , cr );
    if(!m_Text1.IsEmpty()){
        MzDrawText( hdcDst , m_Text1.C_Str(), &rcText1 , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    }
    if(!m_Text2.IsEmpty()){
        MzDrawText( hdcDst , m_Text2.C_Str(), &rcText2 , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    }
    DeleteObject(hf);
}

/////////////////////////////////////
