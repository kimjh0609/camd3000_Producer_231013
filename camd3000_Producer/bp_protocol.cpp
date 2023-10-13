#include <windows.h>
#include <cstdio>
#include <shobjidl.h> 
#include <sys/stat.h>
#include "resource.h"
#include "main.h"
#include <tchar.h>

#define DBG_DEBUG	            0b00000001
#define QMT_DEBUG	            0b00000010
#define OBD_DEBUG	            0b00000100
#define UDR_DEBUG	            0b00001000
#define AT_DEBUG	            0b00010000	// ATC 수신 

#define DBG_STEP_NON          0x00
#define DBG_STEP_NORMAL       0x01 //다른 프로세서 안돌리기 
#define DBG_STEP_MdmUartMode  0x10 //다른 프로세서 안돌리기 
#define DBG_STEP_ObdUartMode  0x20 // 다른 것은 정상이고, 디버그 출력만 안하도록 ..
#define DBG_STEP_UdrUartMode  0x40 
#define DBG_STEP_JigMode      0xf1

char m_dbgFlag = DBG_DEBUG;
char m_jigmode;
char g_jobOpen;
char g_fwSkip, g_hwSkip;
char g_snCheck;
unsigned char m_CertificatefileNUM;
char g_fotaPath[128];
FILE_INFO stFILE_INFO;
FILE_INFO stFILE_INFO0;
FILE_INFO stFILE_INFO1;
FILE_INFO stFILE_INFO2;
FILE_INFO stFILE_INFO3;
FILE_INFO stFILE_INFO4;
FILE_INFO stFILE_INFO5;
FILE_INFO stFILE_INFO6;

void send_SET_DBG(USHORT setTime, char dbgFlag, char dbgStep)
{ 
  char buff[SEND_MAX_SIZE];
  int nsend = 2;

  EditClassPrintf(IDC_EDIT_LOGVIEWER, (TCHAR *)TEXT("--------------> send_SET_DBG(%u,%#02X,%02X)\r\n"), setTime, dbgFlag, dbgStep);
  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_SET_DBG;	

  //psnd_pkt->data_len1 = 0;
  //psnd_pkt->data_len2 = nsend;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = 0;// total_no;

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &setTime, 2);
  dst += 2;
  nsend += 2;
  u16len += 2;
  //if (dbgFlag)
  {
    *dst++ = dbgFlag;
    nsend++;
    u16len++;
  }
  *dst++ = dbgStep;
  nsend++;
  u16len++;

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;
  ComWirte(buff, nsend);
}


void OnBnClickedButtonsetjigmode(void)
{
  //send_SET_DBG(3600, m_dbgFlag, DBG_STEP_JigMode);
}

void OnBnClickedReset(void)
{
  char buff[SEND_MAX_SIZE];
  int nsend = 0;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_RESET;

  psnd_pkt->data_len1 = 0;
  psnd_pkt->data_len2 = 0;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;
  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedGetHWStatus(void)
{
  char buff[SEND_MAX_SIZE];
  int nsend = 0;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_SET_HWMODE;

  psnd_pkt->data_len1 = 0;
  psnd_pkt->data_len2 = 0;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;
  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnDbgfff(void)
{
  m_dbgFlag = 0xff;
  send_SET_DBG(0, m_dbgFlag, DBG_STEP_NORMAL);
}

void OnBnClickedBtnSetmdmuart(void)
{
  m_dbgFlag = m_dbgFlag | AT_DEBUG;
  send_SET_DBG(3600, m_dbgFlag, DBG_STEP_MdmUartMode);
}

void OnBnClickedBtSendqflst(void)
{
  TCHAR *str = (TCHAR *)TEXT("AT+QFLST\r\n");
  ComWirte(str, _tcslen(str));
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, (TCHAR *)TEXT("--------------> AT+QFLST\r\n"));
}

void nBnClickedBtSendQFDEL(void)
{
  TCHAR* str = (TCHAR*)TEXT("AT+QFDEL=\"*\"\r\n");
  ComWirte(str, _tcslen(str));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, (TCHAR*)TEXT("--------------> AT+QFDEL=\"*\"\r\n"));
}

