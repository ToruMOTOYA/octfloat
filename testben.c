#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "oct.h"

static unsigned test_dblconv() {
	const double d=1.33,d2=M_PI,d3=-M_E;
	octflt_t a1=octflt_mkd(d), a2,a3;
	double r,r2,r3;
	dumpOct("1.33",a1);
	r=octflt_dbl(&a1);
	printf("%f\n", r);
	a2=octflt_mkd(d2);
	dumpOct("M_PI",a2);
	r2=octflt_dbl(&a2);
	printf("%f\n", r2);
	a3=octflt_mkd(d3);
	dumpOct("-M_E",a3);
	r3=octflt_dbl(&a3);
	printf("%f",r3);
	return !(r==d && r2==d2 && r3==d3);
}

static unsigned test_longconv() {
	const int64_t l1=0x123456789ABCDEFELL, l2=0x1234LL;
	octflt_t a1=octflt_mkl(l1);
	octflt_t a2=octflt_mkl(l2);
	uint64_t r,s;
	dumpOct("l1",a1);
	dumpOct("l2",a2);
	r=octflt_dbl(&a1);
	s=octflt_dbl(&a2);
	printf("r=%016llX s=%016llX\n",(unsigned long long)r,(unsigned long long)s);
	return 0;
}

static unsigned test_mul() {
	const double d1=2.661242341, d2=128.312387471;
	double rd,r256d;
	octflt_t a1=octflt_mkd(d1);
	octflt_t a2=octflt_mkd(d2);
	octflt_t r;
	r=octflt_m(&a1,&a2);
	dumpOct("r1",r);
	rd=d1*d2;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);
	r=octflt_m(&a1,&r);
	dumpOct("r2",r);
	rd=d1*rd;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);
	r=octflt_m(&a1,&r);
	dumpOct("r3",r);
	rd=d1*rd;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);
	r=octflt_m(&a1,&r);
	dumpOct("r4",r);
	rd=d1*rd;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);
	return 0;
}

static unsigned test_add() {
	const double d2=2.661242341, d1=128.312387471, d3=-3.22;
	double rd,r256d;
	octflt_t a1=octflt_mkd(d1);
	octflt_t a2=octflt_mkd(d2);
	octflt_t a3=octflt_mkd(d3);
	octflt_t r;
	r=octflt_a(&a1,&a2);
	dumpOct("r1",r);
	rd=d1+d2;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);

	r=octflt_a(&a2,&a1);
	dumpOct("r1",r);
	rd=d2+d1;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);

	r=octflt_a(&a1,&a3);
	dumpOct("r1",r);
	rd=d1+d3;
	r256d=octflt_dbl(&r);
	printf("FPU=%f OctFlt=%f\n",rd,r256d);
	return 0;
	
}

#if DEBUG<5
static double dtime(void) {
	struct timeval t;
	gettimeofday(&t,NULL);
	return (double)t.tv_sec+((double)t.tv_usec)*1E-6;
}

static unsigned test_bw() {
	static const unsigned long long N_LOOPS=
#if DEBUG>0
		2000000ULL
#else
		200000000ULL
#endif
		;
	const double s1=3.000001, s2=2., s3=1./3., s4=1./1.9999999;
	octflt_t acc,c1,c2,c3,c4;
	volatile double accd,d;
	unsigned long long i;
	double start,end,t,mops;
	/* double -> octflt */
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		c1=octflt_mkd(s1);
		c2=octflt_mkd(s2);
		c3=octflt_mkd(s3);
		c4=octflt_mkd(s4);
	}
	end=dtime();
	t=end-start;
	mops=4.*N_LOOPS/(1.E6*t);
	printf("octflt <- double : %f Mop/s (%f s)\n",mops,t);
	/* octflt <- octflt * octflt */
	acc=octflt_one;
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		acc=octflt_m(&acc,&c1);
		acc=octflt_m(&acc,&c2);
		acc=octflt_m(&acc,&c3);
		acc=octflt_m(&acc,&c4);
	}
	end=dtime();
	t=end-start;
	mops=4.*N_LOOPS/(1.E6*t);
	accd=octflt_dbl(&acc);
	printf("acc=%f\n",accd);
	printf("octflt <- octflt * octflt : %f Mop/s (%f s)\n",mops,t);

	/* double <- double * double */
	accd=1.;
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		accd=accd*s1;
		accd=accd*s2;
		accd=accd*s3;
		accd=accd*s4;
	}
	end=dtime();
	t=end-start;
	mops=4.*N_LOOPS/(1.E6*t);
	printf("accd=%f\n",accd);
	printf("double <- double * double : %f Mop/s (%f s)\n",mops,t);

	/* octflt <- octflt + octflt */
	acc=octflt_one;
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		acc=octflt_a(&acc,&c1);
		acc=octflt_a(&acc,&c2);
		acc=octflt_a(&acc,&c3);
		acc=octflt_a(&acc,&c4);
	}
	end=dtime();
	t=end-start;
	mops=4.*N_LOOPS/(1.E6*t);
	accd=octflt_dbl(&acc);
	printf("acc=%f\n",accd);
	printf("octflt <- octflt + octflt : %f Mop/s (%f s)\n",mops,t);

	/* double <- double + double */
	accd=1.;
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		accd=accd+s1;
		accd=accd+s2;
		accd=accd+s3;
		accd=accd+s4;
	}
	end=dtime();
	t=end-start;
	mops=4.*N_LOOPS/(1.E6*t);
	printf("accd=%f\n",accd);
	printf("double <- double + double : %f Mop/s (%f s)\n",mops,t);
	
	/* double <- octflt */
	start=dtime();
	for(i=0; i<N_LOOPS; i++) {
		d=octflt_dbl(&acc);
		d=octflt_dbl(&c1);
		d=octflt_dbl(&c2);
		d=octflt_dbl(&c3);
		d=octflt_dbl(&c4);
	}
	end=dtime();
	t=end-start;
	mops=5.*N_LOOPS/(1.E6*t);
	printf("double <- octflt : %f Mop/s (%f s)\n",mops,t);

	return 0;
}
#endif

typedef unsigned (*testbench_fun)(void);

extern int main(int argc, char *argv[]) {
	const testbench_fun tb[] = {
		test_dblconv,
		test_longconv,
		test_mul,
		test_add,
#if DEBUG<5
		test_bw,
#endif
		NULL};
	unsigned i=0;
	while(tb[i]) {
		if(tb[i]()) {
			puts("fail");
		} else {
			putchar('\n');
		}
		i++;
	}
	return 0;
}
