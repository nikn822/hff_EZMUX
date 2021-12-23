#include "hff_TSL2591_TeensyMux.h"
#include <stdlib.h>
#define _aAddress 0x29

TSL2591::TSL2591(){
	_initialized = false;
	_integration = TSL2591_INTEGRATIONTIME_100MS;
	_gain = TSL2591_GAIN_MED;
	_sensorID = -1;
	Wire1.begin();
	Wire.begin();
}

void tcaselect1(uint8_t i) {
	if (i > 7) return;

	Wire1.beginTransmission(TCA);
	Wire1.write(1 << i);
	Wire1.endTransmission();  
}

void tcaselect2(uint8_t i) {
	if (i > 7) return;

	Wire.beginTransmission(TCA);
	Wire.write(1 << i);
	Wire.endTransmission();  
}

boolean TSL2591::begin( i2c_bus _iBus) {
	/* Enable I2C */

	
	
	/* Make sure we have the right device */
	uint8_t id = read8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_DEVICE_ID,_iBus);
	if (id != 0x50) {
		return false;
	}
	_initialized = true;

	// Set default integration time and gain
	setTiming(_integration, _iBus);
	setGain(_gain, _iBus);

	// Note: by default, the device is in power down mode on bootup
	disable();

	return true;
}

void TSL2591::enable(i2c_bus _iBus) {
	if (!begin(_iBus)) {
		return;
	}

	// Enable the device by setting the control bit to 0x01
	write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_ENABLE,
		TSL2591_ENABLE_POWERON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN |
		TSL2591_ENABLE_NPIEN, _iBus);
}
void TSL2591::disable(i2c_bus _iBus) {
	if (!_initialized) {
		if (!begin(_iBus)) {
			return;
		}
	}

	// Disable the device by setting the control bit to 0x00
	write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_ENABLE,
		TSL2591_ENABLE_POWEROFF, _iBus);
}
void Adafruit_TSL2591::setGain(tsl2591Gain_t gain, i2c_bus _iBus) {
	if (!_initialized) {
		if (!begin()) {
			return;
		}
	}

	enable();
	_gain = gain;
	write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CONTROL, _integration | _gain);
	disable();
}
void Adafruit_TSL2591::setTiming(tsl2591IntegrationTime_t integration, _iBus) {
	if (!_initialized) {
		if (!begin()) {
			return;
		}
	}

	enable();
	_integration = integration;
	write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CONTROL, _integration | _gain, _iBus);
	disable();
}

uint16_t TSL2591::getLuminosity(uint8_t channel) {
	uint32_t x = getFullLuminosity();

	if (channel == TSL2591_FULLSPECTRUM) {
		// Reads two byte value from channel 0 (visible + infrared)
		return (x & 0xFFFF);
	}
	else if (channel == TSL2591_INFRARED) {
		// Reads two byte value from channel 1 (infrared)
		return (x >> 16);
	}
	else if (channel == TSL2591_VISIBLE) {
		// Reads all and subtracts out just the visible!
		return ((x & 0xFFFF) - (x >> 16));
	}

	// unknown channel!
	return 0;
}

uint32_t TSL2591::getFullLuminosity(row, col) {
	uint32_t x;
	uint16_t y;
	if(row == 1){
		tcaselect1(col);
		if (!begin()) {
			return 0;
		}

    // Enable the device
		enable(WIRE_BUS);

    // Wait x ms for ADC to complete
		for (uint8_t d = 0; d <= _integration; d++) {
			delay(120);
		}
		y = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW, WIRE_BUS);
		x = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW, WIRE_BUS);
		x <<= 16;
		x |= y;
		disable();
	}

	if(row == 2){
		tcaselect1(col);
		if (!begin()) {
			return 0;
		}

    // Enable the device
		enable(WIRE1_BUS);

    // Wait x ms for ADC to complete
		for (uint8_t d = 0; d <= _integration; d++) {
			delay(120);
		}
		y = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW, WIRE1_BUS);
		x = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW, WIRE1_BUS);
		x <<= 16;
		x |= y;
		disable();
	}
	return x;
}

