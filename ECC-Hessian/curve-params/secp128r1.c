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
	para->p[24] = 0x00;
    para->p[23] = 0xFF;
    para->p[22] = 0xFF;
    para->p[21] = 0xFF;
    para->p[20] = 0xFF;
    para->p[19] = 0xFF;
    para->p[18] = 0xFF;
    para->p[17] = 0xFF;
    para->p[16] = 0xFF;
    para->p[15] = 0xFF;
    para->p[14] = 0xFF;
    para->p[13] = 0xFF;
    para->p[12] = 0xFF;
    para->p[11] = 0xFF;
    para->p[10] = 0xFF;
    para->p[9] = 0xFF;
    para->p[8] = 0xFE;
    para->p[7] = 0xFF;
    para->p[6] = 0xFF;
    para->p[5] = 0xFF;
    para->p[4] = 0xFF;
    para->p[3] = 0xFF;
    para->p[2] = 0xFF;
    para->p[1] = 0xFF;
    para->p[0] = 0xFF;
	
	para->omega[0] = 0x01;
	para->omega[8] = 0x01;
	
	//curve that will be used
	//c
	para->E.c[24] = 0x00;
	para->E.c[23] = 0x16;
	para->E.c[22] = 0xA4;
	para->E.c[21] = 0xC7;
	para->E.c[20] = 0xC2;
	para->E.c[19] = 0x03;
	para->E.c[18] = 0x0F;
	para->E.c[17] = 0xAD;
	para->E.c[16] = 0x13;
	para->E.c[15] = 0x80;
	para->E.c[14] = 0xAB;
	para->E.c[13] = 0xF8;
	para->E.c[12] = 0xC2;
	para->E.c[11] = 0xD4;
	para->E.c[10] = 0x7D;
	para->E.c[9] = 0xC3;
	para->E.c[8] = 0xE0;
	para->E.c[7] = 0xC2;
	para->E.c[6] = 0x0A;
	para->E.c[5] = 0xF6;
	para->E.c[4] = 0x2F;
	para->E.c[3] = 0x6E;
	para->E.c[2] = 0xDD;
	para->E.c[1] = 0x06;
	para->E.c[0] = 0xA7;

	
	//base point
	para->G.x[24] = 0x0;
	para->G.x[23] = 0x52;
	para->G.x[22] = 0xFD;
	para->G.x[21] = 0x0C;
	para->G.x[20] = 0xE7;
	para->G.x[19] = 0x8D;
	para->G.x[18] = 0x06;
	para->G.x[17] = 0x51;
	para->G.x[16] = 0xB4;
	para->G.x[15] = 0xF6;
	para->G.x[14] = 0x6D;
	para->G.x[13] = 0x2F;
	para->G.x[12] = 0x4E;
	para->G.x[11] = 0x12;
	para->G.x[10] = 0xE1;
	para->G.x[9] = 0x70;
	para->G.x[8] = 0xCA;
	para->G.x[7] = 0x3E;
	para->G.x[6] = 0x42;
	para->G.x[5] = 0x9F;
	para->G.x[4] = 0x6A;
	para->G.x[3] = 0x06;
	para->G.x[2] = 0x43;
	para->G.x[1] = 0x3B;
	para->G.x[0] = 0x22;

	para->G.y[24] = 0x1B;
	para->G.y[23] = 0xEC;
	para->G.y[22] = 0xA5;
	para->G.y[21] = 0x03;
	para->G.y[20] = 0x68;
	para->G.y[18] = 0x40;
	para->G.y[17] = 0x3F;
	para->G.y[16] = 0x3D;
	para->G.y[15] = 0x13;
	para->G.y[14] = 0x17;
	para->G.y[13] = 0x39;
	para->G.y[12] = 0x68;
	para->G.y[11] = 0x08;
	para->G.y[10] = 0x2B;
	para->G.y[9] = 0x03;
	para->G.y[8] = 0x53;
	para->G.y[7] = 0x97;
	para->G.y[6] = 0xC7;
	para->G.y[5] = 0x78;
	para->G.y[4] = 0x30;
	para->G.y[3] = 0xA9;
	para->G.y[2] = 0xD9;
	para->G.y[1] = 0x0E;
	para->G.y[0] = 0x5D;

	para->G.z[24] = 0x0;
	para->G.z[23] = 0x2B;
	para->G.z[22] = 0x08;
	para->G.z[21] = 0xF7;
	para->G.z[20] = 0xC0;
	para->G.z[19] = 0xCC;
	para->G.z[18] = 0xAC;
	para->G.z[17] = 0x86;
	para->G.z[16] = 0x15;
	para->G.z[15] = 0x1A;
	para->G.z[14] = 0xA6;
	para->G.z[13] = 0xFE;
	para->G.z[12] = 0xCA;
	para->G.z[11] = 0xBD;
	para->G.z[10] = 0xD2;
	para->G.z[9] = 0xD0;
	para->G.z[8] = 0x52;
	para->G.z[7] = 0xBD;
	para->G.z[6] = 0x60;
	para->G.z[5] = 0x92;
	para->G.z[4] = 0x4F;
	para->G.z[3] = 0x28;
	para->G.z[2] = 0xA6;
	para->G.z[1] = 0xA7;
	para->G.z[0] = 0x8E;

	para->r[24] = 0x0;
	para->r[23] = 0x15;
	para->r[22] = 0x55;
	para->r[21] = 0x55;
	para->r[20] = 0x55;
	para->r[19] = 0x55;
	para->r[18] = 0x55;
	para->r[17] = 0x55;
	para->r[16] = 0x55;
	para->r[15] = 0x55;
	para->r[14] = 0x55;
	para->r[13] = 0x55;
	para->r[12] = 0x55;
	para->r[11] = 0x6D;
	para->r[10] = 0x41;
	para->r[9] = 0x71;
	para->r[8] = 0x3B;
	para->r[7] = 0x3E;
	para->r[6] = 0xFD;
	para->r[5] = 0xF5;
	para->r[4] = 0xFF;
	para->r[3] = 0x14;
	para->r[2] = 0xBC;
	para->r[1] = 0x4D;
	para->r[0] = 0x4B;

 /* EIGHT_BIT_PROCESSOR */
