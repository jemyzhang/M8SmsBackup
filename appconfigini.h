#pragma once
/*
 * @filename appconfigini.h
 * @note Ӧ�ó��������ļ���д��
 * @author JEMYZHANG
 * @date 2009.6.20
 * @ver. 1.1
 * @changelog
 *  ver.1.0 ��ʼ������
 *  ver.1.1 ����MzConfig�࣬�򻯼�ֵ��д
*/

// include the MZFC library header file
#include <mzfc_inc.h>

//�����ļ�·��
//��AppConfigIni��ʼ��
static TCHAR * gs_inifile = NULL;

//�����ļ�������д��
class MzConfig{
public:
	MzConfig(){
		_mainKey = NULL;
		_subKey = NULL;
		_defaultValue = 0;
	}
public:
	void InitKey(TCHAR * mainKey, TCHAR * subKey, DWORD defaultVal = 0){
		_mainKey = mainKey;
		_subKey = subKey;
		_defaultValue = defaultVal;
	}
public:
	DWORD Get();
	void Set(DWORD val);
private:
	//��������ļ�·���Լ���ֵ
	bool checkKey(){
		bool nRet = true;
		if(_mainKey == NULL || _subKey == NULL ||
			gs_inifile == NULL){
			nRet = false;
		}
		return nRet;
	}
private:
	TCHAR * _mainKey;	//����
	TCHAR * _subKey;	//�Ӽ�
	DWORD _defaultValue;	//����ֵ������ʱ��Ĭ��ֵ
};

//Ӧ�ó�������
class AppConfigIni
{
public:
	AppConfigIni(){
		SetupIniPath();
		CreateIni();
		InitIniKey();
	}
protected:
	void SetupIniPath();
	void CreateIni();
	void InitIniKey();
public:
	//�Ƿ�����ʱ��������
	MzConfig IniRefreshWhileLoad;
	//����ʱ�䷶Χ
	MzConfig IniExportSmsStartDate;
	MzConfig IniExportSmsEndDate;
private:
	wchar_t ini_path[MAX_PATH];
};
