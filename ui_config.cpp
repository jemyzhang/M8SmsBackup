#include "ui_config.h"
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

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101

#define MZ_IDC_BTN_USE_SIM		102
#define MZ_IDC_BTN_BOOT_UPDATE		103
#define MZ_IDC_BTN_SETUP_PASSWORD   104
#define MZ_IDC_BTN_UPDATE_METHOD    105
#define MZ_IDC_BTN_SETUP_BACKUP     106
#define MZ_IDC_BTN_SETUP_OPTIMIZE   107
#define MZ_IDC_BTN_SETUP_CONTACT_CLEAR  108
#define MZ_IDC_BTN_SETUP_CONTACT_EXPORT	109
//////

Ui_ConfigWnd::Ui_ConfigWnd(void)
{
}

Ui_ConfigWnd::~Ui_ConfigWnd(void)
{
}

BOOL Ui_ConfigWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_Caption.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption.SetText(LOADSTRING(IDS_STR_CONFIGURATIONS).C_Str());;
	AddUiWin(&m_Caption);

	y+=MZM_HEIGHT_CAPTION;
	m_BtnUseSimPhoneBook.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnUseSimPhoneBook.SetText(LOADSTRING(IDS_STR_USE_SIM_PHONEBOOK).C_Str());
    m_BtnUseSimPhoneBook.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnUseSimPhoneBook.SetTextMaxLen(0);
	m_BtnUseSimPhoneBook.SetEnable(false);
    AddUiWin(&m_BtnUseSimPhoneBook);

	m_BtnUseSimPhoneBookSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnUseSimPhoneBookSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnUseSimPhoneBookSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnUseSimPhoneBookSW.SetID(MZ_IDC_BTN_USE_SIM);
    AddUiWin(&m_BtnUseSimPhoneBookSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnBootUpdate.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnBootUpdate.SetText(LOADSTRING(IDS_STR_BOOTUPDATE).C_Str());
    m_BtnBootUpdate.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnBootUpdate.SetTextMaxLen(0);
	m_BtnBootUpdate.SetEnable(false);
    AddUiWin(&m_BtnBootUpdate);

	m_BtnBootUpdateSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnBootUpdateSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnBootUpdateSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnBootUpdateSW.SetID(MZ_IDC_BTN_BOOT_UPDATE);
    AddUiWin(&m_BtnBootUpdateSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnUpdateMethod.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnUpdateMethod.SetText(LOADSTRING(IDS_STR_REFRESH_METHOD).C_Str());
    m_BtnUpdateMethod.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnUpdateMethod.SetTextMaxLen(0);
    m_BtnUpdateMethod.SetID(MZ_IDC_BTN_UPDATE_METHOD);
    AddUiWin(&m_BtnUpdateMethod);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnSetupPassword.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnSetupPassword.SetText(LOADSTRING(IDS_STR_PWD_SETUP).C_Str());
    m_BtnSetupPassword.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnSetupPassword.SetTextMaxLen(0);
    m_BtnSetupPassword.SetID(MZ_IDC_BTN_SETUP_PASSWORD);
    AddUiWin(&m_BtnSetupPassword);

    y+=MZM_HEIGHT_BUTTONEX;
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
	m_BtnClearContact.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnClearContact.SetText(LOADSTRING(IDS_STR_CONTACT_CLEAR).C_Str());
	m_BtnClearContact.SetText2(LOADSTRING(IDS_STR_CONTACT_CLEAR_DTL).C_Str());
    m_BtnClearContact.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnClearContact.SetTextMaxLen(0);
    m_BtnClearContact.SetID(MZ_IDC_BTN_SETUP_CONTACT_CLEAR);
    AddUiWin(&m_BtnClearContact);

    y+=MZM_HEIGHT_BUTTONEX;
	m_BtnExpContact.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnExpContact.SetText(LOADSTRING(IDS_STR_EXPCON).C_Str());
	m_BtnExpContact.SetText2(LOADSTRING(IDS_STR_EXPCON_DTL).C_Str());
    m_BtnExpContact.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnExpContact.SetTextMaxLen(0);
    m_BtnExpContact.SetID(MZ_IDC_BTN_SETUP_CONTACT_EXPORT);
    AddUiWin(&m_BtnExpContact);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);
	updateUi();
    return TRUE;
}

void Ui_ConfigWnd::updateBootUpdate(){
	if(!appconfig.IniRefreshWhileLoad.Get()){
		m_BtnBootUpdateSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnBootUpdateSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnBootUpdate.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnBootUpdate.Invalidate();
	//m_BtnBootUpdate.Update();
	m_BtnBootUpdateSW.Invalidate();
	//m_BtnBootUpdateSW.Update();
}

void Ui_ConfigWnd::updateUseSimPhoneBook(){
	if(!appconfig.IniUseSimPhoneBook.Get()){
		m_BtnUseSimPhoneBookSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_NO).C_Str());
	}else{
		m_BtnUseSimPhoneBookSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnUseSimPhoneBook.SetText2(LOADSTRING(IDS_STR_OPTION_YES).C_Str());
	}
	m_BtnUseSimPhoneBook.Invalidate();
	//m_BtnUseSimPhoneBook.Update();
	m_BtnUseSimPhoneBookSW.Invalidate();
	//m_BtnUseSimPhoneBookSW.Update();
}

