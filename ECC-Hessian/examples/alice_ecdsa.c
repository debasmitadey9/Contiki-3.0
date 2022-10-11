#include "ecc.h"
#include "ecdsa.h"
#include "contiki.h"
#include "messages.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h> /* For printf() */
#include <string.h>
#include "powertrace.h"
#include "energest.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif
/*---------------------------------------------------------------------------*/
PROCESS ( alice_ecdsa_process, "Alice Ecdsa process" );
PROCESS ( startup_process, "Statup Process" );
AUTOSTART_PROCESSES ( &startup_process );
/*---------------------------------------------------------------------------*/

point_t pbkey_alice;
NN_DIGIT prKey_alice[NUMWORDS];

static struct energy_time last;
static struct energy_time diff;

static void abc_recv ( struct abc_conn* c );
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
static void bacast_signed_message();

static void abc_recv ( struct abc_conn* c )
{
;
}
//CryptoCC2640_init();

point_t gen_pubkey( NN_DIGIT *myPrvKey )
{
        point_t pubKey;

        ecc_gen_public_key ( &pubKey, myPrvKey );
	return pubKey;
}

/*---------------------------------------------------------------------------*/
static void random_data ( void* ptr, uint16_t len )
{
        uint16_t i;

        for ( i=0; i<len; i++ ) 
	{
                ( ( uint8_t* ) ( ptr ) ) [i] = random_rand() % 100;
        }

}


PROCESS_THREAD ( alice_ecdsa_process, ev, data )
{
	static struct etimer et;
        PROCESS_EXITHANDLER ( abc_close ( &abc ); )
        PROCESS_BEGIN();
        powertrace_start(CLOCK_SECOND*5);
        printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
        abc_open ( &abc, 128, &abc_call );
        
        while ( 1 ) 
	{
		
                etimer_set(&et, CLOCK_SECOND* 2+ random_rand()%(CLOCK_SECOND*2));
  		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
              
               msg_header_t* header;
      
        abc_open ( &abc, 128, &abc_call );
        packetbuf_clear();
        header = ( msg_header_t* ) ( packetbuf_dataptr() );
        

        random_data (header->msg, MSG_LEN );
        
        hton_uint16 ( &header->data_len, MSG_LEN );

	static struct etimer nrg;
	energest_flush();
       
        ecdsa_sign ( header->msg, MSG_LEN, header->r, header->s, prKey_alice );
        printf("\nSending Signature:%u\n",header->s);
	printf("\nSending r value:%u\n",header->r);
	printf("\nSending MSG_LEN:%u\n",MSG_LEN);
       

        packetbuf_set_datalen ( sizeof ( msg_header_t ) + MSG_LEN );
	packetbuf_copyfrom(header, 8);
	printf("\nMsg Sending:%u,%u,%u\n",header->msg[0],header->msg[1],header->msg[2]);
        abc_send ( &abc );
	
	printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
        }

        PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD ( startup_process, ev, data )
{
        PROCESS_BEGIN();

        memset ( prKey_alice, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( pbkey_alice.x, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( pbkey_alice.y, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( pbkey_alice.z, 0, NUMWORDS*NN_DIGIT_LEN );
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
	printf("Alice Public Key:%u,%u,%u",pbkey_alice.x,pbkey_alice.y,pbkey_alice.z);
	ecdsa_init ( &pbkey_alice );
        process_start ( &alice_ecdsa_process, NULL );
        
        PROCESS_END();
}
