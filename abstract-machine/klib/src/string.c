#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i = 0;
  while(1) {
    if (s[i] == '\0') {
      return i;
    }
    i++;
  }
  return 0;
}

char *strcpy(char* dst,const char* src) {
  if (NULL == dst || NULL == src) {
    return NULL;
  }
  char* dest_copy = dst;
  while((*dst++ = *src++) != '\0');
  return dest_copy;
}

char* strncpy(char* dst, const char* src, size_t n) {
  if (NULL == dst || NULL == src) {
    return NULL;
  }
  char* dest_cpy = dst;
  while(n > 0 && *src != '\0') {
    *dst++ = *src++;
  }
  *dst = '\0';
  return dest_cpy;
}

char* strcat(char* dst, const char* src) {
  char *tmp = dst;

  while(*dst) {
    dst++;
  }
  while ((*dst++ = *src++) != '\0');
  return tmp;
}

int strcmp(const char* s1, const char* s2) {
  const char *p1 = s1;
  const char *p2 = s2;
  while(*p1 && *p2) {
    if (*p1 < *p2) {
      return -1;
    }
    if (*p1 > *p2) {
      return 1;
    }
    p1++;
    p2++;
  }
  if (*p1 == *p2) {
    return 0;
  }
  if (*p1) {
    return 1;
  }
  if (*p2) {
    return -1;
  }
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  if (!n)
    return(0);

   while (--n && *s1 && *s1 == *s2){
    s1++;
    s2++;
  }
   return(*s1 - *s2);
}

void* memset(void* v,int c,size_t n) {
  void* ret = v;
  while(n--) {
    *(char*)v = (char)c;
    v = (char*)v + 1;
  }
  return ret;
}

void* memmove(void* dst,const void* src,size_t n) {
  void * ret = dst;
  if(dst <= src || (char *)dst >= ((char *)src + n)){
      while (n--){
        *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
   }
   else{
      dst = (char *)dst + n - 1;
      src = (char *)src + n - 1;
      while (n--){
        *(char *)dst = *(char *)src;
        dst = (char *)dst - 1;
        src = (char *)src - 1;
      }
   }
   return(ret);
}

void* memcpy(void* dst, const void* src, size_t n) {
  if(NULL == dst || NULL == src){
		return NULL;
	}
	
	void *ret = dst;
	
	if(dst <= src || (char *)dst >= (char *)src + n){
		//没有内存重叠，从低地址开始复制
		while(n--){
			*(char *)dst = *(char *)src;
			dst = (char *)dst + 1;
			src = (char *)src + 1;
		}
	}else{
		//有内存重叠，从高地址开始复制
		src = (char *)src + n - 1;
		dst = (char *)dst + n - 1;
		while(n--){
			*(char *)dst = *(char *)src;
			dst = (char *)dst - 1;
			src = (char *)src - 1;
		}
	}
	return ret;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  if (!n)
    return(0);
  while ( --n && *(char *)s1 == *(char *)s2){
    s1 = (char *)s1 + 1;
    s2 = (char *)s2 + 1;
   }
   return( *((unsigned char *)s1) - *((unsigned char *)s2) );
}

#endif
