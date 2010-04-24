#include "ui_backup.h"
#include <list>
#include <cMzCommon.h>
using namespace cMzCommon;
#include "resource.h"
#include "LocalDataBase.h"

#define MZ_IDC_TOOLBAR_ACCOUNTS 101
#define MZ_IDC_BACKUP_LIST 102

#define IDC_PPM_CANCEL 108
#define IDC_PPM_DEL		109
#define IDC_PPM_RECOVER 110

extern HINSTANCE LangresHandle;
extern LocalDataBase *g_pldb;

MZ_IMPLEMENT_DYNAMIC(Ui_BackupWnd)
#ifdef _DEBUG
#define BACKUP_PARENT_DIR0	L"\\Disk\\User"
#define BACKUP_PARENT_DIR1	L"\\Disk\\User\\Backup"
#define BACKUP_DIR			L"\\Disk\\User\\Backup\\M8SmsBackup"
#else
#define BACKUP_PARENT_DIR0	L"\\Disk\\User"
#define BACKUP_PARENT_DIR1	L"\\Disk\\User\\Backup"
#define BACKUP_DIR			L"\\Disk\\User\\Backup\\M8SmsBackup"
#endif

list<SYSTEMTIME> dirtimes;

BOOL Ui_BackupWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = 0;
	m_Caption1.SetPos(0,y,GetWidth(),MZM_HEIGHT_HEADINGBAR);
	m_Caption1.SetText(LOADSTRING(IDS_STR_BACKUP_RESTORE).C_Str());
	AddUiWin(&m_Caption1);

	y+=MZM_HEIGHT_HEADINGBAR;
    m_List.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TOOLBARPRO - MZM_HEIGHT_CAPTION);
    m_List.SetID(MZ_IDC_BACKUP_LIST);
	m_List.EnableInsideScroll(true);
	m_List.SetItemHeight(60);
	m_List.SetItemAttribute(UILISTEX_ITEMTYPE_BROWSER);
	m_List.SetSplitLineMode(UILISTEX_SPLITLINE_LEFT);
	m_List.EnableSelectBg(TRUE);
    m_List.EnableDragModeH(true);
    m_List.EnableNotifyMessage(true);
	m_List.UpdateItemAttribute_Del();
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
	m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_BACKUP).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACCOUNTS);
    AddUiWin(&m_Toolbar);

	updateList();
    return TRUE;
}

void Ui_BackupWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_ACCOUNTS:
        {
            int nIndex = lParam;
            if (nIndex == 0) { //����
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
            if (nIndex == 2) { //����
				BOOL nRet = bbackup();
				if (nRet) {
					MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_BACKUP_S).C_Str(),MZV2_MB_NONE,2000,TRUE);
					updateList();
				}
                return;
            }
        }
    }
}

LRESULT Ui_BackupWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
        case MZ_WM_ITEM_ONREMOVE:
            {
                int index = lParam;
                ListItemEx* pItem = m_List.GetItem(index);
                if (pItem)
                {
					if(MzMessageBoxEx(m_hWnd,
						L"ȷʵҪɾ��ѡ�����ݣ�",
						LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
							BOOL nRet = bdelete(pItem->m_textTitle);
							if(nRet){
								MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_DELETE_S).C_Str(),MZV2_MB_NONE,2000,TRUE);
							}
					}else{	//uilistex fixup
						ListItemEx* p = new ListItemEx;
						p->m_textTitle = pItem->m_textTitle;
						p->m_enableDragDelete = true;
						p->m_pImgFirst = 
							m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH, true);
						p->m_pImgFirst_pressed = 
							m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH_PRESSED, true);
						m_List.InsertItem(p,index);
					}
                }
            }
            break;
		case MZ_WM_UILIST_LBUTTONUP_SELECT:
			if(wParam == MZ_IDC_BACKUP_LIST){
                int index = LOWORD(lParam);
                int area = HIWORD(lParam);
                if(area == 2){
					//��������
					if(MzMessageBoxV2(m_hWnd,LOADSTRING(IDS_STR_WARN_RESTORE).C_Str(),MZV2_MB_YESNO,TRUE) != 1){
						return 0;
					}
					//�ָ����ݿ�
					//�ָ�ǰ�Ͽ����ݿ�
					delete g_pldb; g_pldb = 0;

					BOOL nRet = brecover(m_List.GetItem(index)->m_textTitle);
					if(nRet){
						//��������
						MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_RESTORE_S).C_Str(),MZV2_MB_NONE,2000,TRUE);
					}
					EndModal(ID_OK);
				}
			}
			break;
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_BACKUP_LIST && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
                    int nIndex = m_List.CalcIndexOfPos(x, y);
					if(nIndex != -1){
					}
                }
                return 0;
            }
       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_BackupWnd::updateList(){
    getbackuplist();

    m_List.RemoveAll();

	list<SYSTEMTIME>::iterator it = dirtimes.begin();
	int i = 0;
	for(; it != dirtimes.end(); it++){
		SYSTEMTIME tm = *it;
		wchar_t dispname[128];
		wsprintf(dispname,L"%04d-%02d-%02d %02d:%02d:%02d",
			tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
	    ListItemEx *pli = new ListItemEx;
		pli->m_textTitle = dispname;
		pli->m_enableDragDelete = true;
		pli->m_pImgFirst = 
			m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH, true);
		pli->m_pImgFirst_pressed = 
			m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REFRESH_PRESSED, true);
		m_List.AddItem(pli);
	}
	m_List.Invalidate();
}

