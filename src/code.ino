#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Sim800L.h>
#include <AltSoftSerial.h>
String text;
uint8_t index;

#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
AltSoftSerial  gpsSerial(9, 8);
SoftwareSerial mySerial(4, 3);
TinyGPSPlus gps;


int sensorPin = A0;
float volt;
float ntu;
String cmd = "";
String NTU_LAST=" ";
String TEMP_LAST=" ";
String LOC_LAST=" ";
void setup()
{
  Serial.begin(9600);

 
  
 
  gpsSerial.begin(9600);
   mySerial.begin(9600);

  
  Serial.println("Initializing...");
     delay(1000);

  mySerial.println("AT");                 
  updateSerial();
  mySerial.println("AT+CMGF=1");          
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0");  
  updateSerial();
  
  sensors.begin();
}
void loop()
{






    while (gpsSerial.available() > 0)
    { if (gps.encode(gpsSerial.read())) {
        Serial.println("\n---------------------------");
        displaygpsInfo();
        ntuCalc();
  temp();
        
      }
    }



 
pollSms();
 delay(500);

}
void temp() {
  sensors.requestTemperatures();
  Serial.print("Sıcaklık: ");
  Serial.print(sensors.getTempCByIndex(0));
TEMP_LAST =String("Sicaklik: ")+ String(sensors.getTempCByIndex(0));

}
void ntuCalc() {
  volt = 0;
  for (int i = 0; i < 800; i++)
  {
    volt += ((float)analogRead(sensorPin) / 1023) * 5;
  }
  volt = volt / 800;
  volt = round_to_dp(volt, 2);
  if (volt < 2.5) {
    ntu = 3000;
  } else {
    ntu = -1120.4 * square(volt) + 5742.3 * volt - 4353.8;
  }
  Serial.print("NTU: ");
  Serial.println(ntu);
NTU_LAST =String("NTU: ")+ String(ntu);

}
void displaygpsInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("\nLatitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    LOC_LAST= "Latitude: "+String(gps.location.lat())+ "\nLongitude: "+ String(gps.location.lng())+ "\nAltitude:  " +String(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
    LOC_LAST="Konum beklenmekte.";
  }

}
float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
void updateSerial()
{
  delay(500);
  while (Serial.available())
  {

    cmd += (char)Serial.read();

    if (cmd != "") {
      cmd.trim();
      if (cmd.equals("S")) {
        sendSMS();
      } else {
        mySerial.print(cmd);
        mySerial.println("");
      }
    }
  }

  while (mySerial.available())
  {
    Serial.write(mySerial.read());
  }
}

void sendSMS() {
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.println("AT+CMGS=\"+905077941311\"\r");
  delay(500);
  mySerial.print("Durum \n" + NTU_LAST+"\n" + TEMP_LAST +"\n"+ LOC_LAST);
  delay(500);
  mySerial.write(26);
}
void pollSms()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());
  }
  String mesaj =" ";
  while (mySerial.available())
  {

    
   mesaj+=mySerial.readString();
   
   
  }
  Serial.println(mesaj);
  if(mesaj.indexOf("durum")>0){
    Serial.println("sms alındı");
    sendSMS();
    }
}
