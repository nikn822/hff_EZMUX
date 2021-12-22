#include <i2c_t3.h>
#include <Adafruit_TSL2591_t3.h>

#define TCA 0x70

/*  Ardunio Sketch to show updated Adafruit TSL2591 library that can communicate through
 *  the updated Teensy I2c Library (i2c_t3) which makes use of the other I2C ports present
 *  on the Teensy 3.6 Board. This sketch also combines the use of the TCA9548A multiplexer 
 *  with the TSL2591 sensors.
                    +----+----+----+----+
                    | 1  | 2  | 3  | 4  |
 TCA9548A_address1  +----+----+----+----+
                    | 5  | 6  | 7  | 8  |
--------------------+----+----+----+----+
                    | 9  | 10 | 11 | 12 |
 TCA9548A_address2  +----+----+----+----+
                    | 13 | 14 | 15 | 16 |
                    +----+----+----+----+
*/

void tcaselect1(uint8_t i) {
  if (i > 7) return;
 
  Wire1.beginTransmission(TCA);
  Wire1.write(1 << i);
  Wire1.endTransmission();  
}

void tcaselect2(uint8_t i) {
  if (i > 7) return;
 
  Wire2.beginTransmission(TCA);
  Wire2.write(1 << i);
  Wire2.endTransmission();  
}

Adafruit_TSL2591 tsl1 = Adafruit_TSL2591(WIRE1_BUS, -1);
Adafruit_TSL2591 tsl2 = Adafruit_TSL2591(WIRE2_BUS, -1);

void simpleRead(Adafruit_TSL2591 tsl)
{
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
}

void advancedRead(Adafruit_TSL2591 tsl)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
}

void configureSensor(Adafruit_TSL2591 tsl)
{
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}


void setup() {
  // Initialize Adafruit TSL2591 i2c interface over Wire1 and Wire2
  Serial.begin(9600);
  Serial.println("TESTING");
  Wire1.begin();
  Wire2.begin();
}
   
void loop() {

  // Read TSL 1
  tcaselect1(7);
  tsl1.begin();
  Serial.println("TSL Sensor #1");
  simpleRead(tsl1);

  // Read TSL 2
  tcaselect1(6);
  tsl1.begin();
  Serial.println("TSL Sensor #2");
  simpleRead(tsl1);

  // Read TSL 3
  tcaselect2(7);
  tsl2.begin();
  Serial.println("TSL Sensor #3");
  simpleRead(tsl2);

  // Read TSL 4
  tcaselect2(6);
  tsl2.begin();
  Serial.println("TSL Sensor #4");
  simpleRead(tsl2);

  Serial.println();

  delay(500);

}
