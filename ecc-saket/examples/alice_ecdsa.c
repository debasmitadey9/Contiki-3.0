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


/*---------------------------------------------------------------------------*/
PROCESS ( alice_ecdsa_process, "Alice Ecdsa process" );
PROCESS ( startup_process, "Statup Process" );
AUTOSTART_PROCESSES ( &startup_process );
/*---------------------------------------------------------------------------*/

point_t pbkey_alice;
NN_DIGIT prKey_alice[NUMWORDS];

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

      //  for ( i=0; i<len; i++ ) 
	//{
          //      ( ( uint8_t* ) ( ptr ) ) [i] = random_rand() % 100;
        //}

	( ( uint8_t* ) ( ptr ) ) [ 0 ]= 0x4d;
( ( uint8_t* ) ( ptr ) ) [ 1 ]= 0x72 ;
( ( uint8_t* ) ( ptr ) ) [ 2 ]= 0x45 ;
( ( uint8_t* ) ( ptr ) ) [ 3 ]= 0xe2 ;
( ( uint8_t* ) ( ptr ) ) [ 4 ]= 0xd6 ;
( ( uint8_t* ) ( ptr ) ) [ 5 ]= 0x1c ;
( ( uint8_t* ) ( ptr ) ) [ 6 ]= 0xfc ;
( ( uint8_t* ) ( ptr ) ) [ 7 ]= 0xce ;
( ( uint8_t* ) ( ptr ) ) [ 8 ]= 0x2f ;
( ( uint8_t* ) ( ptr ) ) [ 9 ]= 0xea ;
( ( uint8_t* ) ( ptr ) ) [ 10 ]= 0xfd ;
( ( uint8_t* ) ( ptr ) ) [ 11 ]= 0x7e ;
( ( uint8_t* ) ( ptr ) ) [ 12 ]= 0x68 ;
( ( uint8_t* ) ( ptr ) ) [ 13 ]= 0x7c ;
( ( uint8_t* ) ( ptr ) ) [ 14 ]= 0xdb ;
( ( uint8_t* ) ( ptr ) ) [ 15 ]= 0x0e ;

}


PROCESS_THREAD ( alice_ecdsa_process, ev, data )
{
	
	static struct etimer et;
        PROCESS_EXITHANDLER ( abc_close ( &abc ); )
        PROCESS_BEGIN();
        int i;
        powertrace_start(CLOCK_SECOND*5);
        clock_time_t t1 = clock_time();
  	printf("\nStart Time:%lu \n",t1);
        abc_open ( &abc, 128, &abc_call );
        
       // while ( 1 ) 
	//{
		
               // etimer_set(&et, CLOCK_SECOND* 6+ random_rand()%(CLOCK_SECOND*6));
  		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
              
               msg_header_t* header;
      
        abc_open ( &abc, 128, &abc_call );
        packetbuf_clear();
        header = ( msg_header_t* ) ( packetbuf_dataptr() );
        

        random_data (header->msg, MSG_LEN );
        
        hton_uint16 ( &header->data_len, MSG_LEN );

	//static struct etimer nrg;
	//energest_flush();
       
        ecdsa_sign ( header->msg, MSG_LEN, header->r, header->s, prKey_alice );
        printf("\nSending Signature:%u\n",header->s);
	printf("\nSending r value:%u\n",header->r);
	printf("\nSending MSG_LEN:%u\n",MSG_LEN);
       

        packetbuf_set_datalen ( sizeof ( msg_header_t ) + MSG_LEN );
	packetbuf_copyfrom(header, 8);
	for (i=0;i<MSG_LEN;i++)
	printf("\nMsg Sending:%u\n",header->msg[i]);
        abc_send ( &abc );
	clock_time_t t2 = clock_time();
  printf("\nEnd Time:%lu \n",t2);
	printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
       // }
	//printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
	
        PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD ( startup_process, ev, data )
{
        PROCESS_BEGIN();

        memset ( prKey_alice, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( pbkey_alice.x, 0, NUMWORDS*NN_DIGIT_LEN );
        memset ( pbkey_alice.y, 0, NUMWORDS*NN_DIGIT_LEN );
       // memset ( pbkey_alice.z, 0, NUMWORDS*NN_DIGIT_LEN );

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
	printf("Alice Public Key:%u,%u",pbkey_alice.x,pbkey_alice.y);
	ecdsa_init ( &pbkey_alice );
        process_start ( &alice_ecdsa_process, NULL );
        
        PROCESS_END();
}
