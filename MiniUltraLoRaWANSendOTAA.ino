#include <TheThingsNetwork.h>
#include <AltSoftSerial.h>
#include <LowPower.h>

AltSoftSerial loraSerial;

// Set your AppEUI and AppKey
const char *appEui = "70B3D57Exxxxxxx";
const char *appKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
#define SLEEP_PERIOD 60000
#define BAUD_RATE_LORA 19200
#define BAUD_RATE_DEBUG 115200
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  pinMode(2, INPUT);
  pinMode(4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Reset RN2483/RN2903 for a clean power up
  digitalWrite(4, LOW);
  delay(500);
  digitalWrite(4, HIGH);
  
  loraSerial.begin(BAUD_RATE_LORA);
  debugSerial.begin(BAUD_RATE_DEBUG);
  debugSerial.println(F("-- STATUS"));
  
  // Reset is required to autobaud RN2483 into 19200 bps from the
  // default 57600 bps (autobaud process is called within reset())
  ttn.reset();
  ttn.showStatus();
  debugSerial.println(F("-- JOIN"));
  ttn.join(appEui, appKey);
}

void loop()
{
  unsigned char payload[2];
  unsigned char counter;
  float batteryVoltage;
  int adcReading;
  int voltage;

  digitalWrite(LED_BUILTIN, HIGH);
  debugSerial.println(F("-- LOOP"));
  
  // Discard first inaccurate reading
  adcReading = analogRead(A6);
  adcReading = 0;
  // Perform averaging
  for (counter = 10; counter > 0; counter--)
  {
    adcReading += analogRead(A6);
  }
  adcReading = adcReading/10;
  // Convert to volts
  batteryVoltage = adcReading * (3.3 / 1024.0);
  
  debugSerial.print(F("Battery: "));
  debugSerial.print(batteryVoltage);
  debugSerial.println(F(" V"));
  
  // Pack float into int with 2 decimal point resolution
  voltage = batteryVoltage * 100;
  payload[0] = voltage >> 8;
  payload[1] = voltage;
  digitalWrite(LED_BUILTIN, LOW);
  
  // Send & sleep
  ttn.sendBytes(payload, sizeof(payload));
  ttn.sleep(SLEEP_PERIOD);
  
  // Ensure all debugging message are sent before sleep
  debugSerial.flush();
  // Put IO pins (D8 & D9) used for software serial into low power 
  loraSerial.end();
  // Use RN2483/RN2903 as MCU wake-up source after RN2483/RN2903 sleep period 
  // expires 
  attachInterrupt(digitalPinToInterrupt(2), awake, LOW);
  // Put MCU into sleep mode and to be woken up by RN2483/RN2903
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  loraSerial.begin(BAUD_RATE_LORA);
}

void awake()
{
  detachInterrupt(digitalPinToInterrupt(2));
}
