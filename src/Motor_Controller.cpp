#include "Motor_Controller.h"
#include <stdio.h>
#include <time.h>
#include "Utils.h"

using namespace std;


Motor_Controller::Motor_Controller()
{
}

/*
 * This initialize the I2C port
 * Must call this function before using any of the motor controller function function
 * return the fpga status
 */
NiFpga_Status Motor_Controller::init(NiFpga_Session* myrio_session){
	return Utils::setupI2CB(myrio_session, &i2c);
}


/*
 * This function enable a I2C device from its address
 * Must enable all device required before use.
 */
void Motor_Controller::controllerEnable(int address){
	uint8_t data[1] = {0x25};
	I2c_Write(&i2c, address, data, 1);
}

/*
 * Reset one of the I2C device
 */
void Motor_Controller::controllerReset(int address){
	uint8_t data[1] = {0x27};
	I2c_Write(&i2c, address, data, 1);
}

/*
 * Return the battery voltage in centivolt
 */
int Motor_Controller::readBatteryVoltage(int address){
	uint8_t data[2] = {0x53};
	I2c_Write(&i2c, address, data, 1);
	I2c_Read(&i2c, address, data, 2);

	return data[0]<<8 | data[1];
}

/*
 *
 */
int Motor_Controller::readFirmware(int address)
{
	uint8_t data[1] = {0x26};
	I2c_Write(&i2c, address, data, 1);

	I2c_Read(&i2c, address, data, 1);

	return data[0];
}

    

/***********************************
 *    _   _ 
 *   | \ /  
 *   |_/ \_ 
 *       
 * DC
************************************/


/*
 * Set the constant speed of DC motor
 * The speed range is 0 to 720 degrees per second
 */
void Motor_Controller::setMotorSpeed(int address, uint8_t channel, long speed)
{

	uint8_t lobyte = speed & 0xff;
	uint8_t hibyte = speed>>8 & 0xff;

	if(channel==DC_1){ channel = 0x43; }
	if(channel==DC_2){ channel = 0x44; }

	uint8_t data[3] = {channel, hibyte, lobyte};
	I2c_Write(&i2c, address, data, 3);
}

/*
 * Set the constant speed of both DC motor
 * The speed range is (+/-) 0 to 720 degrees per second
 */
void Motor_Controller::setMotorSpeeds(int address, long speed1, long speed2)
{

	uint8_t lobyte1 = speed1 & 0xff;
	uint8_t hibyte1 = speed1>>8 & 0xff;

	uint8_t lobyte2 = speed2 & 0xff;
	uint8_t hibyte2 = speed2>>8 & 0xff;


	uint8_t data[5] = {0x45, hibyte1, lobyte1, hibyte2, lobyte2};
	I2c_Write(&i2c, address, data, 5);
}

/*
 *Sets the power level
 * The power range is (+/-) 0 to 100.
 * Stop mode
 *  coast mode -> 0
 *  brake mode -> 125
 */
void Motor_Controller::setMotorPower(int address, uint8_t channel, uint8_t power)
{

	if(channel==DC_1){ channel = 0x40; }
	if(channel==DC_2){ channel = 0x41; }

	printf("%d\n", power);

	uint8_t data[2] = {channel, power};
	I2c_Write(&i2c, address, data, 2);

}


/*
 *Sets the power level of both DC motor
 * The power range is (+/-) 0 to 100.
 * Stop mode
 *  coast mode -> 0
 *  brake mode -> 125
 */
void Motor_Controller::setMotorPowers(int address, uint8_t power1, uint8_t power2)
{
	uint8_t data[3] = {0x42, power1, power2};
	I2c_Write(&i2c, address, data, 3);
}


/*
 * Implements velocity and positional PID control to set the constant speed and the degree target holding position of one DC motor
 * The speed range is (+/-) 0 to 720 degrees per second
 * The encoder degrees target position is a signed long integer from -536,870,912 to 536,870,911 with a 1-degree resolution
 */
