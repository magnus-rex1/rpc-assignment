#include "socket.h"
#include <sstream>

RPCMessage::RPCMessage(MessageType t)
    : type { t }
    , requestId { 0 }
    , procedureId { 0 }
    , arg1 { 0 }
    , arg2 { 0 }
{
}

RPCMessage::RPCMessage(MessageType t, int p, int arg1, int arg2)
    : type { t }
    , requestId { 0 }
    , procedureId { static_cast<unsigned int>(p) } // I don't like this
    , arg1 { arg1 }
    , arg2 { arg2 }
{
}

void RPCMessage::marshall(Message** message)
{
    std::stringstream ss;
    ss << type << " "
       << requestId << " "
       << procedureId << " "
       << arg1 << " "
       << arg2;

    Message mess((unsigned char*)ss.str().c_str(), ss.str().length());
    **message = mess;
}

void RPCMessage::unmarshall(Message* message)
{
    if (!message) {
        std::cerr << "Error: Null message pointer\n";
        return;
    }

    std::stringstream ss;
    ss << *message;
    unsigned int l_type;
    ss >> l_type >> requestId >> procedureId >> arg1 >> arg2;

    type = MessageType(l_type);
}

Status RPCMessage::op(int left, int right, int* result)
{
    Status status = Status::Bad;

    if (procedureId) {
        switch (procedureId) {
        case 1:
            *result = left + right;
            status = Status::Ok;
            break;
        case 2:
            *result = left - right;
            status = Status::Ok;
            break;
        case 3:
            *result = left * right;
            status = Status::Ok;
            break;
        case 4:
            if (right) { // right > 0
                *result = left / right;
                status = Status::Ok;
            } else {
                std::cerr << "Division by zero error\n";
                status = Status::Bad;
            }
            break;
        default:
            std::cerr << "Unknown procedure\n";
            status = Status::Ok;
            break;
        }
    }

    return status;
}
