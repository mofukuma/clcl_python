/*
 * tool_text
 *
 * tool_text.c
 *
 * Copyright (C) 1996-2003 by Nakashima Tomoaki. All rights reserved.
 *		http://www.nakka.com/
 *		nakka@nakka.com
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

#include "..\CLCLPlugin.h"

/* Define */
#define	INI_FILE_NAME				TEXT("clcl_python.ini")
#define MAX_SCRIPT 8

/* Global Variables */

HINSTANCE hInst;
TCHAR ini_path[BUF_SIZE];

TCHAR date_format[BUF_SIZE];
TCHAR time_format[BUF_SIZE];

TCHAR quote_char[BUF_SIZE];

int word_break_break_cnt;
int word_break_tab_size;
TCHAR word_break_m_oida[BUF_SIZE];
TCHAR word_break_m_bura[BUF_SIZE];
TCHAR word_break_s_oida[BUF_SIZE];
TCHAR word_break_s_bura[BUF_SIZE];

//TCHAR put_text_open_all[MAX_SCRIPT][BUF_SIZE];
//TCHAR put_text_close_all[MAX_SCRIPT][BUF_SIZE];
int script_index = 0;

TCHAR put_text_open[BUF_SIZE];
TCHAR put_text_close[BUF_SIZE];

int delete_crlf_delete_space;

int join_text_add_return;

/* Local Function Prototypes */
static BOOL dll_initialize(void);
static BOOL dll_uninitialize(void);

/*
 * DllMain - メイン
 */
int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		hInst = hInstance;
		dll_initialize();
		break;

	case DLL_PROCESS_DETACH:
		dll_uninitialize();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


