#include <MIDIUSB.h>

const int trigPin = 5; // Trigger pin of ultrasonic sensor
const int echoPin = 6; // Echo pin of ultrasonic sensor

const int MIN_DISTANCE = 5; // Minimum distance in cm
const int MAX_DISTANCE = 50; // Maximum distance in cm

int prevNote = -1; // Previous note played (-1 means no note)
bool notePlaying = false; // Flag to track if a note is currently playing

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  int duration, distance;

  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo duration in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;

  // Ensure the distance is within the specified range
  if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) {
    // Map the distance to MIDI note values (adjust as needed)
    int note = map(distance, MIN_DISTANCE, MAX_DISTANCE, 60,84);

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

  // Print the distance for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

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
