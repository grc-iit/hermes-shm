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

#ifndef HERMES_INCLUDE_MEMORY_BACKEND_POSIX_SHM_MMAP_H
#define HERMES_INCLUDE_MEMORY_BACKEND_POSIX_SHM_MMAP_H

#include "memory_backend.h"
#include "hermes_shm/util/logging.h"
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <hermes_shm/util/errors.h>
#include <hermes_shm/constants/macros.h>
#include <hermes_shm/introspect/system_info.h>

namespace hshm::ipc {

class PosixShmMmap : public MemoryBackend, public UrlMemoryBackend {
 protected:
  int fd_;
  hshm::chararr url_;

 public:
  /** Constructor */
  HSHM_CROSS_FUN
  PosixShmMmap() : fd_(-1) {}

  /** Destructor */
  ~PosixShmMmap() override {
    if (IsOwned()) {
      _Destroy();
    } else {
      _Detach();
    }
  }

  /** Initialize backend */
  bool shm_init(const MemoryBackendId &backend_id,
                size_t size, const hshm::chararr &url) {
    SetInitialized();
    Own();
    shm_unlink(url.c_str());
    fd_ = shm_open(url.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd_ < 0) {
      HILOG(kError, "shm_open failed: {}", strerror(errno));
      return false;
    }
    _Reserve(size + HERMES_SYSTEM_INFO->page_size_);
    url_ = url;
    header_ = (MemoryBackendHeader*)_Map(HERMES_SYSTEM_INFO->page_size_, 0);
    header_->type_ = MemoryBackendType::kPosixShmMmap;
    header_->id_ = backend_id;
    header_->data_size_ = size;
    data_size_ = size;
    data_ = _Map(size, HERMES_SYSTEM_INFO->page_size_);
    return true;
  }

  /** Deserialize the backend */
  bool shm_deserialize(const hshm::chararr &url) override {
    SetInitialized();
    Disown();
    fd_ = shm_open(url.c_str(), O_RDWR, 0666);
    if (fd_ < 0) {
      HILOG(kError, "shm_open failed: {}", strerror(errno));
      return false;
    }
    header_ = (MemoryBackendHeader*)_Map(HERMES_SYSTEM_INFO->page_size_, 0);
    data_size_ = header_->data_size_;
    data_ = _Map(data_size_, HERMES_SYSTEM_INFO->page_size_);
    return true;
  }

  /** Detach the mapped memory */
  void shm_detach() override {
    _Detach();
  }

  /** Destroy the mapped memory */
  void shm_destroy() override {
    _Destroy();
  }

 protected:
  /** Reserve shared memory */
  void _Reserve(size_t size) {
    int ret = ftruncate64(fd_, static_cast<off64_t>(size));
    if (ret < 0) {
      HERMES_THROW_ERROR(SHMEM_RESERVE_FAILED);
    }
  }

  /** Map shared memory */
  virtual char* _Map(size_t size, off64_t off) {
    return _ShmMap(size, off);
  };

  /** Map shared memory */
  char* _ShmMap(size_t size, off64_t off) {
    char *ptr = reinterpret_cast<char*>(
        mmap64(nullptr, size, PROT_READ | PROT_WRITE,
               MAP_SHARED, fd_, off));
    if (ptr == MAP_FAILED) {
      HERMES_THROW_ERROR(SHMEM_CREATE_FAILED);
    }
    return ptr;
  }

  /** Unmap shared memory (virtual) */
  virtual void _Detach() {
    _ShmDetach();
  }

  /** Unmap shared memory */
  void _ShmDetach() {
    if (!IsInitialized()) { return; }
    munmap(data_, data_size_);
    munmap(header_, HERMES_SYSTEM_INFO->page_size_);
    close(fd_);
    UnsetInitialized();
  }

  /** Destroy shared memory */
  void _Destroy() {
    if (!IsInitialized()) { return; }
    _Detach();
    shm_unlink(url_.c_str());
    UnsetInitialized();
  }
};

}  // namespace hshm::ipc

#endif  // HERMES_INCLUDE_MEMORY_BACKEND_POSIX_SHM_MMAP_H
