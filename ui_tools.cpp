#include "ui_tools.h"
#include "resource.h"
#include "appconfigini.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_password.h"
#include "ui_backup.h"
#include "LocalDataBase.h"
#include "ui_ProgressBar.h"
#include "UiFileDialog.h"

#include <fstream>
using namespace std;

extern HINSTANCE LangresHandle;
extern SmsBackupConfig appconfig;
extern int g_password_len;
extern LocalDataBase *g_pldb;

MZ_IMPLEMENT_DYNAMIC(Ui_ToolWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101

#define MZ_IDC_BTN_SETUP_BACKUP     106
#define MZ_IDC_BTN_SETUP_OPTIMIZE   107
#define MZ_IDC_BTN_SETUP_CONTACT_RELOAD  108
#define MZ_IDC_BTN_SETUP_CONTACT_EXPORT	109
#define MZ_IDC_BTN_SETUP_SMS_EXPORT	110

#define MZ_IDC_BTN_SETUP_CLEAR  111
//////

Ui_ToolWnd::Ui_ToolWnd(void)
{
}

Ui_ToolWnd::~Ui_ToolWnd(void)
{
}

BOOL Ui_ToolWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_HeaderTitle.SetPos(0,y, GetWidth(), MZM_HEIGHT_HEADINGBAR);
    m_HeaderTitle.SetText(L"工具");
    AddUiWin(&m_HeaderTitle);

    y += MZM_HEIGHT_HEADINGBAR;
	m_BtnBackup.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnBackup.SetText(LOADSTRING(IDS_STR_BACKUP).C_Str());
	m_BtnBackup.SetText2(LOADSTRING(IDS_STR_BACKUP_RESTORE).C_Str());
    m_BtnBackup.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnBackup.SetTextMaxLen(0);
    m_BtnBackup.SetID(MZ_IDC_BTN_SETUP_BACKUP);
    AddUiWin(&m_BtnBackup);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnOptimize.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnOptimize.SetText(LOADSTRING(IDS_STR_OPTIMIZE).C_Str());
	m_BtnOptimize.SetText2(LOADSTRING(IDS_STR_OPTIMIZE_DTL).C_Str());
    m_BtnOptimize.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnOptimize.SetTextMaxLen(0);
    m_BtnOptimize.SetID(MZ_IDC_BTN_SETUP_OPTIMIZE);
    AddUiWin(&m_BtnOptimize);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnReloadContact.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnReloadContact.SetText(LOADSTRING(IDS_STR_CONTACT_RELOAD).C_Str());
	m_BtnReloadContact.SetText2(LOADSTRING(IDS_STR_CONTACT_RELOAD_DTL).C_Str());
    m_BtnReloadContact.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnReloadContact.SetTextMaxLen(0);
    m_BtnReloadContact.SetID(MZ_IDC_BTN_SETUP_CONTACT_RELOAD);
    AddUiWin(&m_BtnReloadContact);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnExpContact.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnExpContact.SetText(LOADSTRING(IDS_STR_EXPCON).C_Str());
	m_BtnExpContact.SetText2(LOADSTRING(IDS_STR_EXPCON_DTL).C_Str());
    m_BtnExpContact.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnExpContact.SetTextMaxLen(0);
    m_BtnExpContact.SetID(MZ_IDC_BTN_SETUP_CONTACT_EXPORT);
    AddUiWin(&m_BtnExpContact);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnExpSms.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnExpSms.SetText(LOADSTRING(IDS_STR_EXPSMS).C_Str());
	m_BtnExpSms.SetText2(LOADSTRING(IDS_STR_EXPSMS_TIP).C_Str());
    m_BtnExpSms.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnExpSms.SetTextMaxLen(0);
    m_BtnExpSms.SetID(MZ_IDC_BTN_SETUP_SMS_EXPORT);
    AddUiWin(&m_BtnExpSms);

    y+=MZM_HEIGHT_BUTTONEX;
    m_HeaderDB.SetPos(0,y, GetWidth(), MZM_HEIGHT_HEADINGBAR);
    m_HeaderDB.SetText(L"危险操作");
    AddUiWin(&m_HeaderDB);

    y+=MZM_HEIGHT_HEADINGBAR;
	m_BtnClearData.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnClearData.SetText(LOADSTRING(IDS_STR_CLEAR).C_Str());
	m_BtnClearData.SetText2(LOADSTRING(IDS_STR_CLEAR_DTL).C_Str());
    m_BtnClearData.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnClearData.SetTextMaxLen(0);
    m_BtnClearData.SetID(MZ_IDC_BTN_SETUP_CLEAR);
    AddUiWin(&m_BtnClearData);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);
    return TRUE;
}

