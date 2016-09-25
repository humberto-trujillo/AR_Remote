 // Implementacion UDP funcionando correctamente
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <MCP3008.h>
// is used in I2Cdev.h
#include "Wire.h"

#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;



// Configuracion de pines SPI
#define CLOCK_PIN 14
#define MISO_PIN 12
#define MOSI_PIN 13
#define CS_PIN 16

MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN); // configureer de MCP3008 constructor.

//Variables que se deben actualizar con el Triguer del Arma
volatile int lastTime = 0;
volatile int triguer = 0;
volatile int countTriguer = 0;
volatile int duration = 0;


volatile int lastTimeR = 0;
volatile int recarga = 0;
volatile int countTriguerR = 0;
volatile int durationR = 0;


const char* ssid = "TitoNetgear";
const char* password = "shocoshavo";

unsigned int localPort = 5001;
unsigned int remotePort1 = 5001;

char packetBuffer[255];
char ReplyBuffer[50];// = "acknowledged";
//String trama = "";

IPAddress ipRemote(192, 168, 1, 2);

WiFiUDP Udp;

void setup(void){
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  attachInterrupt(2, pinChanged, CHANGE);
  attachInterrupt(0, cbackRecargar, CHANGE);
    
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  //Udp.begin(localPort);

  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  accelgyro.initialize();
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}


void loop(void){

  if(duration>50){
    triguer = 1;
    countTriguer++;
    if(countTriguer > 5){
      triguer = 0;
      countTriguer=0;
      duration=0;
    }  
  }

  if(durationR>50){
    recarga = 1;
    countTriguerR++;
    if(countTriguerR > 5){
      recarga = 0;
      countTriguerR=0;
      durationR=0;
    }  
  }
  
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level

  int valx = adc.readADC(0); // lees kanaal 0 van de MCP3008 ADC.
  int valy = adc.readADC(1); // lees kanaal 1 van de MCP3008 ADC.

    // read raw accel/gyro measurements from device
  accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

 
  // send a reply, to the IP address and port that sent us the packet we received
  //Udp.beginPacket(ipRemote, remotePort1);
  
  //trama para envio: x,y,fire,btn

 // String trama = "255,255,0,0";
  String trama = "";
  trama+=valx;
  trama+=",";
  trama+=valy;
  trama+=",";
  
  trama+=triguer;
  trama+=",";
  trama+=recarga;
  trama+=",";
  
  trama+=ax;
  trama+=",";
  trama+=ay;
  trama+=",";
  trama+=az;
  
  trama+=",";
  trama+=gx;
  trama+=",";
  trama+=gy;
  trama+=",";
  trama+=gz;
  
  trama+=",";
  trama+=mx;
  trama+=",";
  trama+=my;
  trama+=",";
  trama+=mz;

  trama.toCharArray(ReplyBuffer, trama.length() + 1);
  
  Udp.beginPacket("192.168.1.2", 13000);
  Udp.write(ReplyBuffer);
  Serial.println(ReplyBuffer);
  Udp.endPacket();

  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(50);
}

void pinChanged()
{
  int time = micros();
  duration = time - lastTime;
  lastTime = time;
}

void cbackRecargar()
{
  int time2 = micros();
  durationR = time2 - lastTimeR;
  lastTimeR = time2;
}
