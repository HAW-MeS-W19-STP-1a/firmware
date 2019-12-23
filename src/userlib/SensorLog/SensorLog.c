#include <string.h>
#include "SensorLog.h"


/*- Modulglobale Variablen ---------------------------------------------------*/
static SensorLogItem sLogBuffer[NUM_SENSORLOG_RINGITEMS];
static unsigned uHead;


void SensorLog_Init(void)
{
  SensorLog_Clear();
}

void SensorLog_Clear(void)
{
  memset(sLogBuffer, 0, sizeof(sLogBuffer));
  uHead = 0;
}

SensorLogItem* SensorLog_Advance(void)
{
  register unsigned uRet = uHead;
  ++uHead;
  if (uHead >= NUM_SENSORLOG_RINGITEMS)
  {
    uHead = 0;
  }
  return &sLogBuffer[uRet];
}

SensorLogItem* SensorLog_Dump(unsigned uOffset)
{
  register unsigned uIndex;
  uIndex = uHead + uOffset;
  while (uIndex >= NUM_SENSORLOG_RINGITEMS)
  {
    uIndex -= NUM_SENSORLOG_RINGITEMS;
  }
  
  return &sLogBuffer[uIndex];
}