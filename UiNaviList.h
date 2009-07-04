#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <list>
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

//
class UiNaviButton :
    public UiButton
{
public:
    UiNaviButton(int id, LPCTSTR text = NULL, LPCTSTR text1 = NULL, LPCTSTR text2 = NULL);
    ~UiNaviButton();
public:
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
    virtual void SetText(LPCTSTR text){
        if(text == NULL){
            if(m_Text) delete [] m_Text;
            m_Text = NULL;
            return;
        }
        C::newstrcpy(&m_Text,text);
    }
    virtual void SetText1(LPCTSTR text){
        if(text == NULL){
            if(m_Text1) delete [] m_Text1;
            m_Text1 = NULL;
            return;
        }
        C::newstrcpy(&m_Text1,text);
    }
    virtual void SetText2(LPCTSTR text){
        if(text == NULL){
            if(m_Text2) delete [] m_Text2;
            m_Text2 = NULL;
            return;
        }
        C::newstrcpy(&m_Text2,text);
    }
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
        SetState(MZCS_BUTTON_PRESSED);
        Invalidate();
        Update();
        return UiButton::OnLButtonUp(fwKeys,xPos,yPos);
    }
private:
    LPWSTR m_Text;
    LPWSTR m_Text1;
    LPWSTR m_Text2;
};

class UiNaviList :
    public UiCaption
{
public:
    UiNaviList(void) {}
    ~UiNaviList(void) {}
public:
    void push(UiNaviButton *mButton){
        if(mButton){
            if(GetChildByID(mButton->GetID())){   //已经存在此id的按钮
                delete mButton;
                mButton = 0;
                return;
            }
            size_t nCount = GetChildrenCount();
            for(size_t i = 0; i < nCount; i++){
                UiNaviButton* mButton = (UiNaviButton *)GetChild(i);
                mButton->SetState(MZCS_BUTTON_NORMAL);
            }
            mButton->SetPos(0,nCount * 120,GetWidth(),120);
            mButton->SetState(MZCS_BUTTON_PRESSED);
            AddChild(mButton);
            Invalidate();
            Update();
        }
    }
    void pop(){
        size_t nCount = GetChildrenCount();
        if(nCount == 0) return;
        //设置按钮高亮
        if(nCount - 2 > 0){
            UiNaviButton* mprevButton = (UiNaviButton *)GetChild(nCount-2);
            mprevButton->SetState(MZCS_BUTTON_PRESSED);
        }
        //get the last child
        UiNaviButton* mButton = (UiNaviButton *)GetChild(nCount-1);
        RemoveChild(mButton);
        Invalidate();
        Update();
        delete mButton;
    }
    //pop the button until the specfic one
    void popUntil(int nID){
        size_t nCount = GetChildrenCount();
        if(nCount == 0) return;
        UiNaviButton* mButton = (UiNaviButton *)GetChildByID(nID);
        mButton->SetState(MZCS_BUTTON_PRESSED); //设置按钮高亮
        if(mButton == NULL) return;
        for(size_t i = nCount - 1; i >= 0; i--){
            UiNaviButton* mb = (UiNaviButton *)GetChild(i);
            if(mb == mButton){
                break;
            }else{
                RemoveChild(mb);
                delete mb;
            }
        }
        Invalidate();
        Update();
    }
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        //MzDrawGridDlgBG(hdcDst,prcWin);
        UiCaption::PaintWin(hdcDst,prcWin,prcUpdate);
    }
};
