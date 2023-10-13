#include <windows.h>
#include "resource.h"
#include "main.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hDlgMain;
LPSTR lpszClass = (LPSTR)TEXT("First");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	HRESULT hr = CoInitialize(NULL);

	g_hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_CAMD3000_PRODUCTION), HWND_DESKTOP, MainDlgProc);
	// 프로그램을 종료하기 전에 COM 라이브러리를 해제함.
	CoUninitialize();
	return 0;
#if 0
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
#endif
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE:
		//DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TUNEIT_PRODUCTION), hWnd, MainDlgProc);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//PDEV_BROADCAST_PORT pHdr;
	//PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	TCHAR szStr[MAX_PATH] = { 0, };
	TCHAR buff[4096];
	static int i = 0;
	int ret , len;
	HWND hDlgItem;

	switch (iMessage)
	{
	case WM_INITDIALOG:
#if 1
		GetWindowText(hDlg, buff,4096);
		len = strlen(buff);
		len += wsprintf(&buff[len], TEXT("                 "));
#if defined FOR_FAST_ASSEMBLY
		len += wsprintf(&buff[len], TEXT("MAKE ASSEMBLE(지그검사를 진행하지않음)"));
#else
#if defined CHK_APP_VER_3
		len += wsprintf(&buff[len], TEXT("APP(%d_%d_%d_%d)"), CHK_APP_VER_0, CHK_APP_VER_1, CHK_APP_VER_2, CHK_APP_VER_3); 
#elif defined CHK_APP_VER_2
		len += wsprintf(&buff[len], TEXT("APP(%d_%d_%d_x)"), CHK_APP_VER_0, CHK_APP_VER_1, CHK_APP_VER_2); 
#elif defined CHK_APP_VER_1
		len += wsprintf(&buff[len], TEXT("APP(%d_%d_x_x)"), CHK_APP_VER_0, CHK_APP_VER_1);
#elif defined CHK_APP_VER_0
		len += wsprintf(&buff[len], TEXT("APP(%d_x_x_x)"), CHK_APP_VER_0);
#endif
#if defined CHK_OBD_VER_3
		len += wsprintf(&buff[len], TEXT("OBD(%d_%d_%d_%d)"), CHK_OBD_VER_0, CHK_OBD_VER_1, CHK_OBD_VER_2, CHK_OBD_VER_3);
#elif defined CHK_OBD_VER_2
		len += wsprintf(&buff[len], TEXT("OBD(%d_%d_%d_x)"), CHK_OBD_VER_0, CHK_OBD_VER_1, CHK_OBD_VER_2);
#elif defined CHK_OBD_VER_1
		len += wsprintf(&buff[len], TEXT("OBD(%d_%d_x_x)"), CHK_OBD_VER_0, CHK_OBD_VER_1);
#elif defined CHK_OBD_VER_0
		len += wsprintf(&buff[len], TEXT("OBD(%d_x_x_x)"), CHK_OBD_VER_0);
#endif
#endif
#ifdef __LAP_TEST__
		len += wsprintf(&buff[len], TEXT("__LAP_TEST__"));
#endif

		SetWindowText(hDlg, buff);
#endif
#ifdef SAVE_LOG
		make_log_file();

#endif
		hDlgMain = hDlg;
		SetClassLongPtr(hDlg, GCLP_HICON, (LONG)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON2)));
		SendDlgItemMessage(hDlgMain, IDC_EDIT_SERIAL_INDEX, EM_SETLIMITTEXT, 2, 0);   //4자리수 입력
		SendDlgItemMessage(hDlgMain, IDC_EDIT_SERIAL_MID, EM_SETLIMITTEXT, 4, 0);   //4자리수 입력
		SendDlgItemMessage(hDlgMain, IDC_EDIT_SERIAL_NO, EM_SETLIMITTEXT, 4, 0);   //4자리수 입력
		SendDlgItemMessage(hDlgMain, IDC_CHECK_FW_SKIP, BM_SETCHECK, BST_CHECKED, 0); g_fwSkip = 1;
		SendDlgItemMessage(hDlgMain, IDC_CHECK_HW_SKIP, BM_SETCHECK, BST_CHECKED, 0); g_hwSkip = 1;
		SendDlgItemMessage(hDlgMain, IDC_EDIT_LOGVIEWER, EM_SETLIMITTEXT, 0, 0);       //edit control 버퍼 증가   
		SendDlgItemMessage(hDlgMain, IDC_COMBO_BAUARATE, CB_ADDSTRING, 0, (LPARAM)"115200");
		SetDlgItemText(hDlgMain, IDC_COMBO_BAUARATE, (LPCSTR)"115200");

