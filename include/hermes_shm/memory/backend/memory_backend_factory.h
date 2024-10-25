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


#ifndef HERMES_MEMORY_BACKEND_MEMORY_BACKEND_FACTORY_H_
#define HERMES_MEMORY_BACKEND_MEMORY_BACKEND_FACTORY_H_

#include "memory_backend.h"
#include "posix_mmap.h"
#include "posix_shm_mmap.h"
#include "null_backend.h"
#include "array_backend.h"
#include "hermes_shm/memory/memory_manager_.h"
#ifdef HERMES_ENABLE_CUDA
#include "cuda_shm_mmap.h"
#include "cuda_malloc.h"
#endif

namespace hshm::ipc {

class MemoryBackendFactory {
 public:
  /** Initialize a new backend */
  template<typename BackendT, typename ...Args>
  static MemoryBackend* shm_init(
    size_t size, const hshm::chararr &url, Args ...args) {
    // PosixShmMmap
    if constexpr(std::is_same_v<PosixShmMmap, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<PosixShmMmap>();
      if (!backend->shm_init(size, url, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }
#ifdef HERMES_ENABLE_CUDA
    // CudaShmMmap
    if constexpr(std::is_same_v<CudaShmMmap, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<CudaShmMmap>();
      if (!backend->shm_init(size, url, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }
    // CudaMalloc
    if constexpr(std::is_same_v<CudaMalloc, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<CudaMalloc>();
      if (!backend->shm_init(size, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }
#endif
    // PosixMmap
    if constexpr(std::is_same_v<PosixMmap, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<PosixMmap>();
      if (!backend->shm_init(size, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }
    // NullBackend
    if constexpr(std::is_same_v<NullBackend, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<NullBackend>();
      if (!backend->shm_init(size, url, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }
    // ArrayBackend
    if constexpr(std::is_same_v<ArrayBackend, BackendT>) {
      auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<ArrayBackend>();
      if (!backend->shm_init(size, std::forward<Args>(args)...)) {
        HERMES_THROW_ERROR(MEMORY_BACKEND_CREATE_FAILED);
      }
      return backend;
    }

    HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
  }

  /** Deserialize an existing backend */
  static MemoryBackend* shm_deserialize(
    MemoryBackendType type, const hshm::chararr &url) {
    switch (type) {
      // PosixShmMmap
      case MemoryBackendType::kPosixShmMmap: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<PosixShmMmap>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }

#ifdef HERMES_ENABLE_CUDA
      // CudaShmMmap
      case MemoryBackendType::kCudaShmMmap: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<CudaShmMmap>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }

      // CudaMalloc
      case MemoryBackendType::kCudaMalloc: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<CudaMalloc>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }
#endif

      // PosixMmap
      case MemoryBackendType::kPosixMmap: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<PosixMmap>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }

      // NullBackend
      case MemoryBackendType::kNullBackend: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<NullBackend>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }

      // ArrayBackend
      case MemoryBackendType::kArrayBackend: {
        auto backend = HERMES_MEMORY_MANAGER->GetDefaultAllocator()->NewObj<ArrayBackend>();
        if (!backend->shm_deserialize(url)) {
          HERMES_THROW_ERROR(MEMORY_BACKEND_NOT_FOUND);
        }
        return backend;
      }

      // Default
      default: return nullptr;
    }
  }
};

}  // namespace hshm::ipc

#endif  // HERMES_MEMORY_BACKEND_MEMORY_BACKEND_FACTORY_H_
