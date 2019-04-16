#include "contiki.h"
#include "sys/etimer.h"
#include "button-sensor.h"
#include "batmon-sensor.h"
#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
#define BUF_SIZE 8
static int buffer[BUF_SIZE];
static int buf_c = 0;

static struct etimer et_sensor;

/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Sensor process");
/*---------------------------------------------------------------------------*/

int get_temp_average(void) {
int avg = 0;
for(int i=0; i<BUF_SIZE; i++)
avg += buffer[i];
return avg/BUF_SIZE;
}

/**
 * Executa regras vinculadas aos LED's
 * de acordo com o temperatura do equipamento.
 */
void exec_rules(int temp){
    bool conditionNormalTemp = 21 <= temp && temp <= 23;
    if(conditionNormalTemp){
        leds_off(LEDS_RED);
        leds_off(LEDS_GREEN);
    }else if(temp > 23){
        leds_on(LEDS_RED);
    }else{
        leds_on(LEDS_GREEN);
    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

  leds_init();

  SENSORS_ACTIVATE(batmon_sensor);

  etimer_set(&et_sensor, 1*CLOCK_SECOND); // a cada segundo

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)  // se passou um segundo
    {
        etimer_reset(&et_sensor); // reinicia timer

        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor
        buffer[buf_c++] = val;
        printf("Leu %d\n", val);

        exec_rules(val);
    }
  }

  PROCESS_END();
}
