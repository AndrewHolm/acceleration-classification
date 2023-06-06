#include <TensorFlowLite.h>
#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include <tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "accel_model2.h" // This is the header file containing the model's data (weights and biases)
#include <vector>
#include <cmath>


//variable required by TFLu
//-----------------------------------------------------------------------------

// Model parsed by the TFLu parser
const tflite::Model* model = nullptr;

// The pointer to the interpreter
tflite::MicroInterpreter* interpreter = nullptr; 
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;

// Input and output tensors
TfLiteTensor* tflu_i_tensor = nullptr;
TfLiteTensor* tflu_o_tensor = nullptr;

// Input shape 
// 50hz sample rate
const int SAMPLE_RATE = 100;
const int NUM_SAMPLES = 100;

// Tensor arena size, memory required by interpreter TFLu does not use dynamic
//  allocation. Arena size is determined by model size through experiments.
const int kTensorArenaSize = 15* 1024;

// allocation of memory 
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// Create a resolver to load the model's operators
static tflite::AllOpsResolver resolver;


// Setup
//-----------------------------------------------------------------------------

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  //load the TFLite model from the C-byte array
model = tflite::GetModel(model_tflite);

// make sure model schema version is compatible (from tflite website)
if (model->version() != TFLITE_SCHEMA_VERSION) {
  TF_LITE_REPORT_ERROR(error_reporter,
  "Model provided is schema version %d not equal not equal to supported version "
  "  %d. \n", model->version(), TFLITE_SCHEMA_VERSION);  
}

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }
  BLE.setLocalName("MNIST Classifier");


  // Initialize TensorFlow Lite
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize); //, error_reporter
  interpreter = &static_interpreter;


  // Allocate memory for the model's input and output tensors
  interpreter->AllocateTensors();

  // Get pointers to the model's input and output tensors
  tflu_i_tensor = interpreter->input(0);
  tflu_o_tensor = interpreter->output(0);

  // Function to normalize the data (zero mean and unit variance)
  //-----------------------------------------------------------------------------


    float normalizedData[NUM_SAMPLES];
    
    // Calculate mean
    float sum = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += not_moving_test[i];
    }
    float mean = sum / NUM_SAMPLES;
    
    // Calculate variance
    float variance = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        variance += std::pow(not_moving_test[i] - mean, 2);
    }
    variance /= NUM_SAMPLES;
    
    // Calculate standard deviation
    float stdDev = std::sqrt(variance);
    
    // Normalize the data
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float normalizedValue = (not_moving_test[i] - mean) / stdDev;
        normalizedData[i] = normalizedValue;
    }
  
  // Copy the normalized date to the input tensor
  for (int i = 0; i < NUM_SAMPLES; i++) {
    tflu_i_tensor->data.f[i] = normalizedData[i];
  }


}


void loop() {

  interpreter->Invoke();

  float prob = tflu_o_tensor->data.f[0];
    
  Serial.print("Probability ");
  Serial.print(": ");
  Serial.println(prob);
  delay(100000000);


}
