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
  currTime = millis();

  if(counter < 15) {
    arrayOfLastAccels(counter) = AcZ;
  }
  else //if the array is full {
    if(positionInArray == 14) {
      positionInArray = 1;
      arrayOfLastAccels(positionInArray) = AcZ;
    }
    else {
      arrayOfLastAccels(positionInArray) = AcZ;
      positionInArray++;
    }

    stdDeviation = stddev(arrayOfLastAccels, 15); //CHECK

    if(stdDeviation > stdThreshold) {
      if(!isMidstep) {
        prevPeakTime = currPeakTime;
        currPeakTime = maxTime;

        maximum = -4000;
          
      }
      midstep = true;
    }
    else {
      midstep = false;
    }

    if(AcZ > maximum) {
      maximum = AcZ;
      maxTime = currTime;
    }
  }
  
};

bool isFreezing() {
  if(periodIndicatesFreezing && amplitudeIndicatesFreezing())
  { return true; }
}

void runMotor() {

};

void resetAll() {

};

bool periodIndicatesFreezing() {
  if(currAccelTime - prevAccelTime < 0.75*avgStepTime) {
    return true;
  }
  else {
    avgStepTime = (avgStepTime*counter + currAccelTime)/counter;
  }
}

}

