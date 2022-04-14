#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <queue.h>
LiquidCrystal_I2C lcd(0x27,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

int led1 = 6;
int led2 = 7;

struct pinRead
{
  int pin;
  float value;
};

QueueHandle_t structQueue;

void TaskReadPin0(void *pvParameters)
{
  (void) pvParameters; 
  for (;;)
  {   
    struct pinRead currentPinRead; 
    currentPinRead.pin = 0; 
    currentPinRead.value = analogRead(A0);                                                      
    xQueueSend(structQueue, &currentPinRead, portMAX_DELAY);
    //xQueueSend(structQueue, &currentPinRead, 0);  
    Serial.println("Read from sensor (A0)"); 
    taskYIELD();
    vTaskDelay(600/portTICK_PERIOD_MS);
  }
}

void TaskReadPin5(void *pvParameters)
{
  (void) pvParameters; 
  for (;;)
  {   
    struct pinRead currentPinRead; 
    currentPinRead.pin = 5; 
    currentPinRead.value = digitalRead(5); 
    xQueueSend(structQueue, &currentPinRead, portMAX_DELAY);
    //xQueueSend(structQueue, &currentPinRead, 0);
    Serial.println("Read from sensor (5)");
    taskYIELD();
    vTaskDelay(600/portTICK_PERIOD_MS); 
  }
}

//Receive task
void TaskReceive(void * pvParameters)
{
  (void) pvParameters; 
  for (;;) 
  {
   float GAS = 0; 
   int Val = 0; 
   struct pinRead currentPinRead; 
   if (xQueueReceive(structQueue, &currentPinRead, portMAX_DELAY) == pdPASS)
   //if (xQueueReceive(structQueue, &currentPinRead, 0) == pdPASS)
    {
      Serial.print("Pin: ");   
      Serial.print(currentPinRead.pin);
      Serial.print(" Value: ");
      Serial.println(currentPinRead.value);
      if(currentPinRead.pin == 0){        
        GAS = currentPinRead.value; 
        lcd.setCursor(0, 0);
        lcd.print("GAS= ");
        lcd.setCursor(5, 0);
        lcd.print(GAS);
        if(GAS>400) {
          lcd.setCursor(12, 0);
          lcd.print("WAN");
          digitalWrite(led1,HIGH);
        }
        else{
          lcd.setCursor(12, 0);
          lcd.print("NOR");
          digitalWrite(led1,LOW);
        }
      }
      if(currentPinRead.pin == 5)
      {
        Val = int(currentPinRead.value);
        lcd.setCursor(0, 1);
        lcd.print("Val= ");
        lcd.setCursor(5, 1);
        lcd.print(Val);
        if(Val == 1)
        {
          lcd.setCursor(8, 1);
          lcd.print("LED OFF");
          digitalWrite(led2,LOW);
        }
        if(Val == 0)
        {
          lcd.setCursor(8, 1);
          lcd.print("LED ON ");
          digitalWrite(led2,HIGH);
        }
      }
    }
    taskYIELD(); // terminate the task and inform schulder about it
    vTaskDelay(250/portTICK_PERIOD_MS);
  }
}
  
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(5,INPUT);
  pinMode(A0,INPUT);
  structQueue = xQueueCreate(15, // Queue length
                sizeof(struct pinRead)); // Queue item size                     
  if(structQueue!=NULL)
  {
    // Create task that consumes the queue if it was created.
    xTaskCreate(TaskReceive,       // Task function
                "Displaydata", // Task name
                150,  // Stack size
                NULL, //Task parameters
                2, // Priority
                NULL); //task handle
    // Create task that publish data in the queue if it was created.
    xTaskCreate(TaskReadPin0, // Task function
                "AnalogReadPin0", // Task name
                150, // Stack size
                NULL, //Task parameters
                1, // Priority
                NULL); //task handle

    xTaskCreate(TaskReadPin5, // Task function
                "DigitalReadPin5", // Task name
                150, // Stack size
                NULL, //Task parameters
                1, // Priority
                NULL); //task handle
  }
  vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:
}

