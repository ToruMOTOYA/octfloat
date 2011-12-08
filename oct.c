#include <stdio.h>
#include <math.h>
#include "oct.h"
/* bytes 32 ........ 24 ........ 16 ........ 08 ........ 00 
 *         SEEEEMMMM    MMMMMMMM    MMMMMMMM    MMMMMMMM
 * s: 1-bit sign bit
 * e: 19-bit exponent field
 * m: 236-bit mantissa
 * 
 */
static unsigned octSgn(const octflt_t *x);
static int32_t octExp(const octflt_t *x);
static octflt_t octFrac(const octflt_t *x);
static void octFracExt(octflt_t *x);
static void octShl(octflt_t *x, unsigned l);
static void octShr(octflt_t *x, unsigned r);
static const int32_t EXP_BIAS=0x0003FFFF;
static const uint64_t MASK_OCTMAN_HH=0x00000FFFFFFFFFFFULL;
/* .rodata */
const octflt_t octflt_one=MK256(0x3FFFF00000000000ULL,0ULL,0ULL,0ULL);
const octflt_t octflt_zero=MK256(0ULL,0ULL,0ULL,0ULL);
const octflt_t octflt_nzero=MK256(0x8000000000000000ULL,0ULL,0ULL,0ULL);
const octflt_t octflt_nan=MK256(0x7FFFF00000000000ULL,0xDEADULL,0ULL,0ULL);
const octflt_t octflt_pinf=MK256(0x7FFFF00000000000ULL,0ULL,0ULL,0ULL);
const octflt_t octflt_ninf=MK256(0xFFFFF00000000000ULL,0ULL,0ULL,0ULL);

/* .text */
static unsigned octSgn(const octflt_t *x) {
	return x->hh>>63ULL;
}
static int32_t octExp(const octflt_t *x) {
	int32_t w=(x->hh>>44ULL)&0x7FFFF;
	return w;
}
static octflt_t octFrac(const octflt_t *x) {
	octflt_t t;
	t.ll=x->ll;
	t.lh=x->lh;
	t.hl=x->hl;
	t.hh=x->hh&MASK_OCTMAN_HH;
	return t;
}
static void octFracExt(octflt_t *x) {
	static const uint64_t BIT_OCTMAN_EXT_HH=0x0000100000000000ULL;
	x->hh |= BIT_OCTMAN_EXT_HH;
}
static void octShl(octflt_t *x, unsigned l) {
	unsigned dl;
	while(l>=64) {
		x->hh=x->hl;
		x->hl=x->lh;
		x->lh=x->ll;
		x->ll=0ULL;
		l-=64;
	}
	dl=64-l;
	x->hh=((x->hh)<<l) | ((x->hl)>>dl);
	x->hl=((x->hl)<<l) | ((x->lh)>>dl);
	x->lh=((x->lh)<<l) | ((x->ll)>>dl);
	x->ll=x->ll<<l;
}
static void octShr(octflt_t *x, unsigned r) {
	unsigned dr;
	while(r>=64) {
		x->ll=x->lh;
		x->lh=x->hl;
		x->hl=x->hh;
		x->hh=0ULL;
		r-=64;
	}
	dr=64-r;
	x->ll=((x->ll)>>r) | ((x->lh)<<dr);
	x->lh=((x->lh)>>r) | ((x->hl)<<dr);
	x->hl=((x->hl)>>r) | ((x->hh)<<dr);
	x->hh=((x->hh)>>r);
}

