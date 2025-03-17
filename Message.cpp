#include "socket.h"

/*
 * Message implementation
 */

Message::Message(unsigned char* message, unsigned int len)
{
    length = len;
    data = new unsigned char[len + 1];
    strcpy((char*)data, (char*)message);
}

Message::Message(unsigned int len)
{
    data = new unsigned char[len + 1];
    length = len;
}

// Deep copy semantics
//
// Deep copy constructor
Message::Message(const Message& other)
    : data { nullptr }
{
    if (other.data) {
        data = new unsigned char[strlen((const char*)other.data) + 1];
        strcpy((char*)data, (const char*)other.data);
    }
}

// Deep copy assignment
Message& Message::operator=(const Message& other)
{
    if (this != &other) {
        delete[] data; // free old memory
        data = nullptr;
        if (other.data) {
            data = new unsigned char[strlen((char*)other.data) + 1];
            strcpy((char*)data, (char*)other.data);
        }
    }
    return *this;
}

unsigned char* Message::GetMessage() const
{
    return data;
}

unsigned int Message::GetLength() const
{
    return length;
}
