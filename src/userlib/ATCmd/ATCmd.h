#ifndef USERLIB_ATCMD_H_
#define USERLIB_ATCMD_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdbool.h> 


/*- Typdefinitionen ----------------------------------------------------------*/
typedef enum {
  ATCmd_DataModeSrc_None,
  ATCmd_DataModeSrc_GPS,
  ATCmd_DataModeSrc_Debug
} ATCmd_DataModeSrc;


/*- DataMode Variablen -------------------------------------------------------*/
extern ATCmd_DataModeSrc eDataModeSrc;
extern bool bDataMode;


/*- Funktionsprototypen ------------------------------------------------------*/
void ATCmd_Init(void);
void ATCmd_Poll(void);
bool ATCmd_GetDataMode(ATCmd_DataModeSrc eSource);

#endif /* USERLIB_ATCMD_H_ */