//--------------------------------------------------------------
//--    BNO055
//--    IMU 9DOF Sensor Fusion
//--------------------------------------------------------------
//--    BQ
//--------------------------------------------------------------
//--    Library created by 
//--        Alvaro Ferran (alvaroferran)
//--    based on the work of
//--        Kris Winer (https://github.com/kriswiner/BNO-055)
//--------------------------------------------------------------
//--    Released on September 2015
//--    under the GPL v2
//--------------------------------------------------------------


#include "Arduino.h"
#include "BNO055.h"
#include <Wire.h>  


//PUBLIC

BNO055::BNO055(uint8_t address){
    BNO055_ADDRESS = address;

    GPwrMode = NormalG;    // Gyro power mode
    Gscale = GFS_250DPS;  // Gyro full scale
    //Godr = GODR_250Hz;    // Gyro sample rate
    Gbw = GBW_23Hz;       // Gyro bandwidth
    Ascale = AFS_2G;      // Accel full scale
    //Aodr = AODR_250Hz;    // Accel sample rate
    APwrMode = NormalA;    // Accel power mode
    Abw = ABW_31_25Hz;    // Accel bandwidth, accel sample rate divided by ABW_divx
    //Mscale = MFS_4Gauss;  // Select magnetometer full-scale resolution
    MOpMode = Regular;    // Select magnetometer perfomance mode
    MPwrMode = Normal;    // Select magnetometer power mode
    Modr = MODR_10Hz;     // Select magnetometer ODR when in BNO055 bypass mode
    PWRMode = Normalpwr;    // Select BNO055 power mode
    OPRMode = NDOF;       //
}


void BNO055::init() {
    // Select BNO055 config mode
    writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, CONFIGMODE );
    delay(25);
    // Select page 1 to configure sensors
    writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x01);
    // Configure ACC
    writeByte(BNO055_ADDRESS, BNO055_ACC_CONFIG, APwrMode << 5 | Abw << 3 | Ascale );
    // Configure GYR
    writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_0, Gbw << 3 | Gscale );
    writeByte(BNO055_ADDRESS, BNO055_GYRO_CONFIG_1, GPwrMode);
    // Configure MAG
    writeByte(BNO055_ADDRESS, BNO055_MAG_CONFIG, MPwrMode << 5 | MOpMode << 3 | Modr );
    // Select page 0 to read sensors
    writeByte(BNO055_ADDRESS, BNO055_PAGE_ID, 0x00);
    // Select BNO055 gyro temperature source 
    writeByte(BNO055_ADDRESS, BNO055_TEMP_SOURCE, 0x01 );
    // Select BNO055 sensor units (temperature in degrees C, rate in dps, accel in mg)
    writeByte(BNO055_ADDRESS, BNO055_UNIT_SEL, 0x01 );
    // Select BNO055 system power mode
    writeByte(BNO055_ADDRESS, BNO055_PWR_MODE, PWRMode );
    // Select BNO055 system operation mode
    writeByte(BNO055_ADDRESS, BNO055_OPR_MODE, OPRMode );
    delay(25);
 }


void BNO055::readEul(){
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_EUL_HEADING_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    euler.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    euler.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    euler.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ;
    euler.x= (float)euler.intX/16.;
    euler.y= (float)euler.intY/16.;
    euler.z= (float)euler.intZ/16.;
}


void BNO055::readQuat(){
    uint8_t rawData[8];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_QUA_DATA_W_LSB, 8, &rawData[0]);  // Read the six raw data registers sequentially into data array
    quat.intQ0 = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    quat.intQ1 = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    quat.intQ2 = ((int16_t)rawData[5] << 8) | rawData[4] ;
    quat.intQ3 = ((int16_t)rawData[7] << 8) | rawData[6] ;
    quat.q0= (float)quat.intQ0/16384.;
    quat.q1= (float)quat.intQ1/16384.;
    quat.q2= (float)quat.intQ2/16384.;
    quat.q3= (float)quat.intQ3/16384.;

}


void BNO055::readAccel(){
    uint8_t rawData[6];  // x/y/z accel register data stored here
    readBytes(BNO055_ADDRESS, BNO055_ACC_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers into data array
    accel.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;      // Turn the MSB and LSB into a signed 16-bit value
    accel.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    accel.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ; 
    accel.x = (float)accel.intX;
    accel.y = (float)accel.intY;
    accel.z = (float)accel.intZ;
}


void BNO055::readGyro(){
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_GYR_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    gyro.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    gyro.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    gyro.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ; 
    gyro.x = (float)gyro.intX/16.;
    gyro.y = (float)gyro.intY/16.;
    gyro.z = (float)gyro.intZ/16.;
}


void BNO055::readMag(){
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_MAG_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    mag.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    mag.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    mag.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ;
    mag.x = (float)mag.intX/1.6;
    mag.y = (float)mag.intY/1.6;
    mag.z = (float)mag.intZ/1.6;
}


void BNO055::readLinAcc(){
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_LIA_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    linAcc.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    linAcc.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    linAcc.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ;
    linAcc.x = (float)linAcc.intX;
    linAcc.y = (float)linAcc.intY;
    linAcc.z = (float)linAcc.intZ;
}


void BNO055::readGrav(){
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    readBytes(BNO055_ADDRESS, BNO055_GRV_DATA_X_LSB, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    grav.intX = ((int16_t)rawData[1] << 8) | rawData[0] ;       // Turn the MSB and LSB into a signed 16-bit value
    grav.intY = ((int16_t)rawData[3] << 8) | rawData[2] ;  
    grav.intZ = ((int16_t)rawData[5] << 8) | rawData[4] ;
    grav.x = (float)grav.intX;
    grav.y = (float)grav.intY;
    grav.z = (float)grav.intZ;
}


void BNO055::readTemp(){
    temp.intC=(int16_t)readByte(BNO055_ADDRESS, BNO055_TEMP);  // Read the two raw data registers sequentially into data array 
    temp.c=(float)temp.intC;
    temp.f=(float)temp.c*1.8+32; 
}


//PRIVATE


void BNO055::writeByte(uint8_t address, uint8_t subAddress, uint8_t data){
    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    Wire.write(data);                 // Put data in Tx buffer
    Wire.endTransmission();           // Send the Tx buffer
}



uint8_t BNO055::readByte(uint8_t address, uint8_t subAddress){
    uint8_t data; // `data` will store the register data   
    Wire.beginTransmission(address);         // Initialize the Tx buffer
    Wire.write(subAddress);                  // Put slave register address in Tx buffer
    Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
    Wire.requestFrom(address, (size_t) 1);   // Read one byte from slave register address 
    data = Wire.read();                      // Fill Rx buffer with result
    return data;                             // Return data read from slave register
}



void BNO055::readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest){  
    Wire.beginTransmission(address);   // Initialize the Tx buffer
    Wire.write(subAddress);            // Put slave register address in Tx buffer
    Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t i = 0;
    Wire.requestFrom(address, (size_t) count);  // Read bytes from slave register address 
    while (Wire.available()) {
        dest[i++] = Wire.read(); 
    }         
}