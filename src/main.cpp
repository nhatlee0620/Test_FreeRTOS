#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <queue.h>
LiquidCrystal_I2C lcd(0x27,16,2); //Khai báo địa chỉ I2C (0x27 or 0x3F) và LCD 16x02

int led1 = 6;
int led2 = 7;
int led3 = 8;
int sensorIn = 5;
int sensorOut = 9;
int visitors = 0;
struct pinRead
{
  int pin;
  float value;
};

QueueHandle_t structQueue;
QueueHandle_t intQueue;

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
    vTaskDelay(350/portTICK_PERIOD_MS);
  }
}

// void TaskReadPin1(void *pvParameters)
// {
//   (void) pvParameters;
  
//   for (;;)
//   {   
//     struct pinRead currentPinRead;  // define a structure of type pinRead 
//     currentPinRead.pin = 1; // assign value '0' to pin element of struct
//     currentPinRead.value = analogRead(A1)*500.0/1024.0; 
//     // Read adc value from A0 channel and store it in value element of struct
//     // 1mV = 1'C
//     xQueueSend(structQueue, &currentPinRead, portMAX_DELAY);//write struct message to queue
//     Serial.println("");
//     Serial.println("TempRead from sensor(A1)");//print on serial monitor
//     taskYIELD(); //terminate the task and inform schulder about it
//     vTaskDelay(350 / portTICK_PERIOD_MS);//make this task sleep in "var" ms
//   }
// }

void SensorTask(void *pvParameters)
{
  for(;;)
  {
    int sensorInState = digitalRead(sensorIn);
    int sensorOutState = digitalRead(sensorOut);
    if(sensorInState == LOW)
    {
      visitors++;
      delay(100);
    }
    if(sensorOutState == LOW && visitors > 0)
    {
      visitors--;
      delay(100);
    }
    xQueueSend(intQueue, &visitors, portMAX_DELAY == pdPASS);
    vTaskDelay(600/portTICK_PERIOD_MS);
  }
}

void Blynk_Light_Task(void *pvParameters)
{
  int receive_Value = 0;
  for(;;)
  {
    if(xQueueReceive(intQueue, &receive_Value, portMAX_DELAY) == pdPASS)
    {
      Serial.println("People: ");
      Serial.println(receive_Value);
      if(receive_Value>0)
      {
        Serial.println("LED 2 ON");
        digitalWrite(led2,HIGH);
      }
      else
      {
        Serial.println("LED 2 OFF");
        digitalWrite(led2,LOW);
      }
    }
    taskYIELD();
    vTaskDelay(350/portTICK_PERIOD_MS);
  }
}
// void TaskReadPin5(void *pvParameters)
// {
//   (void) pvParameters; 
//   for (;;)
//   {   
//     struct pinRead currentPinRead; 
//     currentPinRead.pin = 5; 
//     currentPinRead.value = digitalRead(5); 
//     xQueueSend(structQueue, &currentPinRead, portMAX_DELAY);
//     //xQueueSend(structQueue, &currentPinRead, 0);
//     Serial.println("Read from sensor (5)");
//     taskYIELD();
//     vTaskDelay(350/portTICK_PERIOD_MS); 
//   }
// }

// void TaskReadPin9(void *pvParameters)
// {
//   (void) pvParameters; 
//   for (;;)
//   {   
//     struct pinRead currentPinRead; 
//     currentPinRead.pin = 9; 
//     currentPinRead.value = digitalRead(9); 
//     xQueueSend(structQueue, &currentPinRead, portMAX_DELAY);
//     //xQueueSend(structQueue, &currentPinRead, 0);
//     Serial.println("Read from sensor (9)");
//     taskYIELD();
//     vTaskDelay(350/portTICK_PERIOD_MS); 
//   }
// }

//Receive task
void TaskReceive(void * pvParameters)
{
  (void) pvParameters; 
  for (;;) 
  {
   float GAS = 0; 
   float temp = 0;
  //  int first = 0; 
  //  int second = 0;
   struct pinRead currentPinRead; 
   if (xQueueReceive(structQueue, &currentPinRead, portMAX_DELAY) == pdPASS)
   //if (xQueueReceive(structQueue, &currentPinRead, 0) == pdPASS)
    {
      Serial.print("Pin: ");   
      Serial.print(currentPinRead.pin);
      Serial.print(" Value: ");
      Serial.println(currentPinRead.value);
      if(currentPinRead.pin == 0)
      {        
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
      // if(currentPinRead.pin == 1)
      // {
      //   temp = currentPinRead.value;
      //   Serial.println("TEMP: ");
      //   Serial.print(temp);
      // }
      // if(currentPinRead.pin == 5)
      // {
      //   first = int(currentPinRead.value);
      //   lcd.setCursor(0, 1);
      //   lcd.print("First= ");
      //   lcd.setCursor(5, 1);
      //   lcd.print(first);
      //   if(first == 1)
      //   {
      //     lcd.setCursor(8, 1);
      //     lcd.print("LED OFF");
      //     digitalWrite(led2,LOW);
      //   }
      //   if(first == 0)
      //   {
      //     lcd.setCursor(8, 1);
      //     lcd.print("LED ON ");
      //     digitalWrite(led2,HIGH);
      //   }
      // }
      // if(currentPinRead.pin == 9)
      // {
      //   second = int(currentPinRead.value);
      //   lcd.setCursor(0, 1);
      //   lcd.print("Second= ");
      //   lcd.setCursor(5, 1);
      //   lcd.print(second);
      //   if(second == 1)
      //   {
      //     lcd.setCursor(8, 1);
      //     lcd.print("LED OFF");
      //     digitalWrite(led3,LOW);
      //   }
      //   if(second == 0)
      //   {
      //     lcd.setCursor(8, 1);
      //     lcd.print("LED ON ");
      //     digitalWrite(led3,HIGH);
      //   }
      // }
    }
    taskYIELD(); // terminate the task and inform schulder about it
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
  
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(9,INPUT);
  pinMode(5,INPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  intQueue = xQueueCreate(15, sizeof(int));
  if(intQueue!=NULL)
  {
    xTaskCreate(SensorTask, "Sender", 95, NULL, 1, NULL);
    xTaskCreate(Blynk_Light_Task, "Receiver", 95, NULL, 2, NULL);
  }
  structQueue = xQueueCreate(15, // Queue length
                sizeof(struct pinRead)); // Queue item size                     
  if(structQueue!=NULL)
  {
    // Create task that consumes the queue if it was created.
    xTaskCreate(TaskReceive,       // Task function
                "Displaydata", // Task name
                95,  // Stack size
                NULL, //Task parameters
                2, // Priority
                NULL); //task handle
    // Create task that publish data in the queue if it was created.
    xTaskCreate(TaskReadPin0, // Task function
                "AnalogReadPin0", // Task name
                95, // Stack size
                NULL, //Task parameters
                1, // Priority
                NULL); //task handle
    // xTaskCreate(TaskReadPin1, // Task function
    //             "AnalogReadPin1", // Task name
    //             95, // Stack size
    //             NULL, //Task parameters
    //             1, // Priority
    //             NULL); //task handle

    // xTaskCreate(TaskReadPin5, // Task function
    //             "DigitalReadPin5", // Task name
    //             95, // Stack size
    //             NULL, //Task parameters
    //             1, // Priority
    //             NULL); //task handle
    // xTaskCreate(TaskReadPin9, // Task function
    //             "DigitalReadPin9", // Task name
    //             95, // Stack size
    //             NULL, //Task parameters
    //             1, // Priority
    //             NULL); //task handle
  }
  vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:
}

