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




static void
test_sec_lvl_6()
{
  uint8_t key[16] = { 0xC0 , 0xC1 , 0xC2 , 0xC3 ,
                      0xC4 , 0xC5 , 0xC6 , 0xC7 ,
                      0xC8 , 0xC9 , 0xCA , 0xCB ,
                      0xCC , 0xCD , 0xCE , 0xCF };
                      
  uint8_t data[5] = { 0x2B , 0xDC , 0x84 , 0x21 , 0x43 };
  printf("Data:%u,%u,%u,%u,%u\n",data[0],data[1],data[2],data[3],data[4]);
  uint8_t nonce[8] = { 0xAC , 0xDE , 0x48 , 0x00 ,
                                         0x00 , 0x00 , 0x00 , 0x01 };
  printf("Testing encryption ... ");
  CCM_STAR.set_key(key);
  ctr(data,0x5,nonce);
  printf("Encrypted data:%u,%u,%u,%u,%u\n",data[0],data[1],data[2],data[3],data[4]);
 /* ctr(data,0x5,nonce);
  printf("Decrypted Data:%u,%u,%u,%u,%u\n",data[0],data[1],data[2],data[3],data[4]);*/
}

/*---------------------------------------------------------------------------*/



PROCESS_THREAD(ccm_star_tests_process, ev, data)
{
  static struct etimer et;
  PROCESS_EXITHANDLER(abc_close(&abc);)
  PROCESS_BEGIN();
  abc_open( &abc, 128, &abc_call );
  
  while(1){
  etimer_set(&et, CLOCK_SECOND* 2+ random_rand()%(CLOCK_SECOND*2));
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  packetbuf_copyfrom("Hello", 13);
  test_sec_lvl_6();
  abc_send(&abc);
  
  printf("Encrypted data sent\n");
  printf("energy cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU) );
  }
  PROCESS_END();
}  
