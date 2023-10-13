#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <Commctrl.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <shlwapi.h>
#include <initguid.h>
#include <windows.h>
#include <setupapi.h>
#include "resource.h"
#include "main.h"

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "Shlwapi.lib")

void EditClassLogClear(int EditClass)
{
  SendDlgItemMessage(hDlgMain, EditClass, EM_SETSEL, 0, -1);
  SendDlgItemMessage(hDlgMain, EditClass, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)TEXT(""));
}

void EditClassLastPositionMove(HWND hDlg,int iDlgItem)
{
	int len;

	len = GetWindowTextLength(GetDlgItem(hDlg,iDlgItem));
	SendDlgItemMessage(hDlg,iDlgItem,EM_SETSEL,len,len);
}

void EditClassPrintf1(int EditClass, TCHAR* szFormat)
{
  if (m_jigmode)
    return;
  EditClassLastPositionMove(hDlgMain, EditClass);
  SendDlgItemMessage(hDlgMain, EditClass, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szFormat);

}

//TCHAR   szBuffer[4096 * 3];
void EditClassPrintf (int EditClass,TCHAR * szFormat, ...)
{
	TCHAR   szBuffer[4096+1024];
  va_list pArgList ;

  //if (m_jigmode)
  //  return;
     
  va_start (pArgList, szFormat) ;
  // The last argument to wvsprintf points to the arguments
  //_vsntprintf_s (szBuffer, sizeof (szBuffer) / sizeof (TCHAR), szFormat, pArgList) ;
  memset(szBuffer, 0, sizeof(szBuffer));
  _vsntprintf_s(szBuffer, 4096 * 3, szFormat, pArgList);
  va_end (pArgList) ;
    
  EditClassLastPositionMove(hDlgMain,EditClass);
	SendDlgItemMessage(hDlgMain,EditClass,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)szBuffer);
#ifdef SAVE_LOG
    save_log(szBuffer);
#endif
}

void EditClassPrintf2(int EditClass, TCHAR* szFormat, ...)
{
  TCHAR   szBuffer[4096 + 1024];
  va_list pArgList;

  va_start(pArgList, szFormat);
  // The last argument to wvsprintf points to the arguments
  //_vsntprintf_s (szBuffer, sizeof (szBuffer) / sizeof (TCHAR), szFormat, pArgList) ;
  memset(szBuffer, 0, sizeof(szBuffer));
  _vsntprintf_s(szBuffer, 4096 * 3, szFormat, pArgList);
  va_end(pArgList);

  EditClassLastPositionMove(hDlgMain, EditClass);
  SendDlgItemMessage(hDlgMain, EditClass, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szBuffer);
#ifdef SAVE_LOG
  save_log(szBuffer);
#endif
}

void ComboClassComPortEnumerate(int Control)
{
	static char comPorts[1000/*255*/]={0,};
  TCHAR szStr[MAX_PATH] = { 0, };
  TCHAR buff[4096];
  int i,j,ii;

  for (i=0; i < 1000/*255*/; i++) {
    wsprintf(szStr, TEXT("COM%d"), i);
    j = QueryDosDevice(szStr, (LPSTR)buff, 4000);
    if (j != 0) //QueryDosDevice returns zero if it didn't find an object
    {
    	for( ii=0;ii<1000/*255*/;ii++ ){
    		if( i==comPorts[ii] )
    			break;
    	}
      if (ii == 1000/*255*/) {
        comPorts[i] = i;
        SendDlgItemMessage(hDlgMain, Control, CB_ADDSTRING, 0, (LPARAM)szStr);
      }
    }
  }
}

bool GetFileOpen(COMDLG_FILTERSPEC *filter,TCHAR * szFileName)
{
  int len;
  bool ret=false;
  //COMDLG_FILTERSPEC ComDlgFS[3] = { {L"C++ code files", L"*.cpp;*.h;*.rc"},{L"Executable Files", L"*.exe;*.dll"}, {L"All Files",L"*.*"} };
  COMDLG_FILTERSPEC ComDlgFS[2] = { {L"app file", L"*.app"},{L"All Files",L"*.*"} };
  ComDlgFS[0].pszName = filter->pszName;
  ComDlgFS[0].pszSpec = filter->pszSpec;

  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
    COINIT_DISABLE_OLE1DDE);
  if (SUCCEEDED(hr))
  {
    IFileOpenDialog* pFileOpen;

    // Create the FileOpenDialog object.
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
      IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
      pFileOpen->SetFileTypes(2, ComDlgFS);
      // Show the Open dialog box.
      hr = pFileOpen->Show(NULL);

      // Get the file name from the dialog box.
      if (SUCCEEDED(hr))
      {
        IShellItem* pItem;
        hr = pFileOpen->GetResult(&pItem);
        if (SUCCEEDED(hr))
        {
          PWSTR pszFilePath;
          hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
          len = wcslen(pszFilePath);

          // Display the file name to the user.
          if (SUCCEEDED(hr))
          {
            WideCharToMultiByte(CP_ACP, 0, pszFilePath, -1, szFileName, len, NULL, NULL);
            //MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
            //EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), szFileName);
            CoTaskMemFree(pszFilePath);
            ret = true;
          }
          pItem->Release();
        }
      }
      pFileOpen->Release();
    }
    CoUninitialize();
  }
  return ret;
}

