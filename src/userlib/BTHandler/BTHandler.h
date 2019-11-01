#ifndef USERLIB_BTHANDLER_H_
#define USERLIB_BTHANDLER_H_

#include <stdbool.h>

void BTHandler_Init(void);
void BTHandler_Poll(void);
void BTHandler_Task1s(void);
void BTHandler_TakeWakeup(void);
bool BTHandler_IsActive(void);

#endif /* USERLIB_BTHANDLER_H_ */