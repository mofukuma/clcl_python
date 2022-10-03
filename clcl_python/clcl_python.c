/*
 * tool_text
 *
 * put_text.c
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

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include "resource.h"

/* Define */

/* Global Variables */
extern HINSTANCE hInst;
extern TCHAR ini_path[];

extern TCHAR put_text_open[];
extern TCHAR put_text_close[];

extern int script_index;

extern void switch_index(int i);


/* Local Function Prototypes */

/*
 * set_put_text_proc - 挟み込む文字を設定
 */
static BOOL CALLBACK set_put_text_proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	TCHAR func_name[BUF_SIZE];
	wsprintf(func_name, TEXT("clcl_python_%d"), script_index + 1);

	switch (uMsg) {
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT_OPEN, WM_SETTEXT, 0, (LPARAM)put_text_open);
		SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_SETTEXT, 0, (LPARAM)put_text_close);
		break;

	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			SendDlgItemMessage(hDlg, IDC_EDIT_OPEN, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)put_text_open);
			SendDlgItemMessage(hDlg, IDC_EDIT_CLOSE, WM_GETTEXT, BUF_SIZE - 1, (LPARAM)put_text_close);

			WritePrivateProfileString(func_name, TEXT("open"), put_text_open, ini_path);
			WritePrivateProfileString(func_name, TEXT("close"), put_text_close, ini_path);
			EndDialog(hDlg, TRUE);
			break;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


// python文字列→TCHAR
// 参考：https://github.com/mhammond/pywin32
BOOL PyWinObject_AsWCHAR(PyObject *stringObject, WCHAR **pResult, BOOL bNoneOK /*= FALSE*/,
	DWORD *pResultLen /*= NULL*/)
{
	BOOL rc = TRUE;
	Py_ssize_t resultLen = 0;

	// Do NOT accept 'bytes' for any 'WCHAR' API.
	if (PyUnicode_Check(stringObject)) {
		*pResult = PyUnicode_AsWideCharString(stringObject, &resultLen);
		if (*pResult == NULL) {
			PyErr_SetString(PyExc_MemoryError, "Getting WCHAR string");
			return FALSE;
		}
	}
	else if (stringObject == Py_None) {
		if (bNoneOK) {
			*pResult = NULL;
		}
		else {
			PyErr_SetString(PyExc_TypeError, "None is not a valid string in this context");
			rc = FALSE;
		}
	}
	else {
		const char *tp_name = stringObject && stringObject->ob_type ? stringObject->ob_type->tp_name : "<NULL!!>";
		PyErr_Format(PyExc_TypeError, "Objects of type '%s' can not be converted to Unicode.", tp_name);
		rc = FALSE;
	}
	if (rc && pResultLen) {

		*pResultLen = (DWORD)resultLen;
	}
	return rc;
}

void PyWinObject_FreeWCHAR(WCHAR *str) { PyMem_Free(str); }


TCHAR* run_python_1(const TCHAR* run_script, const TCHAR *run_method, const TCHAR *text) {

	//　python 実行準備
	Py_Initialize();
	wchar_t* program = Py_DecodeLocale("./python/python", NULL);
	Py_SetProgramName(program);

	PyObject *sys = PyImport_ImportModule("sys");
	PyObject *sys_path = PyObject_GetAttrString(sys, "path");
	PyObject *dir = PyUnicode_DecodeFSDefault("./python/");
	PyObject *dir2 = PyUnicode_DecodeFSDefault(".");

	PyList_Append(sys_path, dir);
	PyList_Append(sys_path, dir2);

	TCHAR *ret;

	PyObject *runscriptobj = PyUnicode_FromWideChar(run_script, -1);

	PyObject *pModule = PyImport_Import(runscriptobj);
	if (pModule == NULL) {
		PyWinObject_AsWCHAR(runscriptobj, &ret, FALSE, NULL);
		Py_Finalize();
		return ret;
	}

	PyObject *inputtext = PyUnicode_FromWideChar(text, -1);

	PyObject *method = PyUnicode_FromWideChar(run_method, -1);
	PyObject *pTmp = PyObject_CallMethodObjArgs(pModule, method, inputtext, NULL);
	if (pTmp == NULL) {
		PyWinObject_AsWCHAR(method, &ret, FALSE, NULL);
		Py_Finalize();
		return ret;
	}

	PyObject *txtobj;

	PyArg_Parse(pTmp, "O", &txtobj);

	PyWinObject_AsWCHAR(txtobj, &ret, FALSE, NULL);

	Py_DECREF(method);
	Py_DECREF(txtobj);
	Py_DECREF(inputtext);

	Py_Finalize();

	return ret;
}


/*
 *  python実行して結果をテキストでクリップボードへ
 */