/* arith */
extern octflt_t octflt_m(const octflt_t *m1, const octflt_t *m2) {
	octflt_t r[2];
	octflt_t fra1, fra2;
	octflt_t t;
	unsigned lead0,shlsz;
	uint64_t e,e0,e1,s,s0,s1;
	fra1=octFrac(m1);
	octFracExt(&fra1);
	fra2=octFrac(m2);
	octFracExt(&fra2);
	s0=octSgn(m1);
	s1=octSgn(m2);
	e0=octExp(m1);
	e1=octExp(m2);
	if(e0==0x7FFFF || e0==0) { /* m1: 0, NaN or Inf */
		r[0].hh=m1->hh^((m2->hh)&(1ULL<<63));
		r[0].hl=m1->hl;
		r[0].lh=m1->lh;
		r[0].ll=m1->ll;
		return r[0];
	} else if(e1==0x7FFFF || e1==0) { /* m2: 0, NaN or Inf */
		r[0].hh=m2->hh^((m1->hh)&(1ULL<<63));
		r[0].hl=m2->hl;
		r[0].lh=m2->lh;
		r[0].ll=m2->ll;
		return r[0];
	}
	e=e0+e1-EXP_BIAS;
	s=s0^s1;
	/* multiply */
#if DEBUG>4
	dumpOct("m1",*m1); dumpOct("m2",*m2);
	dumpOct("f1",fra1); dumpOct("f2",fra2);
#endif
	mul256(r,&fra1,&fra2);
#if DEBUG>4
	dumpOct("r[h]",r[H512]); dumpOct("r[l]",r[L512]);
#endif
	lead0=__builtin_clzl(r[H512].hh); /* 39 or 38 */
	if(lead0==38) e++;
	if(e<1) {
		return s?octflt_nzero:octflt_zero;
	} else if(e>=0x7FFFF) {
		return s?octflt_ninf:octflt_pinf;
	}
#if DEBUG>4
	printf("lead0=%d\n",lead0);
#endif
	shlsz=lead0-19;
	octShl(&r[H512],shlsz);
	t=r[L512];
	octShr(&t,256-shlsz);
	r[H512].ll=r[H512].ll|t.ll;
	octShl(&r[L512],shlsz);
#if DEBUG>4
	dumpOct("t",t);
	dumpOct("shifted h",r[H512]);
	dumpOct("shifted l",r[L512]);
	printf("e=%05X\n",(unsigned)e);
#endif
	r[H512].hh= (s?1ULL<<63:0) | (e<<44) | octFrac(&r[H512]).hh;
	return r[H512];
}
static void swap(uint64_t *a,uint64_t *b) {
	uint64_t t;
	t=*b;
	*b=*a;
	*a=t;
}
static unsigned gt256(const octflt_t *x, const octflt_t *y) {
	unsigned r;
	if(x->hh>y->hh)
		r=1;
	else if(x->hl>y->hl)
		r=1;
	else if(x->lh>y->lh)
		r=1;
	else if(x->ll>y->ll)
		r=1;
	else
		r=0;
	return r;
}
static void neg236(octflt_t *x) {
	static const octflt_t one=MK256(0ULL,0ULL,0ULL,1ULL);
	x->hh=~(x->hh);
	x->hl=~(x->hl);
	x->lh=~(x->lh);
	x->ll=~(x->ll);
	add256(x,x,&one);
	/*x->hh=x->hh&0x7FFFFFFFFFFFULL;*/
}
extern octflt_t octflt_a(const octflt_t *a1, const octflt_t *a2) {
	/* add */
	octflt_t r,f1,f2;
	uint64_t e,e1,e2,s,s1,s2/*,ss1,ss2*/;
	int sft;
	/* assert(a1*a2>=0.); */
	s1=octSgn(a1);
	s2=octSgn(a2);
	e1=octExp(a1);
	e2=octExp(a2);
	f1=octFrac(a1);
	f2=octFrac(a2);
	/*ss1=(s1?0:(1<<19))|e1;
	  ss2=(s2?0:(1<<19))|e2;*/
	if(e1<e2 || ((e1==e2) && gt256(&f2,&f1))) {
		swap(&s1,&s2);
		swap(&e1,&e2);
		r=f2;
		f2=f1;
		f1=r;
	}
	if(e1) octFracExt(&f1);
	if(e2) octFracExt(&f2);
	/* shift f1, f2 */
#if DEBUG>4
	printf("e1=%05X, e2=%05X\n",(int)e1,(int)e2);
	dumpOct("f1",f1); dumpOct("f2",f2);
#endif
	sft=e1-e2;
	octShr(&f2,sft);
	/* negate */
	if(s1) neg236(&f1);
	if(s2) neg236(&f2);
#if DEBUG>4
	puts("shift");
	dumpOct("f1",f1); dumpOct("f2",f2);
#endif
	/* add */
	add256(&r,&f1,&f2);
#if DEBUG>4
	dumpOct("r",r);
#endif
	/* extract sign */
	s=(r.hh)&(1ULL<<46)?1ULL:0ULL;
	if(s) neg236(&r);
	r.hh=r.hh&0x3FFFFFFFFFFFULL;
	/* zero detect */
	if(!(r.hh | r.hl | r.lh | r.ll))
		return s?octflt_nzero:octflt_zero;
	/* normalize */
	sft=19-__builtin_clzl(r.hh); /* right, 19: std */
#if DEBUG>4
	dumpOct("negr",r);
	printf("shift=%d\n",sft);
#endif
	e=e1+sft;
	if(sft>0)
		octShr(&r,sft);
	else
		octShl(&r,-sft);
#if DEBUG>4
	dumpOct("sftr",r);
#endif
	r=octFrac(&r);
	r.hh |=(s?1ULL<<63:0)| (e<<44);
	return r;
}
/* convert */
static unsigned extract754sgnb(double x) {
	uint64_t *xp=(void*)&x;
	return (*xp)>>63ULL;
}
static int32_t extract754expb(double x) {
	static uint64_t MASK754_EXPB=0x7FF0000000000000ULL;
	uint64_t *xp=(void*)&x;
	uint64_t masked=(*xp)&MASK754_EXPB;
	uint64_t shifted=masked>>52ULL;
	int32_t e=(int32_t)shifted;
	return e;
}
static uint64_t extract754fracb(double x) {
	uint64_t *xp=(void*)&x;
	return (*xp)&0x000FFFFFFFFFFFFFULL;
}
static const int32_t F754EXPBIAS=1023;
extern double octflt_dbl(const octflt_t *x) {
	union {
		uint64_t l;
		double d;
	} u;
	int32_t eb=octExp(x);
	int32_t e=eb-EXP_BIAS;
	octflt_t m=octFrac(x);
	unsigned s=octSgn(x);
	uint64_t rfrac=(m.hh<<8ULL) | (m.hl>>56ULL);
	/* TBD: round, then update exponent */
	/* printf("e=%08X\n",eb); */
	if(e>1023) {
		u.d=INFINITY;
	} else if(e<-1022) {
		u.d=s?-1.:1.;
	} else {
		uint64_t be=e+F754EXPBIAS;
		u.l=(s?(1ULL<<63ULL):0)|(be<<52ULL)| rfrac;
	}
#if DEBUG>0
	dumpDbl(u.d);
#endif
	return u.d;
}
extern octflt_t octflt_mkl(int64_t x) {
	octflt_t r;
	if(!x) {
		r=octflt_zero;
	} else {
		unsigned head1/*=63*/;
		unsigned s=x<0?1:0;
		uint64_t absx=(uint64_t) (x<0?-x:x);
		uint64_t e;
		uint64_t mh,ml;
		/*while(!(absx&(1ULL<<head1))) head1--;*/
		head1=63-__builtin_clzl(absx);
		e=head1+EXP_BIAS;
		x ^= (1ULL << head1);	/* remove heading 1 */
		if(head1>=44) {
			mh=absx>>(head1-44ULL);
			ml=absx<<(64ULL+44ULL-head1);
		} else {
			mh=absx<<(44ULL-head1);
			ml=0ULL;
		}
		r.hh=(s?(1ULL<<63ULL):0ULL) | (e<<44ULL) | mh;
		r.hl=ml;
		r.lh=0ULL;
		r.ll=0ULL;
	}
	return r;
}
extern octflt_t octflt_mkd(double x) {
	octflt_t r;
	int32_t e=extract754expb(x);
	unsigned s=extract754sgnb(x);
	uint64_t f=extract754fracb(x);
	int32_t nbe,ode;

	nbe=e-F754EXPBIAS;
	ode=nbe+EXP_BIAS;
	/* printf("e=%d nbe=%d, ode=%d\n",e,nbe,ode); */
	r.hh= (s?(1ULL<<63ULL):0ULL) | (((uint64_t)ode)<<44ULL) | (f>>8);
	r.hl= (f<<56ULL);
	r.lh= 0ULL;
	r.ll= 0ULL;
	return r;
}
