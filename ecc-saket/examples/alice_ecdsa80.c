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
( ( uint8_t* ) ( ptr ) ) [ 16 ]= 0x3e ;
( ( uint8_t* ) ( ptr ) ) [ 17 ]= 0x51 ;
( ( uint8_t* ) ( ptr ) ) [ 18 ]= 0x32 ;
( ( uint8_t* ) ( ptr ) ) [ 19 ]= 0xd3 ;
( ( uint8_t* ) ( ptr ) ) [ 20 ]= 0xa5 ;
( ( uint8_t* ) ( ptr ) ) [ 21 ]= 0x2c ;
( ( uint8_t* ) ( ptr ) ) [ 22 ]= 0x45 ;
( ( uint8_t* ) ( ptr ) ) [ 23 ]= 0xd2 ;
( ( uint8_t* ) ( ptr ) ) [ 24 ]= 0x8f ;
( ( uint8_t* ) ( ptr ) ) [ 25 ]= 0xab ;
( ( uint8_t* ) ( ptr ) ) [ 26 ]= 0x5b ;
( ( uint8_t* ) ( ptr ) ) [ 27 ]= 0x6c ;
( ( uint8_t* ) ( ptr ) ) [ 28 ]= 0x85 ;
( ( uint8_t* ) ( ptr ) ) [ 29 ]= 0x62 ;
( ( uint8_t* ) ( ptr ) ) [ 30 ]= 0xdb ;
( ( uint8_t* ) ( ptr ) ) [ 31 ]= 0xe0 ;
( ( uint8_t* ) ( ptr ) ) [ 32 ]= 0x3e ;
( ( uint8_t* ) ( ptr ) ) [ 33 ]= 0x11 ;
( ( uint8_t* ) ( ptr ) ) [ 34 ]= 0x12 ;
( ( uint8_t* ) ( ptr ) ) [ 35 ]= 0xe3 ;
( ( uint8_t* ) ( ptr ) ) [ 36 ]= 0xa5 ;
( ( uint8_t* ) ( ptr ) ) [ 37 ]= 0x6d ;
( ( uint8_t* ) ( ptr ) ) [ 38 ]= 0x36 ;
( ( uint8_t* ) ( ptr ) ) [ 39 ]= 0xd5 ;
( ( uint8_t* ) ( ptr ) ) [ 40 ]= 0x8b ;
( ( uint8_t* ) ( ptr ) ) [ 41 ]= 0x73 ;
( ( uint8_t* ) ( ptr ) ) [ 42 ]= 0x5b ;
( ( uint8_t* ) ( ptr ) ) [ 43 ]= 0x9c ;
( ( uint8_t* ) ( ptr ) ) [ 44 ]= 0x85 ;
( ( uint8_t* ) ( ptr ) ) [ 45 ]= 0x39 ;
( ( uint8_t* ) ( ptr ) ) [ 46 ]= 0x78 ;
( ( uint8_t* ) ( ptr ) ) [ 47 ]= 0xe0 ;
( ( uint8_t* ) ( ptr ) ) [ 48 ]= 0x4e ;
( ( uint8_t* ) ( ptr ) ) [ 49 ]= 0x15 ;
( ( uint8_t* ) ( ptr ) ) [ 50 ]= 0x82 ;
( ( uint8_t* ) ( ptr ) ) [ 51 ]= 0xa3 ;
( ( uint8_t* ) ( ptr ) ) [ 52 ]= 0xd5 ;
( ( uint8_t* ) ( ptr ) ) [ 53 ]= 0x64 ;
( ( uint8_t* ) ( ptr ) ) [ 54 ]= 0x16 ;
( ( uint8_t* ) ( ptr ) ) [ 55 ]= 0x45 ;
( ( uint8_t* ) ( ptr ) ) [ 56 ]= 0x53 ;
( ( uint8_t* ) ( ptr ) ) [ 57 ]= 0x72 ;
( ( uint8_t* ) ( ptr ) ) [ 58 ]= 0x71 ;
( ( uint8_t* ) ( ptr ) ) [ 59 ]= 0x3d ;
( ( uint8_t* ) ( ptr ) ) [ 60 ]= 0x85 ;
( ( uint8_t* ) ( ptr ) ) [ 61 ]= 0x39 ;
( ( uint8_t* ) ( ptr ) ) [ 62 ]= 0x98 ;
( ( uint8_t* ) ( ptr ) ) [ 63 ]= 0x30 ;
( ( uint8_t* ) ( ptr ) ) [ 64 ]= 0x4b ;
( ( uint8_t* ) ( ptr ) ) [ 65 ]= 0x15 ;
( ( uint8_t* ) ( ptr ) ) [ 66 ]= 0x42 ;
( ( uint8_t* ) ( ptr ) ) [ 67 ]= 0xa3 ;
( ( uint8_t* ) ( ptr ) ) [ 68 ]= 0x34 ;
( ( uint8_t* ) ( ptr ) ) [ 69 ]= 0x24 ;
( ( uint8_t* ) ( ptr ) ) [ 70 ]= 0x16 ;
( ( uint8_t* ) ( ptr ) ) [ 71 ]= 0x54 ;
( ( uint8_t* ) ( ptr ) ) [ 72 ]= 0x53 ;
( ( uint8_t* ) ( ptr ) ) [ 73 ]= 0x22 ;
( ( uint8_t* ) ( ptr ) ) [ 74 ]= 0xb1 ;
( ( uint8_t* ) ( ptr ) ) [ 75 ]= 0x3d ;
( ( uint8_t* ) ( ptr ) ) [ 76 ]= 0x73 ;
( ( uint8_t* ) ( ptr ) ) [ 77 ]= 0x39 ;
( ( uint8_t* ) ( ptr ) ) [ 78 ]= 0x78 ;
( ( uint8_t* ) ( ptr ) ) [ 79 ]= 0x20 ;
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
