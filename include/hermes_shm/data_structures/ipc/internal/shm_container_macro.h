#ifndef HERMES_DATA_STRUCTURES_INTERNAL_SHM_CONTAINER_MACRO_H_
#define HERMES_DATA_STRUCTURES_INTERNAL_SHM_CONTAINER_MACRO_H_
#include "hermes_shm/constants/macros.h"
#define HIPC_CONTAINER_TEMPLATE(CLASS_NAME,TYPED_CLASS)\
public:\
/**====================================\
 * Variables & Types\
 * ===================================*/\
hipc::allocator_id_t alloc_id_;\
\
/**====================================\
 * Constructors\
 * ===================================*/\
\
/** Default constructor. Deleted. */\
TYPE_UNWRAP(CLASS_NAME)() = delete;\
\
/** Move constructor. Deleted. */\
TYPE_UNWRAP(CLASS_NAME)(TYPE_UNWRAP(CLASS_NAME) &&other) = delete;\
\
/** Copy constructor. Deleted. */\
TYPE_UNWRAP(CLASS_NAME)(const TYPE_UNWRAP(CLASS_NAME) &other) = delete;\
\
/** Initialize container */\
HSHM_CROSS_FUN void init_shm_container(hipc::Allocator *alloc) {\
  alloc_id_ = alloc->GetId();\
}\
\
/**====================================\
 * Destructor\
 * ===================================*/\
\
/** Destructor. */\
HSHM_INLINE_CROSS_FUN ~TYPE_UNWRAP(CLASS_NAME)() = default;\
\
/** Destruction operation */\
HSHM_INLINE_CROSS_FUN void shm_destroy() {\
  if (IsNull()) { return; }\
  shm_destroy_main();\
  SetNull();\
}\
\
/**====================================\
 * Header Operations\
 * ===================================*/\
\
/** Get a typed pointer to the object */\
template<typename POINTER_T>\
HSHM_INLINE_CROSS_FUN POINTER_T GetShmPointer() const {\
  return GetAllocator()->template Convert<TYPE_UNWRAP(TYPED_CLASS), POINTER_T>(this);\
}\
\
/**====================================\
 * Query Operations\
 * ===================================*/\
\
/** Get the allocator for this container */\
HSHM_INLINE_CROSS_FUN hipc::Allocator* GetAllocator() const {\
  return HERMES_MEMORY_MANAGER->GetAllocator(alloc_id_);\
}\
\
/** Get the shared-memory allocator id */\
HSHM_INLINE_CROSS_FUN hipc::allocator_id_t& GetAllocatorId() const {\
  return GetAllocator()->GetId();\
}\

#endif  // HERMES_DATA_STRUCTURES_INTERNAL_SHM_CONTAINER_MACRO_H_
