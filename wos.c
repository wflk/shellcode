/**
  Copyright © 2016 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

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

#if defined(__x86_64__)
typedef uint64_t ptr_t;
#else
typedef uint32_t ptr_t;
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
    printf ("\n  E: %s : %s\n", buffer, error);
    LocalFree (error);
  } else {
    printf ("\n  E: %s : %lu\n", buffer, dwError);
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
  #ifdef WIN
  DWORD op;
  #endif
  int ok=0;
  
  printf ("\n  Allocating executable memory...");
#ifdef WIN
  func=(get_ctx_t)VirtualAlloc (0, w_SIZE, 
    MEM_COMMIT, PAGE_READWRITE);
#else
  func=(get_ctx_t)mmap (0, w_SIZE, 
    PROT_EXEC | PROT_WRITE | PROT_READ, 
    MAP_ANON  | MAP_PRIVATE, -1, 0);
#endif

#ifdef WIN
  if (func!=NULL)
#else
  if (func!=MAP_FAILED)
#endif
  {
    printf ("\n  Executing function...");
    memcpy (func, w, w_SIZE);
    #ifdef WIN
      if (VirtualProtect((LPVOID)func, w_SIZE, PAGE_EXECUTE, &op)) {
        func(c);
        ok=1;
      } else {
        xstrerror("VirtualProtect()");
      }
    #else
    func(c);
    ok=1;
    #endif
#ifdef WIN
    VirtualFree (func, w_SIZE, MEM_RELEASE);
#else
    munmap (func, w_SIZE);
#endif
  } else {
    #ifdef WIN
    xstrerror("VirtualAlloc()");
    #else
    printf ("\n  E: mmap(): %i\n", errno);
    #endif
  }
  return ok;
}
#endif

int main(void) {
  proc_ctx pc;
  ptr_t sc_v;
  char *os="Unrecognized";
  char *arch="unknown";
  
  setbuf(stdout, NULL);
  
  printf ("\n  sizeof(uint16_t) = %i"
          "\n  sizeof(uint32_t) = %i"
          "\n  sizeof(void*)    = %i\n", 
    sizeof(uint16_t), sizeof(uint32_t), sizeof(void*));
  
  memset(&pc, 0, sizeof(pc));
  
  if (get_ctx(&pc)) {
    
    // determine operating system
    sc_v = (ptr_t)pc.sc;
    sc_v &= 0xFF;
    
    if (sc_v==0 || sc_v==0x05) {
      os="Windows";
      if (pc.cs==0x23 || pc.cs==0x33) { 
        arch="64"; 
      } else arch="32";
    } else if (sc_v==0x06) {
      os="OSX"; // might be wrong
      if (pc.ds==0) {
        arch="64";
      } else arch="32";
    } else if (sc_v==0x09) {
      if (pc.ds==0x23 || pc.ds==0x33) {
        os="OpenBSD";
        if (pc.ds==0x23) {
          arch="64";
        } else arch="32";
      } else if (pc.ds==0x2B || pc.ds==0x3B) {
        os="FreeBSD";
        if (pc.ds==0x43) {
          arch="64";
        } else arch="32";
      } else {
        os="BSD";
      }
    } else if (sc_v==0xF2 || sc_v==0xF7) {
      os="Linux";
      if (pc.cs==0x23 || pc.cs==0x33) { 
        arch="64"; 
      } else arch="32";
    }
    
    printf ("\n\n  OS       : %s x%s", os, arch);
    
    printf ("\n  Binary   : %i-bit",
      pc.emu ? 32 : 64);
    
    printf ("\n  Segments : cs=0x%02X ds=0x%02X es=0x%02X",
      pc.cs, pc.ds, pc.es);
      
    printf ("\n  Segments : fs=0x%02X gs=0x%02X ss=0x%02X\n",
      pc.fs, pc.gs, pc.ss);
    
    printf ("\n  Stack Ptr: %p",   pc.sp);
    printf ("\n  Syscall E: %p\n", pc.sc);
  } else {
    printf ("\nsomething went wrong in function..");
  }
  return 0;
}
