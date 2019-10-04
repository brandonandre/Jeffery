#include "Utils.h"

#include <stdio.h>
#include "MyRio.h"
#include "NiFpga.h"
#include "I2C.h"
#include <time.h>

/*
 *  Setup I2C on port B
 * return the fpga status
 */
NiFpga_Status Utils::setupI2CB(NiFpga_Session* myrio_session, MyRio_I2c* i2c){

	NiFpga_Status status;

	uint8_t selectReg;

	/*
	 * Initialize the I2C struct with registers from the FPGA personality.
	 */
	i2c->addr = NiFpga_MyRio1900Fpga60_ControlU8_I2CBADDR;//I2CAADDR;
	i2c->cnfg = NiFpga_MyRio1900Fpga60_ControlU8_I2CBCNFG;//I2CBCNFG;
	i2c->cntl = NiFpga_MyRio1900Fpga60_ControlU8_I2CBCNTL;//I2CBCNTL;
	i2c->cntr = NiFpga_MyRio1900Fpga60_ControlU8_I2CBCNTR;//I2CBCNTR;
	i2c->dati = NiFpga_MyRio1900Fpga60_IndicatorU8_I2CBDATI;//I2CBDATI;
	i2c->dato = NiFpga_MyRio1900Fpga60_ControlU8_I2CBDATO;//I2CBDATO;
	i2c->go = NiFpga_MyRio1900Fpga60_ControlBool_I2CBGO;//I2CBGO;
	i2c->stat = NiFpga_MyRio1900Fpga60_IndicatorU8_I2CBSTAT;//I2CBSTAT;

	/*
	 * Enable the I2C functionality on Connector B
	 * Read the value of the SELECTB register.
	 */
	status = NiFpga_ReadU8(*myrio_session, NiFpga_MyRio1900Fpga60_ControlU8_SYSSELECTB, &selectReg);
	MyRio_ReturnValueIfNotSuccess(status, status, "Could not read from the SYSSELECTB register!");

	/*
	 * Set bit7 of the SELECT register to enable the I2C functionality. The
	 * functionality of this bit is specified in the documentation.
	 */
	selectReg = selectReg | (1 << 7);

	/*
	 * Write the updated value of the SELECT register.
	 */
	status = NiFpga_WriteU8(*myrio_session, NiFpga_MyRio1900Fpga60_ControlU8_SYSSELECTB, selectReg);
	MyRio_ReturnValueIfNotSuccess(status, status, "Could not write to the SYSSELECTB register!");

	/*
	 * Set the speed of the I2C block.
	 *
	 * Standard mode (100 kbps) = 213.
	 * Fast mode (400 kbps) = 63.
	 */
	I2c_Counter(i2c, 213);

	 /*
	  * Enable the I2C block.
	  */
	I2c_Configure(i2c, I2c_Enabled);

	return status;
}



/*
 *  Wait for a specified amount of seconds
 */
void Utils::waitFor (unsigned int secs) {
    int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

/*
 *  Wait for a specified amount of microseconds
 */
void Utils::waitForMicro (int micro_seconds) {
    clock_t start_time = clock(); 
    while (clock() < start_time + micro_seconds);
}