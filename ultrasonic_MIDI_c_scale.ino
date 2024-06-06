#include <MIDIUSB.h>

const int trigPin = 5; // Trigger pin of ultrasonic sensor
const int echoPin = 6; // Echo pin of ultrasonic sensor
const int xPin = A0;
const int yPin = A1;

const int MIN_DISTANCE = 5; // Minimum distance in cm
const int MAX_DISTANCE = 50; // Maximum distance in cm

// MIDI notes for C major scale with octave notes
const int notes[] = {60, 62, 64, 65, 67, 69, 71, 72}; // C, D, E, F, G, A, B, C (Octave)

int prevNote = -1; // Previous note played (-1 means no note)
bool notePlaying = false; // Flag to track if a note is currently playing

void setup() {
  Serial.begin(9600);

  // Set ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long duration, distance;

  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

  if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) {
    
    byte index = map(distance, MIN_DISTANCE, MAX_DISTANCE, 0, 7);
  
    int note = notes[index];

    // Check if the note is different from the previous note
    if (note != prevNote || !notePlaying) {
      // Send MIDI note off for the previous note if it was playing
      if (notePlaying) {
        noteOff(1, prevNote, 0); // Velocity 0, channel 1
           MidiUSB.flush();
        notePlaying = false; // Update note playing state
      }
      // Send MIDI note on for the current note
      noteOn(1, note, 127); // Velocity 127, channel 1
         MidiUSB.flush();
      prevNote = note; // Update previous note
      notePlaying = true; // Update note playing state
    }
  } else {
    // If distance is out of range, turn off the previous note if it was playing
    if (notePlaying) {
      noteOff(1, prevNote, 0); // Velocity 0, channel 1
         MidiUSB.flush();
      notePlaying = false; // Update note playing state
    }
  }
    byte midiValueX = map(xValue, 0, 1023, 0, 127);
    byte midiValueY = map(yValue, 0, 1023, 0, 127);

  controlChange(0, 1, midiValueX);
         MidiUSB.flush();
  controlChange(0, 2, midiValueY); 
         MidiUSB.flush();

  // Print the distance for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println(xValue);
  Serial.println(yValue);

  // Add a delay before the next measurement
  delay(50);
}

//------------------------------------------------------------------------------------------------------//
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