int isFileExist(char* file,char *buffer)
{
  HANDLE hFind;
  WIN32_FIND_DATA wfd;
  char path[128] = { 0, };
  char strDriver[MAX_PATH] = { 0, };
  char strDir[MAX_PATH] = { 0, };
  char strfilename[MAX_PATH] = { 0, };
  char strfileext[MAX_PATH] = { 0, };
  int len;

  if ((len=GetCurrentDirectory(128, path)) > 0) {
    memcpy(&path[len], file, strlen(file));
    hFind = FindFirstFile(path, &wfd);
    if (hFind != INVALID_HANDLE_VALUE) {
      //EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file Name :%s\r\n"), wfd.cFileName);
      _splitpath_s(path, strDriver,MAX_PATH, strDir, MAX_PATH, strfilename, MAX_PATH, strfileext, MAX_PATH);
      memset(path, 0, sizeof(path));
      sprintf_s(path, "%s%s%s", strDriver, strDir, wfd.cFileName);
      memcpy(buffer, path, strlen(path));
     
      FindClose(hFind);
      return 1;
    }
    FindClose(hFind);
    return 0;
  }
  return 0;
}

//Buffer length
#define BUFF_LEN 20
void GetComPort(TCHAR* pszComePort, char* vid, char* pid)
{
  HDEVINFO DeviceInfoSet;
  DWORD DeviceIndex = 0;
  SP_DEVINFO_DATA DeviceInfoData;
  TCHAR *DevEnum =TEXT("USB");
  char ExpectedDeviceId[80] = { 0 }; //Store hardware id
  BYTE szBuffer[1024] = { 0 };
  DEVPROPTYPE ulPropertyType;
  DWORD dwSize = 0;
  DWORD Error = 0;
  DWORD find = 0;

  //create device hardware id
  /*
  strcpy_s(ExpectedDeviceId, TEXT("vid_"));
  strcpy_s(ExpectedDeviceId, vid);
  strcpy_s(ExpectedDeviceId, TEXT("&pid_"));
  strcpy_s(ExpectedDeviceId, pid);
  */
  sprintf_s(ExpectedDeviceId,"USB\\VID_%s&PID_%s", vid, pid);
  //SetupDiGetClassDevs returns a handle to a device information set

  DeviceInfoSet = SetupDiGetClassDevs(
    NULL,
    DevEnum,
    NULL,
    DIGCF_ALLCLASSES | DIGCF_PRESENT);

  if (DeviceInfoSet == INVALID_HANDLE_VALUE)
    return;

  //Fills a block of memory with zeros
  ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  //Receive information about an enumerated device
  while (SetupDiEnumDeviceInfo(
    DeviceInfoSet,
    DeviceIndex,
    &DeviceInfoData))
  {
    DeviceIndex++;
    //Retrieves a specified Plug and Play device property
    if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
      &ulPropertyType, (BYTE*)szBuffer,
      sizeof(szBuffer),   // The size, in bytes
      &dwSize))
    {
      HKEY hDeviceRegistryKey;
      //Get the key
      hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
      if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
      {
        Error = GetLastError();
        break; //Not able to open registry
      }
      else
      {
        // Read in the name of the port
        TCHAR pszPortName[BUFF_LEN];
        DWORD dwSize = sizeof(pszPortName);
        DWORD dwType = 0;
        if ((RegQueryValueEx(hDeviceRegistryKey, TEXT("PortName"), NULL, &dwType, (LPBYTE)pszPortName, &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          // Check if it really is a com port
          if (_tcsnicmp(pszPortName, _T("COM"), 3) == 0)
          {
            if (strstr((char *)szBuffer, ExpectedDeviceId)) {
              int nPortNr = _ttoi(pszPortName + 3);
              if (nPortNr != 0)
              {
                _tcscpy_s(pszComePort, BUFF_LEN, pszPortName);
              }
            }
          }
        }
        // Close the key now that we are finished with it
        RegCloseKey(hDeviceRegistryKey);
      }
    }
  }
  if (DeviceInfoSet)
  {
    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
  }
}

void saveRegistryKey(char* serial)
{
  LONG error = 0;
  HKEY hKey;
  DWORD dwDisp, dwData;
  char lpData[11] = { 0, };
  /*
  error = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\tuneitCnt01", 0, KEY_ALL_ACCESS, &hKey);
  if (error != ERROR_SUCCESS)
    error = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\BP\\tuneitCnt01", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
  */
  /*
  error = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\pmgrow", 0, KEY_ALL_ACCESS, &hKey);
  if (error != ERROR_SUCCESS)
    error = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\BP\\pmgrow", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
  */
  error = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\camd3000", 0, KEY_ALL_ACCESS, &hKey);
  if (error != ERROR_SUCCESS)
      error = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\BP\\camd3000", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
  //dwData = 0x1;
  //error = RegSetValueEx(hKey, "serialNumber", 0, REG_DWORD, &dwData, 4);
  memcpy(lpData, serial, 10);
  error = RegSetValueEx(hKey, "serialNumber", 0, REG_SZ, (BYTE*)lpData, strlen(lpData));
  RegCloseKey(hKey);
}

int readRegistryKey(char* serial)
{
  DWORD dwType, cbData;
  HKEY hSubKey;
  long lRet;
  int ret;
  char pszString[255] = { 0, };

  //if ((lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\tuneitCnt01", 0, KEY_READ, &hSubKey)) == ERROR_SUCCESS)
  //if ((lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\pmgrow", 0, KEY_READ, &hSubKey)) == ERROR_SUCCESS)
  if ((lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\BP\\camd3000", 0, KEY_READ, &hSubKey)) == ERROR_SUCCESS)
  {
    cbData = 255;
    if ((lRet = RegQueryValueEx(hSubKey, "serialNumber", NULL, NULL, (LPBYTE)pszString, &cbData)) == ERROR_SUCCESS) {
      //success
      memcpy(serial, pszString, strlen(pszString));
      ret = 0;
    }
    else {
      //failed
      ret = 1;
    }
    RegCloseKey(hSubKey);
    return ret;
  }
  return 1;
}


