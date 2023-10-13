#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <Commctrl.h>
#include <shobjidl.h> 
#include <strsafe.h>
#include "resource.h"
#include "main.h"
#include <direct.h>		//mkdir

using namespace ExcelFormat;
char *title_report[] = { "NO","Serial NO","IMEI","ICCID","APP FW","OBD FW","ESIM Serial"};
#define EXEL_INDEX_NO 0
#define EXEL_INDEX_SERIAL_NO 1
#define EXEL_INDEX_IMEI     2
#define EXEL_INDEX_ICCID    3
#define EXEL_INDEX_VER_APP  4
#define EXEL_INDEX_VER_OBD  5
#define EXEL_INDEX_ESIM_Serial  6
BasicExcel xls;
BasicExcelWorksheet *sheet;



#if 1
int check_Excel(BasicExcelWorksheet* sheet, char* serial, char* imei, char* iccid, int* pErrRow, int* pErrCol)
{
    int row = 0, nErrRow = 0, ErrCol = 0;
    const char* r_serial;
    const char* r_imei;
    const char* r_iccid;
    while (1) {
        r_serial = sheet->Cell(row, EXEL_INDEX_SERIAL_NO)->GetString();// serial);
        if (r_serial == NULL)
            break;
        if (strcmp(r_serial, serial) == 0)
        {
            nErrRow = row;
            ErrCol = EXEL_INDEX_SERIAL_NO;
        }
#ifndef _DEBUG
        r_imei =  sheet->Cell(row, EXEL_INDEX_IMEI)->GetString();//(imei);
        if (strcmp(r_imei, imei) == 0)
        {
            nErrRow = row;
            ErrCol = EXEL_INDEX_IMEI;
        }
        r_iccid = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(iccid);
        if (strcmp(r_iccid, iccid) == 0)
        {
            nErrRow = row;
            ErrCol = EXEL_INDEX_ICCID;
        }
#endif
        row++;
    }
    *pErrRow = nErrRow;
    *pErrCol = ErrCol;
    return row;
}
const char* m_pread;
const char* m_pread2;
int save_Excel(int save, char* serial, char* empver, char* ebpver, char* imei, char* iccid,char* ret_str_err)
{
  BasicExcel xls;
  BasicExcelWorksheet* sheet;
  static char fileName[100];
  static int first = 0;
  int row = 0;
  int err = 0;
  char str_err[100], strNO[10], strEsimSerial[200], strSaveAs[200];
  bool ret;
  bool isNew;

  char strFolderPath[] = { ".\\Excel_File" };// excel 폴더 저장 주소

  //--------------------------------------------------------------------------
  if (!first) {
      first = 1;
      int nResult = _mkdir(strFolderPath);

      memset(fileName, 0, 100);
      sprintf_s(fileName, "%s\\Assembly Result(IMSC)_23.xls", strFolderPath);

  }
  //--------------------------------------------------------------------------
  if (xls.Load(fileName))
  {
      isNew = false;
  }
  else {
      xls.New(1);
      isNew = true;
  }
  sheet = xls.GetWorksheet(0);
  //--------------------------------------------------------------------------
  if (isNew){
    XLSFormatManager fmt_mgr(xls);
    ExcelFont font_bold;
    font_bold._weight = FW_BOLD;
    CellFormat fmt_bold(fmt_mgr);
    fmt_bold.set_font(font_bold);
    for (int col = 0; col < sizeof(title_report) / sizeof(title_report[0]); col++) {
        BasicExcelCell* cell = sheet->Cell(row, col);
        cell->Set(title_report[col]);
        cell->SetFormat(fmt_bold);
    }
  }
  //--------------------------------------------------------------------------  
  int nErrRow, nErrCol;
  row = check_Excel(sheet, serial, imei, iccid, &nErrRow, &nErrCol);
 if (nErrRow)
 {
     sprintf_s(str_err, "%s와  %d 라인의 %s이 같음)", serial, nErrRow,title_report[nErrCol]);
     memcpy(ret_str_err, str_err, 50);
     return row;
  }
  //--------------------------------------------------------------------------

  memset(strNO, 0, sizeof(strNO));
  sprintf_s(strNO, "%d", row);
  sheet->Cell(row, EXEL_INDEX_NO)->Set(strNO);
 // m_pread = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);
  sheet->Cell(row, EXEL_INDEX_SERIAL_NO)->Set(serial);
  sheet->Cell(row, EXEL_INDEX_IMEI)->Set(imei);
  sheet->Cell(row, EXEL_INDEX_ICCID)->Set(iccid);
  sheet->Cell(row, EXEL_INDEX_VER_APP)->Set(empver);
  sheet->Cell(row, EXEL_INDEX_VER_OBD)->Set(ebpver);
  //m_pread = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);
  memset(strEsimSerial, 0, sizeof(strEsimSerial));
  memcpy(strEsimSerial, iccid , 10);//3~11 //memcpy(strEsimSerial, iccid + 11, 7);
  sheet->Cell(row, EXEL_INDEX_ESIM_Serial)->Set(strEsimSerial);
  //m_pread2 = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);

 // sprintf_s(strSaveAs, "%s\\%s_%s", strFolderPath, serial, fileName );
 sprintf_s(strSaveAs, "%s\\%s_Assembly Result(IMSC)_23.xls", strFolderPath, serial);
  ret = xls.SaveAs(strSaveAs); 
  if (xls.Load(strSaveAs))
  {
      sheet = xls.GetWorksheet(0);
      row = check_Excel(sheet, serial, imei, iccid, &nErrRow, &nErrCol);
      row--;
  }
#ifdef _DEBUG
  if (xls.Load(fileName))
  {
      sheet = xls.GetWorksheet(0);
      sheet->Cell(row, EXEL_INDEX_NO)->Set(strNO);
      m_pread = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);
      sheet->Cell(row, EXEL_INDEX_SERIAL_NO)->Set(serial);
      sheet->Cell(row, EXEL_INDEX_VER_APP)->Set(empver);
      sheet->Cell(row, EXEL_INDEX_VER_OBD)->Set(ebpver);
      sheet->Cell(row, EXEL_INDEX_IMEI)->Set(imei);
      sheet->Cell(row, EXEL_INDEX_ICCID)->Set(iccid);
      m_pread = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);
      sheet->Cell(row, EXEL_INDEX_ESIM_Serial)->Set(strEsimSerial);
      m_pread2 = sheet->Cell(row, EXEL_INDEX_ICCID)->GetString();//(imei);
  }
