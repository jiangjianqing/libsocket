#ifndef PROTOBUFUTILS_H
#define PROTOBUFUTILS_H

#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

using namespace google::protobuf;
using namespace std;

class ProtobufUtils
{
public:
    ProtobufUtils() = delete;

    static Message* createMessage(const string& typeName);
};

#endif // PROTOBUFUTILS_H