void Ui_BackupWnd::getbackuplist(){
	//��ȡ�����ļ���
	list<CMzString> _dirs;
	int nDir = File::ListDirectory(BACKUP_DIR,_dirs);
	if(nDir == 0){
		return;	//no list
	}

	//����Ŀ¼
    dirtimes.clear();

	list<CMzString>::iterator i = _dirs.begin();
	for(; i != _dirs.end();i++){
		CMzString dir = *i;
		SYSTEMTIME s;
		swscanf(dir.C_Str(),L"%04d%02d%02d%02d%02d%02d",
			&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);
		if(s.wYear == 0 || s.wMonth == 0 || s.wDay == 0 ||
			s.wYear > 2100 || s.wMonth > 12 || s.wDay > 31 ||
			s.wHour > 23 || s.wMinute > 59 || s.wSecond > 59){	//��Ч
			continue;
		}
		if(dirtimes.size() == 0){
			dirtimes.push_back(s);
		}else{
			list<SYSTEMTIME>::iterator it = dirtimes.begin();
			for(;it != dirtimes.end(); it++){
				SYSTEMTIME tm = *it;
				if(DateTime::compareDate(s,tm)>0){
					dirtimes.insert(it,s);
					break;
				}
			}
			if(it == dirtimes.end()){
				dirtimes.push_back(s);
			}
		}
	}
}

BOOL Ui_BackupWnd::brecover(CMzString &itemname){
	BOOL nRet = false;
	SYSTEMTIME s;
	swscanf(itemname.C_Str(),L"%04d-%02d-%02d %02d:%02d:%02d",
		&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);

	wchar_t dispname[32];
	wsprintf(dispname,L"%04d%02d%02d%02d%02d%02d",
		s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);

	CMzString fpath = BACKUP_DIR;
	fpath = fpath + L"\\";
	fpath = fpath + dispname;
	//��ȡ��ǰĿ¼
	wchar_t currpath[MAX_PATH];
	File::GetCurrentPath(currpath);
	//׼���ļ�
	list<CMzString> sfile;
	sfile.push_back(L"sms.db");
	sfile.push_back(L"m8smsbackup.ini");

	nRet = File::BackupFiles(fpath,currpath,sfile);
	return nRet;
}

BOOL Ui_BackupWnd::bdelete(CMzString &itemname){
	BOOL nRet = false;
	SYSTEMTIME s;
	swscanf(itemname.C_Str(),L"%04d-%02d-%02d %02d:%02d:%02d",
		&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);

	wchar_t dispname[32];
	wsprintf(dispname,L"%04d%02d%02d%02d%02d%02d",
		s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);

	CMzString fpath = BACKUP_DIR;
	fpath = fpath + L"\\";
	fpath = fpath + dispname;
	nRet = File::DelDirectory(fpath.C_Str());
	return nRet;
}

BOOL Ui_BackupWnd::bbackup(){
	BOOL nRet = false;
	//��鱸��Ŀ¼�Ƿ����
	nRet = File::DirectoryExists_New(BACKUP_PARENT_DIR0);
	nRet = File::DirectoryExists_New(BACKUP_PARENT_DIR1);
	//������������½���Ŀ¼
	nRet = File::DirectoryExists_New(BACKUP_DIR);
	//�������ڴ�������Ŀ¼
	CMzString dir = File::CreateDirectoryByDate(BACKUP_DIR,nRet);
	//׼���ļ�
	list<CMzString> s;
	s.push_back(L"sms.db");
	s.push_back(L"m8smsbackup.ini");
	//��ȡ��ǰĿ¼
	wchar_t currpath[MAX_PATH];
	File::GetCurrentPath(currpath);
	//��������
	nRet = File::BackupFiles(currpath,dir,s);
	return nRet;
}

//int UiBackupList::OnLButtonDown_DelControl(int index, UINT fwKeys, int xPos, int yPos){
//void UiBackupList::OnRemoveItem(int nIndex){
//	if(MzMessageBoxEx(GetParentWnd(),
//		L"ȷʵҪɾ��ѡ�����ݣ�",
//		LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
//		return UiListEx::OnRemoveItem(nIndex);//, fwKeys, xPos, yPos);
//	}
//	return;
//}