void OnBnClickedBtnFsend(char *fwfile)
{
  FILE* fp;
  BP_INFO_PACKET file_info;

  if (fwfile == NULL) {
    memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
    COMDLG_FILTERSPEC ComDlgFS = { L"app file", L"*.app" };
    if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
      return;
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);
  }
  else
    memcpy(stFILE_INFO.szFilePath, fwfile, strlen(fwfile));
  
  int nsend = sizeof(BP_INFO_PACKET);

  // Now defining the char pointer to be a buffer for wcstomb/wcstombs
  int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
  if (err != 0) {
    //printf("_test : %s error  = %d\n", path, err);
    //SetCurrentDirectory(strCurrentDir);
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), stFILE_INFO.szFilePath);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  int fsize = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  stFILE_INFO.file_size = fsize;
  stFILE_INFO.cur_no = 0;
  stFILE_INFO.total_no = (fsize % 1024) == 0 ? (fsize / 1024) : (fsize / 1024) + 1;

  char buff[SEND_MAX_SIZE];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file size = %d\r\n"), fsize);
  fseek(fp, 0, SEEK_SET);

  memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  snprintf((char*)file_info.file_name, 8, "BTM-APP");
  file_info.file_type = BP_PACKET_UPDATE_APP;
  file_info.file_len = fsize;
  snprintf((char*)file_info.file_ver, 8, "0.00.11");
  file_info.total_no = stFILE_INFO.total_no;
  file_info.crc = 0x1234;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_UPDATE_APP;
  psnd_pkt->type = 0x1;			// APP Download Cmd = 0x01, OBD Download Cmd = 0x02, FBP Download Cmd = 0x03, 

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &file_info, sizeof(BP_INFO_PACKET));
  dst += sizeof(BP_INFO_PACKET);
  nsend += sizeof(BP_INFO_PACKET);
  u16len += sizeof(BP_INFO_PACKET);

  // file close..
  fclose(fp);

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnfBle(void)
{
  memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
  COMDLG_FILTERSPEC ComDlgFS = { L"nRf Dfu", L"*.ble" };
  if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
    return;
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);

  FILE* fp;
  BP_INFO_PACKET file_info;

  int nsend = sizeof(BP_INFO_PACKET);

  // Now defining the char pointer to be a buffer for wcstomb/wcstombs
  int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
  if (err != 0) {
    //printf("_test : %s error  = %d\n", path, err);
    //SetCurrentDirectory(strCurrentDir);
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), stFILE_INFO.szFilePath);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  int fsize = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  stFILE_INFO.file_size = fsize;
  stFILE_INFO.cur_no = 0;
  stFILE_INFO.total_no = (fsize % 1024) == 0 ? (fsize / 1024) : (fsize / 1024) + 1;

  char buff[SEND_MAX_SIZE];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file size = %d\r\n"), fsize);
  fseek(fp, 0, SEEK_SET);
  int nread = fread((unsigned char *)&file_info, 1, sizeof(BP_INFO_PACKET), fp);
  
  //memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  //snprintf((char*)file_info.file_name, 8, "BTM-BLE");
  //file_info.file_type = BP_PACKET_UPDATE_BLE;
  file_info.file_len = fsize;
  file_info.total_no = stFILE_INFO.total_no;
  //snprintf((char*)file_info.file_ver, 8, "0.00.11");
  //file_info.crc = 0x1234;
  
  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_UPDATE_BLE;
  psnd_pkt->type = 0x1;			// APP Download Cmd = 0x01, OBD Download Cmd = 0x02, FBP Download Cmd = 0x03, 

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &file_info, sizeof(BP_INFO_PACKET));
  dst += sizeof(BP_INFO_PACKET);
  nsend += sizeof(BP_INFO_PACKET);
  u16len += sizeof(BP_INFO_PACKET);

  // file close..
  fclose(fp);

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnfObd(void)
{
  memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
  COMDLG_FILTERSPEC ComDlgFS = { L"rom", L"*.rom" };
  if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
    return;
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);

  FILE* fp;
  BP_INFO_PACKET file_info;

  int nsend = sizeof(BP_INFO_PACKET);

  // Now defining the char pointer to be a buffer for wcstomb/wcstombs
  int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
  if (err != 0) {
    //printf("_test : %s error  = %d\n", path, err);
    //SetCurrentDirectory(strCurrentDir);
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), stFILE_INFO.szFilePath);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  int fsize = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  stFILE_INFO.file_size = fsize;
  stFILE_INFO.cur_no = 0;
  stFILE_INFO.total_no = (fsize % 1024) == 0 ? (fsize / 1024) : (fsize / 1024) + 1;

  char buff[SEND_MAX_SIZE];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file size = %d\r\n"), fsize);
  fseek(fp, 0, SEEK_SET);

  memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  snprintf((char*)file_info.file_name, 8, "BTM-OBD");
  file_info.file_type = BP_PACKET_UPDATE_OBD;
  file_info.file_len = fsize;
  snprintf((char*)file_info.file_ver, 8, "0.00.11");
  file_info.total_no = stFILE_INFO.total_no;
  file_info.crc = 0x1234;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_UPDATE_OBD;
  psnd_pkt->type = 0x1;			// APP Download Cmd = 0x01, OBD Download Cmd = 0x02, FBP Download Cmd = 0x03, 

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &file_info, sizeof(BP_INFO_PACKET));
  dst += sizeof(BP_INFO_PACKET);
  nsend += sizeof(BP_INFO_PACKET);
  u16len += sizeof(BP_INFO_PACKET);

  // file close..
  fclose(fp);

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnfTuneitObd(void)
{
  memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
  COMDLG_FILTERSPEC ComDlgFS = { L"rom", L"*.rom" };
  if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
    return;
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);

  FILE* fp;
  BP_INFO_PACKET file_info;

  int nsend = sizeof(BP_INFO_PACKET);

  // Now defining the char pointer to be a buffer for wcstomb/wcstombs
  int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
  if (err != 0) {
    //printf("_test : %s error  = %d\n", path, err);
    //SetCurrentDirectory(strCurrentDir);
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), stFILE_INFO.szFilePath);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  int fsize = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  stFILE_INFO.file_size = fsize;
  stFILE_INFO.cur_no = 0;
  stFILE_INFO.total_no = (fsize % 1024) == 0 ? (fsize / 1024) : (fsize / 1024) + 1;

  char buff[SEND_MAX_SIZE];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file size = %d\r\n"), fsize);
  fseek(fp, 0, SEEK_SET);

  memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  snprintf((char*)file_info.file_name, 8, "TIT-OBD");
  file_info.file_type = BP_PACKET_UPDATE_NODE;
  file_info.file_len = fsize;
  snprintf((char*)file_info.file_ver, 8, "0.00.11");
  file_info.total_no = stFILE_INFO.total_no;
  file_info.crc = 0x1234;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_UPDATE_NODE;
  psnd_pkt->type = 0x1;			// APP Download Cmd = 0x01, OBD Download Cmd = 0x02, FBP Download Cmd = 0x03, 

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &file_info, sizeof(BP_INFO_PACKET));
  dst += sizeof(BP_INFO_PACKET);
  nsend += sizeof(BP_INFO_PACKET);
  u16len += sizeof(BP_INFO_PACKET);

  // file close..
  fclose(fp);

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnfFbp(void)
{
  memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
  COMDLG_FILTERSPEC ComDlgFS = { L"fbp", L"*.fbp" };
  if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
    return;
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);

  FILE* fp;
  BP_INFO_PACKET file_info;

  int nsend = sizeof(BP_INFO_PACKET);

  // Now defining the char pointer to be a buffer for wcstomb/wcstombs
  int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
  if (err != 0) {
    //printf("_test : %s error  = %d\n", path, err);
    //SetCurrentDirectory(strCurrentDir);
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), stFILE_INFO.szFilePath);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  int fsize = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  stFILE_INFO.file_size = fsize;
  stFILE_INFO.cur_no = 0;
  stFILE_INFO.total_no = (fsize % 1024) == 0 ? (fsize / 1024) : (fsize / 1024) + 1;

  char buff[SEND_MAX_SIZE];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file size = %d\r\n"), fsize);
  fseek(fp, 0, SEEK_SET);

  memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  snprintf((char*)file_info.file_name, 8, "BTM-FBP");
  file_info.file_type = BP_PACKET_UPDATE_FBP;
  file_info.file_len = fsize;
  snprintf((char*)file_info.file_ver, 8, "0.00.11");
  file_info.total_no = stFILE_INFO.total_no;
  file_info.crc = 0x1234;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_UPDATE_FBP;
  psnd_pkt->type = 0x1;			// APP Download Cmd = 0x01, OBD Download Cmd = 0x02, FBP Download Cmd = 0x03, 

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;
  memcpy(dst, &file_info, sizeof(BP_INFO_PACKET));
  dst += sizeof(BP_INFO_PACKET);
  nsend += sizeof(BP_INFO_PACKET);
  u16len += sizeof(BP_INFO_PACKET);

  // file close..
  fclose(fp);

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void OnBnClickedBtnFileBrowse(unsigned short command)
{
  DWORD filelen;
  TCHAR* file_Name;
  TCHAR str[200];
  unsigned short edit, editAtCommand;

  memset(stFILE_INFO.szFilePath, 0, sizeof(stFILE_INFO.szFilePath));
  COMDLG_FILTERSPEC ComDlgFS = { L"All Files",L"*.*" };
  if (!GetFileOpen(&ComDlgFS, stFILE_INFO.szFilePath))
    return;
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("open path :%s\r\n"), stFILE_INFO.szFilePath);

  WIN32_FIND_DATA FindFileData;
  HANDLE hFile = FindFirstFile(stFILE_INFO.szFilePath, &FindFileData);
  if (hFile != INVALID_HANDLE_VALUE){
    filelen = FindFileData.nFileSizeLow;
    file_Name = FindFileData.cFileName;
    wsprintf(str,"AT+QFUPL=\"%s\",%d", file_Name, filelen);
    if (command == IDC_BTN_FILE_BROWSE) {
      edit = IDC_EDIT; 
      editAtCommand = IDC_EDIT_ATCOMMAND;
      memcpy((char*)&stFILE_INFO0, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE1) {
      edit = IDC_EDIT1;
      editAtCommand = IDC_EDIT_ATCOMMAND1;
      memcpy((char*)&stFILE_INFO1, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE2) {
      edit = IDC_EDIT2;
      editAtCommand = IDC_EDIT_ATCOMMAND2;
      memcpy((char*)&stFILE_INFO2, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE3) {
      edit = IDC_EDIT3;
      editAtCommand = IDC_EDIT_ATCOMMAND3;
      memcpy((char*)&stFILE_INFO3, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE4) {
      edit = IDC_EDIT4;
      editAtCommand = IDC_EDIT_ATCOMMAND4;
      memcpy((char*)&stFILE_INFO4, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE5) {
      edit = IDC_EDIT5;
      editAtCommand = IDC_EDIT_ATCOMMAND5;
      memcpy((char*)&stFILE_INFO5, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    else if (command == IDC_BTN_FILE_BROWSE6) {
      edit = IDC_EDIT6;
      editAtCommand = IDC_EDIT_ATCOMMAND6;
      memcpy((char*)&stFILE_INFO6, (char*)&stFILE_INFO, sizeof(FILE_INFO));
    }
    SetDlgItemText(hDlgMain, editAtCommand,str);
    SetDlgItemText(hDlgMain, edit, stFILE_INFO.szFilePath);
    EditClassLastPositionMove(hDlgMain, edit);
  }
  FindClose(hFile);
}

void copyfile(char *Path)
{
  FILE* fp;
  char buff[64];
  int filelen, wLen, rLen;

  int err = fopen_s(&fp, Path, "rb");
  if (err != 0) {
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("file open error :%s\r\n"), Path);
    return;
  }

  fseek(fp, 0, SEEK_END);				// 파일 포인터를 파일의 끝으로 이동시킴
  filelen = ftell(fp);				// 파일 포인터의 현재 위치를 얻음

  wLen = 0;
  while (wLen < filelen)
  {
    rLen = filelen - wLen;
    if (rLen > 64)
      rLen = 64;

    fseek(fp, wLen, SEEK_SET);
    int nread = fread(buff, 1, rLen, fp);
    ComWirte(buff, rLen);
    wLen += rLen;
  }
  // file close..
  fclose(fp);
}
/*
void copyCertificates(void)
{
  char Path[MAX_PATH];
  char buff[64];
  int nsend;

  TCHAR* str = (TCHAR*)TEXT("AT+QFDEL=\"*\"\r\n");
  ComWirte(str, _tcslen(str));
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, (TCHAR*)TEXT("--------> All files delete!!!\r\n"));
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying 823acf1721-certificate.pem.crt(1/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"823acf1721-certificate.pem.crt\",1220");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\823acf1721-certificate.pem.crt");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying 823acf1721-private.pem.key(2/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"823acf1721-private.pem.key\",1675");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\823acf1721-private.pem.key");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying ac820313ef-certificate.pem.crt.dev(3/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"ac820313ef-certificate.pem.crt.dev\",1224");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\ac820313ef-certificate.pem.crt.dev");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying ac820313ef-private.pem.key.dev(4/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"ac820313ef-private.pem.key.dev\",1679");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\ac820313ef-private.pem.key.dev");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying AmazonRootCA1-RSA2048.pem(5/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"AmazonRootCA1-RSA2048.pem\",1187");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\AmazonRootCA1-RSA2048.pem");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying df04311f42-certificate.pem.crt.qa(6/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"df04311f42-certificate.pem.crt.qa\",1220");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\df04311f42-certificate.pem.crt.qa");
  copyfile(Path);
  Sleep(500);
  EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("--------> copying df04311f42-private.pem.key.qa(7/7) \r\n"));
  nsend = wsprintf(buff, "%s\r\n", "AT+QFUPL=\"df04311f42-private.pem.key.qa\",1675");
  ComWirte(buff, nsend);
  Sleep(500);
  GetCurrentDirectory(MAX_PATH, Path);
  wsprintf(Path + strlen(Path), "%s", "\\total_certificate\\df04311f42-private.pem.key.qa");
  copyfile(Path);
  stComm.fileListReqTm = 3;
  m_CertificatefileNUM = 0xff;
}
*/
void OnBnClickedGetInfo(void)
{
  char buff[SEND_MAX_SIZE];
  int nsend = 0;

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_GET_INFO;	 

  psnd_pkt->data_len1 = 0;
  psnd_pkt->data_len2 = 0;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;
  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
}

void checkFOTA(void)
{
  if (!g_jobOpen)
    return;
  EnableWindow(GetDlgItem(hDlgMain, IDC_BTN_HW_TEST), true);
  EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_FW_SKIP), false);
  EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_HW_SKIP), false);
  
  SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_LTE, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_ESIM, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_ACCEROMETER, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_APP_VERSION, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_OBD_VERSION, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_DB_VERSION, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_MDM_VERSION, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_IMEI, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_CCID, TEXT(""));
  SetDlgItemText(hDlgMain, IDC_STATIC_OBDCOMM, TEXT(""));
  SendDlgItemMessage(hDlgMain, IDC_CHECK_LED, BM_SETCHECK, BST_UNCHECKED, 0);
  SendDlgItemMessage(hDlgMain, IDC_CHECK_BLE, BM_SETCHECK, BST_UNCHECKED, 0);
  
 stComm.isOBDok = FALSE;
  memset(g_fotaPath, 0, sizeof(g_fotaPath));
  if (isFileExist("\\fw\\*.app", g_fotaPath)) {
    if (stComm.FOTAReq <= 1) {
      if (g_fwSkip) {
        stComm.FOTAReq = 0;
        //SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("FW 업데이트 SKIP"));
        SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("Boooting..."));
      }
      else {
        stComm.FOTAReq = 1;
        SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("FW 업데이트 진행"));
      }
    }
    else
      SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("리부팅"));
    return;
  }
  stComm.FOTAReq = 0;
  
  //SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("FW 업데이트 SKIP"));
  SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("Boooting..."));

}
void getSerialName(char* _str_SER,int lenMax,BOOL isForFile = FALSE)
{
    int strLen;
    char  str_SER_index[100], str_SER_mid[100], str_SER_no[100], str_SER[200];
    memset(str_SER_index, 0, 100);
    memset(str_SER_mid, 0, 100);
    memset(str_SER_no, 0, 100);
    memset(str_SER, 0, 100);
    GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_INDEX, str_SER_index, 100);
    GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_MID, str_SER_mid, 100);
    GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, str_SER_no, 100);
    if (isForFile)
        sprintf_s(str_SER, "%s_%s_%s", str_SER_index, str_SER_mid, str_SER_no);
    else
        sprintf_s(str_SER, "%s%s%s", str_SER_index, str_SER_mid, str_SER_no);
    strLen = strlen(str_SER)+1;
    if ((strLen +1) > lenMax)  strLen = lenMax -1;
    memset(_str_SER, 0, lenMax);//20230828
    memcpy(_str_SER, str_SER, strLen);
    _str_SER[strLen + 1] = 0;
}
void saveSerialNumber(void)
{
  int len, row;

  char str_ICCID[100], str_sICCID[100], str_IMEI[100], str_SER_file[100], str_SER_mid[100],str_SER_no[100], str_SER_reg[100];
  char str_empVer[100], str_obdVer[100], str_err[100];

  getSerialName(str_SER_file,100,TRUE);

#if false//def _DEBUG
  sprintf_s(str_ICCID, "898230012300021535");
  sprintf_s(str_IMEI, "868675060112521");
  sprintf_s(str_empVer, "1111111");
  sprintf_s(str_obdVer, "2222222");
#else
  GetDlgItemText(hDlgMain, IDC_STATIC_CCID, str_ICCID, 100);
  GetDlgItemText(hDlgMain, IDC_STATIC_IMEI, str_IMEI, 100);
  GetDlgItemText(hDlgMain, IDC_STATIC_APP_VERSION, str_empVer, 100);
  GetDlgItemText(hDlgMain, IDC_STATIC_OBD_VERSION, str_obdVer, 100);
#endif
  len = strlen(str_ICCID);
  if (len > 2) {
    memmove(str_sICCID, str_ICCID + 11, 7);
    //str_ICCID[18] = 0;//뒤 XF 글자를 자르기 위함.
  }
  memset(str_err, 0, 100);
#ifdef SAVE_INTO_CSV
  row = save_csv(0, str_SER_file, str_empVer, str_obdVer, str_IMEI, str_ICCID, str_err);
#endif
#ifdef SAVE_INTO_EXCEL
  row = save_Excel(0, str_SER_file, str_empVer, str_obdVer, str_IMEI, str_ICCID, str_err);
#endif
  if (str_err[0] == NULL)
  {
      GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_MID, str_SER_mid, 100);
      GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, str_SER_no, 100);
      len = atoi(str_SER_no) + 1;
      sprintf_s(str_SER_no, "%04d", len);
      SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, str_SER_no);
      sprintf_s(str_SER_reg, "%s%04d", str_SER_mid, len);
      saveRegistryKey(str_SER_reg);
