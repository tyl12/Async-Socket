#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_STR_SIZE  1024   //数据包的长度
#define MSG_INVALID (0x00FF)
#define MSG_FRAME_INFO (1)
#define MSG_TRANSFER_STR (2)

/*
struct MsgHead{
    int msgId;
};
typedef struct MsgHead msgHead_t;
*/

struct MsgFrameInfo{
    int format;
    int width;
    int height;
    int bufLen;
};
typedef struct MsgFrameInfo msgFrameInfo_t;

struct MsgTransferString{
    char str[MAX_STR_SIZE];
};

#endif
