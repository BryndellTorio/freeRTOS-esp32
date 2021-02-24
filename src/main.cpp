#include <Arduino.h>
#include <stdlib.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// User defined variables
static int led_delay = 500;

static const uint8_t buf_len = 20;

void TaskBlink(void *pvParameters)  // This is a task.
{
        /*
           Blink
           Turns on an LED on for one second, then off for one second, repeatedly.

           If you want to know what pin the on-board LED is connected to on your ESP32 model, check
           the Technical Specs of your board.
         */

        // initialize digital LED_BUILTIN on pin 13 as an output.
        pinMode(LED_BUILTIN, OUTPUT);

        while(1) {
                digitalWrite( LED_BUILTIN, HIGH );   // turn the LED on (HIGH is the voltage level)
                vTaskDelay(led_delay / portTICK_PERIOD_MS);  // one tick delay (15ms) in between reads for stability
                digitalWrite( LED_BUILTIN, LOW );    // turn the LED off by making the voltage LOW
                vTaskDelay(led_delay / portTICK_PERIOD_MS);  // one tick delay (15ms) in between reads for stability
        }
}

void TaskReadSerial(void *pvParameters)  // This is a task.
{
        /*
           AnalogReadSerial
           Reads an analog input on pin A3, prints the result to the serial monitor.
           Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
           Attach the center pin of a potentiometer to pin A3, and the outside pins to +5V and ground.

           This example code is in the public domain.
         */

        char c;
        char *tmp;
        uint8_t index;
        char buf[buf_len];
        uint8_t idx = 0;

        memset(buf, 0, sizeof(buf));

        while(1) {
                // print out the value you read:
                if ( Serial.available() > 0 ) {
                        c = Serial.read();
                        index++;
                        buf[index] = c;
                }

                if (c == '\n') {
                        if (index > 2) {
                                for (int i = 1; i < index; i++) {
                                        Serial.print(buf[i]);
                                }
                                Serial.println(); //make the serial monitor go to next line before printing the new input.
                                index = 0; //Reset buffer index.
                                memset(buf, 0, sizeof(buf)); //Reset buffer.
                                c = NULL;
                        } else {
                                index = 0;
                        }
                }

                vTaskDelay(200 / portTICK_PERIOD_MS);

                /* if ( c == '\n' ) { */
                /*         led_delay = atoi(buf); */
                /*         Serial.print("Updated LED delay: "); */
                /*         Serial.println(led_delay); */
                /*         memset(buf, 0, buf_len); */
                /*         idx = 0; */
                /* } else { */
                /*         if (idx < buf_len -1) { */
                /*                 buf[idx] = c; */
                /*                 idx++; */
                /*         } */
                /* } */
        }
}

// the setup function runs once when you press reset or power the board
void setup() {

        // initialize serial communication at 115200 bits per second:
        Serial.begin(115200);
        vTaskDelay( led_delay / portTICK_PERIOD_MS);
        //Serial.println("Input a number to change the LED delay: ");
        Serial.println("Device ready.");

        // Now set up two tasks to run independently.
        xTaskCreatePinnedToCore(
                        TaskBlink
                        ,  "TaskBlink"   // A name just for humans
                        ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
                        ,  NULL
                        ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                        ,  NULL
                        ,  ARDUINO_RUNNING_CORE);

        xTaskCreatePinnedToCore(
                        TaskReadSerial
                        ,  "TaskReadSerial"
                        ,  1024  // Stack size
                        ,  NULL
                        ,  1  // Priority
                        ,  NULL
                        ,  ARDUINO_RUNNING_CORE);

        // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
        // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

