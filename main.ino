#include <Microphone.h>
#include <FFT.h>
#include <LED.h>

const int bassLED = 26; // Bás
const int midLED = 27; // Stredné tóny
const int trebleLED = 25; // Vysoké tóny

Microphone mic(34); // Pín pre mikrofón
FFT fft(); // FFT analýza

void setup() {
  Serial.begin(115200);
  // Configure PWM for LEDs
  ledcSetup(0, 5000, 8); // Channel 0: 5kHz, 8-bit
  ledcAttachPin(bassLED, 0);
  ledcSetup(1, 5000, 8); // Channel 1
  ledcAttachPin(midLED, 1);
  ledcSetup(2, 5000, 8); // Channel 2
  ledcAttachPin(trebleLED, 2);
}

void loop() {
  mic.begin();
  fft.begin();
  int samples[1024];
  mic.read(samples, 1024);
  fft.symmetricToMagnitude(samples, samples + 512);

  int maxBass = 0, maxMid = 0, maxTreble = 0;

  for (int i = 0; i < 512; i++) {
    if (i < 50) maxBass = max(maxBass, samples[i]); // 0-50 Hz
    else if (i < 200) maxMid = max(maxMid, samples[i]); // 50-200 Hz
    else maxTreble = max(maxTreble, samples[i]); // 200+ Hz
  }

  // Map amplitudes to PWM brightness (0-255)
  int bassBrightness = constrain(map(maxBass, 0, 2000, 0, 255), 0, 255);
  int midBrightness = constrain(map(maxMid, 0, 3000, 0, 255), 0, 255);
  int trebleBrightness = constrain(map(maxTreble, 0, 4000, 0, 255), 0, 255);

  // Set LED brightness via PWM
  ledcWrite(0, bassBrightness); // Bass
  ledcWrite(1, midBrightness); // Mid
  ledcWrite(2, trebleBrightness); // Treble

  delay(100);
}

// PWM-based LED brightness control (replaces binary on/off)
void setLEDBrightness(int channel, int brightness) {
  ledcWrite(channel, brightness);
}