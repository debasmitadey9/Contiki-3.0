#include "contiki.h"
#include "energest.h"
#include "lib/random.h"
#include "net/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "powertrace.h"
#include "dev/cc2420-aes.h"
#include <stdio.h> /* For printf() */
#include <string.h>

#define KEYLEN 16
#define MAX_DATALEN 16

PROCESS(bob_receive,"Bob example");
AUTOSTART_PROCESSES(&bob_receive);



static void abc_recv ( struct abc_conn* c );
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
const uint8_t *key[] = {"0x4D92199549E0F2EF009B4160F3582E5528A11A45017F3EF8"};
uint8_t *data = 0x165;

static void abc_recv ( struct abc_conn* c )
{
printf("abc message received '%s' \n",(char *)packetbuf_dataptr());
}

PROCESS_THREAD(bob_receive,ev,data)
{
static struct etimer et;
PROCESS_EXITHANDLER(abc_close(&abc);)
PROCESS_BEGIN();
powertrace_start(CLOCK_SECOND*5);
printf("Ticks per second:%u\n",RTIMER_SECOND);
cc2420_aes_set_key(key,0);
abc_open ( &abc, 128, &abc_call );
while ( 1 ) 
	{
               etimer_set(&et, CLOCK_SECOND*2+random_rand()%(CLOCK_SECOND*2));
		 PROCESS_WAIT_EVENT_UNTIL ( etimer_expired(&et) );
		cc2420_aes_cipher(data,16,0);
		packetbuf_copyfrom("Hello",6);
		abc_send(&abc);
		printf("abc message sent\n");
                printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
        }

        PROCESS_END();
}


