public:
/**====================================
 * Variables & Types
 * ===================================*/
hipc::AllocatorId alloc_id_;

/**====================================
 * Constructors
 * ===================================*/

/** Initialize container */
HSHM_CROSS_FUN void init_shm_container(hipc::Allocator *alloc) {
  alloc_id_ = alloc->GetId();
}

/**====================================
 * Destructor
 * ===================================*/

/** Destructor. */
HSHM_INLINE_CROSS_FUN ~CLASS_NAME() = default;

/** Destruction operation */
HSHM_INLINE_CROSS_FUN void shm_destroy() {
  if (IsNull()) { return; }
  shm_destroy_main();
  SetNull();
}

/**====================================
 * Header Operations
 * ===================================*/

/** Get a typed pointer to the object */
template<typename POINTER_T>
HSHM_INLINE_CROSS_FUN POINTER_T GetShmPointer() const {
  return GetAllocator()->template Convert<TYPED_CLASS, POINTER_T>(this);
}

/**====================================
 * Query Operations
 * ===================================*/

/** Get the allocator for this container */
HSHM_INLINE_CROSS_FUN hipc::Allocator* GetAllocator() const {
  return HERMES_MEMORY_MANAGER->GetAllocator(alloc_id_);
}

/** Get the shared-memory allocator id */
HSHM_INLINE_CROSS_FUN hipc::AllocatorId& GetAllocatorId() const {
  return GetAllocator()->GetId();
}