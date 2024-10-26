/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HERMES_MEMORY_H
#define HERMES_MEMORY_H

#include <cstdint>
#include <vector>
#include <string>
#include <hermes_shm/memory/memory.h>
#include "hermes_shm/constants/macros.h"
#include <limits>
#include "hermes_shm/data_structures/containers/chararr.h"
#include "../../constants/macros.h"

namespace hshm::ipc {

enum class MemoryBackendType {
  kPosixShmMmap,
  kCudaShmMmap,
  kCudaMalloc,
  kMallocBackend,
  kArrayBackend,
  kPosixMmap,
};

/** ID for memory backend */
class MemoryBackendId {
 public:
  u32 id_;

  MemoryBackendId() = default;

  MemoryBackendId(u32 id) : id_(id) {}

  MemoryBackendId(const MemoryBackendId &other) : id_(other.id_) {}

  MemoryBackendId(MemoryBackendId &&other) noexcept : id_(other.id_) {}

  MemoryBackendId &operator=(const MemoryBackendId &other) {
    id_ = other.id_;
    return *this;
  }

  MemoryBackendId &operator=(MemoryBackendId &&other) noexcept {
    id_ = other.id_;
    return *this;
  }

  static MemoryBackendId GetRoot() {
    return {0};
  }

  static MemoryBackendId Get(u32 id) {
    return {id + 1};
  }

  bool operator==(const MemoryBackendId &other) const {
    return id_ == other.id_;
  }

  bool operator!=(const MemoryBackendId &other) const {
    return id_ != other.id_;
  }
};
typedef MemoryBackendId memory_backend_id_t;

struct MemoryBackendHeader {
  MemoryBackendType type_;
  MemoryBackendId id_;
  size_t data_size_;
};

#define MEMORY_BACKEND_INITIALIZED 0x1
#define MEMORY_BACKEND_OWNED 0x2

class UrlMemoryBackend {};

class MemoryBackend {
 public:
  MemoryBackendHeader *header_;
  char *data_;
  size_t data_size_;
  bitfield32_t flags_;

 public:
  HSHM_CROSS_FUN
  MemoryBackend() : header_(nullptr), data_(nullptr) {}

  HSHM_CROSS_FUN
  virtual ~MemoryBackend() = default;

  /** Mark data as valid */
  HSHM_CROSS_FUN
  void SetInitialized() {
    flags_.SetBits(MEMORY_BACKEND_INITIALIZED);
  }

  /** Check if data is valid */
  HSHM_CROSS_FUN
  bool IsInitialized() {
    return flags_.Any(MEMORY_BACKEND_INITIALIZED);
  }

  /** Mark data as invalid */
  HSHM_CROSS_FUN
  void UnsetInitialized() {
    flags_.UnsetBits(MEMORY_BACKEND_INITIALIZED);
  }

  /** This is the process which destroys the backend */
  HSHM_CROSS_FUN
  void Own() {
    flags_.SetBits(MEMORY_BACKEND_OWNED);
  }

  /** This is owned */
  HSHM_CROSS_FUN
  bool IsOwned() {
    return flags_.Any(MEMORY_BACKEND_OWNED);
  }

  /** This is not the process which destroys the backend */
  HSHM_CROSS_FUN
  void Disown() {
    flags_.UnsetBits(MEMORY_BACKEND_OWNED);
  }

  /// Each allocator must define its own shm_init.
  // virtual bool shm_init(size_t size, ...) = 0;
  virtual bool shm_deserialize(const hshm::chararr &url) = 0;
  virtual void shm_detach() = 0;
  virtual void shm_destroy() = 0;
};

}  // namespace hshm::ipc

#endif  // HERMES_MEMORY_H
