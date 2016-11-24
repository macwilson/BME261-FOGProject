#include <Average.h>
#include <Wire.h>


//IF FREEZING OF GAIT IS DETECTED, LIGHT THE LIGHT ON THE ARDUINO


//*******************GLOBAL VARIABLES************************
// IMU VARIABLES
int mpu = 0x68; //I2C address of IMU
int16_t AcX =0,AcY=0,AcZ=0,Tmp=0,GyX=0,GyY=0,GyZ = 0; //G = gyroscope, A = accelerometer 

// FIND PEAK VARIABLES
double arrayOfLastAccels[15];
int positionInArray = 0;
double stdDeviation = 0;
double currTime = 0;
double stdThreshold = 1000;
bool isMidstep = false;
double maximum = -4000;
double avgStepTime = 1;
double currPeakTime = 0;
double prevPeakTime = 0;
double maxTime = 0;
double prevPeak = 0;
double currPeak = 0;
double currStepTime = 0;
double lastStepTime = 0;

//OTHER
int led = 13;
int counter = 1; //check this (might be 0)



//*******************ARDUINO CONTROL***********************
void setup() {
  initializeIMU();  

  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() {
  getIMUdata();
  getPeakData();
  updateAvgStepTime();
  

  if(isFreezing()) {
    runMotor();
    resetAll();
  }

  
  counter++;
  delay(20); 
}


//*****************OTHER METHODS***************************
// HANDLING IMU
void initializeIMU() {
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B); //PWR_MGMT_1 register
  Wire.write(0); //Wake up MPU
  Wire.endTransmission(true);
};

void getIMUdata() {
  Wire.beginTransmission(mpu); //begin register of accel data
  Wire.write(0x3B);
  Wire.endTransmission(false); //nothing happens until now
  Wire.requestFrom(mpu,14,true);
  AcX = Wire.read() << 8|Wire.read(); //bitshift left | bitwise OR
  AcY = Wire.read() << 8|Wire.read();
  AcZ = Wire.read() << 8|Wire.read();
  Tmp = Wire.read() << 8|Wire.read();
  GyX = Wire.read() << 8|Wire.read();
  GyY = Wire.read() << 8|Wire.read(); //angular velocity ABOUT the y axis
  GyZ = Wire.read() << 8|Wire.read();
};

// FINDING FREEZING
void getPeakData() {

  /*come out with last 2 peak times
   * last 2 peak accelerations 
   * 
   * 
   * 
   */
  currTime = millis()/1000;


//fill up the array initially
  if(counter < 15) {
    arrayOfLastAccels[counter] = AcZ;
  }

  //once the array is full just replace the values
  else //if the array is full 
  {
    if(positionInArray == 14) {
      positionInArray = 1;
    }
    arrayOfLastAccels[positionInArray] = AcZ;
    positionInArray++;
  }

//calculate the standard deviation from the array values
    stdDeviation = Average.stddev(arrayOfLastAccels, 15); //CHECK


//start to detect peaks
    
    if(stdDeviation < stdThreshold) {
      if(!isMidstep) {
        prevPeakTime = currPeakTime;
        currPeakTime = maxTime;
        prevPeak = currPeak;
        currPeak = maximum; 
        maximum = -4000; //reset max to start finding a new one
        currStepTime = currPeakTime - prevPeakTime;
        //avgStepTime = (avgStepTime*(counter-1) + (currPeakTime-prevPeakTime))/counter;
      }
      isMidstep = true;
    }
      
    else {
      isMidstep = false;
      if(AcZ > maximum) {
        maximum = AcZ;
        maxTime = currTime;
      }
    }
  }
  
}

bool isFreezing() {
  if(periodIndicatesFreezing && amplitudeIndicatesFreezing()) { 
    return true; 
  }
}

void runMotor() {

}

void resetAll() {

}

void updateAvgStepTime() {
  if(currStepTime != lastStepTime) {
    avgStepTime = (avgStepTime*(counter-1) + currStepTime)/counter;
    lastStepTime = currStepTime;
  }
}

bool periodIndicatesFreezing() {
  if(currPeakTime - prevPeakTime < 0.75*avgStepTime) {
    return true;
  }
}

bool amplitudeIndicatesFreezing() {
  
}

}

