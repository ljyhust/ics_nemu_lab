#include <proc.h>
#include <elf.h>
#include "fs.h"

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  assert(filename != NULL);
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr elf_head;
  fs_read(fd, &elf_head, sizeof(Elf_Ehdr));

  Elf_Phdr elf_phdr[elf_head.e_phnum];
  fs_lseek(fd, elf_head.e_phoff, SEEK_SET);
  fs_read(fd, elf_phdr, sizeof(Elf_Phdr) * elf_head.e_phnum);

  for (int i = 0; i < elf_head.e_phnum; i++) {
    if(elf_phdr[i].p_type == PT_LOAD) {
      fs_lseek(fd, elf_phdr[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)elf_phdr[i].p_vaddr, elf_phdr[i].p_memsz);
      if (elf_phdr[i].p_filesz < elf_phdr[i].p_memsz) {
        memset((void *)(elf_phdr[i].p_vaddr + elf_phdr[i].p_filesz),
         0, (elf_phdr[i].p_memsz - elf_phdr[i].p_filesz));
      }
    }
  }

  return (uintptr_t)elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
  // this method means below
  // void (* method_p)();
  // method_p = entry;
  // method_p(); 
}

