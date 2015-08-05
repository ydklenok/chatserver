// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: C2SChat.proto

#ifndef PROTOBUF_C2SChat_2eproto__INCLUDED
#define PROTOBUF_C2SChat_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_C2SChat_2eproto();
void protobuf_AssignDesc_C2SChat_2eproto();
void protobuf_ShutdownFile_C2SChat_2eproto();

class C2SChat;

// ===================================================================

class C2SChat : public ::google::protobuf::Message {
 public:
  C2SChat();
  virtual ~C2SChat();

  C2SChat(const C2SChat& from);

  inline C2SChat& operator=(const C2SChat& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const C2SChat& default_instance();

  void Swap(C2SChat* other);

  // implements Message ----------------------------------------------

  C2SChat* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const C2SChat& from);
  void MergeFrom(const C2SChat& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string uid = 1;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 1;
  inline const ::std::string& uid() const;
  inline void set_uid(const ::std::string& value);
  inline void set_uid(const char* value);
  inline void set_uid(const char* value, size_t size);
  inline ::std::string* mutable_uid();
  inline ::std::string* release_uid();
  inline void set_allocated_uid(::std::string* uid);

  // required string ctx = 2;
  inline bool has_ctx() const;
  inline void clear_ctx();
  static const int kCtxFieldNumber = 2;
  inline const ::std::string& ctx() const;
  inline void set_ctx(const ::std::string& value);
  inline void set_ctx(const char* value);
  inline void set_ctx(const char* value, size_t size);
  inline ::std::string* mutable_ctx();
  inline ::std::string* release_ctx();
  inline void set_allocated_ctx(::std::string* ctx);

  // @@protoc_insertion_point(class_scope:C2SChat)
 private:
  inline void set_has_uid();
  inline void clear_has_uid();
  inline void set_has_ctx();
  inline void clear_has_ctx();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* uid_;
  ::std::string* ctx_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_C2SChat_2eproto();
  friend void protobuf_AssignDesc_C2SChat_2eproto();
  friend void protobuf_ShutdownFile_C2SChat_2eproto();

  void InitAsDefaultInstance();
  static C2SChat* default_instance_;
};
// ===================================================================


// ===================================================================

// C2SChat

// required string uid = 1;
inline bool C2SChat::has_uid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void C2SChat::set_has_uid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void C2SChat::clear_has_uid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void C2SChat::clear_uid() {
  if (uid_ != &::google::protobuf::internal::kEmptyString) {
    uid_->clear();
  }
  clear_has_uid();
}
inline const ::std::string& C2SChat::uid() const {
  return *uid_;
}
inline void C2SChat::set_uid(const ::std::string& value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void C2SChat::set_uid(const char* value) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(value);
}
inline void C2SChat::set_uid(const char* value, size_t size) {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  uid_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* C2SChat::mutable_uid() {
  set_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    uid_ = new ::std::string;
  }
  return uid_;
}
inline ::std::string* C2SChat::release_uid() {
  clear_has_uid();
  if (uid_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = uid_;
    uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void C2SChat::set_allocated_uid(::std::string* uid) {
  if (uid_ != &::google::protobuf::internal::kEmptyString) {
    delete uid_;
  }
  if (uid) {
    set_has_uid();
    uid_ = uid;
  } else {
    clear_has_uid();
    uid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string ctx = 2;
inline bool C2SChat::has_ctx() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void C2SChat::set_has_ctx() {
  _has_bits_[0] |= 0x00000002u;
}
inline void C2SChat::clear_has_ctx() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void C2SChat::clear_ctx() {
  if (ctx_ != &::google::protobuf::internal::kEmptyString) {
    ctx_->clear();
  }
  clear_has_ctx();
}
inline const ::std::string& C2SChat::ctx() const {
  return *ctx_;
}
inline void C2SChat::set_ctx(const ::std::string& value) {
  set_has_ctx();
  if (ctx_ == &::google::protobuf::internal::kEmptyString) {
    ctx_ = new ::std::string;
  }
  ctx_->assign(value);
}
inline void C2SChat::set_ctx(const char* value) {
  set_has_ctx();
  if (ctx_ == &::google::protobuf::internal::kEmptyString) {
    ctx_ = new ::std::string;
  }
  ctx_->assign(value);
}
inline void C2SChat::set_ctx(const char* value, size_t size) {
  set_has_ctx();
  if (ctx_ == &::google::protobuf::internal::kEmptyString) {
    ctx_ = new ::std::string;
  }
  ctx_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* C2SChat::mutable_ctx() {
  set_has_ctx();
  if (ctx_ == &::google::protobuf::internal::kEmptyString) {
    ctx_ = new ::std::string;
  }
  return ctx_;
}
inline ::std::string* C2SChat::release_ctx() {
  clear_has_ctx();
  if (ctx_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = ctx_;
    ctx_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void C2SChat::set_allocated_ctx(::std::string* ctx) {
  if (ctx_ != &::google::protobuf::internal::kEmptyString) {
    delete ctx_;
  }
  if (ctx) {
    set_has_ctx();
    ctx_ = ctx;
  } else {
    clear_has_ctx();
    ctx_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_C2SChat_2eproto__INCLUDED