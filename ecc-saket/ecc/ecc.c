/**
 * \addtogroup ecc
 *
 * @{
 */

/**
 * \file
 * 			Source file for the Elliptic Curve point arithmetic functions.
 * \author
 * 			Kasun Hewage <kasun.ch@gmail.com>, port to Contiki
 *			Vilhelm Jutvik <ville@imorgon.se>, bug fixes
 *			Suraj Neupane <neupanesuraj@gmail.com>, ecies support added
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include "ecc.h"

#define TRUE  1
#define FALSE 0

/**
 * Enable mixed projective coordinate addition.
 */
#define ADD_MIX
/**
 * Enable repeated point doubling.
 */
#define REPEAT_DOUBLE

/*
 * parameters for ECC operations
 */
static curve_params_t param;
/*
 * precomputed array for base point
 */
static point_t pBaseArray[NUM_POINTS];
/*
 * masks for sliding window method
 */
static NN_DIGIT mask[NUM_MASKS];

/*
 * Pseudorandom functions. Not beautiful.
 */
static uint32_t rand32()
{
	// This should be independent of host byte order
	uint32_t rtvl;
	( ( uint16_t* ) &rtvl ) [0] = random_rand();
	( ( uint16_t* ) &rtvl ) [1] = random_rand();

	return rtvl;
}

/**
	* This function exists in ipsec_random.c as well, but we declare it here as well
	* in order to facilitate the stand-alone usage of ContikiECC
	*/
static uint16_t rand16()
{
	uint16_t te = (unsigned short)rand();
	return te;
}

/**
 * \brief             Test whether the ith bit in a is one
 */
static NN_DIGIT
b_testbit ( NN_DIGIT* a, int16_t i )
{

	return ( * ( a + ( i / NN_DIGIT_BITS ) ) & ( ( NN_DIGIT ) 1 << ( i % NN_DIGIT_BITS ) ) );
}
/*---------------------------------------------------------------------------*/
/**
 * \brief             Set P0's x and y to zero
 */
static void
p_clear ( point_t* P0 )
{
	NN_AssignZero ( P0->x, NUMWORDS );
	NN_AssignZero ( P0->y, NUMWORDS );
	NN_AssignZero ( P0->z, NUMWORDS );
}
/*---------------------------------------------------------------------------*/
/**
 * \brief             P0 = P1
 */
static void
p_copy ( point_t* P0, point_t* P1 )
{
	NN_Assign ( P0->x, P1->x, NUMWORDS );
	NN_Assign ( P0->y, P1->y, NUMWORDS );
	NN_Assign ( P0->z, P1->z, NUMWORDS );
}
/*---------------------------------------------------------------------------*/
/**
 * \brief             Test whether x and y of P0 is all zero
 */

/*int ecc_point2octet ( uint8_t *octet, NN_UINT octet_len, point_t *P)
{

        if ( compress ) 
	{
                if ( octet_len < KEYDIGITS*NN_DIGIT_LEN+1 ) 
		{
                        //too small octet
                        return -1;
                } else 
		{
                        //compressed point representation
                        if ( ( 1 & P->y[0] ) == 0 ) 
			{
                                octet[0] = 0x02;
                        } else 
			{
                                octet[0] = 0x03;
                        }
                        NN_Encode ( octet+1, KEYDIGITS*NN_DIGIT_LEN, P->x, KEYDIGITS );
                        return KEYDIGITS*NN_DIGIT_LEN+1;
                }
        } else 
	{ //non compressed
                if ( octet_len < 2*KEYDIGITS*NN_DIGIT_LEN+1 ) 
		{
                        return -1;
                } else {
                        octet[0] = 0x04;
                        NN_Encode ( octet+1, KEYDIGITS*NN_DIGIT_LEN, P->x, KEYDIGITS );
                        NN_Encode ( octet+1+KEYDIGITS*NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN, P->y, KEYDIGITS );
                        NN_Encode ( octet+1+KEYDIGITS*NN_DIGIT_LEN+KEYDIGITS**NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN, P->z, KEYDIGITS );
                        return 3*KEYDIGITS*NN_DIGIT_LEN+1;
                }
        }
}

int ecc_octet2point ( point_t *P, uint8_t *octet, uint16_t octet_len )
{
        NN_DIGIT alpha[NUMWORDS], tmp[NUMWORDS];

        if ( octet[0] == 0 ) { //infinity
                NN_AssignZero ( P->x, NUMWORDS );
                NN_AssignZero ( P->y, NUMWORDS );
                NN_AssignZero ( P->z, NUMWORDS );
        } else if ( octet[0] == 4 ) { //non compressed
                NN_Decode ( P->x, NUMWORDS, octet+1, KEYDIGITS*NN_DIGIT_LEN );
                NN_Decode ( P->y, NUMWORDS, octet+1+KEYDIGITS*NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN );
                NN_Decode ( P->y, NUMWORDS, octet+1+KEYDIGITS*NN_DIGIT_LEN+KEYDIGITS*NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN );
                return 3*KEYDIGITS*NN_DIGIT_LEN+1;
        } else if ( octet[0] == 2 || octet[0] == 3 ) { //compressed form
                NN_Decode ( P->x, NUMWORDS, octet+1, KEYDIGITS*NN_DIGIT_LEN );
                //compute y
                NN_ModSqrOpt ( alpha, P->x, param.p, param.omega, NUMWORDS );
                NN_ModMultOpt ( alpha, alpha, P->x, param.p, param.omega, NUMWORDS );
                NN_ModMultOpt ( tmp, param.E.a, P->x, param.p, param.omega, NUMWORDS );
                NN_ModAdd ( tmp, tmp, alpha, param.p, NUMWORDS );
                NN_ModAdd ( tmp, tmp, param.E.b, param.p, NUMWORDS );
                NN_ModSqrOpt ( P->y, tmp, param.p, NUMWORDS, param.omega );
                if ( octet[0] == 3 ) {
                        NN_ModSub ( P->y, param.p, P->y, param.p, NUMWORDS );
                }
                return KEYDIGITS*NN_DIGIT_LEN+1;
        }
        return -1;
}*/

