/*- Headerdateien ------------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "commlib.h"
#include "ATCmd_CmdFunc.h"
#include "ATCmd.h"


/*- Typdefinitionen ----------------------------------------------------------*/
typedef bool (*ATCmd_Callback)(const char* pszBuf);

typedef struct tag_ATCmd_CbConf {
  const char* pszCmdStr;
  const ATCmd_Callback pfTestCmd;
  const ATCmd_Callback pfReadCmd;
  const ATCmd_Callback pfWriteCmd;
  const ATCmd_Callback pfExecCmd;
} ATCmd_CbConf;

/*- Modulglobale Variablen ---------------------------------------------------*/
static const ATCmd_CbConf asCommands[] = {
  {"CTEMP",   ATCmd_OK,       ATCmd_TempRead,   0,                0},
  {"CPRES",   ATCmd_OK,       ATCmd_PresRead,   0,                0},
  {"CHUM",    ATCmd_OK,       ATCmd_HumRead,    0,                0},
  {"CTIME",   ATCmd_TimeTest, ATCmd_TimeRead,   ATCmd_TimeWrite,  0},
  {"CALIGN",  ATCmd_OK,       ATCmd_AlignRead,  0,                0},
  {"CGNSPOS", ATCmd_PosTest,  ATCmd_PosRead,    ATCmd_PosWrite,   0},
  {"CPWR",    ATCmd_OK,       ATCmd_PwrRead,    0,                0},
  {"CINTV",   ATCmd_IntvTest, 0,                ATCmd_IntvWrite,  0},
  {"CGUI",    ATCmd_OK,       ATCmd_GuiRead,    0,                0},
};
#define NUM_ATCMD_CONF (sizeof(asCommands)/sizeof(*asCommands))

/*- Lokale Funktionen --------------------------------------------------------*/
static void ATCmd_SendOK(void)
{
  sprintf((volatile char*)aucUart1TxBuf, "OK\r\n");
  UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
  while(!UART1_IsTxReady());
}

static void ATCmd_SendError(void)
{
  sprintf((volatile char*)aucUart1TxBuf, "ERROR\r\n");
  UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
  while(!UART1_IsTxReady());
}

void ATCmd_Init(void)
{
  ;
}

void ATCmd_Poll(void)
{
  unsigned char ucIndex;
  bool bResult = false;
  
  if (UART1_IsRxReady())
  {
    if (UART1_GetRxCount() >= 2)
    {
      /* Mindestens "AT"                                  */
      UART1_FlushTx();
      if (strncmp(&aucUart1RxBuf[0], "AT", 2) == 0)
      {
        /* "AT" gefunden                                  */
        if (aucUart1RxBuf[2] == '+')
        {
          /* Befehl folgt                                 */
          for (ucIndex = 0; ucIndex < NUM_ATCMD_CONF; ++ucIndex)
          {
            if (strncmp(&aucUart1RxBuf[3], asCommands[ucIndex].pszCmdStr, strlen(asCommands[ucIndex].pszCmdStr)) == 0)
            {
              /* Command String gefunden                  */
              char* pszCmd = &aucUart1RxBuf[3 + strlen(asCommands[ucIndex].pszCmdStr)];              
              if (pszCmd[0] == '=')
              {
                if (pszCmd[1] == '?')
                {
                  /* Test Command */
                  bResult = (asCommands[ucIndex].pfTestCmd == 0) ? false : asCommands[ucIndex].pfTestCmd(0);
                }
                else
                {
                  /* Write Command */
                  bResult = (asCommands[ucIndex].pfWriteCmd == 0) ? false : asCommands[ucIndex].pfWriteCmd(pszCmd + 1);
                }
              }
              else if (pszCmd[0] == '?')
              {
                /* Read Command */
                bResult = (asCommands[ucIndex].pfReadCmd == 0) ? false : asCommands[ucIndex].pfReadCmd(0);
              }
              else if (pszCmd[0] == '\r')
              {
                /* Execute Command */
                bResult = (asCommands[ucIndex].pfExecCmd == 0) ? false : asCommands[ucIndex].pfExecCmd(0);
              }
              
              /* Eintrag gefunden - Suche abbrechen       */
              break;
            }
          }
        }
        else
        {
          /* Nur "AT" - immer mit OK antworten            */
          bResult = true;
        }
      }
    }
    
    /* "OK" oder "ERROR" senden                           */
    if (bResult)
    {
      ATCmd_SendOK();
    }
    else
    {
      ATCmd_SendError();
    }
    
    /* Empfang starten                                    */
    UART1_FlushRx();
    UART1_ReceiveUntil('\r', COMMLIB_UART1_MAX_BUF);
  }
}