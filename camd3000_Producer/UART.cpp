#include <Windows.h>
#include <tchar.h>
#include "resource.h"
#include "main.h"

//---------Header & Tail
#define STX 0x02   			
#define ETX 0x03   			

//---------Message Class
#define RLM				    			0x00
#define REQ  				    		0x01
#define REP		  		    		    0x02
#define NTF 				    		0x03
#define STS 				    		0x04

//---------Module ID
#define MICOM_ID						0x01

HANDLE hThread;
HANDLE hCom;
OVERLAPPED ovRead;
OVERLAPPED ovWrite;

_stComm stComm;


//uart init
void  threadComm(HANDLE hCom)
{
	TCHAR szStr[MAX_PATH] = { 0, };
	TCHAR buff[MAX_PATH] = { 0, };
	DCB sPstate;
  OVERLAPPED ov;
  ZeroMemory(&ov, sizeof ov);

  ov.hEvent = CreateEvent(0, TRUE, FALSE, 0);
  SetCommMask(hCom, EV_RXCHAR);

  DWORD event;
  DWORD readByte;
  DWORD dwError;
  COMSTAT comState; 
	int i;

	stComm.bThreadEnable = 1;
  while(stComm.bThreadEnable)
  {
  	event = 0;

		if (hCom == INVALID_HANDLE_VALUE) {
			Sleep(5);
			continue;
		}
		WaitCommEvent(hCom, &event, &ov);
		if (!GetCommState(hCom, &sPstate)) {
			UartClose(0);

			SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("OPEN"));
			if (g_jobOpen)
				EnableWindow(GetDlgItem(hDlgMain, IDC_BTN_HW_TEST), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_UDR), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_LTE), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_ESIM), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_ACCEROMETER), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_OBDCOMM), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_LED), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SET_SN), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_INDEX), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_MID), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_NO), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_SERIAL), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_LED), false);
			EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_BLE), false);
			
			m_jigmode = 0;
			g_snCheck = 0;
			
			hCom = INVALID_HANDLE_VALUE;
			stComm.bAutoConnectedReq = TRUE;
			stComm.nHwtestresultReq = 0;
			stComm.LEDCheckReqTm = 0;
			continue;
		}
		if ((event & EV_RXCHAR) == EV_RXCHAR) {
      ClearCommError(  hCom, &dwError, &comState);
      if (stComm.dwRxByte = comState.cbInQue) {
				memset(stComm.gRxBuff,0,sizeof(stComm.gRxBuff));
        ReadFile(hCom, stComm.gRxBuff, stComm.dwRxByte , &readByte, &ovRead) ;
				//for( i=0; i<stComm.dwRxByte ;i++)
				//	MultiByteToWideChar(CP_ACP,0,(LPSTR)stComm.gRxBuff+i,-1,(LPWSTR)(stComm.gURxBuff+i),1);
        SendMessage( hDlgMain, WM_COMRECEIVE, 0, (LPARAM)&stComm);
      }
    }   
  }
  CloseHandle(ov.hEvent);
}

BOOL  ComWirte(TCHAR *str, int length)
{
	BOOL ret;
	DWORD error;
	DWORD written;

	ret = WriteFile(hCom, str, length, &written, &ovWrite);
	error = GetLastError();
	if( error==ERROR_IO_PENDING )
		ret = TRUE;
	return ret;
}

void UartClose(char skip )
{
	DWORD dwErrorCode = 0;

	if (hCom == INVALID_HANDLE_VALUE)
		return;
	if (!skip) {
		stComm.bThreadEnable = 0;
	}
	SetCommMask(hCom, 0);
  PurgeComm(hCom, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	CloseHandle(hCom);
	hCom = INVALID_HANDLE_VALUE;
	stComm.bAutoConnectedReq = false;
}

BYTE UartSetup(TCHAR *port,HWND hWnd,int skip)
{ 
	DCB sPstate;
	COMMTIMEOUTS CommTimeOuts;
	unsigned int length =0;
	DWORD dwTimeout;

	hCom = CreateFile(port,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
	if(hCom==INVALID_HANDLE_VALUE)
	{
		//MessageBox(hWnd,TEXT("포트 열 수 없음"),TEXT("Error"),MB_OK);
		return 0;
	}
	
	SetCommMask(hCom, EV_RXCHAR);
  SetupComm(hCom, 4096, 4096);
  PurgeComm(hCom, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// Setup Time Out
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant   = 0 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant   = 0;
	SetCommTimeouts( hCom, &CommTimeOuts ) ;

	// Setup Connection
	sPstate.DCBlength = sizeof(sPstate);
	if( GetCommState(hCom,&sPstate) )
	{
	}
	else
	{
		//MessageBox(hWnd,TEXT("포트 상태 읽기 에러"),TEXT("Error"),MB_OK);
		return 0;
	}
	sPstate.BaudRate = CBR_115200;
	sPstate.ByteSize = 8;
	sPstate.Parity = NOPARITY;
	sPstate.StopBits = ONESTOPBIT;
	if( SetCommState(hCom,&sPstate)==0 )
	{
		//MessageBox(hWnd,TEXT("포트 상태 설정 에러"),TEXT("Error"),MB_OK);
		return 0;
	}

	if (!skip) {
		DWORD threadID;
		hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadComm, (LPVOID)hCom, 0, &threadID);
	}

	return 1;
}