#ifdef FOR_FAST_ASSEMBLY
      sprintf_s(str_SER_no, "%s\r\n엑셀 저장에 성공 하였습니다.", str_SER_file);
      MessageBox(hDlgMain, str_SER_no, TEXT("OK"), MB_OK);
#endif
  }
  else
  {
      getSerialName(str_SER_file, 100);
      sprintf_s(str_SER_no, "%s\r\n엑셀 저장에 실패 하였습니다.\r\n(err: %s)", str_SER_file, str_err);
      MessageBox(hDlgMain, str_SER_no, TEXT("ERROR"), MB_OK);
  }
}

void OnBnClickedSaveSerialNember(void)
{
#if 1
  char str_SER[100] = { 0, };
  char buff[SEND_MAX_SIZE];
  int nsend = 0;

  //GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, str_SER, 100);
  //var = GetDlgItemInt(hDlgMain, IDC_EDIT_SERIAL_NO, NULL, FALSE);
  getSerialName(str_SER, sizeof(str_SER));
  
  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  memset(buff, 0, SEND_MAX_SIZE);

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_SERIAL_NO;	// serial number setting command

  //psnd_pkt->data_len1 = 0;
  //psnd_pkt->data_len2 = 4;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = 0;

  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

  u16len = strlen(str_SER) + 1;
  memcpy((char*)dst, (char*)&str_SER, u16len);
  dst += u16len;
  nsend += u16len;


  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
#else
  char str_SER[100] = { 0, };
  char buff[SEND_MAX_SIZE];
  int nsend = 0;
  GetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NEUBER, str_SER, 100);
  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  psnd_pkt->stx1 = STX0;
  psnd_pkt->stx2 = STX1;
  psnd_pkt->hvid_bp = HVID_BP;
  psnd_pkt->hdid_bp = HDID_BP;
  psnd_pkt->command = BP_PACKET_M4_COMMAND;
  psnd_pkt->type = BP_PACKET_M4_SERIAL_NO;

  psnd_pkt->data_len1 = 0;
  psnd_pkt->data_len2 = 0;

  psnd_pkt->cur_no = 0;
  psnd_pkt->total_no = stFILE_INFO.total_no;
  ////////////////////////////////////
  USHORT u16len = 0;
  nsend = 12;

  unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

  u16len = strlen(str_SER) + 1;
  strcpy_s((char*)dst, u16len, str_SER);
  psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
  psnd_pkt->data_len2 = u16len & 0xff;

  unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
  *dst++ = ((crc >> 8) & 0xff);
  *dst++ = ((crc) & 0xff);
  nsend += 2;

  *dst++ = ETX0;
  *dst = ETX1;
  nsend += 2;

  ComWirte(buff, nsend);
#endif
}