#if (defined _DEBUG | defined FOR_FAST_ASSEMBLY)
		hDlgItem =GetDlgItem(hDlgMain, IDC_BUTTON_SETJIGMODE);
		ShowWindow(hDlgItem, SW_SHOW);
#endif 
		
		getLastSerialNeumer();
		SetTimer(hDlgMain, 1, 500, MainTimerProc);
#if 1
		GetComPort(szStr, TEXT("0416"), TEXT("5011"));
		if (szStr[0]!=NULL) {
			SendDlgItemMessage(hDlgMain, IDC_COMBO_COM_PORT, CB_GETLBTEXT, 0, (LPARAM)szStr);
			SetDlgItemText(hDlgMain, IDC_COMBO_COM_PORT, (LPCSTR)szStr);
			if (doUartConnection(0) == false)
				MessageBox(hDlg, TEXT("COM포트 이미 열려있음!!"), TEXT("ERROR"), MB_OK);
			else
				SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("CLOSE"));
		}
		else {
			MessageBox(hDlg, TEXT("CAMD-3000 연결되지 않음"), TEXT("ERROR"), MB_OK);
			//PostMessageA(hDlg,WM_CLOSE,0,0);//
		}
#else
		ComboClassComPortEnumerate(IDC_COMBO_COM_PORT);
		if (SendDlgItemMessage(hDlgMain, IDC_COMBO_COM_PORT, CB_GETCOUNT, 0, 0)) {
			SendDlgItemMessage(hDlgMain, IDC_COMBO_COM_PORT, CB_GETLBTEXT, 0, (LPARAM)buff);
			SetDlgItemText(hDlgMain, IDC_COMBO_COM_PORT, (LPCSTR)buff);
			if (doUartConnection(0) == false)
				MessageBox(hDlg, TEXT("COM포트 에러!!"), TEXT("ERROR"), MB_OK);
			else
				SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("CLOSE"));
		}
#endif
		return TRUE;
	case WM_CLOSE:
		//save_Excel(1, NULL, NULL, NULL, NULL, NULL);
		KillTimer(hDlgMain,1);
		UartClose(0);
		EndDialog(hDlg, 0);
		return TRUE;
	case WM_QUERYENDSESSION:
		//save_Excel(1, NULL, NULL, NULL, NULL, NULL);
		return TRUE;
	case WM_COMMAND:
		doControlCommand(wParam);
		return TRUE;
	case WM_COMRECEIVE:
		ret = parseBPProtocol();
		if (!ret) {
			if (stComm.dwRxByte < 2000) {
				EditClassPrintf1(IDC_EDIT_LOGVIEWER, (TCHAR*)stComm.gRxBuff); 
#ifdef SAVE_LOG
				save_log((TCHAR*)stComm.gRxBuff);
#endif
			}
			else
				stComm.dwRxByte = 0;
		}
		return TRUE;
#if 0
	case WM_DEVICECHANGE:
		switch (wParam) {
		case DBT_DEVICEARRIVAL:
			pHdr = (PDEV_BROADCAST_PORT)lParam;
			if (pHdr->dbcp_devicetype == DBT_DEVTYP_PORT) {
				pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
				//GetVid(pDevInf->dbcc_name)
			}
			SetWindowText(hDlg, TEXT("DBT_DEVICEARRIVAL"));
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			SetWindowText(hDlg, TEXT("DBT_DEVICEREMOVECOMPLETE "));
			break;
		}
		return TRUE;
#endif
	}
	return FALSE;
}


bool doUartConnection(int skip)
{
	TCHAR szStr[MAX_PATH];
	TCHAR buff[MAX_PATH];

	GetDlgItemText(hDlgMain, IDC_COMBO_COM_PORT, (LPSTR)buff, 100);
	wsprintf(szStr, TEXT("\\\\.\\%s"), buff);
	if (!UartSetup(szStr, hDlgMain, skip)) {
		return false;
	}
	return true;
}
