/* the first include is just for the printf function */
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef char ALIGN[5];

/* the header for each memory block, this will be used in the free() function*/
union header {
  struct {
    size_t size;
    unsigned is_free;
    union header *next;
  } s;
  /* force the header to be aligned to 8-byte boundaries */
  ALIGN stub;
};

/* the following variables are used to keep track of the memory blocks */
union header *head = NULL,  *tail = NULL;
/* to prevent two threads from allocating memory at the same time */
pthread_mutex_t global_malloc_lock;

/* function to allocate memory */
union header *get_free_block(size_t size) {
  union header *curr = head;
  while(curr) {
    /* if the block is free and large enough, return it */
    if (curr->s.is_free && curr->s.size >= size) {
      return curr;
    }
    curr = curr->s.next;
  }
  return NULL;
}

void *malloc(size_t size){
  size_t total_size;
  void *block;
  union header *header;

  if (!size)
    return NULL;
  
  pthread_mutex_lock(&global_malloc_lock);
  header = get_free_block(size);

  /* this means we found a free block to accomodate the requested memory ammount */
  if (header) {
    header->s.is_free = 0;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header + 1);
  }

  /* we need to get memory to fit the requested block and header from the OS */
  total_size = sizeof(union header) + size;
  block = sbrk(total_size);
  if(block == (void*) - 1){
    pthread_mutex_unlock(&global_malloc_lock);
    return NULL;
  }

  header = block;
  header->s.size = size;
  header->s.is_free = 0;
  header->s.next = NULL;
  

  if (!head)
    head = header;

  if (tail)
    tail->s.next = header;

  tail = header;
  pthread_mutex_unlock(&global_malloc_lock);
  return (void*)(header + 1);
}

void free(void *block){
  union header *header, *tmp;
  void *programbreak;
  if (!block)
    return;
  pthread_mutex_lock(&global_malloc_lock);
  header = (union header*)block - 1;
  programbreak = sbrk(0);
  if ((char*)block + header->s.size == programbreak) {
    if (head == tail) {
      head = tail = NULL;
    } else {
      tmp = head;
      while (tmp) {
        if (tmp->s.next == tail) {
          tmp->s.next = NULL;
          tail = tmp;
        }
        tmp = tmp->s.next;
      }
    }
    sbrk(0 - sizeof(union header) - header->s.size);
    pthread_mutex_unlock(&global_malloc_lock);
    return;
  }
  header->s.is_free = 1;
  pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize){
  size_t size;
  void *block;
  if (!num || !nsize)
    return NULL;
  size = num * nsize;
  /* check for overflow */
  if (nsize != size / num)
    return NULL;
  block = malloc(size);
  if (!block)
    return NULL;
  memset(block, 0, size);
  return block;
}

void print_mem_list(){
  union header *curr = head;
  printf("head = %p, tail = %p \n", (void*)head, (void*)tail);
  while(curr){
    printf("addr = %p, size = %zu, is_free=%u, next=%p\n",
           (void*)curr, curr->s.size, curr->s.is_free, (void*)curr->s.next);
    curr = curr->s.next;
  }
}
