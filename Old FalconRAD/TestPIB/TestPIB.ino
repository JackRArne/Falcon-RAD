#include <Arduino.h>
#include <FastCRC.h>

FastCRC8 CRC8;

void setup() {
  Serial.begin(115200); // Initialize Serial for debugging
  Serial1.begin(115200); // Initialize Serial1 for Featherboard's RX/TX communication
}

void sendPacket() {
  unsigned long long currentTime = millis(); // Get current time in milliseconds
  currentTime /= 1000; // Convert milliseconds to seconds

  byte reqpacket[] = {0xFF, 0x12, 0x12, 0x12, 0x12, 0x13, 0xFF, 0xFF, 0xFF};
  
  // Fill reqpacket with currentTime
  for (int i = 7; i >= 0; i--) {
    reqpacket[i+1] = (currentTime >> (i * 8)) & 0xFF;
  }

  byte crcValue = CRC8.smbus(reqpacket, sizeof(reqpacket));

  Serial1.write(reqpacket, sizeof(reqpacket)); // Send the packet via Serial1
  Serial1.write(crcValue); // Send the CRC via Serial1

  // Convert current time to hours, minutes, and seconds
  unsigned long hours = currentTime / 3600;
  unsigned long mins = (currentTime / 60) % 60;
  unsigned long secs = currentTime % 60;

  // Debug message over USB serial connection with timestamp
  Serial.print(F("Packet sent via RX/TX at "));
  Serial.print(hours);
  Serial.print(F(":"));
  Serial.print(mins);
  Serial.print(F(":"));
  Serial.println(secs);

  delay(500); // Wait for 1 second
}


void receivePacket() {
  if (Serial1.available()) {
    char reply[50];
    int bytesRead = Serial1.readBytes(reply, sizeof(reply));
    
    // Debug message over USB serial connection
    Serial.print(F("Received via RX/TX: "));
    for (int i = 0; i < bytesRead; i++) {
      Serial.print(reply[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    // Debug message over USB serial connection
    Serial.println(F("No data received via RX/TX"));
  }
}

void loop() {
  if (Serial1.available() == 0) {
    sendPacket();
  } else {
    receivePacket();
  }

  delay(500); // Main loop delay
}
