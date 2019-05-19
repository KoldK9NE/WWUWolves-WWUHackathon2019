#include <AWS_IOT.h>
#include <WiFi.h>

AWS_IOT hornbill;

char WIFI_SSID[]="hackathon2019";
char WIFI_PASSWORD[]="fearlesscoder";
char HOST_ADDRESS[]="aoxfeu3je2zwn-ats.iot.us-west-2.amazonaws.com";
char CLIENT_ID[]= "client id";
char TOPIC_NAME[]= "$aws/things/toaster/shadow/update";

int connectionPin = 2; 
int securityPin = 15;
int motionPin = 18;
int toasterPin = 23;

int val = 0;

#define touch_pin_numer T4
const int gledPin = 5;
const int VALUE_THRESHOLD = 40;
int TOUCH_SENSOR_VALUE;

int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}



void setup() {
    delay(1000); 
    pinMode(gledPin, OUTPUT);
    pinMode(connectionPin, OUTPUT);
    pinMode(toasterPin, OUTPUT);
    pinMode(motionPin, INPUT);
    Serial.begin(115200);
    delay(2000);

    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 5 seconds for connection:
        delay(5000);
    }

    Serial.println("Connected to wifi");

    if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);

        if(0==hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
            digitalWrite(connectionPin, HIGH);
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }

    delay(2000);

}

void loop() {

    val = digitalRead(motionPin);
    if(val != 0)
    {
      Serial.print("High ALERT: Motion Detected\n");
      digitalWrite(securityPin, HIGH);
      delay(500);
    }

    TOUCH_SENSOR_VALUE = touchRead(touch_pin_numer);
    if(TOUCH_SENSOR_VALUE < VALUE_THRESHOLD)
    {
      digitalWrite(gledPin, !digitalRead(gledPin));
    }
    //else
    //{
    //  digitalWrite(gledPin, LOW);
    //}

    if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);

        digitalWrite(toasterPin, HIGH);
        delay(1000);
        digitalWrite(toasterPin, LOW);

    }
    if(tick >= 10)   // publish to topic every 5seconds
    {
        tick=0;
        //sprintf(payload,"{ \"state\": { \"desired\": {\"activate\":1}}}");
        //if(hornbill.publish(TOPIC_NAME,payload) == 0)
        //{        
        //    Serial.print("Publish Message:");
        //    Serial.println(payload);
        //}
        //else
        //{
        //    Serial.println("Publish failed");
        //}
    }  
    vTaskDelay(1000 / portTICK_RATE_MS); 
    tick++;
}