void switch_index(int i) {
	script_index = i;
	//put_text_open = put_text_open_all[i];
	//put_text_close = put_text_close_all[i];
	TCHAR func_name[BUF_SIZE];
	TCHAR script_name[BUF_SIZE];
	wsprintf(func_name, TEXT("clcl_python_%d"), i + 1);
	wsprintf(script_name, TEXT("sc%d"), i + 1);
	GetPrivateProfileString(func_name, TEXT("open"), script_name, put_text_open, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(func_name, TEXT("close"), TEXT("main"), put_text_close, BUF_SIZE - 1, ini_path);
}


/*
 * dll_initialize - 初期化
 */
static BOOL dll_initialize(void)
{
	TCHAR app_path[BUF_SIZE];

	TCHAR *p, *r;

	GetModuleFileName(hInst, app_path, BUF_SIZE - 1);
	for (p = r = app_path; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if (IsDBCSLeadByte((BYTE)*p) == TRUE) {
			p++;
			continue;
		}
#endif	// UNICODE
		if (*p == TEXT('\\') || *p == TEXT('/')) {
			r = p;
		}
	}
	*r = TEXT('\0');
	wsprintf(ini_path, TEXT("%s\\%s"), app_path, INI_FILE_NAME);
	/*
	GetPrivateProfileString(TEXT("convert_date"), TEXT("date_format"), TEXT(""), date_format, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(TEXT("convert_date"), TEXT("time_format"), TEXT(""), time_format, BUF_SIZE - 1, ini_path);

	GetPrivateProfileString(TEXT("quote"), TEXT("char"), TEXT(">"), quote_char, BUF_SIZE - 1, ini_path);

	word_break_break_cnt = GetPrivateProfileInt(TEXT("word_break"), TEXT("break_cnt"), 80, ini_path);
	word_break_tab_size = GetPrivateProfileInt(TEXT("word_break"), TEXT("tab_size"), 8, ini_path);
	GetPrivateProfileString(TEXT("word_break"), TEXT("m_oida"),
		TEXT("‘“￥＄（［｛「〔《『【〈≪"), word_break_m_oida, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(TEXT("word_break"), TEXT("m_bura"),
		TEXT("、。，．？！゛゜…‥・’”：；）〕］｝〉》」』】≫々ゝゞぁぃぅぇぉっゃゅょァィゥェォッャュョー"),
		word_break_m_bura, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(TEXT("word_break"), TEXT("s_oida"), TEXT("\\$([{｢"), word_break_s_oida, BUF_SIZE - 1, ini_path);
	GetPrivateProfileString(TEXT("word_break"), TEXT("s_bura"), TEXT(",.?!%:;)]}｣｡､ﾞﾟ"), word_break_s_bura, BUF_SIZE - 1, ini_path);
	*/
	//GetPrivateProfileString(TEXT("clcl_python"), TEXT("open"), TEXT("sc1"), put_text_open, BUF_SIZE - 1, ini_path);
	//GetPrivateProfileString(TEXT("clcl_python"), TEXT("close"), TEXT("main"), put_text_close, BUF_SIZE - 1, ini_path);


	for (int i=0; i < MAX_SCRIPT; i++) {
		switch_index(i);

		TCHAR func_name[BUF_SIZE];
		TCHAR script_name[BUF_SIZE];
		wsprintf(func_name, TEXT("clcl_python_%d") , i+1);
		wsprintf(script_name, TEXT("sc%d"), i+1);

		GetPrivateProfileString(func_name, TEXT("open"), script_name, put_text_open, BUF_SIZE - 1, ini_path);
		GetPrivateProfileString(func_name, TEXT("close"), TEXT("main"), put_text_close, BUF_SIZE - 1, ini_path);
	}

	/*

	delete_crlf_delete_space = GetPrivateProfileInt(TEXT("delete_crlf"), TEXT("delete_space"), 1, ini_path);

	join_text_add_return = GetPrivateProfileInt(TEXT("join_text"), TEXT("add_return"), 1, ini_path);
	*/


	return TRUE;
}

/*
 * dll_uninitialize - 終了処理
 */
static BOOL dll_uninitialize(void)
{
	return TRUE;
}

/*
 * get_tool_info_w - ツール情報取得
 */
__declspec(dllexport) BOOL CALLBACK get_tool_info_w(const HWND hWnd, const int index, TOOL_GET_INFO *tgi)
{
	if (index >= MAX_SCRIPT)
		return FALSE;



	TCHAR title_name[BUF_SIZE];
	TCHAR func_name[BUF_SIZE];

	wsprintf(title_name, TEXT("Python実行 %d"), index + 1);
	wsprintf(func_name, TEXT("clcl_python_%d"), index + 1);

	lstrcpy(tgi->title, title_name);
	lstrcpy(tgi->func_name, func_name);
	lstrcpy(tgi->cmd_line, TEXT(""));
	tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE;


	return TRUE;


	/*

	switch (index) {

	case 0:
		lstrcpy(tgi->title, TEXT("Python(&P)"));
		lstrcpy(tgi->func_name, TEXT("clcl_python"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 0:
		lstrcpy(tgi->title, TEXT("日時変換(&D)"));
		lstrcpy(tgi->func_name, TEXT("convert_date"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_ITEM_TO_CLIPBOARD;
		return TRUE;

	case 1:
		lstrcpy(tgi->title, TEXT("小文字に変換(&L)"));
		lstrcpy(tgi->func_name, TEXT("to_lower"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 2:
		lstrcpy(tgi->title, TEXT("大文字に変換(&U)"));
		lstrcpy(tgi->func_name, TEXT("to_upper"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;
	case 3:
		lstrcpy(tgi->title, TEXT("引用(&Q)"));
		lstrcpy(tgi->func_name, TEXT("quote"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 4:
		lstrcpy(tgi->title, TEXT("引用解除(&N)"));
		lstrcpy(tgi->func_name, TEXT("unquote"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 5:
		lstrcpy(tgi->title, TEXT("テキスト整形(&W)"));
		lstrcpy(tgi->func_name, TEXT("word_break"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 6:
		lstrcpy(tgi->title, TEXT("Python(&P)"));
		lstrcpy(tgi->func_name, TEXT("put_text"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;


	case 7:
		lstrcpy(tgi->title, TEXT("改行の除去(&R)"));
		lstrcpy(tgi->func_name, TEXT("delete_crlf"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE | CALLTYPE_VIEWER;
		return TRUE;

	case 8:
		lstrcpy(tgi->title, TEXT("テキストの連結(&O)"));
		lstrcpy(tgi->func_name, TEXT("join_text"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_VIEWER;
		return TRUE;

	case 9:
		lstrcpy(tgi->title, TEXT("テキスト編集(&E)"));
		lstrcpy(tgi->func_name, TEXT("text_edit"));
		lstrcpy(tgi->cmd_line, TEXT(""));
		tgi->call_type = CALLTYPE_MENU | CALLTYPE_MENU_COPY_PASTE;
		return TRUE;
	}
		*/

	return FALSE;
}
/* End of source */