int
p_iszero ( point_t* P0 )
{
	char result = FALSE;

	if ( NN_Zero ( P0->x, NUMWORDS ) )
	{
		if ( NN_Zero ( P0->y, NUMWORDS ) )
		{
			if ( NN_Zero ( P0->z, NUMWORDS ) )
			{
				result = TRUE;
			}
		}
	}

	return result;
}

int ecc_check_point ( point_t *P )
{
        NN_DIGIT tmp1[NUMWORDS], tmp2[NUMWORDS], tmp3[NUMWORDS];

        if ( NN_Zero ( P->x, NUMWORDS ) )
                return -1;
        if ( NN_Cmp ( P->x, param.p, NUMWORDS ) >= 0 )
                return -1;
        if ( NN_Zero ( P->y, NUMWORDS ) )
                return -1;
        if ( NN_Cmp ( P->y, param.p, NUMWORDS ) >= 0 )
                return -1;
        if ( NN_Zero ( P->z, NUMWORDS ) )
                return -1;
        if ( NN_Cmp ( P->z, param.p, NUMWORDS ) >= 0 )
                return -1;

        memset ( tmp1, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( tmp2, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( tmp3, 0, NUMWORDS*NN_DIGIT_LEN );
        
        //x^3 + y^3 + z^3 = Cxyz
        NN_ModSqrOpt ( tmp2, P->x, param.p, param.omega, NUMWORDS);
        NN_ModMultOpt ( tmp2, tmp2, P->x, param.p, param.omega, NUMWORDS );
        NN_ModSqrOpt ( tmp1, P->y, param.p, param.omega, NUMWORDS);
        NN_ModMultOpt ( tmp1, tmp1, P->y, param.p, param.omega, NUMWORDS );
        NN_ModSqrOpt ( tmp3, P->z, param.p, param.omega, NUMWORDS);
        NN_ModMultOpt ( tmp3, tmp3, P->z, param.p, param.omega, NUMWORDS );
        NN_ModAdd ( tmp1, tmp1, tmp2, param.p, NUMWORDS );
        NN_ModAdd ( tmp1, tmp1, tmp3, param.p, NUMWORDS );
        
        NN_ModMultOpt ( tmp2, P->x, P->y, param.p, param.omega, NUMWORDS );
        NN_ModMultOpt ( tmp2, tmp2, P->z, param.p, param.omega, NUMWORDS );
        NN_ModMultOpt ( tmp2, tmp2, param.E.c, param.p, param.omega, NUMWORDS );
        if ( NN_Cmp ( tmp1, tmp2, NUMWORDS ) != 0 )
                return -2;

        return 1;
        
        
}
/*---------------------------------------------------------------------------*/
/**
 * \brief             Test whether points P1 and P2 are equal
 */
static int
p_equal ( point_t* P1, point_t* P2 )
{
	if ( NN_Equal ( P1->x, P2->x, NUMWORDS ) )
	{
		if ( NN_Equal ( P1->y, P2->y, NUMWORDS ) )
		{
			if ( NN_Equal (P1->z, P1->z, NUMWORDS) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief             Test whether Z is one
 */
/*static int
Z_is_one ( NN_DIGIT* z )
{
	uint8_t i;

	for ( i = 1; i < NUMWORDS; i++ )
	{
		if ( z[i] )
		{
			return FALSE;
		}
	}

	if ( z[0] == 1 )
	{
		return TRUE;
	}

	return FALSE;
}*/
/*---------------------------------------------------------------------------*/

/*static void
c_add_mix ( point_t* P0, NN_DIGIT* Z0, point_t* P1, NN_DIGIT* Z1, point_t* P2 )
{
	NN_DIGIT t1[NUMWORDS];
	NN_DIGIT t2[NUMWORDS];
	NN_DIGIT t3[NUMWORDS];
	NN_DIGIT t4[NUMWORDS];
	NN_DIGIT Z2[NUMWORDS];

	// P2 == infinity 
	if ( NN_Zero ( P2->x, NUMWORDS ) )
	{
		if ( NN_Zero ( P2->y, NUMWORDS ) )
		{
			p_copy ( P0, P1 );
			NN_Assign ( Z0, Z1, NUMWORDS );
			return;
		}
	}

	// P1 == infinity 
	if ( NN_Zero ( Z1, NUMWORDS ) )
	{
		p_copy ( P0, P2 );
		NN_AssignDigit ( Z0, 1, NUMWORDS );
		return;
	}

	// T1 = Z1^2 
	NN_ModSqrOpt ( t1, Z1, param.p, param.omega, NUMWORDS );
	// T2 = T1*Z1 
	NN_ModMultOpt ( t2, t1, Z1, param.p, param.omega, NUMWORDS );
	// T1 = T1*P2->x 
	NN_ModMultOpt ( t1, t1, P2->x, param.p, param.omega, NUMWORDS );
	// T2 = T2*P2->y 
	NN_ModMultOpt ( t2, t2, P2->y, param.p, param.omega, NUMWORDS );
	// T1 = T1-P1->x 
	NN_ModSub ( t1, t1, P1->x, param.p, NUMWORDS );
	// T2 = T2-P1->y 
	NN_ModSub ( t2, t2, P1->y, param.p, NUMWORDS );

	if ( NN_Zero ( t1, NUMWORDS ) )
	{
		if ( NN_Zero ( t2, NUMWORDS ) )
		{
			NN_AssignDigit ( Z2, 1, NUMWORDS );
			ecc_dbl_proj ( P0, Z0, P2, Z2 );
			return;
		}
		else
		{
			NN_AssignDigit ( Z0, 0, NUMWORDS );
			return;
		}
	}

	// Z3 = Z1*T1 
	NN_ModMultOpt ( Z0, Z1, t1, param.p, param.omega, NUMWORDS );
	// T3 = T1^2 
	NN_ModSqrOpt ( t3, t1, param.p, param.omega, NUMWORDS );
	// T4 = T3*T1 
	NN_ModMultOpt ( t4, t3, t1, param.p, param.omega, NUMWORDS );
	// T3 = T3*P1->x 
	NN_ModMultOpt ( t3, t3, P1->x, param.p, param.omega, NUMWORDS );
	// T1 = 2*T3 
	NN_LShift ( t1, t3, 1, NUMWORDS );
	NN_ModSmall ( t1, param.p, NUMWORDS );
	// P0->x = T2^2 
	NN_ModSqrOpt ( P0->x, t2, param.p, param.omega, NUMWORDS );
	// P0->x = P0->x-T1 
	NN_ModSub ( P0->x, P0->x, t1, param.p, NUMWORDS );
	// P0->x = P0->x-T4 
	NN_ModSub ( P0->x, P0->x, t4, param.p, NUMWORDS );
	// T3 = T3-P0->x 
	NN_ModSub ( t3, t3, P0->x, param.p, NUMWORDS );
	// T3 = T3*T2 
	NN_ModMultOpt ( t3, t3, t2, param.p, param.omega, NUMWORDS );
	// T4 = T4*P1->y 
	NN_ModMultOpt ( t4, t4, P1->y, param.p, param.omega, NUMWORDS );
	// P0->y = T3-T4 
	NN_ModSub ( P0->y, t3, t4, param.p, NUMWORDS );

	return;
}*/

/*---------------------------------------------------------------------------*/
void
ecc_init()
{
	int i = KEY_BIT_LEN;
	int j = NN_DIGIT_BITS;
	printf ( "ECC INITIALIZED: key bit len: %u NN_DIGIT_BITS: %u\n", i, j );
	/* get parameters */
	get_curve_param ( &param );
	//printf("Curve Prime number: %u", param.p);
	/**
	 * Window method disabled as for now since it will cause pBaseArray to be
	 * garbage collected by the compiler and thus save memory (albeit at a
	 * cost of CPU).
	 * IMPORTANT: This must be re-enabled if you want to use the ECDSA functions.

		// precompute array for base point*/
	//printf("Curve G value: %u", param.G);	
	ecc_win_precompute(&(param.G), pBaseArray);
}
/*---------------------------------------------------------------------------*/
void
ecc_get_order ( NN_DIGIT* order )
{
	NN_Assign ( order, param.r, NUMWORDS );
}

curve_params_t *
ecc_get_param()
{
	return &param;
}
/*---------------------------------------------------------------------------*/
void
ecc_add ( point_t* P0, point_t* P1, point_t* P2 )
{
	
	//printf("\nEcc_add function called!\n");
	//NN_DIGIT Z0[NUMWORDS];
	//NN_DIGIT Z1[NUMWORDS];
	//NN_DIGIT Z2[NUMWORDS];
	NN_DIGIT A[NUMWORDS];
	NN_DIGIT B[NUMWORDS];

	p_clear ( P0 );
	//NN_AssignZero ( Z0, NUMWORDS );
	//NN_AssignZero ( Z1, NUMWORDS );
	//NN_AssignZero ( Z2, NUMWORDS );
	//Z1[0] = 0x01;
	//Z2[0] = 0x01;


	      //P1 = (x1, y1, z1), P2 = (x2, y2, z2)
	        // x3 = y1^2x2z2 - y2^2x1z1, y3 = x1^2y2z2 - x2^2y1z1, z3 = z1^2y2x2 - z2^2x1y1
	        
	        NN_ModSqrOpt ( A, P0->y, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( A, A, P1->x, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( A, A, P1->z, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSqrOpt ( B, P1->y, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( B, B, P0->x, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( B, B, P0->z, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSub ( P2->x, A, B, param.p, NUMWORDS );
	        
	        
      	        NN_ModSqrOpt ( A, P0->x, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( A, A, P1->y, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( A, A, P1->z, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSqrOpt ( B, P1->x, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( B, B, P0->y, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( B, B, P0->z, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSub ( P2->y, A, B, param.p, NUMWORDS );
	        
      	        NN_ModSqrOpt ( A, P0->z, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( A, A, P1->x, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( A, A, P1->y, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSqrOpt ( B, P1->z, param.p, param.omega,NUMWORDS );
	        NN_ModMultOpt ( B, B, P0->x, param.p, param.omega, NUMWORDS );
      	        NN_ModMultOpt ( B, B, P0->y, param.p, param.omega, NUMWORDS );
	        
	        NN_ModSub ( P2->z, A, B, param.p, NUMWORDS );
	        
		//printf("\nADDED points :%u,%u,%u\n",P2->x,P2->y,P2->z);

	
}
/*---------------------------------------------------------------------------*/
void
ecc_dbl_proj ( point_t* P0, point_t* P1 )
{
	//printf("\nECC_dbl_proj function called!\n");
	NN_DIGIT T1[NUMWORDS];
	NN_DIGIT T2[NUMWORDS];
	NN_DIGIT T3[NUMWORDS];
	
	//if ( NN_Zero ( Z1, NUMWORDS ) )
	//{
	//	NN_AssignZero ( Z0, NUMWORDS );
	//	return;
	//}
	
	//n1
	
	//x3 = y1(z1^3 - x1^3) 
	NN_ModSqrOpt ( T1, P1->z, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T1, T1, P1->z, param.p, param.omega, NUMWORDS );
	NN_ModSqrOpt ( T2, P1->x, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T2, T2, P1->x, param.p, param.omega, NUMWORDS );
	NN_ModSub ( T3, T1, T2, param.p, NUMWORDS );
	NN_ModMultOpt ( P0->x, P1->y, T3, param.p, param.omega, NUMWORDS );
	//y3 = x1(y1^3 - z1^3)
	NN_ModSqrOpt ( T1, P1->y, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T1, T1, P1->y, param.p, param.omega, NUMWORDS );
	NN_ModSqrOpt ( T2, P1->z, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T2, T2, P1->z, param.p, param.omega, NUMWORDS );
	NN_ModSub ( T3, T1, T2, param.p, NUMWORDS );
	NN_ModMultOpt ( P0->y, P1->x, T3, param.p, param.omega, NUMWORDS );
	//z3 = z1(x1^3 - y1^3)
	NN_ModSqrOpt ( T1, P1->x, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T1, T1, P1->x, param.p, param.omega, NUMWORDS );
	NN_ModSqrOpt ( T2, P1->y, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( T2, T2, P1->y, param.p, param.omega, NUMWORDS );
	NN_ModSub ( T3, T1, T2, param.p, NUMWORDS );
	NN_ModMultOpt ( P0->z, P1->z, T3, param.p, param.omega, NUMWORDS );
	
	//printf("Doubled points :%u,%u,%u",P0->x,P0->y,P0->z);
}
/* --------------------------------------------------------------------------*/
/*void
ecc_dbl_proj ( point_t* P0, NN_DIGIT* Z0, point_t* P1, NN_DIGIT* Z1 )
{
	NN_DIGIT n0[NUMWORDS];
	NN_DIGIT n1[NUMWORDS];
	NN_DIGIT n2[NUMWORDS];
	NN_DIGIT n3[NUMWORDS];

	if ( NN_Zero ( Z1, NUMWORDS ) )
	{
		NN_AssignZero ( Z0, NUMWORDS );
		return;
	}

	// n1
	if ( Z_is_one ( Z1 ) )
	{
		//n1 = 3 * P1->x^2 + param.E.a 
		NN_ModSqrOpt ( n0, P1->x, param.p, param.omega, NUMWORDS );
		NN_LShift ( n1, n0, 1, NUMWORDS );
		NN_ModSmall ( n1, param.p, NUMWORDS );
		NN_ModAdd ( n0, n0, n1, param.p, NUMWORDS );
		NN_ModAdd ( n1, n0, param.E.a, param.p, NUMWORDS );
	}
	else
	{
		if ( param.E.a_minus3 )
		{
			// for a = -3
			 // n1 = 3 * (X1 + Z1^2) * (X1 - Z1^2) = 3 * X1^2 - 3 * Z1^4
			 
			NN_ModSqrOpt ( n1, Z1, param.p, param.omega, NUMWORDS );
			NN_ModAdd ( n0, P1->x, n1, param.p, NUMWORDS );
			NN_ModSub ( n2, P1->x, n1, param.p, NUMWORDS );
			NN_ModMultOpt ( n1, n0, n2, param.p, param.omega, NUMWORDS );
			NN_LShift ( n0, n1, 1, NUMWORDS );
			NN_ModSmall ( n0, param.p, NUMWORDS );
			NN_ModAdd ( n1, n0, n1, param.p, NUMWORDS );

		}
		else
			if ( param.E.a_zero )
			{
				// n1 = 3 * P1->x^2 
				NN_ModSqrOpt ( n0, P1->x, param.p, param.omega, NUMWORDS );
				NN_LShift ( n1, n0, 1, NUMWORDS );
				NN_ModSmall ( n1, param.p, NUMWORDS );
				NN_ModAdd ( n1, n0, n1, param.p, NUMWORDS );
			}
			else
			{
				// n1 = 3 * P1->x^2 + param.E.a * Z1^4 
				NN_ModSqrOpt ( n0, P1->x, param.p, param.omega, NUMWORDS );
				NN_LShift ( n1, n0, 1, NUMWORDS );
				NN_ModSmall ( n1, param.p, NUMWORDS );
				NN_ModAdd ( n0, n0, n1, param.p, NUMWORDS );
				NN_ModSqrOpt ( n1, Z1, param.p, param.omega, NUMWORDS );
				NN_ModSqrOpt ( n1, n1, param.p, param.omega, NUMWORDS );
				NN_ModMultOpt ( n1, n1, param.E.a, param.p, param.omega, NUMWORDS );
				NN_ModAdd ( n1, n1, n0, param.p, NUMWORDS );
			}
	}

	// Z0 = 2 * P1->y * Z1 
	if ( Z_is_one ( Z1 ) )
	{
		NN_Assign ( n0, P1->y, NUMWORDS );
	}
	else
	{
		NN_ModMultOpt ( n0, P1->y, Z1, param.p, param.omega, NUMWORDS );
	}

	NN_LShift ( Z0, n0, 1, NUMWORDS );
	NN_ModSmall ( Z0, param.p, NUMWORDS );

	// n2 = 4 * P1->x * P1->y^2 
	NN_ModSqrOpt ( n3, P1->y, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( n2, P1->x, n3, param.p, param.omega, NUMWORDS );
	NN_LShift ( n2, n2, 2, NUMWORDS );
	NN_ModSmall ( n2, param.p, NUMWORDS );

	// P0->x = n1^2 - 2 * n2 
	NN_LShift ( n0, n2, 1, NUMWORDS );
	NN_ModSmall ( n0, param.p, NUMWORDS );
	NN_ModSqrOpt ( P0->x, n1, param.p, param.omega, NUMWORDS );
	NN_ModSub ( P0->x, P0->x, n0, param.p, NUMWORDS );

	// n3 = 8 * P1->y^4 
	NN_ModSqrOpt ( n0, n3, param.p, param.omega, NUMWORDS );
	NN_LShift ( n3, n0, 3, NUMWORDS );
	NN_ModSmall ( n3, param.p, NUMWORDS );

	// P0->y = n1 * (n2 - P0->x) - n3 
	NN_ModSub ( n0, n2, P0->x, param.p, NUMWORDS );
	NN_ModMultOpt ( n0, n1, n0, param.p, param.omega, NUMWORDS );
	NN_ModSub ( P0->y, n0, n3, param.p, NUMWORDS );

}
//---------------------------------------------------------------------------
void
ecc_add_proj ( point_t* P0, NN_DIGIT* Z0, point_t* P1, NN_DIGIT* Z1, point_t* P2, NN_DIGIT* Z2 )
{
	NN_DIGIT n0[NUMWORDS];
	NN_DIGIT n1[NUMWORDS];
	NN_DIGIT n2[NUMWORDS];
	NN_DIGIT n3[NUMWORDS];
	NN_DIGIT n4[NUMWORDS];
	NN_DIGIT n5[NUMWORDS];
	NN_DIGIT n6[NUMWORDS];

	if ( NN_Zero ( Z1, NUMWORDS ) )
	{
		p_copy ( P0, P2 );
		NN_Assign ( Z0, Z2, NUMWORDS );
		return;
	}

	if ( NN_Zero ( Z2, NUMWORDS ) )
	{
		p_copy ( P0, P1 );
		NN_Assign ( Z0, Z1, NUMWORDS );
		return;
	}

	// double 
	if ( p_equal ( P1, P2 ) )
	{
		ecc_dbl_proj ( P0, Z0, P1, Z1 );
		return;
	}

	//add_proj
	 // n1, n2
	 
	if ( Z_is_one ( Z2 ) )
	{
		// n1 = P1->x, n2 = P1->y 
		NN_Assign ( n1, P1->x, NUMWORDS );
		NN_Assign ( n2, P1->y, NUMWORDS );
	}
	else
	{
		// n1 = P1->x * Z2^2 
		NN_ModSqrOpt ( n0, Z2, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( n1, P1->x, n0, param.p, param.omega, NUMWORDS );
		// n2 = P1->y * Z2^3 
		NN_ModMultOpt ( n0, n0, Z2, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( n2, P1->y, n0, param.p, param.omega, NUMWORDS );
	}

	// n3, n4 
	if ( Z_is_one ( Z1 ) )
	{
		// n3 = P2->x, n4 = P2->y 
		NN_Assign ( n3, P2->x, NUMWORDS );
		NN_Assign ( n4, P2->y, NUMWORDS );
	}
	else
	{
		// n3 = P2->x * Z1^2 
		NN_ModSqrOpt ( n0, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( n3, P2->x, n0, param.p, param.omega, NUMWORDS );
		// n4 = P2->y * Z1^3 
		NN_ModMultOpt ( n0, n0, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( n4, P2->y, n0, param.p, param.omega, NUMWORDS );
	}

	// n5 = n1 - n3, n6 = n2 - n4 
	NN_ModSub ( n5, n1, n3, param.p, NUMWORDS );
	NN_ModSub ( n6, n2, n4, param.p, NUMWORDS );

	if ( NN_Zero ( n5, NUMWORDS ) )
	{
		if ( NN_Zero ( n6, NUMWORDS ) )
		{
			// P1 and P2 are same point 
			ecc_dbl_proj ( P0, Z0, P1, Z1 );
			return;
		}
	}
	else
	{
		// P1 is the inverse of P2 
		NN_AssignZero ( Z0, NUMWORDS );
		return;
	}

	// 'n7' = n1 + n3, 'n8' = n2 + n4 
	NN_ModAdd ( n1, n1, n3, param.p, NUMWORDS );
	NN_ModAdd ( n2, n2, n4, param.p, NUMWORDS );

	// Z0 = Z1 * Z2 * n5 
	if ( Z_is_one ( Z1 ) && Z_is_one ( Z2 ) )
	{
		NN_Assign ( Z0, n5, NUMWORDS );
	}
	else
	{
		if ( Z_is_one ( Z1 ) )
		{
			NN_Assign ( n0, Z2, NUMWORDS );
		}
		else
			if ( Z_is_one ( Z2 ) )
			{
				NN_Assign ( n0, Z1, NUMWORDS );
			}
			else
			{
				NN_ModMultOpt ( n0, Z1, Z2, param.p, param.omega, NUMWORDS );
			}

		NN_ModMultOpt ( Z0, n0, n5, param.p, param.omega, NUMWORDS );
	}

	// P0->x = n6^2 - n5^2 * 'n7' 
	NN_ModSqrOpt ( n0, n6, param.p, param.omega, NUMWORDS );
	NN_ModSqrOpt ( n4, n5, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( n3, n1, n4, param.p, param.omega, NUMWORDS );
	NN_ModSub ( P0->x, n0, n3, param.p, NUMWORDS );

	// 'n9' = n5^2 * 'n7' - 2 * P0->x 
	NN_LShift ( n0, P0->x, 1, NUMWORDS );
	NN_ModSmall ( n0, param.p, NUMWORDS );
	NN_ModSub ( n0, n3, n0, param.p, NUMWORDS );

	// P0->y = (n6 * 'n9' - 'n8' * 'n5^3') / 2 
	NN_ModMultOpt ( n0, n0, n6, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( n5, n4, n5, param.p, param.omega, NUMWORDS );
	NN_ModMultOpt ( n1, n2, n5, param.p, param.omega, NUMWORDS );
	NN_ModSub ( n0, n0, n1, param.p, NUMWORDS );

	if ( ( n0[0] % 2 ) == 1 )
	{
		NN_Add ( n0, n0, param.p, NUMWORDS );
	}

	NN_RShift ( P0->y, n0, 1, NUMWORDS );

}*/
/*---------------------------------------------------------------------------*/
void
ecc_win_precompute ( point_t* baseP, point_t* pointArray )
{
	//printf("\necc_win_precompute getting called\n");
	uint8_t i;
	
	uint16_t jj;
	NN_Assign ( pointArray[0].x, baseP->x, NUMWORDS );
	NN_Assign ( pointArray[0].y, baseP->y, NUMWORDS );
	NN_Assign ( pointArray[0].z, baseP->z, NUMWORDS );

	for ( i = 1; i < NUM_POINTS; i++ )
	{
		ecc_add ( & ( pointArray[i] ), & ( pointArray[i-1] ), baseP );
	}

	for ( i = 0; i < NUM_MASKS; i++ )
	{
		//printf("mask %d \n", mask[i]);
		mask[i] = BASIC_MASK << ( W_BITS*i );
	}
}
/*---------------------------------------------------------------------------*/
/*
 * P0 = n * P1
 */
 
void
ecc_mul ( point_t* P0, point_t* P1, NN_DIGIT* n )
{
	//printf("\nEntered ecc_mul\n");
	int16_t i, tmp;
	NN_DIGIT Z0[NUMWORDS];
	NN_DIGIT Z1[NUMWORDS];

	/* clear point */
	p_clear ( P0 );
	for ( i = tmp-1; i >= 0; i-- )
	{
		//printf("\nEntered the for loop\n");
		ecc_dbl_proj( P0, P1 );

		if ( b_testbit ( n, i ) )
		{


			ecc_add ( P0, P0, P1 );

			//ecc_add_proj ( P0, Z0, P0, Z0, P1, Z1 );

		}
	}
	
}

/*void
ecc_mul ( point_t* P0, point_t* P1, NN_DIGIT* n )
{
	int16_t i, tmp;
	NN_DIGIT Z0[NUMWORDS];
	NN_DIGIT Z1[NUMWORDS];

	// clear point 
	p_clear ( P0 );

	// convert to Jprojective coordinate 
	NN_AssignZero ( Z0, NUMWORDS );
	NN_AssignZero ( Z1, NUMWORDS );
	Z1[0] = 0x01;

	tmp = NN_Bits ( n, NUMWORDS );

	for ( i = tmp-1; i >= 0; i-- )
	{
		ecc_dbl_proj ( P0, Z0, P0, Z0 );

		if ( b_testbit ( n, i ) )
		{

#ifdef ADD_MIX
			c_add_mix ( P0, Z0, P0, Z0, P1 );
#else
			ecc_add_proj ( P0, Z0, P0, Z0, P1, Z1 );
#endif
		}
	}

	// convert back to affine coordinate 
	if ( !Z_is_one ( Z0 ) )
	{
		NN_ModInv ( Z1, Z0, param.p, NUMWORDS );
		NN_ModMultOpt ( Z0, Z1, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( Z0, Z0, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS );
	}

}*/
/*---------------------------------------------------------------------------*/
/*void
ecc_m_dbl_projective ( point_t* P0, NN_DIGIT* Z0, uint8_t m )
{
	uint8_t i;
	NN_DIGIT W[NUMWORDS];
	NN_DIGIT A[NUMWORDS];
	NN_DIGIT B[NUMWORDS];
	NN_DIGIT t1[NUMWORDS];
	NN_DIGIT y2[NUMWORDS];

	if ( NN_Zero ( Z0, NUMWORDS ) )
	{
		return;
	}

	// P0->y = 2*P0->y 
	NN_LShift ( P0->y, P0->y, 1, NUMWORDS );
	NN_ModSmall ( P0->y, param.p, NUMWORDS );
	// W = Z^4 
	NN_ModSqrOpt ( W, Z0, param.p, param.omega, NUMWORDS );
	NN_ModSqrOpt ( W, W, param.p, param.omega, NUMWORDS );

	for ( i=0; i<m; i++ )
	{
		if ( param.E.a_minus3 )
		{
			// A = 3(X^2-W) 
			NN_ModSqrOpt ( A, P0->x, param.p, param.omega, NUMWORDS );
			NN_ModSub ( A, A, W, param.p, NUMWORDS );
			NN_LShift ( t1, A, 1, NUMWORDS );
			NN_ModSmall ( t1, param.p, NUMWORDS );
			NN_ModAdd ( A, A, t1, param.p, NUMWORDS );
		}
		else
			if ( param.E.a_zero )
			{
				// A = 3*X^2 
				NN_ModSqrOpt ( t1, P0->x, param.p, param.omega, NUMWORDS );
				NN_LShift ( A, t1, 1, NUMWORDS );
				NN_ModSmall ( A, param.p, NUMWORDS );
				NN_ModAdd ( A, A, t1, param.p, NUMWORDS );
			}
			else
			{
				// A = 3*X^2 + a*W 
				NN_ModSqrOpt ( t1, P0->x, param.p, param.omega, NUMWORDS );
				NN_LShift ( A, t1, 1, NUMWORDS );
				NN_ModSmall ( A, param.p, NUMWORDS );
				NN_ModAdd ( A, A, t1, param.p, NUMWORDS );
				NN_ModMultOpt ( t1, param.E.a, W, param.p, param.omega, NUMWORDS );
				NN_ModAdd ( A, A, t1, param.p, NUMWORDS );
			}

		// B = X*Y^2 
		NN_ModSqrOpt ( y2, P0->y, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( B, P0->x, y2, param.p, param.omega, NUMWORDS );
		// X = A^2 - 2B 
		NN_ModSqrOpt ( P0->x, A, param.p, param.omega, NUMWORDS );
		NN_LShift ( t1, B, 1, NUMWORDS );
		NN_ModSmall ( t1, param.p, NUMWORDS );
		NN_ModSub ( P0->x, P0->x, t1, param.p, NUMWORDS );
		// Z = Z*Y 
		NN_ModMultOpt ( Z0, Z0, P0->y, param.p, param.omega, NUMWORDS );
		NN_ModSqrOpt ( y2, y2, param.p, param.omega, NUMWORDS );

		if ( i < m-1 )
		{
			// W = W*Y^4 
			NN_ModMultOpt ( W, W, y2, param.p, param.omega, NUMWORDS );
		}

		// Y = 2A(B-X)-Y^4 
		NN_LShift ( A, A, 1, NUMWORDS );
		NN_ModSmall ( A, param.p, NUMWORDS );
		NN_ModSub ( B, B, P0->x, param.p, NUMWORDS );
		NN_ModMultOpt ( A, A, B, param.p, param.omega, NUMWORDS );
		NN_ModSub ( P0->y, A, y2, param.p, NUMWORDS );
	}

	if ( ( P0->y[0] % 2 ) == 1 )
	{
		NN_Add ( P0->y, P0->y, param.p, NUMWORDS );
	}

	NN_RShift ( P0->y, P0->y, 1, NUMWORDS );
}*/

/*---------------------------------------------------------------------------*/
/*
 * scalar point multiplication
 * P0 = n*basepoint
 * pointArray is array of basepoint, pointArray[0] = basepoint, pointArray[1] = 2*basepoint ...
 */
 
void
ecc_win_mul ( point_t* P0, NN_DIGIT* n, point_t* pointArray )
{

	int16_t i, tmp;
	int8_t j;
	NN_DIGIT windex;
//#ifndef REPEAT_DOUBLE
	int8_t k;
//#endif

	p_clear ( P0 );
	
	tmp = NN_Digits ( n, NUMWORDS );

	for ( i = tmp - 1; i >= 0; i-- )
	{
		for ( j = NN_DIGIT_BITS/W_BITS - 1; j >= 0; j-- )
		{

//#ifndef REPEAT_DOUBLE

			for ( k = 0; k < W_BITS; k++ )
			{
				ecc_dbl_proj ( P0, P0 );
			}

//#else
			//ecc_dbl_projective ( P0, W_BITS );
//#endif

			windex = mask[j] & n[i];

			if ( windex )
			{
				windex = windex >> ( j*W_BITS );

#ifdef ADD_MIX
				ecc_add ( P0, P0, & ( pointArray[windex-1] ) );
#else
				//ecc_add_proj ( P0, Z0, P0, Z0, & ( pointArray[windex-1] ), Z1 );
#endif
			}
		}
	}

		
}
/*void
ecc_win_mul ( point_t* P0, NN_DIGIT* n, point_t* pointArray )
{

	int16_t i, tmp;
	int8_t j;
	NN_DIGIT windex;
	NN_DIGIT Z0[NUMWORDS];
	NN_DIGIT Z1[NUMWORDS];
#ifndef REPEAT_DOUBLE
	int8_t k;
#endif

	p_clear ( P0 );

	// Convert to Jprojective coordinate 
	NN_AssignZero ( Z0, NUMWORDS );
	NN_AssignZero ( Z1, NUMWORDS );
	Z1[0] = 0x01;

	tmp = NN_Digits ( n, NUMWORDS );

	for ( i = tmp - 1; i >= 0; i-- )
	{
		for ( j = NN_DIGIT_BITS/W_BITS - 1; j >= 0; j-- )
		{

#ifndef REPEAT_DOUBLE

			for ( k = 0; k < W_BITS; k++ )
			{
				ecc_dbl_proj ( P0, Z0, P0, Z0 );
			}

#else
			ecc_m_dbl_projective ( P0, Z0, W_BITS );
#endif

			windex = mask[j] & n[i];

			if ( windex )
			{
				windex = windex >> ( j*W_BITS );

#ifdef ADD_MIX
				c_add_mix ( P0, Z0, P0, Z0, & ( pointArray[windex-1] ) );
#else
				ecc_add_proj ( P0, Z0, P0, Z0, & ( pointArray[windex-1] ), Z1 );
#endif
			}
		}
	}


	// Convert back to affine coordinate 
	if ( !Z_is_one ( Z0 ) )
	{
		NN_ModInv ( Z1, Z0, param.p, NUMWORDS );
		NN_ModMultOpt ( Z0, Z1, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( Z0, Z0, Z1, param.p, param.omega, NUMWORDS );
		NN_ModMultOpt ( P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS );
	}
	
	/*
	int ii;
	for (ii =0; ii < NUMWORDS; ii++)
	{
		printf("base %u p0x %u p0y %u\n", pointArray->x[ii], P0->x[ii], P0->y[ii]);
	}*/
//}

/*---------------------------------------------------------------------------*/
void
ecc_win_mul_base ( point_t* P0, NN_DIGIT* n )
{
	ecc_win_mul ( P0, n, pBaseArray );
}
/*---------------------------------------------------------------------------*/
point_t*
ecc_get_base_p()
{
	return & ( param.G );
}

/*---------------------------------------------------------------------------*/

/**
  * \param PrivateKey Must IKE_DH_SCALAR_CONTIKIECC_LEN bytes long
  */
void ecc_gen_private_key ( NN_DIGIT* PrivateKey )
{
	srand(NULL);
	NN_UINT order_digit_len, order_bit_len;
	bool done = FALSE;
	uint8_t ri;
	NN_DIGIT digit_mask;

	order_bit_len = NN_Bits ( param.r, NUMWORDS );
	order_digit_len = NN_Digits ( param.r, NUMWORDS );

	while ( !done )
	{
		//PrivateKey[] = { 323FA316 9D8E9C65 93F59476 BC142000 AB5BE0E2 49C43426 };
		/*
		PrivateKey[24] = 0x0;
		PrivateKey[23] = 0x32;
		PrivateKey[22] = 0x3F;
		PrivateKey[21] = 0xA3;
		PrivateKey[20] = 0x16;
		PrivateKey[19] = 0x9D;
		PrivateKey[18] = 0x8E;
		PrivateKey[17] = 0x9C;
		PrivateKey[16] = 0x65;
		PrivateKey[15] = 0x93;
		PrivateKey[14] = 0xF5;
		PrivateKey[13] = 0x94;
		PrivateKey[12] = 0x76;
		PrivateKey[11] = 0xBC;
		PrivateKey[10] = 0x14;
		PrivateKey[9] = 0x20;
		PrivateKey[8] = 0x00;
		PrivateKey[7] = 0xAB;
		PrivateKey[6] = 0x5B;
		PrivateKey[5] = 0xE0;
		PrivateKey[4] = 0xE2;
		PrivateKey[3] = 0x49;
		PrivateKey[2] = 0xC4;
		PrivateKey[1] = 0x34;
		PrivateKey[0] = 0x26;
		*/

		for ( ri = 0; ri < order_digit_len; ri++ )
		{
#ifdef THIRTYTWO_BIT_PROCESSOR
			PrivateKey[ri] = rand32();
			//printf("\nChoosing rand32\n");
#else
			PrivateKey[ri] = random_rand();
			//printf("\nChoosing rand16\n");
			//printf("\nPrivateKey[ri]:%u\n",PrivateKey[ri]);
#endif
		}
		

		for ( ri = order_digit_len; ri < NUMWORDS; ri++ )
		{
			PrivateKey[ri] = 0;
		}

		if ( order_bit_len % NN_DIGIT_BITS != 0 )
		{
			digit_mask = MAX_NN_DIGIT >> ( NN_DIGIT_BITS - order_bit_len % NN_DIGIT_BITS );
			PrivateKey[order_digit_len - 1] = PrivateKey[order_digit_len - 1] & digit_mask;
		}

		NN_ModSmall ( PrivateKey, param.r, NUMWORDS );
		printf("\nPrivate key:%u\n",PrivateKey);
		if ( NN_Zero ( PrivateKey, NUMWORDS ) != 1 )
			done = TRUE;
	}
}


/*---------------------------------------------------------------------------*/

void ecc_gen_public_key ( point_t* PublicKey, NN_DIGIT* PrivateKey )
{
//#ifdef SLIDING_WIN
//	win_mul ( PublicKey, PrivateKey, pBaseArray );
//#else
	ecc_mul ( PublicKey, & ( param.G ), PrivateKey );
//#endif
}


/** @} */
