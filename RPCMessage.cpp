#include "socket.h"
#include <climits>
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
    , procedureId { static_cast<unsigned int>(p) } // Uh oh!
    , arg1 { arg1 }
    , arg2 { arg2 }
{
}

void RPCMessage::marshall(Message** message)
{
    std::stringstream ss;

    ss << requestId << "|"
       << procedureId << "|"
       << arg1 << "|"
       << arg2;

    Message mess((unsigned char*)ss.str().c_str(), ss.str().length());
    **message = mess; // deep copy
}

void RPCMessage::unmarshall(Message* message)
{
    unsigned int l_requestId, l_procedureId;
    int l_arg1, l_arg2;
    l_requestId = l_procedureId = l_arg1 = l_arg2 = 0;
    std::stringstream ss;

    if (!message) {
        std::cerr << "Error: Empty message\n";
        return;
    }

    // Pass message into string stream to break it into individual numbers
    ss << *message;

    // extract those numbers
    std::string r, p, a1, a2;
    std::getline(ss, r, '|');
    std::getline(ss, p, '|');
    std::getline(ss, a1, '|');
    std::getline(ss, a2, '|');

    std::stringstream(r) >> l_requestId;
    std::stringstream(p) >> l_procedureId;
    std::stringstream(a1) >> l_arg1;
    std::stringstream(a2) >> l_arg2;

    requestId = l_requestId;
    procedureId = l_procedureId;
    arg1 = l_arg1;
    arg2 = l_arg2;
}

Status RPCMessage::eval()
{
    Status status = Status::Bad;

    if (procedureId) {
        switch (procedureId) {
        case 1:
            status = add(arg1, arg2, &arg1);
            break;
        case 2:
            status = sub(arg1, arg2, &arg1);
            break;
        case 3:
            status = mul(arg1, arg2, &arg1);
            break;
        case 4:
            status = div(arg1, arg2, &arg1);
            break;
        default:
            std::cerr << "Unknown procedure\n";
            status = Status::Bad;
            break;
        }
    }

    return status;
}

Status RPCMessage::add(int left, int right, int* res)
{
    *res = left + right;
    return Status::Ok;
}

Status RPCMessage::sub(int left, int right, int* res)
{
    *res = left - right;
    return Status::Ok;
}

Status RPCMessage::mul(int left, int right, int* res)
{
    *res = left * right;
    return Status::Ok;
}

Status RPCMessage::div(int left, int right, int* res)
{
    Status status;
    if (right) { // right > 0
        *res = left / right;
        status = Status::Ok;
    } else {
        std::cerr << "Division by zero error\n";
        status = Status::Bad;
    }
    return status;
}

bool RPCMessage::isInvalid()
{
    return (requestId == 0)
        && (procedureId == 0)
        && (arg1 == 0)
        && (arg2 == 0);
}

int RPCMessage::getResult()
{
    return arg1;
}