void doControlCommand(WPARAM wparam)
{
  unsigned short command,notification;
  DWORD dwExitCode;
  TCHAR str[MAX_PATH];

  command = LOWORD(wparam);
  notification = HIWORD(wparam);

  switch (command) {
  case IDC_CHECK_FW_SKIP:
    if (SendDlgItemMessage(hDlgMain, IDC_CHECK_FW_SKIP, BM_GETCHECK, 0, 0) == BST_CHECKED) 
      g_fwSkip = 1;
    else 
      g_fwSkip = 0;
    break;
  case IDC_CHECK_HW_SKIP:
    if (SendDlgItemMessage(hDlgMain, IDC_CHECK_HW_SKIP, BM_GETCHECK, 0, 0) == BST_CHECKED) 
      g_hwSkip = 1;
    else 
      g_hwSkip = 0;
    break;
  case IDC_CHECK_LED:
    if (SendDlgItemMessage(hDlgMain, IDC_CHECK_LED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
      EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_SERIAL), true);
      //EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_INDEX), true);
      EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_MID), true);
      EnableWindow(GetDlgItem(hDlgMain, IDC_EDIT_SERIAL_NO), true);
      EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SET_SN), true);
      //EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), true);
      SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("시리얼넘버를 저장하세요!!"));
      stComm.LEDCheckReqTm = 0;
    }
    break;
  case IDC_CONNECT:
    GetDlgItemText(hDlgMain, IDC_CONNECT, str, MAX_PATH);
    if (strcmp(str, "OPEN") == 0) {   //matched
      if (doUartConnection(0)==false) 
        MessageBox(hDlgMain, TEXT("COM포트 에러!!"), TEXT("ERROR"), MB_OK);
      else {
        SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("CLOSE"));
        checkFOTA();
      }
    }
    else {
      DWORD dwErrorCode=0;
      if (hThread)
        TerminateThread(hThread,dwErrorCode);
      UartClose(0);
      SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("OPEN"));
    }
    break;
  case IDC_CLEAR:
    EditClassLogClear(IDC_EDIT_LOGVIEWER);
    break;
  case IDC_BUTTON_SAVE:
    //OnBnClickedSaveSerialNember();
    saveSerialNumber(); //PJS
    stComm.testCompleteReqTm = 1; //PJS
    break;
  case IDC_BTN_HW_TEST:
    if (g_jobOpen == 0) 
      OnBnClickedReset();
    else {
      if(stComm.LEDCheckReqTm>0 || stComm.LEDCheckReqTm==-1)
        OnBnClickedReset();
    }
    break;
  case IDC_BUTTON_GETINFO:
      //if (hThread) {
      //    DWORD dwErrorCode = 0;
      //    TerminateThread(hThread, dwErrorCode);
      //    UartClose(0);
      //    hThread = NULL;
      //}
      stComm.nHwtestresultReq = 2;
     // OnBnClickedGetInfo();
      break;
  case IDC_CHECK_BLE:
      if (SendDlgItemMessage(hDlgMain, IDC_CHECK_BLE, BM_GETCHECK, 0, 0) == BST_CHECKED) {
          //EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SET_SN), true);
          EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), true);
          SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("BLE를 확인하세요!!"));
          stComm.LEDCheckReqTm = 0;
      }
      break;
  case IDC_BUTTON_SET_SN:
      OnBnClickedSaveSerialNember();
      g_snCheck = 1;
      stComm.nHwtestresultReq = 2;
      //EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), true);
      break;
  case IDC_BUTTON_SETJIGMODE:
#ifdef _DEBUG
      saveSerialNumber();
#else
      OnBnClickedGetHWStatus();
#endif
      break;
/*
  case IDC_BTN_FSEND:              //APP download
    OnBnClickedBtnFsend(NULL);
    break;
  case IDC_BTNF_BLE:
    OnBnClickedBtnfBle();
    break;
  case IDC_BTN_FILE_BROWSE:
  case IDC_BTN_FILE_BROWSE1:
  case IDC_BTN_FILE_BROWSE2:
  case IDC_BTN_FILE_BROWSE3:
  case IDC_BTN_FILE_BROWSE4:
  case IDC_BTN_FILE_BROWSE5:
  case IDC_BTN_FILE_BROWSE6:
    OnBnClickedBtnFileBrowse(command);
    break;
*/
  case IDC_EDIT_LOGVIEWER:
    switch (notification) {
    case EN_ERRSPACE:
      MessageBox(hDlgMain, TEXT("buffer overflow"), TEXT("ERROR"), MB_OK);
      break;
    case EN_MAXTEXT:
      MessageBox(hDlgMain, TEXT("It will clear all-text!!!"), TEXT("Alert"), MB_OK);
      EditClassLogClear(IDC_EDIT_LOGVIEWER);
      break;
    default:
      break;
    }
    break;
  }
}

#define QF_DOWN_SIZE_MAX		1024//0x400
int device_resp_service(unsigned char* precv, int size, unsigned char* buff)
{
  BP_PACKET* presp_pkt = (BP_PACKET*)precv;
  FILE* fp;
  BP_INFO_PACKET file_info;

  memset(&file_info, 0, sizeof(BP_INFO_PACKET));

  PBP_PACKET psnd_pkt = (PBP_PACKET)buff;

  if (presp_pkt->command == BP_PACKET_UPDATE_APP || presp_pkt->command == BP_PACKET_UPDATE_OBD || presp_pkt->command == BP_PACKET_UPDATE_FBP
    || presp_pkt->command == BP_PACKET_UPDATE_BLE || presp_pkt->command == BP_PACKET_MEDIAFILE || presp_pkt->command == BP_PACKET_UPDATE_NODE) {
    if (presp_pkt->cur_no == presp_pkt->total_no) {
      // Download Success
      return 0;
    }
    else if (presp_pkt->cur_no < presp_pkt->total_no) {
      char* pCode = (char*)&psnd_pkt->payload;
      if (*(pCode + 0) != 0 && *(pCode + 1) != 0) {
        // Response Code Error
        return -4;
      }

      psnd_pkt->stx1 = STX0;
      psnd_pkt->stx2 = STX1;
      psnd_pkt->hvid_bp = HVID_BP;
      psnd_pkt->hdid_bp = HDID_BP;

      psnd_pkt->command = presp_pkt->command;
      psnd_pkt->type = 0x1;

      stFILE_INFO.cur_no++;
      psnd_pkt->cur_no = stFILE_INFO.cur_no;
      psnd_pkt->total_no = stFILE_INFO.total_no;


      ////////////////////////////////////
      USHORT u16len = 0;
      int nsend = 12;

      unsigned char* dst = (unsigned char*)&psnd_pkt->payload;

      int err = fopen_s(&fp, stFILE_INFO.szFilePath, "rb");
      if (err) {
        // File Open Error
        return -3;
      }

      int nseek = (1024 * presp_pkt->cur_no);
      fseek(fp, nseek, SEEK_SET);

      int ndata = ((stFILE_INFO.file_size - nseek) > QF_DOWN_SIZE_MAX) ? QF_DOWN_SIZE_MAX : (stFILE_INFO.file_size - nseek);
      int nread = fread(dst, 1, ndata, fp);

      fclose(fp);

      dst += nread;
      nsend += nread;
      u16len += nread;

      psnd_pkt->data_len1 = (u16len >> 8) & 0xff;
      psnd_pkt->data_len2 = u16len & 0xff;

      unsigned short crc = crc16_ccitt((UCHAR*)&buff[2], nsend - 2);  //nsend - 2 = len + 10
      *dst++ = ((crc >> 8) & 0xff);
      *dst++ = ((crc) & 0xff);
      nsend += 2;

      *dst++ = ETX0;
      *dst = ETX1;
      nsend += 2;

      return nsend;
    }
    else {
      // Packet Number Over Error
      return -2;
    }
  }
  else {
    // STX , ETX Error
    return -1;
  }

  // It is not Update Command 
  return 0;
}

#define MEMCMP_OK				0
char* ssl_magic = "ssl.";
void parseConfig(PCONFIG_DAT pConfig)
{
  TCHAR str[50];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/----------------parseConfig----------------------------------------/\r\n"));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" host = %s : %d!!!! \r\n"), pConfig->host_ip, pConfig->port_no);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" service = %s \r\n"), pConfig->service_nm);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" ser_no = %s !!!! \r\n"), pConfig->serial_no);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" hwVER : %u.%u\r\n"), pConfig->hwVER[0], pConfig->hwVER[1]);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" MAC ADDRESS : %s\r\n"), pConfig->device_SN);
  //EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/-------------------------------------------------------------------/\r\n"));
  //wsprintf(str, TEXT("%s"), pConfig->device_SN);
  //SetDlgItemText(hDlgMain, IDC_EDIT_MAC_ADDR, str);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
  char magic[10];
  memset(magic, 0, sizeof(magic));
  SSL_DATA* pSslData = (SSL_DATA*)pConfig->etc;
  memcpy(magic, pSslData->ssl_magic, sizeof(pSslData->ssl_magic));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/------------------------parseSSL(%s)-----------------------------/\r\n"), magic);
  if ((memcmp(pSslData->ssl_magic, ssl_magic, 4) == MEMCMP_OK))
  {
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("%s\r\n%s\r\n%s\r\n"), pSslData->ssl_CA, pSslData->ssl_CertificatCrt, pSslData->ssl_PrivatKey);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
}

