#ifndef USERLIB_ATCMD_CMDFUNC_H_
#define USERLIB_ATCMD_CMDFUNC_H_

#include <stdbool.h>

/*- Funktionsprototypen ------------------------------------------------------*/
bool ATCmd_OK(const char* pszBuf);

bool ATCmd_TempRead(const char* pszBuf);

bool ATCmd_PresRead(const char* pszBuf);

bool ATCmd_HumRead(const char* pszBuf);

bool ATCmd_WindRead(const char* pszBuf);

bool ATCmd_TimeTest(const char* pszBuf);
bool ATCmd_TimeRead(const char* pszBuf);
bool ATCmd_TimeWrite(const char* pszBuf);

bool ATCmd_AlignRead(const char* pszBuf);

bool ATCmd_PosTest(const char* pszBuf);
bool ATCmd_PosRead(const char* pszBuf);
bool ATCmd_PosWrite(const char* pszBuf);

bool ATCmd_PwrRead(const char* pszBuf);

bool ATCmd_IntvTest(const char* pszBuf);
bool ATCmd_IntvWrite(const char* pszBuf);

bool ATCmd_GuiRead(const char* pszBuf);

#endif /* USERLIB_ATCMD_CMDFUNC_H_ */