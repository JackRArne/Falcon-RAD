#include <CRC32.h>

CRC32 crc;
String incomingData = "";

void setup() {
  // Start the serial communication
  Serial.begin(115200);
}

void loop() {
  // Check if data is available to read
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    incomingData += inChar;

    // If the incoming character is a newline, process the received data
    if (inChar == '\n') {
      // Assuming the incomingData contains the binary string for the NMEA sentence
      String asciiString = binaryToASCII(incomingData);

      // Parse the ASCII string to extract latitude, longitude, and altitude
      String latitude = parseLatitude(asciiString);
      String longitude = parseLongitude(asciiString);
      String altitude = parseAltitude(asciiString);

      // Append mock datas
      String mockData = "";
      for (int i = 0; i < 180; ++i) {
        mockData += "X"; // 'X' represents mock data
      }

      // Concatenate all data
      String dataToChecksum = "FFFF" + latitude + longitude + altitude + mockData;

      // Calculate CRC32 checksum
      crc.reset();
      crc.update((const uint8_t*)dataToChecksum.c_str(), dataToChecksum.length());
      uint32_t checksum = crc.finalize();

      // Convert checksum to a hexadecimal string
      char hexChecksum[9];
      sprintf(hexChecksum, "%08X", checksum);
      
      // Construct the final message
      String finalMessage = dataToChecksum + hexChecksum + "FFFF";

      // Send the final message
      Serial.println(finalMessage);

      // Clear the incomingData for the next read
      incomingData = "";
    }
  }
}

String binaryToASCII(String binary) {
  String ascii = "";
  for (int i = 0; i < binary.length(); i += 8) {
    String byteString = binary.substring(i, i + 8);
    char character = strtol(byteString.c_str(), NULL, 2);
    ascii += character;
  }
  return ascii;
}

String parseLatitude(String sentence) {
  int startIndex = sentence.indexOf(",") + 1; // Start after the GGA identifier
  int endIndex = sentence.indexOf(",", startIndex);
  String latitude = sentence.substring(startIndex, endIndex);
  return latitude;
}

String parseLongitude(String sentence) {
  int startIndex = sentence.indexOf("N,") + 2; // Start after the latitude direction
  int endIndex = sentence.indexOf(",", startIndex);
  String longitude = sentence.substring(startIndex, endIndex);
  return longitude;
}

String parseAltitude(String sentence) {
  // Altitude is the 9th comma-separated value in GGA
  int commaCount = 0;
  int startIndex = 0;
  for (int i = 0; i < sentence.length(); i++) {
    if (sentence.charAt(i) == ',') {
      commaCount++;
      if (commaCount == 9) {
        startIndex = i + 1;
        break;
      }
    }
  }
  int endIndex = sentence.indexOf(",", startIndex);
  String altitude = sentence.substring(startIndex, endIndex);
  return altitude;
}
