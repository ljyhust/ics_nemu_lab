#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  return NULL;
}

static inline void* pg_alloc(int n) {
  return NULL;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  void* end = (void *)ROUNDUP(heap.end, PGSIZE);
  if ((void *)brk > end) {
    return -1;
  }
  pf = (void *)brk;
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}

void* mm_sbrk(uintptr_t increment) {
  if (!increment) {
    return pf;
  }
  void* pre = pf;
  if (mm_brk(((uintptr_t) pf) + increment)) {
    return (void *)-1;
  }
  return pre;
}