#include "ecc.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

void
get_curve_param(curve_params_t *para)
{

#ifdef EIGHT_BIT_PROCESSOR
	//init parameters
	//prime
	para->p[1] = 0x7;
	para->p[0] = 0xD3;
	
	para->omega[0] = 0x01;
	para->omega[8] = 0x01;
	
	//curve that will be used
	//c

	
	para->E.c[1] = 0x1;
	para->E.c[0] = 0x12;
	
	//base point
	para->G.x[1] = 0x5;
	para->G.x[0] = 0xAB;
	
	para->G.y[1] = 0x2;
	para->G.y[0] = 0xA0;
	
	para->G.z[1] = 0x3;
	para->G.z[0] = 0xA7;
#endif /*EIGHT_BIT_PROCESSOR*/	
}

NN_UINT omega_mul(NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *omega, NN_UINT digits)
{

#ifdef EIGHT_BIT_PROCESSOR

	NN_Assign(a, b, digits);
	NN_Add(a+8, a+8, b, digits+1);
	return (digits+9);
	// EIGHT_BIT_PROCESSOR
#elif defined(SIXTEEN_BIT_PROCESSOR)

    NN_Assign(a, b, digits);
    NN_Add(a+4, a+4, b, digits+1);
    return (digits+5);

    /* SIXTEEN_BIT_PROCESSOR */
#elif defined(THIRTYTWO_BIT_PROCESSOR)

    NN_Assign(a, b, digits);
    NN_Add(&a[2], &a[2], b, digits+1);
    return (digits+3);

#endif /*THIRTYTWO_BIT_PROCESSOR*/
}

