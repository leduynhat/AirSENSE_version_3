/******************************************************************************/
/* This file has been generated by the uGFX-Studio                            */
/*                                                                            */
/* http://ugfx.org                                                            */
/******************************************************************************/

#ifndef _GUI_H_
#define _GUI_H_

#include "iot_ugfx.h"

// Widget groups
#define GROUP_DEFAULT 0

// GListeners
extern GListener gl;

// GHandles
extern GHandle ghContainerPage0;
extern GHandle ghKeyboard1;
extern GHandle ghTextedit1_copy;
extern GHandle ghTextedit1_copy_1;
extern GHandle ghTextedit1_copy_2;
extern GHandle ghContainerPage1;
extern GHandle ghSlider1;
extern GHandle ghSlider1_1;
extern GHandle ghList1;
extern GHandle ghList1_1;

// Function Prototypes
void guiCreate(void);
void guiShowPage(unsigned pageIndex);
void guiEventLoop(void);

#endif /* _GUI_H_ */
