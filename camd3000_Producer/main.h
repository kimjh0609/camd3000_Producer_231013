#ifndef __MAIN__
#define __MAIN__

#include <shobjidl.h>
#include <Dbt.h> 
#include "UART.h"
#include "bp_protocol.h"
#include ".\srcEXCEL\ExcelFormat.h"
#include "make_assembly_csv.h"

#define WM_COMRECEIVE WM_USER+1

//#define __LAP_TEST__
//#define FOR_FAST_ASSEMBLY //
#define SAVE_INTO_CSV
#define SAVE_LOG
#define SAVE_INTO_EXCEL

#define CHK_APP_VER_0  3
#define CHK_APP_VER_1  1
#define CHK_APP_VER_2  0
#define CHK_APP_VER_3  22

////////////////½Â¿ë/////////////////
#define CHK_OBD_VER_0  3
#define CHK_OBD_VER_1  1
#define CHK_OBD_VER_2  3
#define CHK_OBD_VER_3  7

////////////////»ó¿ë/////////////////
//#define CHK_OBD_VER_0  3
//#define CHK_OBD_VER_1  51
//#define CHK_OBD_VER_2  3
//#define CHK_OBD_VER_3  10

extern HWND hDlgMain;
extern HINSTANCE g_hInst;
extern HANDLE hThread;
extern char m_jigmode,g_snCheck;
extern char g_jobOpen;
extern char g_fwSkip, g_hwSkip;

void UartClose(char skip);
BYTE UartSetup(TCHAR *port,HWND hWnd,int skip);
void EditClassPrintf (int EditClass,TCHAR * szFormat, ...);
void EditClassPrintf1(int EditClass, TCHAR* szFormat);
void EditClassPrintf2(int EditClass, TCHAR* szFormat, ...);
void EditClassLogClear(int EditClass);
void ComboClassComPortEnumerate(int Control);
void OnBnClickedBtnSetmdmuart(void);
void OnBnClickedBtnDbgfff(void);
void EditClassLastPositionMove(HWND hDlg, int iDlgItem);
int parseBPProtocol(void);
void doControlCommand(WPARAM wparam);
unsigned short crc16_ccitt(unsigned char* buf, unsigned short len);
bool doUartConnection(int skip);
bool GetFileOpen(COMDLG_FILTERSPEC* filter, TCHAR* szFileName);
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
VOID CALLBACK MainTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
int isFileExist(char* file,char *buffer);
void GetComPort(TCHAR* pszComePort, TCHAR* vid, TCHAR* pid);
void saveRegistryKey(char* serial);
int readRegistryKey(char* serial);
void getLastSerialNeumer(void);
int save_Excel(int save, char* serial, char* empver, char* ebpver, char* imei, char* iccid, char* ret_str_err);
//void make_excel_file();
#endif