void parseQmInfo(PQM_INFO pQmInfo)
{
  TCHAR str[50];

  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/--------------------parseQmInfo------------------------------------/\r\n"));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" qminfo0= %s\r\n"), pQmInfo->ati_info[0]);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" qminfo1= %s\r\n"), pQmInfo->ati_info[1]);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" qminfo2= %s\r\n"), pQmInfo->ati_info[2]);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" IMEI = %s\r\n"), pQmInfo->imei_gsn);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" IMSI = %s \r\n"), pQmInfo->imsi_sim);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" ICCID = %s\r\n"), pQmInfo->iccid_sim);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" apn = %s\r\n"), pQmInfo->apn);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(" cnum = %s\r\n"), pQmInfo->cnum);
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/-------------------------------------------------------------------/\r\n"));

  wsprintf(str, TEXT("%s"), pQmInfo->imei_gsn);//+		pQmInfo->imei_gsn	0x0061ee74 "868675060106168"	unsigned char[24]
  //SetDlgItemText(hDlgMain, IDC_STATIC_IMEI, str);
  //SetDlgItemText(hDlgMain, IDC_EDIT_IMEI, str);
  
  wsprintf(str, TEXT("%s"), pQmInfo->iccid_sim);//+		pQmInfo->iccid_sim	0x0061eeb8 "8982300123000212785F"	unsigned char[24]
  //SetDlgItemText(hDlgMain, IDC_STATIC_CCID, str);
  //SetDlgItemText(hDlgMain, IDC_EDIT_CCID, str);
}
/***** ADC로 변환한 전압 ********/
#define VDD		(3.3)
#define ADC_Convert_cur	(4095/2/VDD ) // 360k/36k --> 100k/100k
#define ADC_Convert	(4095/10/VDD )

//#define ADC_offset	(0 ) // ADC 값을 읽을 때  offset 

//#define ADCnor_G_cur 	 (1.05/ADC_Convert_cur) 
#define ADCnor_G 	 (1.0307/ADC_Convert) 

//#define _ReadNsetV(us)		(us + ADC_offset )									// adc값을 읽은 값을 그대로 변수에 저장
#define _FV(us)		((us) * ADCnor_G )			// 변수를 전압으로 변경 
#define _ADC(fv)		(short)( fv/ADCnor_G )		// 전압 상수 값을  adc 값으로...

BOOL parseHWStatus(pJIG_HW_TEST pHwTest)
{
    TCHAR str_app_ver[50], str_obd_ver[50], str_db_ver[50], strErr[1024 * 10], str_mdm_version[50], str_imei[50], str_iccid[50], str_SER[50];
    int i, nstrErr_pos = 0;
    BOOL isRet = TRUE;
    strErr[0] = 0;

    wsprintf(str_app_ver, TEXT("%d.%d.%d.%d"), pHwTest->app_version[0], pHwTest->app_version[1], pHwTest->app_version[2], pHwTest->app_version[3]);
    SetDlgItemText(hDlgMain, IDC_STATIC_APP_VERSION, str_app_ver);
    wsprintf(str_obd_ver, TEXT("%d.%d.%d.%d"), pHwTest->obd_version[0], pHwTest->obd_version[1], pHwTest->obd_version[2], pHwTest->obd_version[3]);
    SetDlgItemText(hDlgMain, IDC_STATIC_OBD_VERSION, str_obd_ver);
    wsprintf(str_db_ver, TEXT("%d.%d.%d.%d"), pHwTest->db_version[4], pHwTest->db_version[5], pHwTest->db_version[6], pHwTest->db_version[7]);
    SetDlgItemText(hDlgMain, IDC_STATIC_DB_VERSION, str_db_ver);
    wsprintf(str_imei, TEXT("%s"), pHwTest->imei_gsn);
    SetDlgItemText(hDlgMain, IDC_STATIC_IMEI, str_imei);
    i = strlen(pHwTest->iccid_sim);
    if (i > 2) {
        //i -= 2;
        pHwTest->iccid_sim[i] = 0;
        //memmove(pHwTest->iccid_sim, pHwTest->iccid_sim + 11, 8);
    }
    wsprintf(str_iccid, TEXT("%s"), pHwTest->iccid_sim);
    SetDlgItemText(hDlgMain, IDC_STATIC_CCID, str_iccid);

#if (defined FOR_FAST_ASSEMBLY )
    if ((strlen(str_imei) < 15) || (strlen(str_iccid) < 20))
        isRet = FALSE;
#endif

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("\r\n----------------< VER 확인 >----------------\r\n"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.APP : %s\r\n"), str_app_ver);
    if ((CHK_APP_VER_0 != pHwTest->app_version[0])
#ifdef CHK_APP_VER_1
        || (CHK_APP_VER_1 != pHwTest->app_version[1])
#endif
#ifdef CHK_APP_VER_2
        || (CHK_APP_VER_2 != pHwTest->app_version[2])
#endif
#ifdef CHK_APP_VER_3
        || (CHK_APP_VER_3 != pHwTest->app_version[3])
#endif
        )
        nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : APP_VER(%s) 에러\r\n"), str_app_ver);

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 2.OBD : %s\r\n"), str_obd_ver);
    if ((CHK_OBD_VER_0 != pHwTest->obd_version[0])
#ifdef CHK_OBD_VER_1
        || (CHK_OBD_VER_1 != pHwTest->obd_version[1])
#endif
#ifdef CHK_OBD_VER_2
        || (CHK_OBD_VER_2 != pHwTest->obd_version[2])
#endif
#ifdef CHK_OBD_VER_3
        || (CHK_OBD_VER_3 != pHwTest->obd_version[3])
#endif
        )
        nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : OBD_VER(%s) 에러\r\n"), str_obd_ver);
    memcpy(str_mdm_version, pHwTest->mdm_version, 20);
    SetDlgItemText(hDlgMain, IDC_STATIC_MDM_VERSION, str_mdm_version);
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 3.MDM : %s\r\n"), str_mdm_version);

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< LTE 검사 >----------------\r\n"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.IMEI : %s\r\n"), pHwTest->imei_gsn);
    if (strlen(pHwTest->imei_gsn) != 15)      nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (1) IMEI 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 2.CCID : %s\r\n"), pHwTest->iccid_sim);
    if (strlen(pHwTest->iccid_sim) < 18)     nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (2) ESIM 에러\r\n"));
    ;
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< PIN 검사 >----------------\r\n"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.RI,STATUS : %s\r\n"), pHwTest->bQRI_pinOk ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bQRI_pinOk == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (3) RI 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 2.AP-ready STATUS : %s\r\n"), pHwTest->bQApready_pinOk ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bQApready_pinOk == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (4) Apready 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 3.PwrDtt : %s\r\n"), pHwTest->bPwrDttOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bPwrDttOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (5) PwrDtt 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 4.UdrExist : %s\r\n"), pHwTest->bUdrExistOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bUdrExistOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (6) UdrExist 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 5.InternalAnt : %s\r\n"), pHwTest->bInternalAntOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bInternalAntOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (7) InternalAnt 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 6.ExteranlAnt : %s\r\n"), pHwTest->bExteranlAntOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bExteranlAntOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (8) ExteranlAnt 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 7.RelayNExtRsvIN : %s\r\n"), pHwTest->bRelayNExtRsvINOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bRelayNExtRsvINOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (9) Relay or ExtRsvIN 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 8.bExtAccINOK : %s\r\n"), pHwTest->bExtAccINOK ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bExtAccINOK == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (10) bExtAccIN 에러\r\n"));

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 8.RTC : %s\r\n"), pHwTest->bRTC ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bRTC == 0)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (11) RTC 에러\r\n"));


#define LTERSSI_MIN 50
#define SENSOR_MIN (2.5)
#define SENSOR_MAX (2.7)
    float fv_sensor_T = _FV(pHwTest->sensor_T);
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 9.sensor_T(%.1f) : %s\r\n"), fv_sensor_T, ((SENSOR_MIN < fv_sensor_T) && (fv_sensor_T < SENSOR_MAX)) ? TEXT("OK") : TEXT("Failed"));
    if (((SENSOR_MIN < fv_sensor_T) && (fv_sensor_T < SENSOR_MAX)) == FALSE) nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : sensor_T(%.f) 에러\r\n"), fv_sensor_T);
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 10.LTERSSI(%u) : %s\r\n"), pHwTest->LTERSSI, (pHwTest->LTERSSI < LTERSSI_MIN) ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->LTERSSI >= LTERSSI_MIN)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (12) LTERSSI(%u) 에러\r\n"), pHwTest->LTERSSI);

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.T_COMAPNY(%u) : %s\r\n"), pHwTest->T_COMAPNY, (pHwTest->T_COMAPNY == DEF_T_COMAPNY) ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->T_COMAPNY != DEF_T_COMAPNY)  nstrErr_pos += wsprintf(&strErr[nstrErr_pos], TEXT("Failed : (13) T_COMAPNY(%u) 에러\r\n"), pHwTest->T_COMAPNY);


    if (strErr[0] == 0) //  isOK = TRUE;
        SetDlgItemText(hDlgMain, IDC_STATIC_LTE, TEXT("OK"));
    else {
        SetDlgItemText(hDlgMain, IDC_STATIC_LTE, strErr);
#if (!defined _DEBUG & !defined __LAP_TEST__ & !defined FOR_FAST_ASSEMBLY )
        isRet = FALSE;
#endif
    }
    /*
    if (i==7 ) {
      for (i = 0; i < 7; i++) {
        if (!isdigit(pHwTest->iccid_sim[i]))
          break;
      }
      if( i==7 )
        SetDlgItemText(hDlgMain, IDC_STATIC_ESIM, TEXT("OK"));
      else
        SetDlgItemText(hDlgMain, IDC_STATIC_ESIM, TEXT("Failed"));
    }
    else
      SetDlgItemText(hDlgMain, IDC_STATIC_ESIM, TEXT("Failed"));
    */
