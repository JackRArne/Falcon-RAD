const int numBytes = 9;  // Number of bytes to receive
byte receivedBytes[numBytes]; // Array to store received bytes
int receivedCount = 0; // Counter for received bytes

 
byte calccrcarray[numBytes-1];
const int return_size = 19;
byte returnpacket[return_size];
#include "CRC.h"
byte calcdcrc_return[1];
bool crcflag = false;
const int checksumbyte = 9;
byte incom_crc_value[1];
byte incom_calcd_crc[1];
 
int crc1 = 1;
int crc2 = 1;
 
//Setup constants
const int temp_sensor = A0;
const int dos_1 = A1;
const int dos_2 = A2;
const int dos_3 = A3;
const int dos_4 = A4;
 
int inputVal_1 = 0;
int inputVal_2 = 0;
int inputVal_3 = 0;
int inputVal_4 = 0;
int inputVal_5 = 0;
 
byte data[10];
 
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

}
 
void readbytes(){
  if (receivedCount < numBytes) {
    if (Serial.available() > 0) {
      // Read the incoming byte and store it in the array
      receivedBytes[receivedCount] = Serial.read();
      receivedCount++;
    }
  }
}
void crc_check(){
  incom_crc_value[0] = receivedBytes[checksumbyte-1];
  crc1 = int(incom_crc_value[0]);
  calccrcarray[0] = receivedBytes[0];
  calccrcarray[1] = receivedBytes[1];
  calccrcarray[2] = receivedBytes[2];
  calccrcarray[3] = receivedBytes[3];
  calccrcarray[4] = receivedBytes[4];
  calccrcarray[5] = receivedBytes[5];
  calccrcarray[6] = receivedBytes[6];
  calccrcarray[7] = receivedBytes[7];
  //calccrcarray[8] = receivedBytes[8];
 
  incom_calcd_crc[0] = calcCRC8(calccrcarray,numBytes-1);
  crc2 = int(incom_calcd_crc[0]);
  if (crc1 == crc2){
    crcflag = true;
  }
  else{
    crcflag = false;
  }
 
}
 
 
void Gps_pull(){
  for (int j = 0; j<8; j++){
    returnpacket[j] = receivedBytes[j+1];
  }
}
 
void Data_pull(){
  inputVal_1 = analogRead(temp_sensor);
  inputVal_2 = analogRead(dos_1);
  inputVal_3 = analogRead(dos_2);
  inputVal_4 = analogRead(dos_3);
  inputVal_5 = analogRead(dos_4);
  data[0] = highByte(inputVal_1);
  data[1] = lowByte(inputVal_1);
  data[2] = highByte(inputVal_2);
  data[3] = lowByte(inputVal_2);
  data[4] = highByte(inputVal_3);
  data[5] = lowByte(inputVal_3);
  data[6] = highByte(inputVal_4);
  data[7] = lowByte(inputVal_4);
  data[8] = highByte(inputVal_5);
  data[9] = lowByte(inputVal_5);
 // data[0] = highByte(0);
 // data[1] = lowByte(300);
 // data[2] = highByte(0);
 // data[3] = lowByte(0);
 // data[4] = highByte(0);
 // data[5] = lowByte(0);
 // data[6] = highByte(0);
  //data[7] = lowByte(0);
 // data[8] = highByte(0);
 // data[9] = lowByte(0);
  for (int k = 0, l = 0; (k<10) && (l<5+10); k++, l++){
    returnpacket[8+l] = data[k];
  }
}
 
void finalcrc() {
  calcdcrc_return[0] = calcCRC8(returnpacket, return_size - 1); // Calculate CRC for the packet minus the CRC byte itself
  returnpacket[return_size - 1] = calcdcrc_return[0]; // Assign calculated CRC to the last byte of the return packet
}
 
void loop() {
  readbytes();
 
  if (receivedCount == numBytes) {
    //crc_check();
    if (crcflag == false) { // If CRC check is successful, then pull GPS and data
      Gps_pull();
      Data_pull();
      finalcrc();
      digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off
        
      for (int i = 0; i < return_size; i++) { // Send return packet including the CRC byte
        Serial.write(returnpacket[i]);
      }
    }  
    // Reset the counter to start receiving new bytes
    receivedCount = 0;
    crcflag = false;
  }
 
  // Corrected this condition to prevent overflow
  if (receivedCount >= numBytes) {
    receivedCount = 0;
  }
}