
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
  void*    sp;
  // error returned from syscall if 64-bit
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

#define w_SIZE 124

char w[]= {
  /* 0000 */ "\x53"             /* push ebx           */
  /* 0001 */ "\x57"             /* push edi           */
  /* 0002 */ "\x51"             /* push ecx           */
  /* 0003 */ "\x5f"             /* pop edi            */
  /* 0004 */ "\x54"             /* push esp           */
  /* 0005 */ "\x58"             /* pop eax            */
  /* 0006 */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 0009 */ "\x74\x02"         /* jz 0xd             */
  /* 000B */ "\x5f"             /* pop edi            */
  /* 000C */ "\x57"             /* push edi           */
  /* 000D */ "\x31\xc0"         /* xor eax, eax       */
  /* 000F */ "\x48"             /* dec eax            */
  /* 0010 */ "\xf7\xd8"         /* neg eax            */
  /* 0012 */ "\x74\x04"         /* jz 0x18            */
  /* 0014 */ "\x8b\x7c\x24\x0c" /* mov edi, [esp+0xc] */
  /* 0018 */ "\xab"             /* stosd              */
  /* 0019 */ "\x8d\x48\xff"     /* lea ecx, [eax-0x1] */
  /* 001C */ "\x54"             /* push esp           */
  /* 001D */ "\x58"             /* pop eax            */
  /* 001E */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 0021 */ "\x0f\x94\xd0"     /* setz al            */
  /* 0024 */ "\xab"             /* stosd              */
  /* 0025 */ "\x66\x8c\xc8"     /* mov ax, cs         */
  /* 0028 */ "\x66\xab"         /* stosw              */
  /* 002A */ "\x66\x8c\xd8"     /* mov ax, ds         */
  /* 002D */ "\x66\xab"         /* stosw              */
  /* 002F */ "\x66\x8c\xc0"     /* mov ax, es         */
  /* 0032 */ "\x66\xab"         /* stosw              */
  /* 0034 */ "\x66\x8c\xe0"     /* mov ax, fs         */
  /* 0037 */ "\x66\xab"         /* stosw              */
  /* 0039 */ "\x66\x8c\xe8"     /* mov ax, gs         */
  /* 003C */ "\x66\xab"         /* stosw              */
  /* 003E */ "\x66\x8c\xd0"     /* mov ax, ss         */
  /* 0041 */ "\x66\xab"         /* stosw              */
  /* 0043 */ "\x54"             /* push esp           */
  /* 0044 */ "\x58"             /* pop eax            */
  /* 0045 */ "\xab"             /* stosd              */
  /* 0046 */ "\xe3\x1d"         /* jecxz 0x65         */
  /* 0048 */ "\x48"             /* dec eax            */
  /* 0049 */ "\xc1\xc8\x20"     /* ror eax, 0x20      */
  /* 004C */ "\xab"             /* stosd              */
  /* 004D */ "\x51"             /* push ecx           */
  /* 004E */ "\x57"             /* push edi           */
  /* 004F */ "\x6a\xff"         /* push 0xffffffff    */
  /* 0051 */ "\x5f"             /* pop edi            */
  /* 0052 */ "\x6a\x06"         /* push 0x6           */
  /* 0054 */ "\x58"             /* pop eax            */
  /* 0055 */ "\x0f\x05"         /* syscall            */
  /* 0057 */ "\x5f"             /* pop edi            */
  /* 0058 */ "\x59"             /* pop ecx            */
  /* 0059 */ "\xab"             /* stosd              */
  /* 005A */ "\x48"             /* dec eax            */
  /* 005B */ "\xc1\xc8\x20"     /* ror eax, 0x20      */
  /* 005E */ "\xab"             /* stosd              */
  /* 005F */ "\x5f"             /* pop edi            */
  /* 0060 */ "\x5b"             /* pop ebx            */
  /* 0061 */ "\x6a\x01"         /* push 0x1           */
  /* 0063 */ "\x58"             /* pop eax            */
  /* 0064 */ "\xc3"             /* ret                */
  /* 0065 */ "\x66\x8c\xe9"     /* mov cx, gs         */
  /* 0068 */ "\xe3\xf5"         /* jecxz 0x5f         */
  /* 006A */ "\x54"             /* push esp           */
  /* 006B */ "\x58"             /* pop eax            */
  /* 006C */ "\xc1\xe8\x18"     /* shr eax, 0x18      */
  /* 006F */ "\x74\xee"         /* jz 0x5f            */
  /* 0071 */ "\x6a\xff"         /* push 0xffffffff    */
  /* 0073 */ "\x5b"             /* pop ebx            */
  /* 0074 */ "\x6a\x06"         /* push 0x6           */
  /* 0076 */ "\x58"             /* pop eax            */
  /* 0077 */ "\xcd\x80"         /* int 0x80           */
  /* 0079 */ "\xab"             /* stosd              */
  /* 007A */ "\xeb\xe3"         /* jmp 0x5f           */
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
  
  printf ("\n  sizeof(uint16_t) = %i"
          "\n  sizeof(uint32_t) = %i"
          "\n  sizeof(void*)    = %i\n", 
    sizeof(uint16_t), sizeof(uint32_t), sizeof(void*));
  
  memset(&pc, 0, sizeof(pc));
  
  if (get_ctx(&pc)) {
    
    printf ("\n  OS       : %s %i-bit", 
      pc.win ? "Windows" : "NIX",
      pc.emu ? 32 : 64);
    
    printf ("\n  Segments : cs=0x%02X ds=0x%02X es=0x%02X",
      pc.cs, pc.ds, pc.es);
      
    printf ("\n  Segments : fs=0x%02X gs=0x%02X ss=0x%02X\n",
      pc.fs, pc.gs, pc.ss);
    
    printf ("\n  Stack Ptr: %p", pc.sp);
    printf ("\n  Syscall E: %p\n", pc.sc);
  } else {
    printf ("\nsomething went wrong in function..");
  }
  return 0;
}
