#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H


#include <Ints.h>
#include <BASE/message.h>

MessageDispatchResult SetupMainHandler(struct tag_message& message);
i32 SetupNewMap(void);

#endif
