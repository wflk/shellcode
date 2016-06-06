
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

#if defined (_WIN64)
#define __x86_64__
typedef uint64_t ptr_t;
#else
#define __i386__
typedef uint32_t ptr_t;
#endif

#if defined (_WIN32) || defined (_WIN64)
#define WIN
#include <windows.h>
#include <shlwapi.h>
#pragma comment (lib, "shlwapi.lib")
#else
#include <sys/mman.h>
#include <errno.h>
#endif

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

#pragma pack(1)
typedef struct ALIGNED_(1) _proc_ctx_t {
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
  ptr_t    sp;
  // error returned from syscall if 64-bit
  ptr_t    sc;
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

#define w_SIZE 122

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
  /* 0020 */ "\x0f\x94\xd0"     /* setz al            */
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
  /* 0045 */ "\xe3\x1c"         /* jecxz 0x63         */
  /* 0047 */ "\x48"             /* dec eax            */
  /* 0048 */ "\xc1\xc8\x20"     /* ror eax, 0x20      */
  /* 004B */ "\xab"             /* stosd              */
  /* 004C */ "\x51"             /* push ecx           */
  /* 004D */ "\x57"             /* push edi           */
  /* 004E */ "\x6a\xff"         /* push 0xffffffff    */
  /* 0050 */ "\x5f"             /* pop edi            */
  /* 0051 */ "\x6a\x06"         /* push 0x6           */
  /* 0053 */ "\x58"             /* pop eax            */
  /* 0054 */ "\x0f\x05"         /* syscall            */
  /* 0056 */ "\x5f"             /* pop edi            */
  /* 0057 */ "\x59"             /* pop ecx            */
  /* 0058 */ "\xab"             /* stosd              */
  /* 0059 */ "\x48"             /* dec eax            */
  /* 005A */ "\xc1\xc8\x20"     /* ror eax, 0x20      */
  /* 005D */ "\xab"             /* stosd              */
  /* 005E */ "\x5f"             /* pop edi            */
  /* 005F */ "\x6a\x01"         /* push 0x1           */
  /* 0061 */ "\x58"             /* pop eax            */
  /* 0062 */ "\xc3"             /* ret                */
  /* 0063 */ "\x66\x8c\xe9"     /* mov cx, gs         */
  /* 0066 */ "\xe3\xf6"         /* jecxz 0x5e         */
  /* 0068 */ "\x54"             /* push esp           */
  /* 0069 */ "\x58"             /* pop eax            */
  /* 006A */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 006D */ "\x74\xef"         /* jz 0x5e            */
  /* 006F */ "\x6a\xff"         /* push 0xffffffff    */
  /* 0071 */ "\x5b"             /* pop ebx            */
  /* 0072 */ "\x6a\x06"         /* push 0x6           */
  /* 0074 */ "\x58"             /* pop eax            */
  /* 0075 */ "\xcd\x80"         /* int 0x80           */
  /* 0077 */ "\xab"             /* stosd              */
  /* 0078 */ "\xeb\xe4"         /* jmp 0x5e           */
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
  
  printf ("\nsizeof(void*) = %i\nsizeof(uint16_t) = %i\nsizeof(uint32_t) = %i\n", 
    sizeof(void*), sizeof(uint16_t), sizeof(uint32_t));
  
  memset(&pc, 0, sizeof(pc));
  
  if (get_ctx(&pc)) {
    printf ("\n%s %i-bit cs=0x%02X ds=0x%02X es=0x%02X fs=0x%02X gs=0x%02X ss=0x%02X sp=%p sys_close error = %p\n",
      pc.win ? "Windows" : "NIX", pc.emu ? 32 : 64, pc.cs, pc.ds, pc.es, pc.fs, pc.gs, pc.ss, pc.sp, pc.sc);
  } else {
    printf ("\nsomething went wrong in function..");
  }
  return 0;
}