#elif defined(SIXTEEN_BIT_PROCESSOR)

    //init parameters
    //prime
    memset(para->p, 0, NUMWORDS*NN_DIGIT_LEN);
    para->p[11] = 0xFFFF;
    para->p[10] = 0xFFFF;
    para->p[9] = 0xFFFF;
    para->p[8] = 0xFFFF;
    para->p[7] = 0xFFFF;
    para->p[6] = 0xFFFF;
    para->p[5] = 0xFFFF;
    para->p[4] = 0xFFFE;
    para->p[3] = 0xFFFF;
    para->p[2] = 0xFFFF;
    para->p[1] = 0xFFFF;
    para->p[0] = 0xFFFF;

    memset(para->omega, 0, NUMWORDS*NN_DIGIT_LEN);
    para->omega[0] = 0x0001;
    para->omega[4] = 0x0001;
    
    
    //c
    memset(para->E.c, 0, NUMWORDS*NN_DIGIT_LEN);
    para->E.c[11] = 0x16A4;
    para->E.c[10] = 0xC7C2;
    para->E.c[9] = 0x030F;
    para->E.c[8] = 0xAD13;
    para->E.c[7] = 0x80AB;
    para->E.c[6] = 0xF8C2;
    para->E.c[5] = 0xD47D;
    para->E.c[4] = 0xC3E0;
    para->E.c[3] = 0xC20A;
    para->E.c[2] = 0xF62F;
    para->E.c[1] = 0x6EDD;
    para->E.c[0] = 0x06A7;
	
    //x
    memset(para->G.x, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.x[11] = 0x52FD;
    para->G.x[10] = 0x0CE7;
    para->G.x[9] = 0x8D06;
    para->G.x[8] = 0x51B4;
    para->G.x[7] = 0xF66D;
    para->G.x[6] = 0x2F4E;
    para->G.x[5] = 0x12E1;
    para->G.x[4] = 0x70CA;
    para->G.x[3] = 0x3E42;
    para->G.x[2] = 0x9F6A;
    para->G.x[1] = 0x0643;
    para->G.x[0] = 0x3B22;
    
    //y
    memset(para->G.y, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.y[11] = 0x1BEC;
    para->G.y[10] = 0xA503;
    para->G.y[9] = 0x6840;
    para->G.y[8] = 0x3F3D;
    para->G.y[7] = 0x1317;
    para->G.y[6] = 0x3968;
    para->G.y[5] = 0x082B;
    para->G.y[4] = 0x0353;
    para->G.y[3] = 0x97C7;
    para->G.y[2] = 0x7830;
    para->G.y[1] = 0xA9D9;
    para->G.y[0] = 0x0E5D;
	
	
    //z
    memset(para->G.z, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.z[11] = 0x2B08;
    para->G.z[10] = 0xF7C0;
    para->G.z[9] = 0xCCAC;
    para->G.z[8] = 0x8615;
    para->G.z[7] = 0x1AA6;
    para->G.z[6] = 0xFECA;
    para->G.z[5] = 0xBDD2;
    para->G.z[4] = 0xD052;
    para->G.z[3] = 0xBD60;
    para->G.z[2] = 0x924F;
    para->G.z[1] = 0x28A6;
    para->G.z[0] = 0xA78E;
    
    //prime divide the number of points
    memset(para->r, 0, NUMWORDS*NN_DIGIT_LEN);
    para->r[11] = 0x1555;
    para->r[10] = 0x5555;
    para->r[9] = 0x5555;
    para->r[8] = 0x5555;
    para->r[7] = 0x5555;
    para->r[6] = 0x5555;
    para->r[5] = 0x6D41;
    para->r[4] = 0x713B;
    para->r[3] = 0x3EFD;
    para->r[2] = 0xF5FF;
    para->r[1] = 0x14BC;
    para->r[0] = 0x4D4B;
    
       /* SIXTEEN_BIT_PROCESSOR */
#elif defined(THIRTYTWO_BIT_PROCESSOR)

    memset(para->p, 0, NUMWORDS*NN_DIGIT_LEN);
    para->p[5] = 0xFFFFFFFF;
    para->p[4] = 0xFFFFFFFF;
    para->p[3] = 0xFFFFFFFF;
    para->p[2] = 0xFFFFFFFF;
    para->p[1] = 0xFFFFFFFE;
    para->p[0] = 0xFFFFEE37;
    
    memset(para->omega, 0, NUMWORDS*NN_DIGIT_LEN);
    para->omega[0] = 0x000011C9;
    para->omega[1] = 0x00000001;
    
    //base point
    memset(para->G.x, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.x[5] = 0x52FD0CE7;
    para->G.x[4] = 0x8D0651B4;
    para->G.x[3] = 0xF66D2F4E;
    para->G.x[2] = 0x12E170CA;
    para->G.x[1] = 0x3E429F6A;
    para->G.x[0] = 0x06433B22;
    
    memset(para->G.y, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.y[5] = 0x1BECA503;
    para->G.y[4] = 0x68403F3D;
    para->G.y[3] = 0x13173968;
    para->G.y[2] = 0x082B0353;
    para->G.y[1] = 0x97C77830;
    para->G.y[0] = 0xA9D90E5D;
    
    memset(para->G.z, 0, NUMWORDS*NN_DIGIT_LEN);
    para->G.z[5] = 0x2B08F7C0;
    para->G.z[4] = 0xCCAC8615;
    para->G.z[3] = 0x1AA6FECA;
    para->G.z[2] = 0xBDD2D052;
    para->G.z[1] = 0xBD60924F;
    para->G.z[0] = 0x28A6A78E;
    
#endif /* THIRTYTWO_BIT_PROCESSOR */
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

