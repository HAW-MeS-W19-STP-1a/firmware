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

bool ATCmd_GnsPwrTest(const char* pszBuf);
bool ATCmd_GnsPwrWrite(const char* pszBuf);

bool ATCmd_GnsTstTest(const char* pszBuf);
bool ATCmd_GnsTstRead(const char* pszBuf);
bool ATCmd_GnsTstWrite(const char* pszBuf);

bool ATCmd_PwrRead(const char* pszBuf);

bool ATCmd_IntvTest(const char* pszBuf);
bool ATCmd_IntvWrite(const char* pszBuf);

bool ATCmd_GuiRead(const char* pszBuf);

bool ATCmd_ForceWkup(const char* pszBuf);

bool ATCmd_LogClear(const char* pszBuf);

bool ATCmd_DebugTest(const char* pszBuf);
bool ATCmd_DebugRead(const char* pszBuf);
bool ATCmd_DebugWrite(const char* pszBuf);

bool ATCmd_FileTest(const char* pszBuf);
bool ATCmd_FileRead(const char* pszBuf);
bool ATCmd_FileWrite(const char* pszBuf);

#endif /* USERLIB_ATCMD_CMDFUNC_H_ */