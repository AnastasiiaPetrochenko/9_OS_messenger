#ifndef DATALIB_H
#define DATALIB_H

#include <string.h>
#include <QString>

#define DEFAULT_MAX_MESSAGE_SIZE 255    // Maximum text message size without '\0'

typedef unsigned long long id_t;

enum MessageCommand
{
    NEW_USER,
    MESSAGE,
	REMOVE_USER,
	NEW_ID
};

template <quint32 msgSize = DEFAULT_MAX_MESSAGE_SIZE>
struct MessageData
{
    MessageCommand com;
    id_t senderId;
    id_t recvId;
	char msg[msgSize + 1];
    
	inline quint32 GetMessageSize() const { return msgSize; }
	
    inline void SetMessage(const QString &message)
    {
        strcpy_s(msg, msgSize + 1, message.toLocal8Bit());
    }
	
	inline void SetMessage(const char *message)
    {
        strcpy_s(msg, msgSize + 1, message);
    }
};

#endif // DATALIB_H
