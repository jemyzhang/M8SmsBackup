#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <cMzCommon.h>
using namespace cMzCommon;
#include <vector>
using std::vector;
typedef struct tagUiNaviButton{
    UINT id;
    LPTSTR name;
    tagUiNaviButton(UINT id = 0, LPCTSTR name = NULL){
        this->id = id;
        this->name = NULL;
        if(name != NULL){
            C::newstrcpy(&this->name,name);
        }
    }
    ~tagUiNaviButton(){
        if(name) delete name;
    }
}UiNaviButton,*lpUiNaviButton;

class UiNaviList :
    public UiButtonBar
{
public:
    UiNaviList(void) {}
    ~UiNaviList(void) {
        clear();
    }
private:
    vector<lpUiNaviButton> m_buttons;
    //检查是否重复
    bool duplicated(UINT id){
        bool bret = false;
        for(int i = 0; i < m_buttons.size(); i++){
            if(m_buttons.at(i)->id == id){
                bret = true;
                break;
            }
        }
        return bret;
    }
    void clear(){
        for(int i = 0; i < m_buttons.size(); i++){
            delete m_buttons.at(i);
        }
        m_buttons.clear();
    }
protected:
    void updatebutton(){
        this->SetButtonCount(m_buttons.size());
        for(int i = 0; i < m_buttons.size(); i++){
            lpUiNaviButton btn = m_buttons.at(i);
            this->SetButton(i,true,true,btn->name);
        }
        this->SetHighLightButton(m_buttons.size() - 1);
        Invalidate();
    }
public:
    UINT getid(){
        int index = this->GetHighLightButton();
        if(index < m_buttons.size()){
            return m_buttons.at(index)->id;
        }
        return 0;
    }
    //增加按钮
    bool push(lpUiNaviButton button){
        if(button == NULL) return false;
        if(button->name == NULL) return false;
        if(duplicated(button->id)) return false;
        m_buttons.push_back(button);
        updatebutton();
        return true;
    }
    bool push(UINT id, LPCTSTR name){
        lpUiNaviButton button = new UiNaviButton(id,name);
        bool bret = push(button);
        if(bret == false){
            delete button;
        }
        return bret;
    }
    //减少按钮
    bool pop(){
        if(m_buttons.size() == 0) return false;
        lpUiNaviButton btn = m_buttons.back();
        m_buttons.pop_back();
        delete btn;
        updatebutton();
        return true;
    }
    //pop the button until the specfic one
    void popUntil(int nID){
        int nsize = m_buttons.size();
        for(int i = 0; i < nsize; i++){
            lpUiNaviButton btn = m_buttons.back();
            if(btn->id == nID){
                break;
            }
            m_buttons.pop_back();
            delete btn;
        }
        updatebutton();
    }
};