void Motor_Controller::setMotorDegree(int address, uint8_t channel, long speed, long degrees)
{

	uint8_t lobyte = speed & 0xff;
	uint8_t hibyte = speed>>8 & 0xff;

	uint8_t four  = (degrees);
	uint8_t three = (degrees>>8);
	uint8_t two   = (degrees>>16);
	uint8_t one   = (degrees>>24);


	if(channel==DC_1){ channel = 0x58; }
	if(channel==DC_2){ channel = 0x59; }

	uint8_t data[7] = {channel, hibyte, lobyte, one, two, three, four};
	I2c_Write(&i2c, address, data, 7);
}
/*
 * Implements velocity and positional PID control to set the constant speed and the degree target holding position of both DC motor
 * The speed range is (+/-) 0 to 720 degrees per second
 * The encoder degrees target position is a signed long integer from -536,870,912 to 536,870,911 with a 1-degree resolution
 */
void Motor_Controller::setMotorDegrees(int address, long speed1, long degrees1, long speed2, long degrees2)
{
	uint8_t lobyte1 = speed1 & 0xff;
	uint8_t hibyte1 = speed1>>8 & 0xff;

	uint8_t lobyte2 = speed2 & 0xff;
	uint8_t hibyte2 = speed2>>8 & 0xff;

	uint8_t four1  = (degrees1);
	uint8_t three1 = (degrees1>>8);
	uint8_t two1   = (degrees1>>16);
	uint8_t one1   = (degrees1>>24);

	uint8_t four2  = (degrees2);
	uint8_t three2 = (degrees2>>8);
	uint8_t two2   = (degrees2>>16);
	uint8_t one2   = (degrees2>>24);

	uint8_t data[13] = {0x5A, hibyte1, lobyte1, one1, two1, three1, four1, hibyte2, lobyte2, one2, two2, three2, four2};
	I2c_Write(&i2c, address, data, 13);

}

/*
 * Implements velocity and positional PID control to set the constant speed and the encoder count target holding position of one DC motor
 * The speed range is (+/-) 0 to 720 degrees per second
 * The encoder count target position is a signed long integer from -2,147,483,648 to 2,147,483,647
 * Each encoder count = 1/4-degree resolution
 */
void Motor_Controller::setMotorTarget(int address, uint8_t channel, long speed, long target)
{

	uint8_t lobyte = speed & 0xff;
	uint8_t hibyte = speed>>8 & 0xff;

	uint8_t four  = (target);
	uint8_t three = (target>>8);
	uint8_t two   = (target>>16);
	uint8_t one   = (target>>24);

	if(channel==DC_1){ channel = 0x46; }
	if(channel==DC_2){ channel = 0x47; }

	uint8_t data[7] = {channel, hibyte, lobyte, one, two, three, four};
	I2c_Write(&i2c, address, data, 7);

}
/*
 * Implements velocity and positional PID control to set the constant speed and the encoder count target holding position of both DC motor
 * The speed range is (+/-) 0 to 720 degrees per second
 * The encoder count target position is a signed long integer from -2,147,483,648 to 2,147,483,647
 * Each encoder count = 1/4-degree resolution
 */
void Motor_Controller::setMotorTargets(int address, long speed1, long target1, long speed2, long target2)
{
	uint8_t lobyte1 = speed1 & 0xff;
	uint8_t hibyte1 = speed1>>8 & 0xff;

	uint8_t lobyte2 = speed2 & 0xff;
	uint8_t hibyte2 = speed2>>8 & 0xff;

	uint8_t four1  = (target1);
	uint8_t three1 = (target1>>8);
	uint8_t two1   = (target1>>16);
	uint8_t one1   = (target1>>24);

	uint8_t four2  = (target2);
	uint8_t three2 = (target2>>8);
	uint8_t two2   = (target2>>16);
	uint8_t one2   = (target2>>24);

	uint8_t data[13] = {0x48, hibyte1, lobyte1, one1, two1, three1, four1, hibyte2, lobyte2, one2, two2, three2, four2};
	I2c_Write(&i2c, address, data, 13);
}

/*
 * Sets the P, I, and D coefficients for constant speed control.
 */
void Motor_Controller::setMotorSpeedPID(int address, int P, int I, int D)
{
	uint8_t lobyteP = P & 0xff;
	uint8_t hibyteP = P>>8 & 0xff;

	uint8_t lobyteI = I & 0xff;
	uint8_t hibyteI = I>>8 & 0xff;

	uint8_t lobyteD = D & 0xff;
	uint8_t hibyteD = D>>8 & 0xff;

	uint8_t data[7] = {0X56, hibyteP, lobyteP, hibyteI, lobyteI, hibyteD, lobyteD };
	I2c_Write(&i2c, address, data, 7);

}