void Ui_ToolWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BTN_SETUP_BACKUP:
        {
            Ui_BackupWnd dlg;
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_PUSH);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_PUSH);
            int rc = dlg.DoModal();
            if(rc == ID_OK){
                EndModal(ID_OK);
				SetWindowText(L"短信备份");
            }
            break;
        }
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
			if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//取消
				EndModal(ID_OK);
				SetWindowText(L"短信备份");
				return;
			}
            break;
		}
        case MZ_IDC_BTN_SETUP_OPTIMIZE:
        {
            CMzPopupWaitDialog m_progress;
            m_progress.EnableCancelButton(FALSE);
            m_progress.BeginWaitDialog(m_hWnd,L"数据库优化中，请稍候。",TRUE);
            if(g_pldb != 0){
                g_pldb->indexDatabase();
                g_pldb->reorgDatebase();
            }
            m_progress.EndWaitDialog();
            return;
        }
        case MZ_IDC_BTN_SETUP_CONTACT_RELOAD:
        {
            g_pldb->ClearContactTable();
            if(appconfig.IniUseSimPhoneBook.Get()){
                initUiCallbackRefreshContact(m_hWnd);
                refreshSIMContact(uiCallbackRefreshSIMContact);
            }
            initUiCallbackRefreshContact(m_hWnd);
            refreshContact(uiCallbackRefreshContact);
            break;
        }
        case MZ_IDC_BTN_SETUP_CLEAR:
        {
			if(MzMessageBoxV2(m_hWnd,L"确实要清除所有数据?",MZV2_MB_YESNO,TRUE) != 1){
				return;
			}
            g_pldb->ClearContactTable();
            g_pldb->ClearSmsTable();
            MzMessageAutoBoxV2(m_hWnd,L"数据清除成功",MZV2_MB_NONE,2000,TRUE);
            break;
        }
		case MZ_IDC_BTN_SETUP_CONTACT_EXPORT:
		{
			UiFileDialog dlg;
			dlg.SetTitle(L"请设置导出文件名");
			dlg.SetInitFileName(L"contact");
			dlg.SetInitFileSuffix(L".txt");
			dlg.SetInitFolder(L"\\Disk");
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
				m_hWnd, 0, WS_POPUP);
			if( dlg.DoModal() == ID_OK ) {
				wofstream file;
				file.open(dlg.GetFullFileName(),  ios::out);
				if (file.is_open())
				{
                    DateTime::waitms(0);
					if(g_pldb->query_contacts()){
						initUiCallbackExportContact(m_hWnd);
                        int size = g_pldb->query_contact_size();
						for(int i = 0; i < size; i++){
							ContactData_ptr c = g_pldb->query_contact_at(i);
							file << c->Name << endl;
							int pnsz = c->MobileTels.size();
							if(pnsz){
								file << L"手机: ";
								for(int j = 0; j < pnsz; j++){
									file << c->MobileTels.at(j);
									if(j == pnsz - 1){
										file << endl;
									}else{
										file << L",";
									}
								}
							}
							pnsz = c->WorkTels.size();
							if(pnsz){
								file << L"工作: ";
								for(int j = 0; j < pnsz; j++){
									file << c->WorkTels.at(j);
									if(j == pnsz - 1){
										file << endl;
									}else{
										file << L",";
									}
								}
							}

							pnsz = c->HomeTels.size();
							if(pnsz){
								file << L"家庭: ";
								for(int j = 0; j < pnsz; j++){
									file << c->HomeTels.at(j);
									if(j == pnsz - 1){
										file << endl;
									}else{
										file << L",";
									}
								}
							}

							pnsz = c->HomeTel2s.size();
							if(pnsz){
								file << L"其他: ";
								for(int j = 0; j < pnsz; j++){
									file << c->HomeTel2s.at(j);
									if(j == pnsz - 1){
										file << endl;
									}else{
										file << L",";
									}
								}
							}
							file << endl;
							if(!uiCallbackExportContact(c,i+1,size)){
								break;
							}
						}
					}
				}
				file.close();
                uiCallbackExportContact(NULL,0,0);//隐藏进度条
				MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_EXP_FINISHED).C_Str(),MZV2_MB_NONE,2000,TRUE);
			}
			break;
		}
		case MZ_IDC_BTN_SETUP_SMS_EXPORT:
		{
			if(MzMessageBoxV2(m_hWnd,LOADSTRING(IDS_STR_EXPSMS_WARNING).C_Str(),MZV2_MB_YESNO,TRUE) != 1){
				return;
			}
			UiFileDialog dlg;
			dlg.SetTitle(L"请设置导出短信文件名");
			dlg.SetInitFileName(L"sms");
			dlg.SetInitFileSuffix(L".txt");
			dlg.SetInitFolder(L"\\Disk");
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
				m_hWnd, 0, WS_POPUP);
			if( dlg.DoModal() == ID_OK ) {
				wofstream file;
				file.open(dlg.GetFullFileName(),  ios::out);
				if (file.is_open())
				{
					DateTime::waitms(0);
					MzBeginWaitDlg(m_hWnd);
					bool bret = g_pldb->query_sms();
					MzEndWaitDlg();
					DateTime::waitms(0);

					if(bret){
						initUiCallbackExportSms(m_hWnd);
						LPWSTR sname = 0;	//前一个姓名，对比归纳用
                        int size = g_pldb->query_sms_size();
						for(int i = 0; i < size; i++){
							SmsSimpleData_ptr c = g_pldb->query_sms_at(i);
							if(sname == 0 ||
								wcscmp(sname,c->ContactName) != 0){
								if(sname != 0) file << endl;
								file << L"[==========" << c->ContactName << L"==========]" << endl;
                                C::newstrcpy(&sname, c->ContactName);
							}
							if(c->SendReceiveFlag){
                                file << L">我 ";
							}else{
                                file << L">" << c->ContactName << L" ";
							}
							file << c->TimeStamp << L" " << c->MobileNumber << endl;
							file << c->Content << endl;
							if(!uiCallbackExportSms(c,i,size)){
								break;
							}
						}
                        if(sname) delete sname;
					}
				}
				file.close();
                uiCallbackExportSms(NULL,0,0);
				MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_EXP_FINISHED).C_Str(),MZV2_MB_NONE,2000,TRUE);
			}
			break;
		}
	}
}

LRESULT Ui_ToolWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

