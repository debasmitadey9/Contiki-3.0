#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "lib/ccm-star.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "lib/ccm-star.c"
#include "random.h"
#include "lib/aes-128.h"
#include "energest.h"
/*.............................................................*/
PROCESS(ccm_star_tests_process, "CCM* tests process");
AUTOSTART_PROCESSES(&ccm_star_tests_process);
/*.............................................................*/
static void abc_recv ( struct abc_conn* c );
static void abc_recv( struct abc_conn* c )
{
;
}
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
/*..............................................................*/

PROCESS_THREAD(ccm_star_tests_process, ev, data)
{
  static struct etimer et;
  PROCESS_EXITHANDLER(abc_close(&abc);)
  PROCESS_BEGIN();
  powertrace_start(CLOCK_SECOND*5);
  abc_open( &abc, 128, &abc_call );
  int i;
  while(1){
  etimer_set(&et, CLOCK_SECOND* 2+ random_rand()%(CLOCK_SECOND*2));
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  uint8_t data[16] = { 0x4d, 0x72, 0x45, 0xe2,
  			0xd6, 0x1c, 0xfc, 0xce,
  			0x2f, 0xea, 0xfd, 0x7e,
  			0x68, 0x7c, 0xdb, 0x0e };
  printf("Original Data:\n");
  for (i=0;i<16;i++)
  printf("%u\n",data[i]);
  uint8_t key[16] = { 0xC0 , 0xC1 , 0xC2 , 0xC3 ,
                      0xC4 , 0xC5 , 0xC6 , 0xC7 ,
                      0xC8 , 0xC9 , 0xCA , 0xCB ,
                      0xCC , 0xCD , 0xCE , 0xCF };
                      
  
  
  uint8_t nonce[8] = { 0xAC , 0xDE , 0x48 , 0x00 ,
                                         0x00 , 0x00 , 0x00 , 0x01 };
  printf("\nTesting encryption ... ");
  CCM_STAR.set_key(key);
  ctr(data,0x16,nonce);
  printf("\nEncrypted data");
  for (i=0;i<16;i++)
  printf("%u\n",data[i]);
  printf("\n");
  packetbuf_copyfrom(data, 16);
  abc_send(&abc);
  
  printf("Data sent\n");
  printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
  }
  PROCESS_END();
}  
