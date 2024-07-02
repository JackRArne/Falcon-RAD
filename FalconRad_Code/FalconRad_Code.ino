 #include <Arduino.h>
  #include <CRC32.h>

  // Constants
  const int numBytes = 200;  // Adjust as needed depending on maximum expected message length
  const int returnSize = 39; // Size of the return packet to include start, end, and checksum

  // Constants for SAA boundaries
  const float SAA_LAT_MIN = -5000.0;
  const float SAA_LAT_MAX = 0000.0;
  const float SAA_LON_MIN = -9000.0;
  const float SAA_LON_MAX = 4000.0;

  // Pin definitions
  const int tempSensor = A0;
  const int dos1 = A1;
  const int dos2 = A2;
  const int dos3 = A3;
  const int dos4 = A4;

  struct GPSData {
    float time;
    float latitude;
    char latDirection;
    float longitude;
    char longDirection;
    float altitude;
  };  

  struct saaPacket{
    GPSData timeCode;
    int saaData[5][10];
  };

  struct normPacket{
    GPSData timeCode;
    int saaData[5][2];
  };

  // Global variables
  String receivedData; // Using String to hold received data until newline
  byte returnPacket[returnSize];
  bool crcFlag = false;
  int inputVal1, inputVal2, inputVal3, inputVal4, inputVal5;


  void setup() {
    Serial1.begin(115200);
    Serial.begin(115200); // Initialize the USB Serial communication
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  }

  void readBytes() {
    if (Serial1.available()) {
      receivedData = Serial1.readStringUntil('\n');
      Serial.println("Received: " + receivedData);
    }
  }

  void sendSaa(saaPacket value){
    byte* b = (byte*)&value;
    uint32_t calculatedCRC = CRC32::calculate(b, sizeof(saaPacket));
    Serial1.write("\xFF\xFD\xFF");
    Serial1.write("AFS");
    Serial1.write(b, sizeof(saaPacket));
    Serial1.write((byte*)&calculatedCRC, sizeof(uint32_t));
    Serial1.write("\xFF\xFE\xFF");
    Serial.write("sentSaa");
  }
  void sendNorm(normPacket value){
    byte* b = (byte*)&value;
    uint32_t calculatedCRC = CRC32::calculate(b, sizeof(normPacket));
    Serial1.write("\xFF\xFD\xFF");
    Serial1.write("AFN");
    Serial1.write(b, sizeof(normPacket));
    Serial1.write((byte*)&calculatedCRC, sizeof(uint32_t));
    Serial1.write("\xFF\xFE\xFF");
    Serial.write("sentNorm");
  }
       


  String crcCheck() {
    int asteriskIndex = receivedData.indexOf('*');
    if (asteriskIndex > 0) {
        String dataPart = receivedData.substring(2, asteriskIndex);
        String checksumPart = receivedData.substring(asteriskIndex + 1);
        checksumPart = checksumPart.substring(0, checksumPart.length() - 2);

        // Create an array from the dataPart string
        int dataLength = dataPart.length();
        uint8_t *byteBuffer = new uint8_t[dataLength];
        for (int i = 0; i < dataLength; i++) {
            byteBuffer[i] = (uint8_t)dataPart.charAt(i);
        }

        // Calculate the CRC of the entire buffer
        uint32_t calculatedCRC = CRC32::calculate(byteBuffer, dataLength);
        delete[] byteBuffer;  // Free the allocated memory

        // Convert the CRC-32 to a hex string
        char buffer[10];
        sprintf(buffer, "%08X", calculatedCRC);
        String calculatedCRCStr = String(buffer);

        Serial.print("Data: ");
        Serial.print(dataPart);
        Serial.print(", Calculated CRC: ");
        Serial.print(calculatedCRCStr);
        Serial.print(", Received CRC: ");
        Serial.println(checksumPart);

        // Compare the calculated and received CRC case-insensitively
        crcFlag = calculatedCRCStr.equalsIgnoreCase(checksumPart);
        return dataPart;
    } else {
        Serial.println("Invalid data or checksum not found");
        crcFlag = false;
    }
    return "";
  }



  bool processData() {
    if (!crcFlag) {
      Serial.println("CRC check failed.");
      return false;
    } else {
      Serial.println("CRC check passed.");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      return true;
    }
  }
 
  GPSData parseGPSData(String data) {
      GPSData parsedData;
     
      char tempBuffer[100]; // Buffer to hold a copy of the input data

      // Ensure the string is null-terminated
      data.trim();
      data.toCharArray(tempBuffer, sizeof(tempBuffer));
      tempBuffer[sizeof(tempBuffer) - 1] = '\0';  // Safety null termination

      // Debug print the raw data
   

      // Tokenize the string using strtok and commas as delimiters
      char* token = strtok(tempBuffer, ",");
     
      // Extract time
      if (token != NULL) {
          parsedData.time = atof(token);
          token = strtok(NULL, ",");
      }

      // Extract latitude
      if (token != NULL) {
          parsedData.latitude = atof(token);
          token = strtok(NULL, ",");
      }

      // Extract latitude direction
      if (token != NULL) {
          parsedData.latDirection = token[0]; // Assuming the direction is a single character
          token = strtok(NULL, ",");
          if (parsedData.latDirection == 'S'){
            parsedData.latitude = parsedData.latitude * -1;
          }
      }

      // Extract longitude
      if (token != NULL) {
          parsedData.longitude = atof(token);
          token = strtok(NULL, ",");
      }

      // Extract longitude direction
      if (token != NULL) {
          parsedData.longDirection = token[0]; // Assuming the direction is a single character
          token = strtok(NULL, ",");
          if (parsedData.longDirection == 'W'){
            parsedData.longitude = parsedData.longitude * -1;
          }
      }

      // Extract altitude
      if (token != NULL) {
          parsedData.altitude = atof(token);
      }

   

      return parsedData;
  }




// Function to check if the current location is within the SAA
bool isSAA(float latitude, float longitude) {
  return latitude >= SAA_LAT_MIN && latitude <= SAA_LAT_MAX &&
         longitude >= SAA_LON_MIN && longitude <= SAA_LON_MAX;
}

void saaGather(saaPacket *value) {
 
    for (int i = 0; i < 10; i++) {
        value->saaData[0][i] = analogRead(tempSensor);
        value->saaData[1][i] = analogRead(dos1);
        value->saaData[2][i] = analogRead(dos2);
        value->saaData[3][i] = analogRead(dos3);
        value->saaData[4][i] = analogRead(dos4);
        delay(1000); // One-second interval
    }
    // Send the gathered data
}


void normGather(normPacket* value) {

    for (int i = 0; i < 2; i++) {
        value->saaData[0][i] = analogRead(tempSensor);
        value->saaData[1][i] = analogRead(dos1);
        value->saaData[2][i] = analogRead(dos2);
        value->saaData[3][i] = analogRead(dos3);
        value->saaData[4][i] = analogRead(dos4);
        delay(1000); // One-second interval
    }
    // Send the gathered data
}

void loop() {
  readBytes();
  if (receivedData.length() > 0) {
    receivedData = crcCheck();
    if (processData()) {
      GPSData parsedGPSData = parseGPSData(receivedData);
   
      if (isSAA(parsedGPSData.latitude, parsedGPSData.longitude)) {
        saaPacket saa;
        saaGather(&saa);
        saa.timeCode = parsedGPSData;
        sendSaa(saa);
        return;
      } else {
        normPacket norm;
        normGather(&norm);
        norm.timeCode = parsedGPSData;
        sendNorm(norm);
        return;
      }
    }
  }
}