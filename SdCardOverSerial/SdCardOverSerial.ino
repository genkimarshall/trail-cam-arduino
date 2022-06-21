#include <SD.h>
#include <Streaming.h>

const int PIN_CHIP_SELECT = 10;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CHIP_SELECT, OUTPUT);
  if (!SD.begin(PIN_CHIP_SELECT)) {
    Serial << "SD card fail (or not inserted)." << endl;
    while (true);
  }

  File root = SD.open("/");
  while (true) {
    File file = root.openNextFile();
    if (!file) {
      break;
    }
    Serial.write(file.name());
    Serial.write('\n');
    uint32_t fileSize = file.size();
    for (int i = 0; i < 4; i++) {
      Serial.write(0xff & (fileSize >> (i*8)));
    }
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  }
}

void loop() {}
