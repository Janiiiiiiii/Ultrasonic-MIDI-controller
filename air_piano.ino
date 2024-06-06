const int trigPin = 5; // Trigger pin of ultrasonic sensor
const int echoPin = 6; // Echo pin of ultrasonic sensor
const int buzzer = 8;

const int noNote = 0;

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
  pinMode(buzzer,OUTPUT);
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
    int note = map(distance, MIN_DISTANCE, MAX_DISTANCE, 262,523);

    // Check if the note is different from the previous note
    if (note != prevNote || !notePlaying) {
      // Send MIDI note off for the previous note if it was playing
      if (notePlaying) {
        tone(buzzer ,noNote ,1000);
        notePlaying = false; // Update note playing state
      }
      // Send MIDI note on for the current note
      tone(buzzer ,note ,1000);
      prevNote = note; // Update previous note
      notePlaying = true; // Update note playing state
    }
  } else {
    // If distance is out of range, turn off the previous note if it was playing
    if (notePlaying) {
      tone(buzzer ,noNote ,1000);
      notePlaying = false; // Update note playing state
    }
  }

  // Print the distance for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Add a delay before the next measurement
  delay(100);
}
