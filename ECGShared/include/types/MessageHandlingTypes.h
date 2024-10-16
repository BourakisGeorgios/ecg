#ifndef MESSAGE_HANDLING_TYPES_H
#define MESSAGE_HANDLING_TYPES_H

enum class MessageHandling : byte {
    MessageHandled,
    MessageContinue,
    MessageHalt
};

#endif