void Ui_ConfigWnd::updateUpdateMethod(){
	if(appconfig.IniUpdateMethod.Get() == 0){
		m_BtnUpdateMethod.SetText2(LOADSTRING(IDS_STR_REFRESH_WHOLE).C_Str());
	}else{
		m_BtnUpdateMethod.SetText2(LOADSTRING(IDS_STR_REFRESH_FAST).C_Str());
	}
	m_BtnUpdateMethod.Invalidate();
	//m_BtnUpdateMethod.Update();
}

void Ui_ConfigWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_USE_SIM:
		{
			if(m_BtnUseSimPhoneBookSW.GetState() == MZCS_BUTTON_PRESSED){
				appconfig.IniUseSimPhoneBook.Set(1);
			}else{
				appconfig.IniUseSimPhoneBook.Set(0);
			}
			updateUseSimPhoneBook();
			break;
		}
		case MZ_IDC_BTN_BOOT_UPDATE:
		{
			if(m_BtnBootUpdateSW.GetState() == MZCS_BUTTON_PRESSED){
				appconfig.IniRefreshWhileLoad.Set(1);
			}else{
				appconfig.IniRefreshWhileLoad.Set(0);
			}
			updateBootUpdate();
			break;
		}
        case MZ_IDC_BTN_UPDATE_METHOD:
        {
            if(appconfig.IniUpdateMethod.Get()){
				appconfig.IniUpdateMethod.Set(0);
			}else{
				appconfig.IniUpdateMethod.Set(1);
			}
			updateUpdateMethod();
			break;
        }
        case MZ_IDC_BTN_SETUP_PASSWORD:
        {
            Ui_PasswordWnd dlg;
            dlg.setMode(1);
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
                m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            int rc = dlg.DoModal();
            if(rc == ID_OK){
                //启动等待画面
                wchar_t* p = 0;
                int len = 0;
                dlg.getPassword(&p,&len);
                SetPasswordWaitDlg dlg;
                dlg.setPassword(p,len);
                dlg.Create(rcWork.left + 30, rcWork.top + 200, RECT_WIDTH(rcWork) - 60, RECT_HEIGHT(rcWork) - 400,
                    m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
                dlg.DoModal();
                //密码设置完成后退出
                EndModal(ID_OK);
            }
            break;
        }
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
            }
            break;
        }
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//取消
				EndModal(ID_OK);
				return;
			}
            break;
		}
        case MZ_IDC_BTN_SETUP_OPTIMIZE:
        {
            OptimizeWaitDlg dlg;
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left + 30, rcWork.top + 200, RECT_WIDTH(rcWork) - 60, RECT_HEIGHT(rcWork) - 400,
                m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
            dlg.DoModal();
            return;
        }
        case MZ_IDC_BTN_SETUP_CONTACT_CLEAR:
        {
            g_pldb->ClearContactTable();
            if(appconfig.IniUseSimPhoneBook.Get()){
                initUiCallbackRefreshContact();
                refreshSIMContact(uiCallbackRefreshSIMContact);
            }
            initUiCallbackRefreshContact();
            refreshContact(uiCallbackRefreshContact);
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
					if(g_pldb->query_contacts()){
						initProgressBar(L"导出联系人中...",0,g_pldb->query_contact_size());
						for(int i = 0; i < g_pldb->query_contact_size(); i++){
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
							if(!updateProgressBar(i+1)){
								break;
							}
						}
					}
				}
				file.close();
				MzMessageAutoBoxV2(m_hWnd,LOADSTRING(IDS_STR_EXP_FINISHED).C_Str(),MZV2_MB_NONE,2000,TRUE);
			}
			break;
		}
	}
}

LRESULT Ui_ConfigWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

bool OptimizeWaitDlg::CallBackProcess(){
    if(g_pldb == 0) return false;
    g_pldb->indexDatabase();
    g_pldb->reorgDatebase();
    return true;
}

void SetPasswordWaitDlg::setPassword(wchar_t* p, int sz){
    if(p == 0 || sz == 0){
        if(pWd) delete pWd;
        pWd = 0;
        len = 0;
        setMessage(L"清除密码中，请稍候。");
    }else{
        C::newstrcpy(&pWd,p);
        len = sz;
        setMessage(L"设定密码中，请稍候。");
    }
}

bool SetPasswordWaitDlg::CallBackProcess(){
    wchar_t defaultpwd[6] = {0x10,0x15,0x13,0x18,0x08,0x01};

    if(pWd == NULL || len == 0){
        if(g_pldb->encrypt(defaultpwd,6)){
            m_Message.SetText(LOADSTRING(IDS_STR_PWD_CLEAR_S).C_Str());
            g_password_len = 0;
        }else{
            m_Message.SetText(LOADSTRING(IDS_STR_PWD_CLEAR_F).C_Str());
        }
    }else{
        if(g_pldb->encrypt(pWd,len)){
            m_Message.SetText(LOADSTRING(IDS_STR_PWD_SET_S).C_Str());
            g_password_len = 0;
        }else{
            m_Message.SetText(LOADSTRING(IDS_STR_PWD_SET_F).C_Str());
        }
    }
    m_Message.Invalidate();
    //m_Message.Update();

    //销毁数据库连接
    delete g_pldb;
    g_pldb = 0;
    return true;
}