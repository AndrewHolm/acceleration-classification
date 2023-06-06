#include <Arduino_LSM9DS1.h>

#define SAMPLE_RATE_HZ 100
#define SAMPLES_PER_SECOND 100
#define NUM_OF_SECONDS 120
#define NUM_OF_SAMPLES SAMPLES_PER_SECOND * NUM_OF_SECONDS


unsigned long startTime;
static float samples[NUM_OF_SAMPLES];
static int count = 0;

void setup() {
  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("Y\t");



  startTime = millis(); // Set the start time
  delay(10000); // wait 10 seconds 
  Serial.println("Start!");
  
  
}


void loop() {


  if (count < NUM_OF_SAMPLES) { 
    float x, y, z;
    IMU.readAcceleration(x, y, z);
    samples[count++] = z;

    delay(10); // 100Hz
  }
  else{
    Serial.println("Data: ");
    for(int i = 0; i < NUM_OF_SAMPLES; i++){
      //if(i % 50 == 0){
      //  Serial.println("");
     // }
     // else{
      Serial.print(samples[i]);
      Serial.print(" ,");
     // }
    }
    Serial.println("End Data  ");
    delay(1000000000000);
  }
}
