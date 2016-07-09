#pragma once
#include "StdAfx.h"
#include <string>
#include <string>
#include <iostream>

using namespace std;


std::tstring GetFullTimeString(time_t t);

std::tstring GetYMDTimeString(time_t t);

std::tstring GetHMSTimeString(time_t t);

int GetNextItemID();

//check if account is number.
bool IsLegalAccount(tstring& str);

tstring ExtractPhoneNo(const tstring strAccount);
CDuiString ExtractPhoneNo(const CDuiString strAccount);

tstring GetFileSizeFormat(int nFileSize);

tstring GetDefaultFileIcon(tstring& strFileType);

tstring GetPackFileIcon(tstring& strFileType);

tstring GetAttibuteDescription(int nValue);

tstring GetAttibuteValue(tstring& strDesc);

bool IsQingyuePack(tstring& strProperty);

//bool CheckIfGroupCmdMsg(tstring& strRaw, tstring& strPackID, tstring& strRoomID);

bool CheckIfGroupQianyuePack(tstring& strRaw, tstring& strPackID, tstring& strRoomID, tstring&strStatus);

bool GetDuty(const tstring& strOrgID, const tstring& strAccount, vector<tstring>& vecProperty);

bool GetRecipient(const tstring& organization_id, tstring& recipient);        //获取默认接收人

int GetEncoderClsid(const WCHAR* format, CLSID *pClsid);
BOOL SaveBitmapToFile(HBITMAP hBitmap, LPWSTR lpszFileName);

bool MoveControllers(CLabelUI* head, RECT head_pos, SIZE head_size, vector<CLabelUI*> following, RECT total_pos);

bool MoveControllers(CLabelUI* head, RECT head_pos, SIZE head_size, 
	CLabelUI* recipient, RECT recipient_pos, SIZE recipient_size, 
	RECT total_pos);

void ReplaceAll(tstring& str, const tstring& old_value, const tstring& new_value);
void ReplaceAllDistinct(tstring& str, const tstring& old_value, const tstring& new_value);

std::tstring StringToTstring(const string &str);
std::tstring WStringToTString(const wstring &wstr);
std::wstring StringToWstring(const string &str);
std::string WStringToString(const wstring &wstr);
