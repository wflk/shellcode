
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

void get_ctx(proc_ctx *);

int main(void) {
  proc_ctx pc;
  
  setbuf(stdout, NULL);
  
  printf ("\nsizeof(void*) = %u\nsizeof(uint32_t) = %u\n", 
    sizeof(void*), sizeof(uint32_t));
  
  memset(&pc, 0, sizeof(pc));
  get_ctx(&pc);
      
  printf ("\n%s %i-bit cs=0x%02X ds=0x%02X es=0x%02X fs=0x%02X gs=0x%02X ss=0x%02X sp=%p sys_close error = %p\n",
    pc.win ? "Windows" : "NIX", pc.emu ? 32 : 64, pc.cs, pc.ds, pc.es, pc.fs, pc.gs, pc.ss, pc.sp, pc.sc);
  return 0;
}
