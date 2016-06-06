
// just something to retrieve segment registers of x86 based OS
// segfaults on 32-bit BSD (for now)
// odzhan

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined (_WIN32) || defined(_WIN64)
#define WIN
#include <windows.h>
#include <shlwapi.h>
#pragma comment (lib, "shlwapi.lib")
#else
#include <sys/mman.h>
#endif

typedef struct _proc_ctx_t {
  uint32_t emu;
  uint32_t win;
  // segment registers
  uint16_t cs;
  uint16_t ds;
  uint16_t es;
  uint16_t fs;
  uint16_t gs;
  uint16_t ss;
  
  // stack pointer
  void*    sp;
  void*    sc;
} proc_ctx;

#ifdef WIN
/**F*****************************************************************/
void xstrerror (char *fmt, ...) 
/**
 * PURPOSE : Display windows error
 *
 * RETURN :  Nothing
 *
 * NOTES :   None
 *
 *F*/
{
  char    *error=NULL;
  va_list arglist;
  char    buffer[2048];
  DWORD   dwError=GetLastError();
  
  va_start (arglist, fmt);
  wvnsprintf (buffer, sizeof(buffer) - 1, fmt, arglist);
  va_end (arglist);
  
  if (FormatMessage (
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
      (LPSTR)&error, 0, NULL))
  {
    printf ("[ %s : %s\n", buffer, error);
    LocalFree (error);
  } else {
    printf ("[ %s : %lu\n", buffer, dwError);
  }
}
#else
#define xstrerror printf
#endif

#ifndef TEST

#define w_SIZE 117

char w[]= {
  /* 0000 */ "\x57"             /* push edi           */
  /* 0001 */ "\x51"             /* push ecx           */
  /* 0002 */ "\x5f"             /* pop edi            */
  /* 0003 */ "\x54"             /* push esp           */
  /* 0004 */ "\x58"             /* pop eax            */
  /* 0005 */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 0008 */ "\x74\x02"         /* jz 0xc             */
  /* 000A */ "\x5f"             /* pop edi            */
  /* 000B */ "\x57"             /* push edi           */
  /* 000C */ "\x31\xc0"         /* xor eax, eax       */
  /* 000E */ "\x48"             /* dec eax            */
  /* 000F */ "\xf7\xd8"         /* neg eax            */
  /* 0011 */ "\x74\x04"         /* jz 0x17            */
  /* 0013 */ "\x8b\x7c\x24\x08" /* mov edi, [esp+0x8] */
  /* 0017 */ "\xab"             /* stosd              */
  /* 0018 */ "\x8d\x48\xff"     /* lea ecx, [eax-0x1] */
  /* 001B */ "\x54"             /* push esp           */
  /* 001C */ "\x58"             /* pop eax            */
  /* 001D */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 0020 */ "\x0f\x94\xc0"     /* setz al            */
  /* 0023 */ "\xab"             /* stosd              */
  /* 0024 */ "\x66\x8c\xc8"     /* mov ax, cs         */
  /* 0027 */ "\x66\xab"         /* stosw              */
  /* 0029 */ "\x66\x8c\xd8"     /* mov ax, ds         */
  /* 002C */ "\x66\xab"         /* stosw              */
  /* 002E */ "\x66\x8c\xc0"     /* mov ax, es         */
  /* 0031 */ "\x66\xab"         /* stosw              */
  /* 0033 */ "\x66\x8c\xe0"     /* mov ax, fs         */
  /* 0036 */ "\x66\xab"         /* stosw              */
  /* 0038 */ "\x66\x8c\xe8"     /* mov ax, gs         */
  /* 003B */ "\x66\xab"         /* stosw              */
  /* 003D */ "\x66\x8c\xd0"     /* mov ax, ss         */
  /* 0040 */ "\x66\xab"         /* stosw              */
  /* 0042 */ "\x54"             /* push esp           */
  /* 0043 */ "\x58"             /* pop eax            */
  /* 0044 */ "\xab"             /* stosd              */
  /* 0045 */ "\xe3\x17"         /* jecxz 0x5e         */
  /* 0047 */ "\xc1\xe8\x20"     /* shr eax, 0x20      */
  /* 004A */ "\xab"             /* stosd              */
  /* 004B */ "\x51"             /* push ecx           */
  /* 004C */ "\x57"             /* push edi           */
  /* 004D */ "\x6a\xff"         /* push 0xffffffff    */
  /* 004F */ "\x5f"             /* pop edi            */
  /* 0050 */ "\x6a\x06"         /* push 0x6           */
  /* 0052 */ "\x58"             /* pop eax            */
  /* 0053 */ "\x0f\x05"         /* syscall            */
  /* 0055 */ "\x5f"             /* pop edi            */
  /* 0056 */ "\x59"             /* pop ecx            */
  /* 0057 */ "\xab"             /* stosd              */
  /* 0058 */ "\xc1\xc0\x20"     /* rol eax, 0x20      */
  /* 005B */ "\xab"             /* stosd              */
  /* 005C */ "\x5f"             /* pop edi            */
  /* 005D */ "\xc3"             /* ret                */
  /* 005E */ "\x66\x8c\xe9"     /* mov cx, gs         */
  /* 0061 */ "\xe3\xf9"         /* jecxz 0x5c         */
  /* 0063 */ "\x54"             /* push esp           */
  /* 0064 */ "\x58"             /* pop eax            */
  /* 0065 */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 0068 */ "\x74\xf2"         /* jz 0x5c            */
  /* 006A */ "\x6a\xff"         /* push 0xffffffff    */
  /* 006C */ "\x5b"             /* pop ebx            */
  /* 006D */ "\x6a\x06"         /* push 0x6           */
  /* 006F */ "\x58"             /* pop eax            */
  /* 0070 */ "\xcd\x80"         /* int 0x80           */
  /* 0072 */ "\xab"             /* stosd              */
  /* 0073 */ "\xeb\xe7"         /* jmp 0x5c           */
};