#ifndef FOR_FAST_ASSEMBLY
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< GPS 검사 >----------------\r\n"));
    if (pHwTest->bUdrExistOK) {
        SetDlgItemText(hDlgMain, IDC_STATIC_ITEM, TEXT("UDR 통신/감도"));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.UDR-GPS 사용 \r\n"));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 2.UDR 모듈동작검사 :  %s\r\n"), pHwTest->bUDRCommOk ? TEXT("OK") : TEXT("Failed"));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 3.UDR GPS 수신검사 :  %s\r\n"), pHwTest->bUDRGPSOk ? TEXT("OK") : TEXT("Failed"));
        if (pHwTest->bUDRCommOk && pHwTest->bUDRGPSOk)
            SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT("OK"));
        else {
            isRet = FALSE;
            if (pHwTest->bUDRCommOk)
                SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT("Failed : 수신감도 에러"));
            else
                SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT("Failed : 통신에러"));
        }
    }
    else {
        isRet = FALSE;
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.QUECTEL-GPS 사용 \r\n"));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 2.QUECTEL GPS수신검사 :  %s\r\n"), pHwTest->bQGPSOk ? TEXT("OK") : TEXT("Failed"));
        SetDlgItemText(hDlgMain, IDC_STATIC_ITEM, TEXT("QGPS 통신/감도"));
        if (pHwTest->bQGPSOk)
            SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT("OK"));
        else
            SetDlgItemText(hDlgMain, IDC_STATIC_UDR, TEXT("Failed"));
    }
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< 가속도센서 검사 >----------------\r\n"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.가속도 센서 동작 : %s\r\n"), pHwTest->bAccelerometerOk ? TEXT("OK") : TEXT("Failed"));
    if (pHwTest->bAccelerometerOk)
        SetDlgItemText(hDlgMain, IDC_STATIC_ACCEROMETER, TEXT("OK"));
    else {
        isRet = FALSE;
        SetDlgItemText(hDlgMain, IDC_STATIC_ACCEROMETER, TEXT("Failed"));
    }

    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< OBD 통신 검사 >----------------\r\n"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("  1.OBD 통신 검사 :  %s\r\n"), pHwTest->bOBDCommOk ? TEXT("OK") : TEXT("Failed"));
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("  1.OBD JIG 검사 :  %s\r\n"), stComm.isOBDok ? TEXT("OK") : TEXT("Failed"));

    if ((pHwTest->bOBDCommOk) && (stComm.isOBDok == TRUE) )
        SetDlgItemText(hDlgMain, IDC_STATIC_OBDCOMM, TEXT("OK"));
    else {
        isRet = FALSE;
        SetDlgItemText(hDlgMain, IDC_STATIC_OBDCOMM, TEXT("Failed"));
    }
#endif
#ifdef FOR_FAST_ASSEMBLY
    char serial_index[100], serial_mid[100], serial_no[100];
    memset(serial_index, 0, 100);
    memset(serial_mid, 0, 100);
    memset(serial_no, 0, 100);

    if (strlen((char*)pHwTest->device_SN) == 10) {
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("serialNumber: %s\r\n"), (char*)pHwTest->device_SN);
        // EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("%d\r\n"), sizeof(JIG_HW_TEST));
        memcpy(serial_index, &pHwTest->device_SN[0], 2);
        memcpy(serial_mid, &pHwTest->device_SN[2], 4);
        memcpy(serial_no, &pHwTest->device_SN[6], 4);
        SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_INDEX, serial_index);
        SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_MID, serial_mid);
        SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, serial_no);
    }
    g_snCheck = 1;
    return isRet;
#else
    if (g_snCheck == 1) {
        getSerialName(str_SER, sizeof(str_SER));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("----------------< S/N 검사 >----------------\r\n"));
        EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" 1.S/N : %s\r\n"), pHwTest->device_SN);
        if ((memcmp(pHwTest->device_SN, str_SER, sizeof(pHwTest->device_SN)) != MEMCMP_OK)) {
            isRet = FALSE;
            EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" Failed : S/N\r\n"));
        }
    }
#ifdef _DEBUG
    isRet = TRUE;
#endif
  return isRet;
