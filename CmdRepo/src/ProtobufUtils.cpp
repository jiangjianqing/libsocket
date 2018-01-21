#include "ProtobufUtils.h"


Message* ProtobufUtils::createMessage(const string& typeName)
{
  Message* message = nullptr;
  const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
  if (descriptor)
  {
    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype)
    {
      message = prototype->New();
    }
  }
  return message;
}
