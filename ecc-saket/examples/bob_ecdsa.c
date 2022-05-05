#include "ecc.h"
#include "ecdsa.h"
#include "contiki.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

#include "messages.h"

#include <stdio.h> /* For printf() */
#include <string.h>
#include "powertrace.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS ( bob_process, "Bob process" );
//PROCESS ( startup_process, "Statup Process" );
AUTOSTART_PROCESSES ( &bob_process );
/*---------------------------------------------------------------------------*/

point_t pbkey_alice;
NN_DIGIT prKey_alice[NUMWORDS];

static struct energy_time last;
static struct energy_time diff;

static void abc_recv ( struct abc_conn *c );
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
/*---------------------------------------------------------------------------*/
static void
abc_recv ( struct abc_conn *c )
{
        msg_header_t * header;
        
        uint16_t data_len;
        int i;
	
        header = ( msg_header_t * ) ( packetbuf_dataptr()-3 );
        data_len = ntoh_uint16 ( &header->data_len );
	printf("\nmessage received\n");
	
		printf ( "\nreceived data:%u\n", header->msg);
		printf ( "\nreceived r:%u\n", header->r);
		printf ( "\nreceived signature:%u\n", header->s);
	        printf ( "\n");
        i = ecdsa_verify ( header->msg, data_len, header->r, header->s, &pbkey_alice );
	printf("\nValue of i:%d\n",i);
     

        if ( i==1 ) 
	{
		printf("\nSignature Verified!\n");
              
               printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
                
        } 
        else 
	{
              printf ( "unverified\n" );
                
        }
}

point_t gen_pubkey( NN_DIGIT *myPrvKey )
{
        point_t pubKey;

        ecc_gen_public_key ( &pubKey, myPrvKey );
	return pubKey;
}


PROCESS_THREAD ( bob_process, ev, data )
{
	static struct etimer et;
        PROCESS_EXITHANDLER ( abc_close ( &abc ); )
        PROCESS_BEGIN();
	powertrace_start(CLOCK_SECOND*5);
	 memset ( prKey_alice, 0, NUMWORDS*NN_DIGIT_LEN );
        prKey_alice[9] = 0x7b01;
        prKey_alice[8] = 0x2db7;
        prKey_alice[7] = 0x681a;
        prKey_alice[6] = 0x3f28;
        prKey_alice[5] = 0xb918;
        prKey_alice[4] = 0x5c8b;
        prKey_alice[3] = 0x2ac5;
        prKey_alice[2] = 0xd528;
        prKey_alice[1] = 0xdecd;
        prKey_alice[0] = 0x52da;

        ecc_init();
	pbkey_alice = gen_pubkey(prKey_alice);
	
	ecdsa_init ( &pbkey_alice );

       

      printf( "signature size %d\n", 2* ( NUMWORDS * NN_DIGIT_LEN ) );
        abc_open ( &abc, 128, &abc_call );
       
        while ( 1 ) 
	{
                
                etimer_set(&et, CLOCK_SECOND* 2+ random_rand()%(CLOCK_SECOND*2));
  		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  		abc_send(&abc);
        }


        PROCESS_END();
}