#endif
}
char* dcConfig_magic = "dc..";
char* Ble_magic = "b...";
void parseTripMap(POBD_TRIP_MAP pTripMap)
{
  TCHAR str[50]; 
  //wsprintf(str, TEXT("%d.%d.%d.%d"), pTripMap->app_version[0], pTripMap->app_version[1], pTripMap->app_version[2], pTripMap->app_version[3]);
  //SetDlgItemText(hDlgMain, IDC_STATIC_APP_VERSION, str);
  //wsprintf(str, TEXT("%d.%d"), pTripMap->ebp_version[0], pTripMap->ebp_version[1]);
  //SetDlgItemText(hDlgMain, IDC_STATIC_OBD_VERSION, str);
  //wsprintf(str, TEXT("%d.%d.%d.%d"), pTripMap->obd_version[0], pTripMap->obd_version[1],pTripMap->obd_version[2], pTripMap->obd_version[3]);
  //SetDlgItemText(hDlgMain, IDC_STATIC_OBD_VERSION, str);
  //wsprintf(str, TEXT("%d.%d.%d.%d"), pTripMap->db_version[0], pTripMap->db_version[1], pTripMap->db_version[2], pTripMap->db_version[3]);
  //SetDlgItemText(hDlgMain, IDC_STATIC_DB_VERSION, str);
  //wsprintf(str, TEXT("%c%c%c%c%c%c%c"), pTripMap->carCode[0], pTripMap->carCode[1], pTripMap->carCode[2], pTripMap->carCode[3], pTripMap->carCode[4], pTripMap->carCode[5], pTripMap->carCode[6]);
  //SetDlgItemText(hDlgMain, IDC_STATIC_CARCODE, str);
  //memcpy(str, 0, sizeof(str));
  //memcpy(str, pTripMap->vehicle_id, 17);
  //SetDlgItemText(hDlgMain, IDC_EDIT_VINCODE, str);
  return;

  char magic[10];
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/------------------------parseTripMap-----------------------------/\r\n"));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("mdmVer: %s\r\n"), pTripMap->qm_revision);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
  memset(magic, 0, sizeof(magic));
  BLE_DATA* pcBleData = (BLE_DATA*)pTripMap->config_data2;
  memcpy(magic, pcBleData->ble_magic, sizeof(pcBleData->ble_magic));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/------------------------parseTripMap(%s)-----------------------------/\r\n"), magic);
  if ((memcmp(pcBleData->ble_magic, Ble_magic, 4) == MEMCMP_OK))
  {
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("info.addr  %02X%02X%02X%02X%02X%02X\r\n"), pcBleData->info.Addr[0], pcBleData->info.Addr[1], pcBleData->info.Addr[2], pcBleData->info.Addr[3], pcBleData->info.Addr[4], pcBleData->info.Addr[5]);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
  memset(magic, 0, sizeof(magic));
  DC_CONFIG_DATA* pConfig = (DC_CONFIG_DATA*)pTripMap->config_data3;
  memcpy(magic, pConfig->config_magic, sizeof(pConfig->config_magic));
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/------------------------parseTripMap(%s)-----------------------------/\r\n"), magic);
  if ((memcmp(pConfig->config_magic, dcConfig_magic, 4) == MEMCMP_OK))
  {
    UCHAR buff[10];
    // 5		TripDataField		UCHAR	5	TripData의 "DataField List" Flag 설정 값
    memcpy(buff, &pConfig->OnDataField, sizeof(BTM_DATAFIELD));
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- TripDataField,%02X%02X%02X%02X%02X\r\n"), buff[0], buff[1], buff[2], buff[3], buff[4]);
    // 6		TripDataGetTerm	USHORT	2	[초]TripData의 수집주기 ( 0 이면 수집하지 않음)
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- TripDataGetTerm,%u\r\n"), pConfig->OnDataGetTerm);
    // 7		TripDataGetCnt		USHORT	2	TripData수집 최대 횟수 (최대 횟수가 되면 자동 전송하고 수집 데이터 초기화)
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- TripDataGetCnt,%u\r\n"), pConfig->OnDataSendCnt);

    //8	GpsDataField	UCHAR	2	TripData의 "DataField List" Flag 설정 값
    memcpy(buff, &pConfig->OnGpsField, sizeof(BTM_GPSFIELD));
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- GpsDataField,%02X%02X\r\n"), buff[0], buff[1]);
    //9	GpsDataGetTerm	USHORT	2	[초]GpsData의 수집주기 ( 0 이면 수집하지 않음), Default : 1 
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- GpsDataGetTerm,%u\r\n"), pConfig->OnGpsGetTerm);
    //10	GpsDataGetCnt	USHORT	2	GpsData수집 최대 횟수 (최대 횟수가 되면 자동 전송), Default : 60
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- GpsDataGetCnt,%u\r\n"), pConfig->OnGpsSendCnt);
    //11	KASendTerm	USHORT	2	[분]KeepAlive 전송 주기 ( 0이면 전송하지 않음), Default: 10
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- KASendTerm,%u\r\n"), pConfig->KASendTerm);
    //12	KGSendTerm	USHORT	2	[분]KeepGPS 전송 주기 ( 0이면 전송하지 않음), Default: 60
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\t\t--- KGSendTerm,%u\r\n"), pConfig->KGSendTerm);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("/---------------------------------------------------------------------------/\r\n"));
}
typedef struct _BP_INDEX
{
    UCHAR	sender : 1;		// BP_INDEX_SENDER_xx
    UCHAR	index : 7;		// DATA_GROUP_xxx,INDEX_CMD_xxx,BUFCOM_STEP_xxx
} BP_INDEX;

#define USER_DATA_MAX	20
typedef struct _CMD_VER {
    BP_INDEX	index;			// fixed value : index = INDEX_CMD_VER
    UCHAR 	cmd_type;		// fixed value	: cmd_type = CMD_TYPE_GET;
    UCHAR ver[4];
    USHORT flag_ff77; // 0xff77 로 	//USE_CARCODE_VER
    USHORT CarcodeVerH;
    USHORT CarcodeVerL;
    char	CarCode[7];
    char nWrite;
    char cmdver_ver;
    char fuel_type;
    USHORT	fuelTankCapacity;
    UCHAR dbVer[8];
    UCHAR ODO_type;
    UCHAR UserDataSize;
    UCHAR UserData[USER_DATA_MAX];			// 단말기에서 설정한 값 , FBP Download시 삭제됨
    USHORT Lw;
    //	UCHAR reserved[8];
}CMD_VER; // 2 + 4 + 18 + 8 + 1 + 10

void parseCMD_VER(CMD_VER* pver)
{
    /*
    CString str[50];
    UCHAR CarCode[8] = {};
    memcpy(CarCode, pver->CarCode, 7);
    wsprintf(str, TEXT(str.Format("OBD_VER-%u.%u.%u.%u", pver->ver[0], pver->ver[1], pver->ver[2], pver->ver[3]);
    str.AppendFormat("(%s_%u.%u)[%u]", CarCode, pver->CarcodeVerH, pver->CarcodeVerL, pver->nWrite);

    if (pver->fuel_type == 1) {
        str.AppendFormat("G,%.1f[L]", pver->fuelTankCapacity / 10.0f);
    }
    else if (pver->fuel_type == 2) {
        str.AppendFormat("D,%.1f[L]", pver->fuelTankCapacity / 10.0f);
    }
    else if (pver->fuel_type == 3) {
        str.AppendFormat("L,%.1f[L]", pver->fuelTankCapacity / 10.0f);
    }
    else if (pver->fuel_type == 5) {
        str.AppendFormat("E,%.1f[KWh]", pver->fuelTankCapacity / 10.0f);
    }
    if (pver->cmdver_ver > 0) {
        str.AppendFormat("DbVer(%u,%u,%u,%u,%u,%u,%u,%u)", pver->dbVer[0], pver->dbVer[1], pver->dbVer[2], pver->dbVer[3], pver->dbVer[4], pver->dbVer[5], pver->dbVer[6], pver->dbVer[7]);
    }
    str.AppendFormat("ODO_type(%C)", pver->ODO_type);
    str.AppendFormat("%u\r\n", pver->Lw);
    str.AppendFormat("/---------------------------------------------------------------------------/\r\n");
    return str;
    */
}
void getLastSerialNeumer(void)
{
  char serial[100], serial_mid[100], serial_no[100];
  memset(serial, 0, 100);
  memset(serial_mid, 0, 100);
  memset(serial_no, 0, 100);

  if (DEF_T_COMAPNY == KTT)
      SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_INDEX, "BK");
  else
      SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_INDEX, "BL");

  memset(serial, 0, 100);
  if (!readRegistryKey(serial)) {
    if (strlen(serial) == 8) {
      EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("serialNumber: %s\r\n"), serial);
     // EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("%d\r\n"), sizeof(JIG_HW_TEST));
      memcpy(serial_mid, serial, 4);
      memcpy(serial_no, &serial[4], 4);
      SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_MID, serial_mid);
      SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, serial_no);
      return;
    }
  }
  saveRegistryKey("23080001");
  SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_MID, "2308");
  SetDlgItemText(hDlgMain, IDC_EDIT_SERIAL_NO, "0001");
}

