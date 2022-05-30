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
	clock_time_t t1 = clock_time();
  printf("\nStart Time:%lu \n",t1);
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
     
	printf("Clock system time: %u / %u \n",clock_time(),CLOCK_SECOND);
        if ( i==1 ) 
	{
		printf("\nSignature Verified!\n");
              
               printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
                
        } 
        else 
	{
              printf ( "unverified\n" );
                
        }
        clock_time_t t2 = clock_time();
        printf("\nEnd Time:%lu \n",t2);
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
        prKey_alice[11] = 0xd6d7;
        prKey_alice[10] = 0xd4d5;
        prKey_alice[9] = 0xd2d3;
        prKey_alice[8] = 0xd0d1;
        prKey_alice[7] = 0xcecf;
        prKey_alice[6] = 0xcccd;
        prKey_alice[5] = 0xcacb;
        prKey_alice[4] = 0xc8c9;
        prKey_alice[3] = 0xc6c7;
        prKey_alice[2] = 0xc4c5;
        prKey_alice[1] = 0xc2c3;
        prKey_alice[0] = 0xc0c1;

        ecc_init();
	pbkey_alice = gen_pubkey(prKey_alice);
	
	ecdsa_init ( &pbkey_alice );

       

      printf( "signature size %d\n", 2* ( NUMWORDS * NN_DIGIT_LEN ) );
        abc_open ( &abc, 128, &abc_call );
       
       // while ( 1 ) 
	//{
                
               // etimer_set(&et, CLOCK_SECOND* 2+ random_rand()%(CLOCK_SECOND*2));
  		//PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  		abc_send(&abc);
      //  }


        PROCESS_END();
}

