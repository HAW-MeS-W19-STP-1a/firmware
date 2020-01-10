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


/*- Globale Variablen --------------------------------------------------------*/
ATCmd_DataModeSrc eDataModeSrc;
bool bDataMode;


/*- Modulglobale Variablen ---------------------------------------------------*/
static const ATCmd_CbConf asCommands[] = {
  {"CTEMP",   ATCmd_OK,       ATCmd_TempRead,   0,                0},
  {"CPRES",   ATCmd_OK,       ATCmd_PresRead,   0,                0},
  {"CHUM",    ATCmd_OK,       ATCmd_HumRead,    0,                0},
  {"CTIME",   ATCmd_TimeTest, ATCmd_TimeRead,   ATCmd_TimeWrite,  0},
  {"CALIGN",  ATCmd_OK,       ATCmd_AlignRead,  0,                0},
  {"CGNSPOS", ATCmd_PosTest,  ATCmd_PosRead,    ATCmd_PosWrite,   0},
  {"CGNSPWR", ATCmd_GnsPwrTest,0,               ATCmd_GnsPwrWrite,0},
  {"CGNSTST", ATCmd_GnsTstTest,ATCmd_GnsTstRead,ATCmd_GnsTstWrite,0},
  {"CPWR",    ATCmd_OK,       ATCmd_PwrRead,    0,                0},
  {"CINTV",   ATCmd_IntvTest, 0,                ATCmd_IntvWrite,  0},
  {"CGUI",    ATCmd_OK,       ATCmd_GuiRead,    0,                0},
  {"CWKUP",   ATCmd_OK,       0,                0,                ATCmd_ForceWkup},
  {"CLOG",    ATCmd_OK,       0,                0,                ATCmd_LogClear},
  {"CDEBUG",  ATCmd_DebugTest,ATCmd_DebugRead,  ATCmd_DebugWrite, 0},
  {"CFILE",   ATCmd_FileTest, ATCmd_FileRead,   ATCmd_FileWrite,  0},
  {"CTRACK",  ATCmd_TrackTest,ATCmd_TrackRead,  ATCmd_TrackWrite, 0},
  {"CTURN",   0,              0,                ATCmd_TurnWrite,  0}
};
#define NUM_ATCMD_CONF (sizeof(asCommands)/sizeof(*asCommands))

static int iExitDataMode;


/*- Lokale Funktionen --------------------------------------------------------*/
static void ATCmd_SendCONNECT(void)
{
  sprintf((volatile char*)aucUart1TxBuf, "CONNECT\r\n");
  UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
  while(!UART1_IsTxReady());
}

static void ATCmd_SendOK(void)
{
  sprintf((volatile char*)aucUart1TxBuf, "OK\r\n");
  UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
  while(!UART1_IsTxReady());
}

static void ATCmd_SendError(void)
{
  sprintf((volatile char*)aucUart1TxBuf, "ERROR: %s\r\n", aucUart1RxBuf);
  UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
  while(!UART1_IsTxReady());
}

static void ATCmd_ReceiveNext(void)
{
  if (!bDataMode)
  {
    UART1_ReceiveUntil('\r', COMMLIB_UART1_MAX_BUF);
  }
  else
  {
    UART1_Receive(1);
  }
}

void ATCmd_Init(void)
{
  eDataModeSrc = ATCmd_DataModeSrc_None;
  bDataMode = false;
  iExitDataMode = 3;
}

void ATCmd_Poll(void)
{
  unsigned char ucIndex;
  bool bResult = false;
  
  if (UART1_IsRxReady())
  {
    if (!bDataMode && UART1_GetRxCount() >= 2)
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
        else if (aucUart1RxBuf[2] == 'E')
        {
          if (aucUart1RxBuf[3] == '1')
          {
            UART1_SetEchoMode(true);
          }
          else
          {
            UART1_SetEchoMode(false);
          }
          bResult = true;
        }
        else if (aucUart1RxBuf[2] == 'H')
        {
          /* "Hang Up" - Data Mode beenden                */
          bDataMode = false;
          eDataModeSrc = ATCmd_DataModeSrc_None;
          bResult = true;
        }
        else if (aucUart1RxBuf[2] == 'I')
        {
          /* Produktinfo anzeigen                         */
          sprintf((volatile char*)aucUart1TxBuf, "STP_1A WETTERSTATION\r\n");
          UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
          while(!UART1_IsTxReady());
          UART1_FlushTx();
          bResult = true;
        }
        else if (aucUart1RxBuf[2] == 'O')
        {
          /* Data Mode wieder aufnehmen                   */
          if (eDataModeSrc != ATCmd_DataModeSrc_None)
          {
            ATCmd_SendCONNECT();
            bDataMode = true;
            bResult = true; /* wird unten nicht gesendet! */
          }
        }
        else
        {
          /* Nur "AT" - immer mit OK antworten            */
          bResult = (aucUart1RxBuf[2] == '\r');
        }
      }
    }
    else if (bDataMode)
    {
      /* Prüfen, ob Data Mode beendet werden soll           */
      if (aucUart1RxBuf[0] == '+')
      {
        --iExitDataMode;
        if (iExitDataMode == 0)
        {
          /* Data Mode pausieren                            */
          iExitDataMode = 3;
          bDataMode = false;
          bResult = true;
        }
        else
        {
          /* ToDo: Data Mode Pipe zurücksenden              */
        }
        
        /* Warten, ob "+++" eingetippt wurde                */
      }
      else
      {
        /* ToDo: Data Mode Pipe zurücksenden                */
        iExitDataMode = 3;
      }
    }
    
    /* "OK" oder "ERROR" senden                           */
    if (!bDataMode)
    {
      if (bResult)
      {
        ATCmd_SendOK();
      }
      else
      {
        ATCmd_SendError();
      }
    }
    
    /* Empfang starten                                    */
    UART1_FlushRx();
    ATCmd_ReceiveNext();
  }
}

bool ATCmd_GetDataMode(ATCmd_DataModeSrc eSource)
{
  return bDataMode && (eDataModeSrc == eSource);
}