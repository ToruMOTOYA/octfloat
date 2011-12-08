#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#ifndef OCTFLT_BIG_ENDIAN
typedef struct {
	uint64_t ll,lh,hl,hh;
} octflt_t;
#define MK256(HH,HL,LH,LL) {LL,LH,HL,HH}
#define H512 1
#define L512 0
#else /* big endian */
typedef struct {
	uint64_t hh,hl,lh,ll;
} octflt_t;
#define MK256(HH,HL,LH,LL) {HH,HL,LH,LL}
#define H512 0
#define L512 1
#endif /* ifndef BIG_ENDIAN */
extern const octflt_t octflt_zero;
extern const octflt_t octflt_nzero;
extern const octflt_t octflt_one;
extern const octflt_t octflt_nan;
extern const octflt_t octflt_pinf;
extern const octflt_t octflt_ninf;
extern octflt_t octflt_m(const octflt_t *m1, const octflt_t *m2);
extern octflt_t octflt_a(const octflt_t *a1, const octflt_t *a2);
extern double octflt_dbl(const octflt_t *x);
extern octflt_t octflt_mkl(int64_t x);
extern octflt_t octflt_mkd(double x);
/* assembly implementations */
extern void mul256(octflt_t *o, const octflt_t *i1, const octflt_t *i2);
extern uint64_t add256(octflt_t *o, const octflt_t *i1, const octflt_t *i2);

/* use static fun. instead of macros */
static inline void dumpOct(const char *labl,const octflt_t x) {
	printf("%s = %016llX %016llX %016llX %016llX\n",labl,
	       (unsigned long long)x.hh,
	       (unsigned long long)x.hl,
	       (unsigned long long)x.lh,
	       (unsigned long long)x.ll);
}
static inline void dumpDbl(double x) {
	unsigned long long *xp=(void*)&x;
	printf("%016llX\n",*xp);
}

