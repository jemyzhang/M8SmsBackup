#include "UiNaviList.h"


UiNaviButton::UiNaviButton(int id, LPCTSTR text, LPCTSTR text1, LPCTSTR text2)
{
    SetID(id);
    m_Text = NULL;
    m_Text1 = NULL;
    m_Text2 = NULL;
    SetText(text);
    SetText1(text1);
    SetText2(text2);
    SetButtonType(MZC_BUTTON_NONE);
    SetButtonMode(MZC_BUTTON_MODE_HOLD);
}

UiNaviButton::~UiNaviButton(void)
{
    if(m_Text) delete [] m_Text;
    m_Text = NULL;
    if(m_Text1) delete [] m_Text1;
    m_Text1 = NULL;
    if(m_Text2) delete [] m_Text2;
    m_Text2 = NULL;
}

void UiNaviButton::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    if(GetState() == MZCS_BUTTON_PRESSED){
        MzDrawSelectedBg(hdcDst,prcWin);
    }
    int ntext = 2;
    if(m_Text1==NULL && m_Text1==NULL) ntext = 1;
    if(m_Text1 && m_Text2) ntext = 3;

    LONG m_height = RECT_HEIGHT(*prcWin)/ntext;
    RECT rcText = {prcWin->left,prcWin->top,prcWin->right,prcWin->top + m_height};
    RECT rcText1 = {prcWin->left,rcText.bottom,prcWin->right,rcText.bottom + m_height};
    RECT rcText2 = {prcWin->left,rcText1.bottom,prcWin->right,rcText1.bottom + m_height};

    COLORREF cr = RGB(0,0,0); 
    HFONT hf;
    if(m_Text){
        ::SetTextColor( hdcDst , cr );
        hf = FontHelper::GetFont( ntext > 1 ? 25 : 30 );
        SelectObject( hdcDst, hf );
        MzDrawText( hdcDst , m_Text, &rcText , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
        DeleteObject(hf);
    }
    cr = RGB(128,128,128);
    hf = FontHelper::GetFont( 20 );
    SelectObject( hdcDst, hf );
    ::SetTextColor( hdcDst , cr );
    if(m_Text1){
        MzDrawText( hdcDst , m_Text1, &rcText1 , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    }
    if(!m_Text2){
        MzDrawText( hdcDst , m_Text2, &rcText2 , DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
    }
    DeleteObject(hf);
}

/////////////////////////////////////