/*
 * Sets the P, I, and D coefficients for target hold position control.
 */
void Motor_Controller::setMotorTargetPID(int address, int P, int I, int D)
{
	uint8_t lobyteP = P & 0xff;
	uint8_t hibyteP = P>>8 & 0xff;

	uint8_t lobyteI = I & 0xff;
	uint8_t hibyteI = I>>8 & 0xff;

	uint8_t lobyteD = D & 0xff;
	uint8_t hibyteD = D>>8 & 0xff;

	uint8_t data[7] = {0X57, hibyteP, lobyteP, hibyteI, lobyteI, hibyteD, lobyteD };
	I2c_Write(&i2c, address, data, 7);
}

/*
 * Invert the default dirrection of one motor 
 */
void Motor_Controller::setMotorInvert(int address, uint8_t channel, uint8_t invert)
{
	if(channel==DC_1){ channel = 0x51; }
	if(channel==DC_2){ channel = 0x52; }

	uint8_t data[3] = {0X57, channel, invert };
	I2c_Write(&i2c, address, data, 2);;
}

/*
 * Reads the encoder count value
 * Each 1/4 degree count, or 1 degree of rotation equals 4 encoder counts
 * The total count accumulation can range from -2,147,483,648 to 2,147,483,647
 * A clockwise rotation adds to the count value, while a counterclockwise rotation subtracts from the count value
 * The encoder values are set to 0 at power-up and reset.
 */
long Motor_Controller::readEncoderCount(int address, uint8_t channel)
{
	if(channel==DC_1){ channel = 0x49; }
	if(channel==DC_2){ channel = 0x4A; }

	uint8_t data[4] = {channel};
	I2c_Write(&i2c, address, data, 1);

	I2c_Read(&i2c, address, data, 4);

	unsigned long eCount = data[0];
	eCount = (eCount*256)+data[1];
	eCount = (eCount*256)+data[2];
	eCount = (eCount*256)+data[3];

	return eCount;
}

/*
 * Reads the encoder degree value. 
 * This function is similar to the encoder count function, but it returns the motor shaft position in degrees. 
 * The total degree count accumulation can range from -536,870,912 to 536,870,911.
 * A clockwise rotation adds to the count value, while a counterclockwise rotation subtracts from the count value. 
 * The encoder values are set to 0 at power-up and reset.
 */
long Motor_Controller::readEncoderDegrees(int address, uint8_t channel)
{
	if(channel==DC_1){ channel = 0x5B; }
	if(channel==DC_2){ channel = 0x5C; }

	uint8_t data[4] = {channel};
	I2c_Write(&i2c, address, data, 1);

	I2c_Read(&i2c, address, data, 4);

	unsigned long eCount = data[0];
	eCount = (eCount*256)+data[1];
	eCount = (eCount*256)+data[2];
	eCount = (eCount*256)+data[3];

	return eCount;
}

/*
 * Reset the encoder count to 0 of one DC motor
 */
void Motor_Controller::resetEncoder(int address, uint8_t channel)
{
	if(channel==DC_1){ channel = 0x4C; }
	if(channel==DC_2){ channel = 0x4D; }

	uint8_t data[1] = {channel};
	I2c_Write(&i2c, address, data, 1);

}

/*
 * Reset the encoder count to 0 of both DC motor
 */
void Motor_Controller::resetEncoders(int address)
{
	uint8_t data[1] = {0x4E};
	I2c_Write(&i2c, address, data, 1);
}

/*
 * Reads the busy flag read to check on the status of a DC motor that is operating in positional PID mode.
 * The motor busy status will return “1” if it is moving toward a positional target (degrees or encoder count). 
 * When it has reached its target and is in hold mode, the busy status will return “0.”
 */
int Motor_Controller::readMotorBusy(int address, uint8_t channel)
{

	if(channel==DC_1){ channel = 0x4F; }
	if(channel==DC_2){ channel = 0x50; }

	uint8_t data[1] = {channel};
	I2c_Write(&i2c, address, data, 1);

	I2c_Read(&i2c, address, data, 1);

	return data[0];

}

/*
 * Reads the DC motor current of each of one DC motor
 * return current in milliamps
 */
