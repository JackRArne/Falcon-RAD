#include <Arduino.h>

// Global 16-bit packet counter.
uint16_t packetCounter = 0;

// CRC16 calculation using the CRC-CCITT (0x1021) polynomial.
uint16_t calculateCRC16(const uint8_t *data, size_t length) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < length; i++) {
    crc ^= ((uint16_t)data[i] << 8);
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc = (crc << 1);
    }
  }
  return crc;
}

void setup() {
  // Initialize the USB Serial monitor for debugging.
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for the Serial monitor to connect.
  }
  
  // Initialize Serial1 for RS232 communication.
  Serial1.begin(115200);
}

void loop() {
  // Look for the two start bytes (0xFF, 0xFF) to synchronize the packet.
  if (Serial1.available() >= 2) {
    // Wait for first start byte.
    uint8_t firstByte = Serial1.read();
    if (firstByte == 0xFF) {
      // Wait until a second byte is available.
      while (Serial1.available() == 0) {
        ; // Busy-wait; consider adding a timeout in a production environment.
      }
      uint8_t secondByte = Serial1.read();
      if (secondByte == 0xFF) {
        // Start bytes detected. Now wait for the remaining 10 bytes.
        uint8_t packet[12];
        packet[0] = 0xFF;
        packet[1] = 0xFF;
        
        // Wait until at least 10 more bytes are available.
        while (Serial1.available() < 10) {
          ; // Blocking wait until all bytes are received.
        }
        // Read the remaining 10 bytes.
        for (int i = 2; i < 12; i++) {
          packet[i] = Serial1.read();
        }
        
        // --- Validate the received packet checksum ---
        // Compute CRC over the first 10 bytes (all fields except the received CRC).
        uint16_t computedCRC = calculateCRC16(packet, 10);
        uint16_t receivedCRC = ((uint16_t)packet[10] << 8) | packet[11];
        bool checksumValid = (computedCRC == receivedCRC);
        
        // --- Build the echo packet ---
        // Echo packet structure:
        // Bytes 0-1: Start bytes (from received packet)
        // Bytes 2-3: 2-byte counter (big-endian)
        // Bytes 4-7: 4-byte time code (if CRC invalid, the MSB is set to 0xFF)
        // Bytes 8-9: 2 bytes latitude (from received packet)
        // Bytes 10-11: 2 bytes longitude (from received packet)
        // Bytes 12-21: 10 bytes of science data (4x docimeter data from A1, A2, A3, A4 and temperature on A5)
        // Bytes 22-23: 2-byte CRC16 over the first 22 bytes.
        uint8_t echoPacket[24];
        int idx = 0;
        
        // Copy start bytes.
        echoPacket[idx++] = packet[0];
        echoPacket[idx++] = packet[1];
        
        // 2-byte counter in big-endian.
        echoPacket[idx++] = (packetCounter >> 8) & 0xFF;
        echoPacket[idx++] = packetCounter & 0xFF;
        packetCounter++;  // Increment counter for the next packet.
        
        // 4-byte time code: copy from the received packet.
        // If the CRC check failed, set the most significant byte to 0xFF.
        if (!checksumValid) {
          echoPacket[idx++] = 0xFF;
        } else {
          echoPacket[idx++] = 0x00;
        }
        // Copy the remaining 3 time code bytes unmodified.
        echoPacket[idx++] = packet[3];
        echoPacket[idx++] = packet[4];
        echoPacket[idx++] = packet[5];
        
        // 2 bytes latitude.
        echoPacket[idx++] = packet[6];
        echoPacket[idx++] = packet[7];
        
        // 2 bytes longitude.
        echoPacket[idx++] = packet[8];
        echoPacket[idx++] = packet[9];
        
        // --- Read science data ---
        // Read 4 docimeter values from analog pins A1, A2, A3, and A4.
        // Each reading is 16-bit (2 bytes, big-endian).
        uint16_t docimeter[4];
        docimeter[0] = analogRead(A1);
        docimeter[1] = analogRead(A2);
        docimeter[2] = analogRead(A3);
        docimeter[3] = analogRead(A4);
        
        // Read temperature sensor value from analog pin A5.
        // If your sensor is connected to AREF instead of A5, change the parameter below:
        // e.g., update:
        //    uint16_t tempValue = analogRead(A5);
        // to
        //    uint16_t tempValue = analogRead(AREF);
        // NOTE: Many Arduino boards reserve the AREF pin solely as the reference voltage for the ADC.
        // Ensure your board and wiring support reading from AREF before making this change.
        uint16_t tempValue = analogRead(A5);
        
        // Append the 10 bytes of science data:
        // 8 bytes: 4 docimeter values.
        for (int i = 0; i < 4; i++) {
          echoPacket[idx++] = (docimeter[i] >> 8) & 0xFF;  // High byte
          echoPacket[idx++] = docimeter[i] & 0xFF;           // Low byte
        }
        // 2 bytes: temperature sensor data.
        echoPacket[idx++] = (tempValue >> 8) & 0xFF;
        echoPacket[idx++] = tempValue & 0xFF;
        
        // --- Compute new CRC16 ---
        // Calculate CRC16 over the first 22 bytes of the echo packet.
        uint16_t newCRC = calculateCRC16(echoPacket, 22);
        echoPacket[idx++] = (newCRC >> 8) & 0xFF;
        echoPacket[idx++] = newCRC & 0xFF;
        
        // --- Echo the new packet back to the PIB over RS232 ---
        Serial1.write(echoPacket, 24);
      }
    }
  }
}