uint16_t TSL2591::read16(uint8_t reg, i2c_bus _iBus) {
	/*uint8_t buffer[2];
	buffer[0] = reg;
	i2c_dev->write_then_read(buffer, 1, buffer, 2);
	return uint16_t(buffer[1]) << 8 | uint16_t(buffer[0]);*/
	uint8_t buffer[2];					// May change type to uint16_t[]
	uint16_t value = 0;
	if (_iBus == WIRE1_BUS)
	{
		Wire1.beginTransmission(_aAddress); /// TO BE MODIFIED? NO
//#if ARDUINO >= 100
//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
//#else
		Wire1.send(reg);
		//#endif
		Wire1.endTransmission();
		// Wire.requestFrom(_aAddress, (byte)1); /// TO BE MODIFIED? NO
		Wire1.requestFrom(_aAddress, (byte)2);
		//#if ARDUINO >= 100
		//		value = Wire.read(); /// TO BE MODIFIED? NO
		//#else
		buffer[0] = Wire1.readByte(); //// MODIFIED RB receive -> readByte
		buffer[1] = Wire1.readByte();

		//#endif

		return uint16_t(buffer[1]) << 8 | uint16_t(buffer[0]);
	}
	else
	{
		Wire.beginTransmission(_aAddress); /// TO BE MODIFIED? NO
		//#if ARDUINO >= 100
		//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
		//#else
		Wire.send(reg);
		//#endif
		Wire.endTransmission();
		// Wire.requestFrom(_aAddress, (byte)1); /// TO BE MODIFIED? NO
		Wire.requestFrom(_aAddress, (byte)2);
		//#if ARDUINO >= 100
		//		value = Wire.read(); /// TO BE MODIFIED? NO
		//#else
		buffer[0] = Wire.readByte(); //// MODIFIED RB receive -> readByte
		buffer[1] = Wire.readByte();

		//#endif

		return uint16_t(buffer[1]) << 8 | uint16_t(buffer[0]);
	}

	uint8_t TSL2591::read8(uint8_t reg, i2c_bus _iBuss) {
	/*uint8_t buffer[1];
	buffer[0] = reg;
	i2c_dev->write_then_read(buffer, 1, buffer, 1);
	return buffer[0];*/
		uint8_t value = 0;

		if (_iBus == WIRE1_BUS)
		{
		Wire1.beginTransmission(_aAddress); /// TO BE MODIFIED? NO
//#if ARDUINO >= 100
//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
//#else
		Wire1.send(reg);
		//#endif
		Wire1.endTransmission();
		Wire1.requestFrom(_aAddress, (byte)1); /// TO BE MODIFIED? NO
//#if ARDUINO >= 100
//		value = Wire.read(); /// TO BE MODIFIED? NO
//#else
		value = Wire1.readByte(); //// MODIFIED RB receive -> readByte
//#endif

		return value;
	}

	else{
		Wire.beginTransmission(_aAddress); /// TO BE MODIFIED? NO
//#if ARDUINO >= 100
//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
//#else
		Wire.send(reg);
		//#endif
		Wire.endTransmission();
		Wire.requestFrom(_aAddress, (byte)1); /// TO BE MODIFIED? NO
//#if ARDUINO >= 100
//		value = Wire.read(); /// TO BE MODIFIED? NO
//#else
		value = Wire.readByte(); //// MODIFIED RB receive -> readByte
//#endif

		return value;
	}
}
void Adafruit_TSL2591::write8(uint8_t reg, uint8_t value, i2c_bus _iBus) {
	/*uint8_t buffer[2];
	buffer[0] = reg;
	buffer[1] = value;
	i2c_dev->write(buffer, 2);*/

	if (_iBus == WIRE1_BUS)
	{
		Wire1.beginTransmission(_aAddress); //// MODIFIED RB
//#if ARDUINO >= 100
//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
//		Wire.write((uint8_t)value); /// TO BE MODIFIED? NO
//#else
		Wire1.send(reg); /// TO BE MODIFIED? NO
		Wire1.send(value); /// TO BE MODIFIED? NO
//#endif
		Wire1.endTransmission(); /// TO BE MODIFIED? NO

		/* ToDo: Check for error! */
		// return true;
	}
	else if (_iBus == WIRE_BUS)
	{
		Wire.beginTransmission(_aAddress); //// MODIFIED RB
//#if ARDUINO >= 100
//		Wire.write((uint8_t)reg); /// TO BE MODIFIED? NO
//		Wire.write((uint8_t)value); /// TO BE MODIFIED? NO
//#else
		Wire.send(reg); /// TO BE MODIFIED? NO
		Wire.send(value); /// TO BE MODIFIED? NO
//#endif
		Wire.endTransmission(); /// TO BE MODIFIED? NO

		/* ToDo: Check for error! */
		// return true;
	}
	else
	{
		Wire2.beginTransmission(_aAddress); //// MODIFIED RB
//#if ARDUINO >= 100
//		Wire1.write((uint8_t)reg); /// TO BE MODIFIED? NO
//		Wire1.write((uint8_t)value); /// TO BE MODIFIED? NO
//#else
		Wire2.send(reg); /// TO BE MODIFIED? NO
		Wire2.send(value); /// TO BE MODIFIED? NO
//#endif
		Wire2.endTransmission(); /// TO BE MODIFIED? NO

		/* ToDo: Check for error! */
		// return true;
	}
}

}
