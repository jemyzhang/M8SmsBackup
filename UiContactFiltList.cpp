#include "UiContactFiltList.h"
#include "ui_ProgressBar.h"
#include <cMzCommon.h>
using namespace cMzCommon;

bool UiContactFiltList::refresh() {
    if(pldb == NULL) return false;
    if(scontent.length() == 0) return false;
    pldb->query_contact(scontent.c_str());
    if(pldb->query_contact_size() == 0) return false;

    RemoveAll();
    for(UINT i = 0; i < pldb->query_contact_size(); i++){
        ContactData_ptr contact = pldb->query_contact_at(i);
        ListItemEx* item = new ListItemEx;
        item->m_textTitle = contact->Name;
        if(contact->MobileTels.size() > 0){
            item->m_textDescription = contact->MobileTels.at(0);
        }else if(contact->WorkTels.size() > 0){
            item->m_textDescription = contact->WorkTels.at(0);
        }else if(contact->HomeTels.size() > 0){
            item->m_textDescription = contact->HomeTels.at(0);
        }else if(contact->HomeTel2s.size() > 0){
            item->m_textDescription = contact->HomeTel2s.at(0);
        }
        item->m_enableDragDelete = false;
        AddItem(item);
    }
    return true;
}

LPCTSTR UiContactFiltList::GetSelectName(){
    int nID = this->GetSelectedIndex();
    if(nID == -1) return NULL;
    ContactData_ptr contact = pldb->query_contact_at(nID);
    return contact->Name;
}