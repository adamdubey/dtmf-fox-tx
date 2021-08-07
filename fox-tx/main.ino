/*
DTMF Codes
1 = Initialize
5 = TX CW "Hi"
6 = Ident with callsign
7 = Ident with 73
*/

#include <DTMF.h>
#include "pitches.h"

// DTMF
int sensorPin = A0;
float n = 128.0;
float sampling_rate = 8926.0;
DTMF dtmf = DTMF(n, sampling_rate);
float d_mags[8];
char thischar;

// TX
int xmit = 0;
int pause = 1440; // Pause; 22 = 1s

// Melody
int melody[] = {
  NOTE_G4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_E4
};

int noteDurations[] = {
  8, 8, 4, 4, 8, 8, 8, 8, 4, 4, 4, 4
};

// Enter your callsign in charstrings
#define PTT  7
#define BUZZER_PIN 5 
#define BUZZER_FREQUENCY 700
char beaconstring[] = "CQ ";
char idstring[] = "DE ";
int randNumber;

// CW & Definitions
char chartemp[4] ;
char incomingByte = 0;
int interval = 60;

String alphabet = "abcdefghijklmnopqrstuvwxyz1234567890/ ";
String ALPHABET = alphabet;

String morseCode[] = { 
    ".-", // A
    "-...", // B
    "-.-.", // C
    "-..", // D
    ".", // E
    "..-.", // F
    "--.", // G
    "....", // H
    "..", // I
    ".---", // J
    "-.-", // K
    ".-..", // L
    "--", // M
    "-.", // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.", // R
    "...",  // S
    "-", // T
    "..-", // U
    "...-", // V
    ".--", // W
    "-..-", // X
    "-.--", // Y
    "--..", // Z
    ".----", // 1
    "..---", // 2
    "...--", // 3
    "....-", // 4
    ".....", // 5
    "-....", // 6
    "--...", // 7
    "---..", // 8
    "----.", // 9
    "-----", // 0
    "-..-.", // forward slash
    " "   // space
};

void setup() {
  
  pinMode(PTT, OUTPUT);
  digitalWrite(PTT, HIGH);
  Serial.begin(9600);
  
  ALPHABET.toUpperCase();
  randomSeed(analogRead(0));
}

void loop() {

for (int x = 0; x < pause; x ++)
{
  dtmf.sample(sensorPin);
  dtmf.detect(d_mags, 506);
  thischar = dtmf.button(d_mags, 1800.);
  
  // DTMF cases
  if (thischar) {
    switch (thischar) {
        // DTMF 1
      case 49:
        xmit = 1;                            
        break;
        // DTMF 5
      case 53:
        digitalWrite(PTT, LOW);
        delay(2000);
        sendLetter('h');
        sendLetter('i');
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;
        // DTMF 6
      case 54:
        digitalWrite(PTT, LOW);
        delay(2000);
        for (int i = 0; i < sizeof(idstring); i++){
          sendLetter(idstring[i]);
        }
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;
        // DTMF 7
      case 55:
        digitalWrite(PTT, LOW);
        delay(2000);
        sendLetter('7');
        sendLetter('3');
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;
        // Disable TX if DTMF recieved != defined DTMF cases
      default:
        xmit = 0;
      break;
    }
  }
  delay(1);
}

if (xmit == 1)
  {
  digitalWrite(PTT, LOW);
  delay(2000);

  // Play melody
  for (int thisNote = 0; thisNote < 12; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }

  delay(1000);

  for (int i = 0; i < sizeof(beaconstring); i++){
    sendLetter(beaconstring[i]);
    }

  delay(500);
  digitalWrite(PTT, HIGH);
  }
}

void sendLetter (char c) {
   int i;
   for (i = 0; i < alphabet.length(); i = i + 1) {
       if (alphabet[i] == c || ALPHABET[i] == c) {
           sendMorseCode(morseCode[i]);
           return;
       }
   }
   if (c == '\n')
      Serial.println();
}

void sendMorseCode (String tokens) {
   int i;
   for (i = 0; i < tokens.length(); i = i + 1) {
       switch (tokens[i]) {
           case '-':
               sendDash();
               break;
           case '.':
               sendDot();
               break;
           case ' ':
               sendEndOfWord();
               break;
       }
   }
   morseOutputOff(2);
}

void sendEndOfWord() {
   morseOutputOff(4);
}

void sendDot() {
   tone(BUZZER_PIN, BUZZER_FREQUENCY);
   morseOutputOn(1);
   noTone(BUZZER_PIN);
   morseOutputOff(1);
}
void sendDash() {
   tone(BUZZER_PIN, BUZZER_FREQUENCY);
   morseOutputOn(3);
   noTone(BUZZER_PIN);
   morseOutputOff(1);
}

void morseOutputOn (int n) {
   delay(n * interval);
}

void morseOutputOff (int n) {
   delay(n * interval);
}
