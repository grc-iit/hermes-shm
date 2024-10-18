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

#ifndef HERMES_SHM_INCLUDE_HERMES_SHM_DATA_STRUCTURES_IPC_TICKET_QUEUE_H_
#define HERMES_SHM_INCLUDE_HERMES_SHM_DATA_STRUCTURES_IPC_TICKET_QUEUE_H_

#include "hermes_shm/data_structures/ipc/internal/shm_internal.h"
#include "hermes_shm/thread/lock.h"
#include "spsc_queue.h"
#include "hermes_shm/types/qtok.h"

namespace hshm::ipc {

/** Forward declaration of ticket_queue */
template<typename T>
class ticket_queue;

/**
 * MACROS used to simplify the ticket_queue namespace
 * Used as inputs to the HIPC_CONTAINER_TEMPLATE
 * */
#define CLASS_NAME ticket_queue
#define TYPED_CLASS ticket_queue<T>

/**
 * A MPMC queue for allocating tickets. Handles concurrency
 * without blocking.
 * */
template<typename T>
class ticket_queue : public ShmContainer {
 public:
  HIPC_CONTAINER_TEMPLATE((CLASS_NAME), (TYPED_CLASS))
  ShmArchive<spsc_queue<T>> queue_;
  hshm::Mutex lock_;

 public:
  /**====================================
   * Default Constructor
   * ===================================*/

  /** SHM constructor. Default. */
  HSHM_CROSS_FUN
  explicit ticket_queue(Allocator *alloc,
                        size_t depth = 1024) {
    init_shm_container(alloc);
    HSHM_MAKE_AR(queue_, GetAllocator(), depth);
    lock_.Init();
    SetNull();
  }

  /**====================================
   * Copy Constructors
   * ===================================*/

  /** SHM copy constructor */
  HSHM_CROSS_FUN
  explicit ticket_queue(Allocator *alloc,
                        const ticket_queue &other) {
    init_shm_container(alloc);
    SetNull();
    shm_strong_copy_construct_and_op(other);
  }

  /** SHM copy assignment operator */
  HSHM_CROSS_FUN
  ticket_queue& operator=(const ticket_queue &other) {
    if (this != &other) {
      shm_destroy();
      shm_strong_copy_construct_and_op(other);
    }
    return *this;
  }

  /** SHM copy constructor + operator main */
  HSHM_CROSS_FUN
  void shm_strong_copy_construct_and_op(const ticket_queue &other) {
    (*queue_) = (*other.queue_);
  }

  /**====================================
   * Move Constructors
   * ===================================*/

  /** SHM move constructor. */
  HSHM_CROSS_FUN
  ticket_queue(Allocator *alloc,
               ticket_queue &&other) noexcept {
    init_shm_container(alloc);
    if (GetAllocator() == other.GetAllocator()) {
      (*queue_) = std::move(*other.queue_);
      other.SetNull();
    } else {
      shm_strong_copy_construct_and_op(other);
      other.shm_destroy();
    }
  }

  /** SHM move assignment operator. */
  HSHM_CROSS_FUN
  ticket_queue& operator=(ticket_queue &&other) noexcept {
    if (this != &other) {
      shm_destroy();
      if (GetAllocator() == other.GetAllocator()) {
        (*queue_) = std::move(*other.queue_);
        other.SetNull();
      } else {
        shm_strong_copy_construct_and_op(other);
        other.shm_destroy();
      }
    }
    return *this;
  }

  /**====================================
   * Destructor
   * ===================================*/

  /** SHM destructor. */
  HSHM_CROSS_FUN
  void shm_destroy_main() {
    (*queue_).shm_destroy();
  }

  /** Check if the list is empty */
  HSHM_CROSS_FUN
  bool IsNull() const {
    return (*queue_).IsNull();
  }

  /** Sets this list as empty */
  HSHM_CROSS_FUN
  void SetNull() {}

  /**====================================
   * ticket Queue Methods
   * ===================================*/

  /** Construct an element at \a pos position in the queue */
  template<typename ...Args>
  HSHM_INLINE_CROSS_FUN qtok_t emplace(T &tkt) {
    lock_.Lock(0);
    auto qtok = queue_->emplace(tkt);
    lock_.Unlock();
    return qtok;
  }

 public:
  /** Pop an element from the queue */
  HSHM_INLINE_CROSS_FUN qtok_t pop(T &tkt) {
    lock_.Lock(0);
    auto qtok = queue_->pop(tkt);
    lock_.Unlock();
    return qtok;
  }
};

}  // namespace hshm::ipc

#undef TYPED_CLASS
#undef CLASS_NAME

#endif  // HERMES_SHM_INCLUDE_HERMES_SHM_DATA_STRUCTURES_IPC_TICKET_QUEUE_H_
