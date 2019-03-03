#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

typedef struct {
	int*		value;
} buffer_item;

// typedef struct {
//   buffer_item
// } buffer_queue;
//
// // command information
// typedef struct {
// 	char*		cmd;
// 	S_List*		args;
// 	char*		fOut;
// 	char*		fIn;
// 	int			wait;
// 	int			hasPipe;
// 	int			pipeFd[2];
// } S_Cmd;
//
// // all commands linked list
// typedef struct _S_CmdList{
// 	S_Cmd*		c;
// 	struct _S_CmdList* next;
// } S_CmdList;

#endif
