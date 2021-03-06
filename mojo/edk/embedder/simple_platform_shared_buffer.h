// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_EDK_EMBEDDER_SIMPLE_PLATFORM_SHARED_BUFFER_H_
#define MOJO_EDK_EMBEDDER_SIMPLE_PLATFORM_SHARED_BUFFER_H_

#include <stddef.h>

#include "mojo/edk/embedder/platform_shared_buffer.h"
#include "mojo/edk/util/ref_ptr.h"
#include "mojo/public/cpp/system/macros.h"

namespace mojo {
namespace embedder {

// A simple implementation of |PlatformSharedBuffer|.
class SimplePlatformSharedBuffer final : public PlatformSharedBuffer {
 public:
  // Creates a shared buffer of size |num_bytes| bytes (initially zero-filled).
  // |num_bytes| must be nonzero. Returns null on failure.
  static util::RefPtr<SimplePlatformSharedBuffer> Create(size_t num_bytes);

  static util::RefPtr<SimplePlatformSharedBuffer> CreateFromPlatformHandle(
      size_t num_bytes,
      ScopedPlatformHandle platform_handle);

  // |PlatformSharedBuffer| implementation:
  size_t GetNumBytes() const override;
  std::unique_ptr<PlatformSharedBufferMapping> Map(size_t offset,
                                                   size_t length) override;
  bool IsValidMap(size_t offset, size_t length) override;
  std::unique_ptr<PlatformSharedBufferMapping> MapNoCheck(
      size_t offset,
      size_t length) override;
  ScopedPlatformHandle DuplicatePlatformHandle() override;
  ScopedPlatformHandle PassPlatformHandle() override;

 private:
  explicit SimplePlatformSharedBuffer(size_t num_bytes);
  ~SimplePlatformSharedBuffer() override;

  // This is called by |Create()| before this object is given to anyone.
  bool Init();

  // This is like |Init()|, but for |CreateFromPlatformHandle()|. (Note: It
  // should verify that |platform_handle| is an appropriate handle for the
  // claimed |num_bytes_|.)
  bool InitFromPlatformHandle(ScopedPlatformHandle platform_handle);

  const size_t num_bytes_;

  // This is set in |Init()|/|InitFromPlatformHandle()| and never modified
  // (except by |PassPlatformHandle()|; see the comments above its declaration),
  // hence does not need to be protected by a lock.
  ScopedPlatformHandle handle_;

  MOJO_DISALLOW_COPY_AND_ASSIGN(SimplePlatformSharedBuffer);
};

// An implementation of |PlatformSharedBufferMapping|, produced by
// |SimplePlatformSharedBuffer|.
class SimplePlatformSharedBufferMapping final
    : public PlatformSharedBufferMapping {
 public:
  ~SimplePlatformSharedBufferMapping() override;

  void* GetBase() const override;
  size_t GetLength() const override;

 private:
  friend class SimplePlatformSharedBuffer;

  SimplePlatformSharedBufferMapping(void* base,
                                    size_t length,
                                    void* real_base,
                                    size_t real_length)
      : base_(base),
        length_(length),
        real_base_(real_base),
        real_length_(real_length) {}
  void Unmap();

  void* const base_;
  const size_t length_;

  void* const real_base_;
  const size_t real_length_;

  MOJO_DISALLOW_COPY_AND_ASSIGN(SimplePlatformSharedBufferMapping);
};

}  // namespace embedder
}  // namespace mojo

#endif  // MOJO_EDK_EMBEDDER_SIMPLE_PLATFORM_SHARED_BUFFER_H_