int Motor_Controller::readMotorCurrent(int address, uint8_t channel)
{

	if(channel==DC_1){ channel = 0x54; }
	if(channel==DC_2){ channel = 0x55; }

	uint8_t data[2] = {channel};
	I2c_Write(&i2c, address, data, 1);

	I2c_Read(&i2c, address, data, 2);

	int current = data[0];
	current = (current*256)+data[1];

	return current;

}


/************************************
 *    __  _  _        _  
 *   (_  |_ |_) \  / / \ 
 *   __) |_ | \  \/  \_/ 
 *    
 * SERVO                   
 ************************************/

/*
 * Sets the speed of one servo motor
 * The speed parameter can be 0 to 100
 * servo defaults to 100 speed
 */
void Motor_Controller::setServoSpeed(int address, uint8_t channel, uint8_t servospeed){

	if(channel==SERVO_1){ channel = 0x28; }
	if(channel==SERVO_2){ channel = 0x29; }
	if(channel==SERVO_3){ channel = 0x2A; }
	if(channel==SERVO_4){ channel = 0x2B; }
	if(channel==SERVO_5){ channel = 0x2C; }
	if(channel==SERVO_6){ channel = 0x2D; }

	uint8_t data[3] = {channel, servospeed};
	I2c_Write(&i2c, address, data, 2);
}

/*
 * Sets the speed of all six servo motor
 * The speed parameter can be 0 to 100
 * servo defaults to 100 speed
 */
void Motor_Controller::setServoSpeeds(int address, uint8_t servospeed1, uint8_t servospeed2, uint8_t servospeed3, uint8_t servospeed4, uint8_t servospeed5, uint8_t servospeed6){
	uint8_t data[7] = {0x2E, servospeed1, servospeed2, servospeed3, servospeed4, servospeed5, servospeed6};
	I2c_Write(&i2c, address, data, 7);
}


/*
 * Sets the angular position of one servo motor
 * The position range is 0 to 180 degrees
 * Out of range value are ignored.
 */
void Motor_Controller::setServoPosition(int address, uint8_t channel, uint8_t servoposition){

	if(channel==SERVO_1){channel= 0x2F;}
	if(channel==SERVO_2){channel= 0x30;}
	if(channel==SERVO_3){channel= 0x31;}
	if(channel==SERVO_4){channel= 0x32;}
	if(channel==SERVO_5){channel= 0x33;}
	if(channel==SERVO_6){channel= 0x34;}

	uint8_t data[2] = {channel, servoposition};
	I2c_Write(&i2c, address, data, 2);
}

/*
 * Sets the angular position of all six servo motor
 * The position range is 0 to 180 degrees
 * Out of range value are ignored.
 */
void Motor_Controller::setServoPositions(int address, uint8_t servoposition1,uint8_t servoposition2,uint8_t servoposition3,uint8_t servoposition4,uint8_t servoposition5,uint8_t servoposition6){
	uint8_t data[7] = {0x35, servoposition1, servoposition2, servoposition3, servoposition4, servoposition5, servoposition6};
	I2c_Write(&i2c, address, data, 7);
}


/*
Set state of servo motor
 clockwise -> 100
 stopped -> 0
 counterclockwise -> -100
*/
void Motor_Controller::setCRServoState(int address, uint8_t channel, uint8_t servospeed)
{
	if(channel==CR_SERVO_1){channel= 0x36;}
	if(channel==CR_SERVO_2){channel= 0x37;}

	uint8_t data[2] = {channel, servospeed};
	I2c_Write(&i2c, address, data, 2);
}


/*
 * Reads the position of one servo motor 
 * return degrees between 0 and 180
 */
uint8_t Motor_Controller::readServoPosition(int address, uint8_t channel){

	if(channel==SERVO_1){channel= 0x38;}
	if(channel==SERVO_2){channel= 0x39;}
	if(channel==SERVO_3){channel= 0x3A;}
	if(channel==SERVO_4){channel= 0x3B;}
	if(channel==SERVO_5){channel= 0x3C;}
	if(channel==SERVO_6){channel= 0x3D;}

	uint8_t data[1] = {channel};
	I2c_Write(&i2c, address, data, 1);
	I2c_Read(&i2c, address, data, 1);

	return data[0];
}