int parseBPProtocol(void)
{
  static int cnt = 0;
  int i = 0;
  int data_size, pkt_size, rxCrc;
  unsigned char send_buff[UART0_RCV_MAX] = { 0 };
  BOOL	bRet = 0;
  char* pptr = NULL;

  for (i = 0; i<stComm.dwRxByte; i++) {
    if (stComm.gRxBuff[i] == 0xAA && stComm.gRxBuff[i + 1] == 0x55) {
      break;
    }
  }
  if (i >= stComm.dwRxByte) { 
    if (pptr = strstr((TCHAR*)stComm.gRxBuff, "boot_ver :")) {
      Sleep(500);
      OnBnClickedBtnDbgfff();
    }
    else if (pptr = strstr((TCHAR*)stComm.gRxBuff, "System_Check :")) {
      Sleep(500);
      OnBnClickedBtnDbgfff();
    }
    else if (pptr = strstr((TCHAR*)stComm.gRxBuff, "ADC_BAT(0)")) {
      Sleep(500);
      OnBnClickedBtnDbgfff();
    }
    else if (strstr((TCHAR*)stComm.gRxBuff, ("MOD [0x2] 190"))) { //pjs 
        stComm.isOBDok = TRUE;
    }
    if (!g_jobOpen) {
    }
    else if (pptr = strstr((TCHAR*)stComm.gRxBuff, "g_branchLDRom")) {
      SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("리부팅"));
      stComm.FOTAReq = 2;
    }
    else if (pptr = strstr((TCHAR*)stComm.gRxBuff, "+QFLST:")) {
      /*
      if (m_CertificatefileNUM == 0xff) {
        m_CertificatefileNUM = 0;
        do {
          pptr += strlen("+QFLST:");
          m_CertificatefileNUM++;
        } while (pptr = strstr((TCHAR*)pptr, "+QFLST:"));
        stComm.testCompleteReqTm = 1;
      }
      m_jigmode = 0;
      stComm.nHwTemptm = 1;
      */
    }
    //else if (strstr((TCHAR*)stComm.gRxBuff, TEXT("+QCCID:"))) {
    else if (strstr((TCHAR*)stComm.gRxBuff, TEXT("AT+CEREG?"))) { //pjs "AT+CGDCONT=2"))) {
      if (stComm.FOTAReq == 1) {
        stComm.FOTAReq = 2;
        if (strlen(g_fotaPath))
          OnBnClickedBtnFsend(g_fotaPath);
      }
      else {
        stComm.FOTAReq = 0;
        stComm.bHwtestReq = 1;
        OnBnClickedGetHWStatus();
#ifdef FOR_FAST_ASSEMBLY
#endif
      }
    }
    return 0;
  }
  stComm.dwRxByte -= i;
  data_size = ((stComm.gRxBuff[i+2]) << 8) | stComm.gRxBuff[i+3];
  if (data_size >= UART0_RCV_MAX) {
    EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(">> APP : data_size over(data_size = %d) ERROR !!!\r\n"), data_size);
    return 0;
  }
  pkt_size = data_size + PKT_HEAD_SIZE;
  if (stComm.dwRxByte>= pkt_size ) {
    if (stComm.gRxBuff[i+data_size + 14] != 0xAA || stComm.gRxBuff[i+data_size + 15] != 0x66) {
      EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\nReceived Data CRC or ETX Error !! .  = 0x%x, 0x%x \r\n"), stComm.gRxBuff[data_size + 14], stComm.gRxBuff[data_size + 15]);
      return 0;
    }
    cnt++;
    rxCrc = ((stComm.gRxBuff[i+data_size + 12] << 8) | ((stComm.gRxBuff[i+data_size + 13]) & 0xff));
    BP_PACKET* presp_pkt = (BP_PACKET*)stComm.gRxBuff+i;
    if (presp_pkt->command == BP_PACKET_UPDATE_APP) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n*****************\r\n>> APP Down is completed (%d/ %d) !!\r\n\r\n5 Second After Device Reset.. Menu : => CLOSE => OPEN\r\n*****************\r\n"),presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_UPDATE_OBD) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n\r\n>> OBD FW Transfer is completed (%d/ %d) !!\r\nWaiting... OBD Device Down..\r\n"), presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_UPDATE_FBP) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n\r\n>> FBP Transfer is completed (%d/ %d) !!\r\nWaiting... FBP Device Down..\r\n"), presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_UPDATE_BLE) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n\r\n>> BLE Transfer is completed (%d/ %d) !!\r\nWaiting... BLE Device Down..\r\n"), presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_UPDATE_NODE) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n\r\n>> TuneIt OBD Transfer is completed (%d/ %d) !!\r\nWaiting... BLE Device Down..\r\n"), presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_MEDIAFILE) {
      if (presp_pkt->cur_no == presp_pkt->total_no)
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("\r\n\r\n>> Media file Transfer is completed (%d/ %d) !!\r\nWaiting... Media Device Down..\r\n"), presp_pkt->cur_no, presp_pkt->total_no);
    }
    else if (presp_pkt->command == BP_PACKET_M4_COMMAND) {
      if (presp_pkt->type == BP_PACKET_M4_RESET) {
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(">> Board Reset !!!\r\n"));
        g_jobOpen = 1;
        m_jigmode = 0;
        g_snCheck = 0;
        stComm.FOTAReq = 0;
      }
      else if (presp_pkt->type == BP_PACKET_M4_SERIAL_NO) {
        //pjs saveSerialNumber();
        //pjs stComm.testCompleteReqTm = 1;
      }
      else if (presp_pkt->type == BP_PACKET_M4_SERVICE) {
      }
      else if (presp_pkt->type == BP_PACKET_M4_HOST_IP) {
      }
      else if (presp_pkt->type == BP_PACKET_M4_GET_CONFIG) {
      }
      else if (presp_pkt->type == BP_PACKET_M4_GET_INFO) {
#if false
        if ((presp_pkt->cur_no == 0) && (presp_pkt->total_no == 0)) {
          parseConfig((PCONFIG_DAT)&presp_pkt->payload);
        }
        else if ((presp_pkt->cur_no == 1) && (presp_pkt->total_no == 1)) {
          parseQmInfo((PQM_INFO)&presp_pkt->payload);
        }
        else if ((presp_pkt->cur_no == 2) && (presp_pkt->total_no == 2)) {
          //if (m_jigmode) {
           // parseHWStatus((pJIG_HW_TEST)&presp_pkt->payload);
            //stComm.bCertificateCopyReq = 1;
          //}
          //else
            parseTripMap((POBD_TRIP_MAP)&presp_pkt->payload);
        }
        else if ((presp_pkt->cur_no == 3) && (presp_pkt->total_no == 3))
        {
            parseCMD_VER((CMD_VER*)&presp_pkt->payload);
        }
        if (g_SerialNO_ok && g_IMEI_ok && g_ICCID_ok
            //&& g_APP_FW_ok
            //&& g_OBD_FW_ok
            )
        {
            saveSerialNumber(); //PJS
            stComm.testCompleteReqTm = 1; //PJS
        }
#else
          if (parseHWStatus((pJIG_HW_TEST)&presp_pkt->payload))
          {
              if (g_snCheck == 1)
              {
#ifdef FOR_FAST_ASSEMBLY
                  saveSerialNumber(); //PJS
                  stComm.testCompleteReqTm = 1; //PJS
#else
                  EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_BLE), true);
                  //EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), true);
                  SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("BLE NAME  점검"));
                  //MessageBeep(MB_OK);
#endif
              }
              else
              {
                  //stComm.testCompleteReqTm = 1;
                  stComm.LEDCheckReqTm = 20;

                  SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("LED 점검"));
                  EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_LED), true);
                  EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_LED), true);
                  //MessageBeep(MB_OK);
              }
          }
          else {
#ifdef FOR_FAST_ASSEMBLY
              //SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("--"));
#else
              if (g_snCheck == 1)
                  SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("S/N Setting ERR"));
              else {
                  SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("HW JIG ERR"));
                  EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_GETINFO), true);
              }
#endif
          }
#endif
      }
      else if (presp_pkt->type == BP_PACKET_M4_SET_HWMODE) {
        m_jigmode = 1;
        /*
        if( g_hwSkip )
          stComm.bCertificateCopyReq = 1;
        else
        */
#ifdef FOR_FAST_ASSEMBLY
        stComm.nHwtestresultReq = 1;
#else
        stComm.nHwtestresultReq = 20*2;// 5;
#endif
      }
      else if (presp_pkt->type == BP_PACKET_M4_JIG_STATUS) {
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(">> APP : JIG Mode OK!!\r\n"));
        OnBnClickedBtnSetmdmuart();
        //UCHAR hw_test_step = presp_pkt->cur_no;
        //str = check_jig_step((AISD_JIG_DATA*)&presp_pkt->payload);
      }
      /*
      else if (presp_pkt->type == BP_PACKET_M4_SET_DBG) {
        copyCertificates();
      }
      */
    }
    int n_ret = device_resp_service((unsigned char*)stComm.gRxBuff+i, data_size + PKT_HEAD_SIZE, send_buff);
    if (n_ret > 0) {
      bRet = ComWirte((char*)send_buff, n_ret);
      //EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("DATA RECEIVED ETX\r\n"));
      if (bRet == TRUE) {
        EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT(">> APP : Send Data OK no (%d/ %d) (ret = %d) (bRet= %d)!!!\r\n"), stFILE_INFO.cur_no, stFILE_INFO.total_no, n_ret, bRet);
      }
    }
    return 1;
  }

  return 0;
}

VOID CALLBACK MainTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
  static int i = 0;

  ComboClassComPortEnumerate(IDC_COMBO_COM_PORT);
  //EditClassPrintf(IDC_EDIT_LOGVIEWER, TEXT("timer : %d\r\n"), i++);
  if (stComm.bAutoConnectedReq) {
    if (doUartConnection(0) == true) {
      SetDlgItemText(hDlgMain, IDC_CONNECT, (LPCSTR)TEXT("CLOSE"));
      memset(g_fotaPath, 0, sizeof(g_fotaPath));
      checkFOTA();
      stComm.bAutoConnectedReq = false;
    }
  }
  if( stComm.bHwtestReq ){
    stComm.bHwtestReq = 0;
    EditClassLogClear(IDC_EDIT_LOGVIEWER);
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT("Waitting... for checking HW!! \r\n"));
    EnableWindow(GetDlgItem(hDlgMain, IDC_BTN_HW_TEST), true);
    SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("검사시작"));
    EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_UDR), true);
    EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_LTE), true);
    EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_ESIM), true);
    EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_ACCEROMETER), true);
    EnableWindow(GetDlgItem(hDlgMain, IDC_STATIC_OBDCOMM), true);
  }
  if (stComm.nHwtestresultReq) {
    stComm.nHwtestresultReq--;
    EditClassPrintf2(IDC_EDIT_LOGVIEWER, TEXT(" %d"), stComm.nHwtestresultReq);
    if (stComm.nHwtestresultReq == 0) {
        if (hThread == NULL) {
            if (doUartConnection(0) == false)
            {
            }
            Sleep(10);
        }
      OnBnClickedGetInfo();
    }
  }
  /*
  if (stComm.bCertificateCopyReq) {
    stComm.bCertificateCopyReq = 0;
    OnBnClickedBtnSetmdmuart();
  }
  if (stComm.nHwTemptm) {
    stComm.nHwTemptm--;
    if (stComm.nHwTemptm == 0)
      m_jigmode = 1;
  }
  
  if (stComm.fileListReqTm) {
    stComm.fileListReqTm--;
    if (stComm.fileListReqTm == 0) {
      //OnBnClickedBtSendqflst();
    }
  }
  */
  if (stComm.testCompleteReqTm) {
    stComm.testCompleteReqTm--;
    if (stComm.testCompleteReqTm == 0) {
      SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("검사완료"));
      
      EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SET_SN), false);
      EnableWindow(GetDlgItem(hDlgMain, IDC_BUTTON_SAVE), false);
      EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_LED), false);
      EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_BLE), false);
      
    }
  }
  if (stComm.LEDCheckReqTm>0 ) {
    stComm.LEDCheckReqTm--;
    if (stComm.LEDCheckReqTm == 0) {
      stComm.LEDCheckReqTm = -1;
      /*
      SetDlgItemText(hDlgMain, IDC_BTN_HW_TEST, TEXT("LED 불량 or LED항목 체크안함"));
      EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_LED), false);
      EnableWindow(GetDlgItem(hDlgMain, IDC_CHECK_BLE), false);
      */
    }
  }
}
