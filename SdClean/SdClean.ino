#include <SD.h>
#include <Streaming.h>

// NOTE: only works on top level files

const int PIN_CHIP_SELECT = 10;

void setup() {
  Serial.begin(9600);

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
    String filename = file.name();
    file.close();
    SD.remove(filename);
    Serial << "Removed " << filename << endl;
  }

  Serial << "Done" << endl;
}

void loop() {}
