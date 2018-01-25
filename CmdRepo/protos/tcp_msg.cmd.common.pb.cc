// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tcp_msg.cmd.common.proto

#include "tcp_msg.cmd.common.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace tcp_msg {
class CommonHeaderDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<CommonHeader>
      _instance;
} _CommonHeader_default_instance_;
}  // namespace tcp_msg
namespace protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto {
void InitDefaultsCommonHeaderImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::tcp_msg::_CommonHeader_default_instance_;
    new (ptr) ::tcp_msg::CommonHeader();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::tcp_msg::CommonHeader::InitAsDefaultInstance();
}

void InitDefaultsCommonHeader() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsCommonHeaderImpl);
}

::google::protobuf::Metadata file_level_metadata[1];
const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[1];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcp_msg::CommonHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::tcp_msg::CommonHeader, type_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::tcp_msg::CommonHeader)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::tcp_msg::_CommonHeader_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "tcp_msg.cmd.common.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\030tcp_msg.cmd.common.proto\022\007tcp_msg\".\n\014C"
      "ommonHeader\022\036\n\004type\030\001 \001(\0162\020.tcp_msg.CmdT"
      "ype*.\n\007CmdType\022\020\n\014START_UPDATE\020\000\022\021\n\rFINI"
      "SH_UPDATE\020\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 139);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "tcp_msg.cmd.common.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto
namespace tcp_msg {
const ::google::protobuf::EnumDescriptor* CmdType_descriptor() {
  protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::file_level_enum_descriptors[0];
}
bool CmdType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}


// ===================================================================

void CommonHeader::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int CommonHeader::kTypeFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

CommonHeader::CommonHeader()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::InitDefaultsCommonHeader();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:tcp_msg.CommonHeader)
}
CommonHeader::CommonHeader(const CommonHeader& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  type_ = from.type_;
  // @@protoc_insertion_point(copy_constructor:tcp_msg.CommonHeader)
}

void CommonHeader::SharedCtor() {
  type_ = 0;
  _cached_size_ = 0;
}

CommonHeader::~CommonHeader() {
  // @@protoc_insertion_point(destructor:tcp_msg.CommonHeader)
  SharedDtor();
}

void CommonHeader::SharedDtor() {
}

void CommonHeader::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CommonHeader::descriptor() {
  ::protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const CommonHeader& CommonHeader::default_instance() {
  ::protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::InitDefaultsCommonHeader();
  return *internal_default_instance();
}

CommonHeader* CommonHeader::New(::google::protobuf::Arena* arena) const {
  CommonHeader* n = new CommonHeader;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void CommonHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:tcp_msg.CommonHeader)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  type_ = 0;
  _internal_metadata_.Clear();
}

bool CommonHeader::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:tcp_msg.CommonHeader)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .tcp_msg.CmdType type = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          set_type(static_cast< ::tcp_msg::CmdType >(value));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:tcp_msg.CommonHeader)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:tcp_msg.CommonHeader)
  return false;
#undef DO_
}

void CommonHeader::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:tcp_msg.CommonHeader)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .tcp_msg.CmdType type = 1;
  if (this->type() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:tcp_msg.CommonHeader)
}

::google::protobuf::uint8* CommonHeader::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:tcp_msg.CommonHeader)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .tcp_msg.CmdType type = 1;
  if (this->type() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:tcp_msg.CommonHeader)
  return target;
}

size_t CommonHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:tcp_msg.CommonHeader)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // .tcp_msg.CmdType type = 1;
  if (this->type() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void CommonHeader::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:tcp_msg.CommonHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const CommonHeader* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const CommonHeader>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:tcp_msg.CommonHeader)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:tcp_msg.CommonHeader)
    MergeFrom(*source);
  }
}

void CommonHeader::MergeFrom(const CommonHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:tcp_msg.CommonHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.type() != 0) {
    set_type(from.type());
  }
}

void CommonHeader::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:tcp_msg.CommonHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CommonHeader::CopyFrom(const CommonHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:tcp_msg.CommonHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CommonHeader::IsInitialized() const {
  return true;
}

void CommonHeader::Swap(CommonHeader* other) {
  if (other == this) return;
  InternalSwap(other);
}
void CommonHeader::InternalSwap(CommonHeader* other) {
  using std::swap;
  swap(type_, other->type_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata CommonHeader::GetMetadata() const {
  protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_tcp_5fmsg_2ecmd_2ecommon_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace tcp_msg

// @@protoc_insertion_point(global_scope)