#endif 
  if (isNew)
      ret = xls.SaveAs(fileName);
  else {
      ret = xls.Save();// As(fileName);
  }

  if (ret == false)
      row = -1000;
  xls.Close();
  return row;
}
#else
void save_Excel(int save,char *serial, char* empver, char* ebpver, char* imei, char* iccid)
{
  //BasicExcel xls;
  static char fileName[50];
  static int first = 0;
  static int row = 0;
  char tmp[50];
  int col=0;

  if (save) {
    if (first==0 )
      return;
    sprintf_s(tmp, "%s.xls", fileName);
    //xls.SaveAs("a.xls");
    xls.SaveAs(tmp);
    return;
  }

  if (!first) {
    first = 1;
    memset(fileName, 0, 50);
    memcpy(fileName, serial, strlen(serial));
    xls.New(1);
    sheet = xls.GetWorksheet(0);
    XLSFormatManager fmt_mgr(xls);
    ExcelFont font_bold;
    font_bold._weight = FW_BOLD;
    CellFormat fmt_bold(fmt_mgr);
    fmt_bold.set_font(font_bold);
    for (col = 0; col < sizeof(title_report)/sizeof(title_report[0]); col++) {
      BasicExcelCell* cell = sheet->Cell(row, col);
      cell->Set(title_report[col]);
      cell->SetFormat(fmt_bold);
    }
  }
  memset(tmp, 0, sizeof(tmp));
  memmove(tmp, iccid + 11, 7);
  row++;col = 0;
  sheet->Cell(row, col++)->Set(serial);
  sheet->Cell(row, col++)->Set(empver);
  sheet->Cell(row, col++)->Set(ebpver);
  sheet->Cell(row, col++)->Set(imei);
  sheet->Cell(row, col++)->Set(iccid);
  sheet->Cell(row, col++)->Set(tmp);
  /*
  ExcelFont font_red_bold;
  font_red_bold._weight = FW_BOLD;
  font_red_bold._color_index = EGA_GREEN;
  CellFormat fmt_red_bold(fmt_mgr, font_red_bold);
  fmt_red_bold.set_color1(COLOR1_PAT_SOLID);
  fmt_red_bold.set_color2(MAKE_COLOR2(EGA_BLUE, 0));
  CellFormat fmt_green(fmt_mgr, ExcelFont().set_color_index(EGA_GREEN));
  for (col = 0; col < 10; col++) {
    BasicExcelCell* cell = sheet->Cell(row, col);
    cell->Set("xxx");
    cell->SetFormat(fmt_red_bold);
    cell = sheet->Cell(row, ++col);
    cell->Set("yyy");
    cell->SetFormat(fmt_green);
  }
  */
  //xls.SaveAs("a.xls");
}
#endif


void test_Excel(void)
{
  BasicExcel xls;

  xls.New(1);
  BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
  XLSFormatManager fmt_mgr(xls);
  ExcelFont font_bold;
  font_bold._weight = FW_BOLD;
  CellFormat fmt_bold(fmt_mgr);
  fmt_bold.set_font(font_bold);
  int col, row = 0;
  for (col = 0; col < 10; col++) {
    BasicExcelCell* cell = sheet->Cell(row, col);
    cell->Set("TITLE");
    cell->SetFormat(fmt_bold);
  }
  while (++row < 4) {
    for (int col = 0; col < 10; col++)
      sheet->Cell(row, col)->Set("Text");
  }
  row++;
  /*
  ExcelFont font_red_bold;
  font_red_bold._weight = FW_BOLD;
  font_red_bold._color_index = EGA_GREEN;
  CellFormat fmt_red_bold(fmt_mgr, font_red_bold);
  fmt_red_bold.set_color1(COLOR1_PAT_SOLID);
  fmt_red_bold.set_color2(MAKE_COLOR2(EGA_BLUE, 0));
  CellFormat fmt_green(fmt_mgr, ExcelFont().set_color_index(EGA_GREEN));
  for (col = 0; col < 10; col++) {
    BasicExcelCell* cell = sheet->Cell(row, col);
    cell->Set("xxx");
    cell->SetFormat(fmt_red_bold);
    cell = sheet->Cell(row, ++col);
    cell->Set("yyy");
    cell->SetFormat(fmt_green);
  }
  */
  xls.SaveAs("a.xls");
}

