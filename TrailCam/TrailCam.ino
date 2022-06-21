#include <avr/sleep.h>
#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Streaming.h>

const int PIN_PIR = 2;
const int PIN_CAMERA_TX = 3;
const int PIN_CAMERA_RX = 4;
const int PIN_CAMERA_PWR = 5;
const int PIN_SD_CHIP_SELECT = 10;

SoftwareSerial cameraconnection(PIN_CAMERA_TX, PIN_CAMERA_RX);
Adafruit_VC0706 cam(&cameraconnection);

volatile bool motionDetected = false;

void onMotionDetected() {
  motionDetected = true;
}

void setup() {
  Serial.begin(9600);

  if (!SD.begin(PIN_SD_CHIP_SELECT)) {
    Serial << "SD card failed (or not present). Stopping." << endl;
    while (true);
  }

  pinMode(PIN_CAMERA_PWR, OUTPUT);
  digitalWrite(PIN_CAMERA_PWR, LOW);

  pinMode(PIN_PIR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_PIR), onMotionDetected, RISING);
  Serial << "Setup complete." << endl;
}

void makeImgFilename(char *imgFilename) {
  static int count = 0;
  sprintf(imgFilename, "%08d.jpg", count++);
}

void takePicture() {
  digitalWrite(PIN_CAMERA_PWR, HIGH);

  while (delay(100), !cam.begin()) {
    Serial << "No camera found. Trying again in 100ms..." << endl;
  }
  cam.setImageSize(VC0706_640x480);

  while (delay(400), !cam.takePicture()) {
    Serial << "Failed to take picture. Trying again in 300ms..." << endl;
  }
  uint16_t bytesRemaining = cam.frameLength();
  Serial << "Picture taken! Size is " << bytesRemaining << " bytes." << endl;

  char imgFilename[13];
  makeImgFilename(imgFilename);

  Serial << "Saving as " << imgFilename << "..." << endl;
  File imgFile = SD.open(imgFilename, FILE_WRITE);
  if (!imgFile) {
    Serial << "Failed to open file." << endl;
    return;
  }

  unsigned long timer = millis();
  int writeCount = 0;
  while (bytesRemaining > 0) {
    int bufferLen = min(64, bytesRemaining);
    uint8_t *buffer = cam.readPicture(bufferLen);
    if (buffer == NULL) {
      Serial << endl << "<Stalling to let camera module catch up...>" << endl;
      delay(1);
      continue;
    }
    imgFile.write(buffer, bufferLen);
    if (++writeCount >= 64) {
      Serial << ".";
      writeCount = 0;
    }
    bytesRemaining -= bufferLen;
  }
  imgFile.close();
  timer = millis() - timer;
  Serial << endl << "Done! File written in " << timer << " ms." << endl;
  digitalWrite(PIN_CAMERA_PWR, LOW);
}

void loop() {
  Serial << "Going to sleep." << endl;
  Serial.flush();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
  Serial << "Wake from sleep!" << endl;
  if (!motionDetected) {
    return;
  }
  Serial << "Motion detected! Taking picture." << endl;
  takePicture();
  motionDetected = false;
}
