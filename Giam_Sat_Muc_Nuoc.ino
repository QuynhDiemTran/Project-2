
#define BLYNK_TEMPLATE_ID "TMPL6Xnn5Nza7"
#define BLYNK_TEMPLATE_NAME "ESP32"
#define BLYNK_AUTH_TOKEN "m5MVUfAY5vomdLJrT4YW11_Gqk22E2wT"

char ssid[] = "Qq";
char pass[] = "q212002*"; 


int emptyTankDistance = 20 ;  // khoảng cách bể rỗng 
int fullTankDistance =  3 ;  // khoảng cách khi bể đầy 

int triggerPer =   10 ;  
int fullDistance = emptyTankDistance-fullTankDistance;
int R = 4 ;

#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <AceButton.h>
using namespace ace_button; 

// Define connections to sensor,
#define TRIGPIN    27  //D27
#define ECHOPIN    26  //D26
#define wifiLed    2   //D2
#define YellowLed  12  //D12
#define RedLed     14  //D13
#define GreenLed   13  //D14

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3 
#define VPIN_BUTTON_4    V4 
#define VPIN_BUTTON_5    V5 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float  tankVolume ;
float duration;
float distance;
int   waterLevelPer;


char auth[] = BLYNK_AUTH_TOKEN;

//ButtonConfig config1;
//AceButton button1(&config1);

//void handleEvent1(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    //Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_3);
  Blynk.syncVirtual(VPIN_BUTTON_4);
  Blynk.syncVirtual(VPIN_BUTTON_5);

}

void displayData(int value,int b,int c){
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0,1);
  display.print("Percent: ");
  display.print(value);
  display.print(" ");
  display.print("%");

  //display.clearDisplay();
  display.setCursor(0,8);
  display.print("Distance: ");
  display.print(b);
  display.print(" ");
  display.print("cm ");

  display.setCursor(0,16);
  display.print("Volume: ");
  display.print(c);
  display.print(" ");
  display.print("ml ");

  display.display();
}


void measureDistance(){
  // Set the trigger pin LOW for 2uS
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
 
  // Set the trigger pin HIGH for 10us to send pulse
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
 
  // Return the trigger pin to LOW
  digitalWrite(TRIGPIN, LOW);

  duration = pulseIn(ECHOPIN, HIGH);
 
  distance = int (duration / 2/29.412);
  int fullVolume = 854 ;// R*R*3.14*h 

  if (distance > (fullTankDistance - 10)  && distance < emptyTankDistance ){
    
    waterLevelPer = map((int)distance ,emptyTankDistance, fullTankDistance, 0, 100);
    tankVolume = waterLevelPer*0.01*fullVolume;

    displayData(waterLevelPer,distance,tankVolume);
    Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);
    Blynk.virtualWrite(VPIN_BUTTON_2, (String(distance) + " cm"));
    Blynk.virtualWrite(VPIN_BUTTON_3, (String(tankVolume) + " ml"));

    // Print result to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (waterLevelPer < triggerPer){
      digitalWrite(RedLed, HIGH);
      digitalWrite(GreenLed,LOW);
      digitalWrite(YellowLed ,LOW);      
     
    }
    if (distance == fullTankDistance){
      digitalWrite(GreenLed, HIGH);
      digitalWrite(RedLed,LOW);
      digitalWrite(YellowLed,LOW);
    }

    if (distance > fullTankDistance + 2  && waterLevelPer > triggerPer+2 ){
      digitalWrite(YellowLed,HIGH);
      digitalWrite(RedLed,LOW);
      digitalWrite(GreenLed,LOW);
    }   
    
  }
  // Delay before repeating measurement
  delay(100);
}

 
void setup() {
  Serial.begin(115200);
  // Set pinmodes for sensor connections
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(RedLed, OUTPUT);

  pinMode(YellowLed, OUTPUT);

  digitalWrite(wifiLed, LOW);
  digitalWrite(GreenLed, LOW);
  digitalWrite(YellowLed, LOW);
  digitalWrite(RedLed,LOW);
  //config1.setEventHandler(button1Handler);
  
  //button1.init(ButtonPin1);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
 
}
 void loop() {

  measureDistance();

  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  //button1.check();
   
}

