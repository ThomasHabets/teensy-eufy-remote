#include<TimerOne.h>
const int ledPin = 6;
const int analogPin = 14; // Marked C4.
const int digitalPin = 7; // Marked D7.
const int remotePin = 7;
const int speed = 1200 / 20; // Base time of a dit. 1200 / WPM.

volatile int led_state = LOW;
int pwm_state = 0;
void blinkLED(void)
{
  int out = LOW;
  if (pwm_state == 1) {
    out = led_state;
    pwm_state = 0;
  } else {
    pwm_state = 1;
  }
  digitalWrite(digitalPin, out);
}

void setup() {
  pinMode(ledPin, OUTPUT); // initialize the digital pin as an output.
  pinMode(remotePin, OUTPUT);
  pinMode(digitalPin, OUTPUT);
  Timer1.initialize(13);
  Timer1.attachInterrupt(blinkLED);
//  analogWriteFrequency(analogPin, 38000);
  
}



const char* morse(char ch) {
  if (ch >= 'A' && ch <= 'Z') {
    ch += ('a'-'A');
  }
  switch (ch) {
    case 'a': return ".-";
    case 'b': return "-...";
    case 'c': return "-.-.";
    case 'd': return "-..";
    case 'e': return ".";
    case 'f': return "..-.";
    case 'g': return "--.";
    case 'h': return "....";
    case 'i': return "..";
    case 'j': return "---.";
    case 'k': return "-.-";
    case 'l': return ".-..";
    case 'm': return "--";
    case 'n': return "-.";
    case 'o': return "---";
    case 'p': return ".--.";
    case 'q': return "--.-";
    case 'r': return ".-.";
    case 's': return "...";
    case 't': return "-";
    case 'u': return "..-";
    case 'v': return "...-";
    case 'w': return ".--";
    case 'x': return "-..-";
    case 'y': return "-.--";
    case 'z': return "..--";
    case '0': return "-----";
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";
    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";
    case ' ': return " ";
  }
  return "........"; // Prosign HH, for error.
}

void blink(int t) {
  digitalWrite(ledPin, HIGH);
  delay(t);
  digitalWrite(ledPin, LOW);
  delay(t);
}

void blink_string(const char* s) {
  for (; *s; s++) {
    const char* blinks = morse(*s);
    for (; *blinks; blinks++) {
      switch (*blinks) {
      case '.':
        blink(speed);
        break;
      case '-':
        blink(speed*3);
        break;
      case ' ':
        delay(speed*7);
        break;
      }
    }
    delay(speed*3);
  }
  delay(speed*3);
}


void tx(const char* packet) {
  digitalWrite(remotePin, HIGH);
  delay(3);
  digitalWrite(remotePin, LOW);
  delay(3);
  const int bit_width = 520;
  for (; *packet; packet++) {
    if (*packet == '1') {
      digitalWrite(remotePin, HIGH);
    }
    delayMicroseconds(bit_width);
    digitalWrite(remotePin, LOW);
    delayMicroseconds(bit_width);
  }
}
void tx_pwm(const char* packet) {
  led_state = HIGH;
  const int lead_width = 1315;
  delayMicroseconds(lead_width);
  led_state = LOW;
  delayMicroseconds(lead_width);
  const int bit_width = 228;
  for (; *packet; packet++) {
    if (*packet == '1') {
      led_state = HIGH;
    }
    delayMicroseconds(bit_width);
    led_state = LOW;
    delayMicroseconds(bit_width);
  }
}
void tx_analog(const char* packet) {
  const int value = 128;
  Timer1.pwm(remotePin, value);
  delay(3);
  analogWrite(remotePin, 0);
  delay(3);
  const int bit_width = 520;
  for (; *packet; packet++) {
    if (*packet == '1') {
      Timer1.pwm(remotePin, value);
    }
    delayMicroseconds(bit_width);
    Timer1.pwm(remotePin, 0);
    delayMicroseconds(bit_width);
  }
}


void loop() {
  blink_string("OK");
  digitalWrite(5, HIGH);
  // 22                    2 1  2    4 1 1 1 22                      2 1    4      
  //const char* btn_play = "110101101111010101011111111111111111111110110101111";
  // 22                    2 1  2    4 1 1 1  2  2                 18  2 1 1  2 1      
  const char* btn_play = "1101011011110101010110110111111111111111111011010101101";
  for (int i = 0; i < 5; i++) {
    tx_pwm(btn_play);
    delay(20);
  }
  blink_string("K K K");
  delay(1000);
}