static int run_python(DATA_INFO *di, const TCHAR *str_open, const TCHAR *str_close)
{
	HANDLE ret;
	BYTE *from_mem, *to_mem;
	TCHAR *r;
	TCHAR *text;

	TCHAR* buf;

	int size;


	// コピー元ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}

	size = lstrlen((TCHAR *)from_mem) + 1;

	buf = (TCHAR*)malloc(sizeof(TCHAR) * size);

	lstrcpy(buf, (TCHAR *)from_mem);


	GlobalUnlock(di->data);
	// クリップボード取得完了



	// コピー元再ロック
	if ((from_mem = GlobalLock(di->data)) == NULL) {
		return TOOL_ERROR;
	}
	//python 実行
	text = run_python_1(str_open, str_close, buf);


	// サイズを取得
	size = lstrlen((TCHAR *)text) + 1;

	// コピー先確保
	if ((ret = GlobalAlloc(GHND, sizeof(TCHAR) * size)) == NULL) {
		GlobalUnlock(di->data);
		return TOOL_ERROR;
	}
	// コピー先ロック
	if ((to_mem = GlobalLock(ret)) == NULL) {
		GlobalFree(ret);
		GlobalUnlock(di->data);
		return TOOL_ERROR;
	}

	r = lstrcpy((TCHAR *)to_mem, text) + lstrlen(text);

	GlobalUnlock(ret);
	GlobalUnlock(di->data);

	GlobalFree(di->data);
	di->data = ret;
	di->size = sizeof(TCHAR) * size;

	PyWinObject_FreeWCHAR(text);
	free(buf);

	return TOOL_DATA_MODIFIED;
}

/*
 * clcl_python　埋め込みpythonスクリプト実行
 */
int clcl_python(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi)
{
	DATA_INFO *di;
	TCHAR str_open[BUF_SIZE];
	TCHAR str_close[BUF_SIZE];
	TCHAR *p, *r;
	int ret = TOOL_SUCCEED;

	// 設定の２つの文字列の取得
	if (tei->cmd_line != NULL && *tei->cmd_line != TEXT('\0')) {
		for (p = tei->cmd_line, r = str_open; *p != TEXT('\0') && *p != TEXT(','); p++) {
			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) {
				p++;
			}
			*(r++) = *p;
		}
		*r = TEXT('\0');
		if (*p == TEXT(',')) {
			p++;
		}
		for (r = str_close; *p != TEXT('\0'); p++) {
			if (*p == TEXT('\\') && *(p + 1) == TEXT(',')) {
				p++;
			}
			*(r++) = *p;
		}
		*r = TEXT('\0');

	} else {
		/*
		if ((tei->cmd_line == NULL || *tei->cmd_line == TEXT('\0')) &&
			((tei->call_type & CALLTYPE_MENU) || (tei->call_type & CALLTYPE_VIEWER))) {
			if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), GetForegroundWindow(), set_put_text_proc, 0) == FALSE) {
				return TOOL_CANCEL;
			}
		}*/
		lstrcpy(str_open, put_text_open);
		lstrcpy(str_close, put_text_close);
	}



	for (; tdi != NULL; tdi = tdi->next) {
		if (SendMessage(hWnd, WM_ITEM_CHECK, 0, (LPARAM)tdi->di) == -1) {
			continue;
		}

#ifdef UNICODE
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("UNICODE TEXT"), (LPARAM)tdi->di);
#else
		di = (DATA_INFO *)SendMessage(hWnd, WM_ITEM_GET_FORMAT_TO_ITEM, (WPARAM)TEXT("TEXT"), (LPARAM)tdi->di);
#endif
		if (di != NULL && di->data != NULL) {
			ret |= run_python(di, str_open, str_close);
		}
	}


	return ret;
}
__declspec(dllexport) int CALLBACK clcl_python_1(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(0);  return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_2(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(1);  return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_3(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(2); return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_4(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(3); return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_5(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(4); return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_6(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(5); return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_7(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(6); return clcl_python(hWnd, tei, tdi); }
__declspec(dllexport) int CALLBACK clcl_python_8(const HWND hWnd, TOOL_EXEC_INFO *tei, TOOL_DATA_INFO *tdi) { switch_index(7); return clcl_python(hWnd, tei, tdi); }




BOOL clcl_python_property(const HWND hWnd, TOOL_EXEC_INFO *tei) {
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PUT_TEXT), hWnd, set_put_text_proc, 0);
	return TRUE;
}
/*
 * put_text_property - プロパティ表示
 */
__declspec(dllexport) BOOL CALLBACK clcl_python_1_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(0);   return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_2_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(1);  return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_3_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(2);   return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_4_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(3);  return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_5_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(4);  return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_6_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(5);   return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_7_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(6);   return clcl_python_property(hWnd, tei); }
__declspec(dllexport) BOOL CALLBACK clcl_python_8_property(const HWND hWnd, TOOL_EXEC_INFO *tei) { switch_index(7);   return clcl_python_property(hWnd, tei); }



/* End of source */
