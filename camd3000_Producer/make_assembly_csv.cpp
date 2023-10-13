#include <windows.h>
#include <cstdio>
#include <shobjidl.h> 
#include <sys/stat.h>
#include "resource.h"
#include "main.h"
#include <tchar.h>
#include <ctime> //C++
#include <string.h>


char* m_strBleInfoFilePath = ".\\Assembly Result(IMSC)_23.csv";
char* title_report_csv[] = { "NO","Serial NO","IMEI","ICCID","APP FW","OBD FW","ESIM Serial" };//"NO,Serial NO,IMEI,ICCID,APP FW,OBD FW,ESIM Serial";
#define EXEL_INDEX_NO 0
#define EXEL_INDEX_SERIAL_NO 1
#define EXEL_INDEX_IMEI     2
#define EXEL_INDEX_ICCID    3
#define EXEL_INDEX_VER_APP  4
#define EXEL_INDEX_VER_OBD  5
#define EXEL_INDEX_ESIM_Serial  6


int check_csv(char* FilePath, char* serial, char* imei, char* iccid, int* pErrRow, int* pErrCol)
{
	int row = 0, nErrRow = 0, ErrCol = 0;

	char buffer[200], * ps, *context;

	FILE* stream;
	int err = fopen_s(&stream,FilePath, "rt");

//	if (err == NULL) return 0;
	if (stream == NULL) return 0;
	while (fgets(buffer, 200, stream) != NULL) // 텍스트파일을 한줄씩 읽습니다.
//  while (fgets(buffer, 200, stream) != NULL) // 텍스트파일을 한줄씩 읽습니다.
	{
		ps = strchr(buffer, '\n'); // 제일 뒤쪽의 new line의 char을 찿아.
		if (ps != NULL) *ps = '\0';// new line의 char을 null문자로 바꿉니다.

		int cn = 0;
		ps = strtok_s(buffer, ",", &context); // context 에는 분리된 후 남은 문자열이 들어간다.
		if (ps == NULL)
			break;
		while (ps) {
			switch (cn)
			{
			case EXEL_INDEX_NO:
				break;
			case EXEL_INDEX_SERIAL_NO:
				if (strcmp(ps, serial) == 0)
				{
					nErrRow = row;
					ErrCol = EXEL_INDEX_SERIAL_NO;
				}
				break;
#ifndef _DEBUG
			case EXEL_INDEX_IMEI:
				if (strcmp(ps, imei) == 0)
				{
					nErrRow = row;
					ErrCol = EXEL_INDEX_IMEI;
				}
				break;
			case EXEL_INDEX_ICCID:
				if (strcmp(ps, iccid) == 0)
				{
					nErrRow = row;
					ErrCol = EXEL_INDEX_ICCID;
				}
				break;
#endif
			case EXEL_INDEX_VER_APP:
				break;
			case EXEL_INDEX_VER_OBD:
				break;
			case EXEL_INDEX_ESIM_Serial:
				break;
			}
			ps = strtok_s(NULL, ",", & context);
			cn++;
		}
		row++;
	}
	fclose(stream);

	*pErrRow = nErrRow;
	*pErrCol = ErrCol;
	return row;
}
int save_csv(int save, char* serial, char* empver, char* ebpver, char* imei, char* iccid, char* ret_str_err)
{
	static char fileName[100];
	int row = 0;
	int err = 0;
	char str_err[100], strNO[10], strEsimSerial[200];
	bool ret;
	//--------------------------------------------------------------------------  
	int nErrRow =0, nErrCol = 0;
	row = check_csv(m_strBleInfoFilePath, serial, imei, iccid, &nErrRow, &nErrCol);
	if (nErrRow)
	{
		sprintf_s(str_err, "%s와  %d 라인의 %s이 같음)", serial, nErrRow, title_report_csv[nErrCol]);
		memcpy(ret_str_err, str_err, 50);
		return row;
	}
	//--------------------------------------------------------------------------
	FILE* fp = NULL;
	fopen_s(&fp, m_strBleInfoFilePath, "a+");
	if (row == 0)
	{
		fprintf(fp, "%s,%s,%s,%s,%s,%s,%s\n"
		 ,title_report_csv[0], title_report_csv[1], title_report_csv[2], title_report_csv[3], title_report_csv[4], title_report_csv[5], title_report_csv[6]);
		row++;
	}
	memset(strNO, 0, sizeof(strNO));
	sprintf_s(strNO, "%d", row);
	memset(strEsimSerial, 0, sizeof(strEsimSerial));
	memcpy(strEsimSerial, iccid + 11, 7);
	fprintf(fp, "%s,%s,%s,%s,%s,%s,%s\n", strNO, serial, imei, iccid, empver, ebpver, strEsimSerial);
	fclose(fp);
	return row;
}
#include <stdio.h>
#include <direct.h>		//mkdir
#include <errno.h>		//errno

//////////////////////excel save///////////////////////
char m_logfilePath[200];
char strFolderPath1[] = { ".\\log" };
void make_log_file()
{
	int nResult = _mkdir(strFolderPath1);

	time_t now = time(NULL);
	struct tm date;
	int err = localtime_s(&date,&now);
	sprintf_s(m_logfilePath, "%s\\%02d%02d%02d.log", strFolderPath1, date.tm_year-100, date.tm_mon+1, date.tm_mday);
}
void save_log(char* data)
{
	FILE* fp = NULL;
	fopen_s(&fp, m_logfilePath, "a+");
	fprintf(fp, "%s,", data);
	fclose(fp);
}

