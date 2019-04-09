#include "contiki.h"
#include "sys/etimer.h"
#include "button-sensor.h"
#include "batmon-sensor.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
#define BUF_SIZE 8
static int buffer[BUF_SIZE];
static int buf_c = 0;

static struct etimer et_sensor;
static struct etimer et_uart;
static struct etimer et_temp;

/*---------------------------------------------------------------------------*/
PROCESS(sensor_process, "Sensor process");
PROCESS(uart_process, "Serial process");
PROCESS(temp_process, "MaxMinTemp Process");
PROCESS(btn_process, "Button Process");

#define PROCESS_EVENT_BTN 0x8e

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&sensor_process, &uart_process, &temp_process); //, &btn_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data)
{
  PROCESS_BEGIN();

//  SENSORS_ACTIVATE(batmon_sensor);
//
//  etimer_set(&et_sensor, 1*CLOCK_SECOND); // a cada segundo
//
//  while(1) {
//    PROCESS_WAIT_EVENT();
//    if(ev == PROCESS_EVENT_TIMER)  // se passou um segundo
//    {
//        etimer_reset(&et_sensor); // reinicia timer
//
//        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP); // lê sensor
//
//        buffer[buf_c++] = val;
//
//
//        printf("Leu %d\n", val);
//    }
//  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(uart_process, ev, data)
{
  PROCESS_BEGIN();

  /* Insira seu código aqui */
  etimer_set(&et_uart, 10*CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)  // se passaram 10 segundos
    {
        etimer_reset(&et_uart); // reinicia timer

        int avg = 0;
        for(int i=0;i<BUF_SIZE;i++) {
            avg += buffer[i];
        }
        avg = avg/BUF_SIZE;
        printf("Temperatura media: %d\n", avg);
    }
  }

  PROCESS_END();
  }

/*
 * ############# MAX & MIN TEMPERATURE ################
 */
PROCESS_THREAD(temp_process, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&et_temp, 20*CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER)
    {
        etimer_reset(&et_temp); // reinicia timer

        int min = 200, max = 0;
        for(int i=0;i<BUF_SIZE;i++) {
            int temp = buffer[i];
            if(temp < min)
                min = temp;
            if(temp > max)
                max = temp;
        }
        printf("Temperatura Max: %d\n", max);
        printf("Temperatura Min: %d\n", min);
    }
  }
  PROCESS_END();
}

PROCESS_THREAD(btn_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(batmon_sensor);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_BTN)
    {
        int val = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);

        buffer[buf_c++] = val;

        printf("Leu %d\n", val);
    }
  }

  PROCESS_END();
}