typedef void (*get_ctx_t)(proc_ctx*);

int get_ctx(proc_ctx *c)
{
  get_ctx_t func;
  int ok=0;
  
#ifdef WIN
  func=(get_ctx_t)VirtualAlloc (0, w_SIZE, 
    MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
  func=(get_ctx_t)mmap (0, w_SIZE, 
    PROT_EXEC | PROT_WRITE | PROT_READ, 
    MAP_ANON  | MAP_PRIVATE, -1, 0);
#endif
  if (func!=NULL)
  {
    memcpy (func, w, w_SIZE);
    func(c);
    ok=1;
#ifdef WIN
    VirtualFree (func, w_SIZE, MEM_RELEASE);
#else
    munmap (func, w_SIZE);
#endif
  } else {
    #ifdef WIN
    xstrerror("VirtualAlloc()");
    #else
    printf ("\nmmap(): %i\n", errno);
    #endif
  }
  return ok;
}
#endif

int main(void) {
  proc_ctx pc;
  
  setbuf(stdout, NULL);
  
  printf ("\nsizeof(void*) = %i\nsizeof(uint32_t) = %i\n", 
    sizeof(void*), sizeof(uint32_t));
  
  memset(&pc, 0, sizeof(pc));
  
  if (get_ctx(&pc)) {
    printf ("\n%s %i-bit cs=0x%02X ds=0x%02X es=0x%02X fs=0x%02X gs=0x%02X ss=0x%02X sp=%p sys_close error = %p\n",
      pc.win ? "Windows" : "NIX", pc.emu ? 32 : 64, pc.cs, pc.ds, pc.es, pc.fs, pc.gs, pc.ss, pc.sp, pc.sc);
  } else {
    printf ("\nsomething went wrong in function..");
  }
  return 0;
}
