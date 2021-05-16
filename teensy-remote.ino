#include <TimerOne.h>
#include <Wire.h>

// For morse code.
constexpr int ledPin = 6;
constexpr int morse_speed = 1200 / 15; // Base time of a Morse code dit. 1200 / WPM.

class remote
{
public:
    static void set(int val);
    static void interrupt_handler(void);
    static int pin() { return pin_; }

private:
    static volatile int out_state_;
    static int pwm_state_;
    static int last_;
    static const int pin_;
};
const int remote::pin_ = 7;
int remote::last_ = HIGH;
volatile int remote::out_state_ = LOW;
int remote::pwm_state_ = 0;

void remote::set(int val)
{
    noInterrupts();
    out_state_ = val;
    interrupts();
}

void remote::interrupt_handler(void)
{
    int out = LOW;
    if (pwm_state_ == 1) {
        out = out_state_;
        pwm_state_ = 0;
    } else {
        pwm_state_ = 1;
    }
    if (last_ != out) {
        digitalWrite(pin_, out);
        last_ = out;
    }
}

void i2c_handler(int);
void setup()
{
    pinMode(ledPin, OUTPUT); // initialize the digital pin as an output.
    pinMode(remote::pin(), OUTPUT);


    Timer1.initialize(13); // Tuned experimentally on teensy++.
    Timer1.attachInterrupt(remote::interrupt_handler);
    Wire.begin(4); // i2c address 4
    Wire.onReceive(i2c_handler);
    blink_string("OK");
}

const char* morse(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        ch += ('a' - 'A');
    }
    switch (ch) {
    case 'a':
        return ".-";
    case 'b':
        return "-...";
    case 'c':
        return "-.-.";
    case 'd':
        return "-..";
    case 'e':
        return ".";
    case 'f':
        return "..-.";
    case 'g':
        return "--.";
    case 'h':
        return "....";
    case 'i':
        return "..";
    case 'j':
        return "---.";
    case 'k':
        return "-.-";
    case 'l':
        return ".-..";
    case 'm':
        return "--";
    case 'n':
        return "-.";
    case 'o':
        return "---";
    case 'p':
        return ".--.";
    case 'q':
        return "--.-";
    case 'r':
        return ".-.";
    case 's':
        return "...";
    case 't':
        return "-";
    case 'u':
        return "..-";
    case 'v':
        return "...-";
    case 'w':
        return ".--";
    case 'x':
        return "-..-";
    case 'y':
        return "-.--";
    case 'z':
        return "..--";
    case '0':
        return "-----";
    case '1':
        return ".----";
    case '2':
        return "..---";
    case '3':
        return "...--";
    case '4':
        return "....-";
    case '5':
        return ".....";
    case '6':
        return "-....";
    case '7':
        return "--...";
    case '8':
        return "---..";
    case '9':
        return "----.";
    case ' ':
        return " ";
    }
    return "........"; // Prosign HH, for error.
}

void blink(int t)
{
    digitalWrite(ledPin, HIGH);
    delay(t);
    digitalWrite(ledPin, LOW);
    delay(t);
}

void blink_string(const char* s)
{
    for (; *s; s++) {
        const char* blinks = morse(*s);
        for (; *blinks; blinks++) {
            switch (*blinks) {
            case '.':
                blink(morse_speed);
                break;
            case '-':
                blink(morse_speed * 3);
                break;
            case ' ':
                delay(morse_speed * 7);
                break;
            }
        }
        delay(morse_speed * 3);
    }
    delay(morse_speed * 3);
}


void tx_pwm(const char* packet)
{
    const int lead_width = 1315;
    const int bit_width = 228;

    for (int i = 0; i < 5; i++) {
        delay(20);

        remote::set(HIGH);
        delayMicroseconds(lead_width);
        remote::set(LOW);
        delayMicroseconds(lead_width);
        for (; *packet; packet++) {
            if (*packet == '1') {
                remote::set(HIGH);
            }
            delayMicroseconds(bit_width);
            remote::set(LOW);
            delayMicroseconds(bit_width);
        }
    }
}
// s = '2 1 2[…]'
// ''.join(['1' * x + '0' for x in [int(x) for x in s.split()]]).rstrip('0')
// 2 1 2 8 1 1 1 18 1 1 2 1 1 1
const char* btn_home = "110101101111111101010101111111111111111110101011010101";

// 2 1 2 4 2 26 4
const char* btn_auto = "11010110111101101111111111111111111111111101111";

// 2 1 2 4 2 4 22 2 2
const char* btn_up = "1101011011110110111101111111111111111111111011011";

// 2 1 2 4 2 1 3 1 20 1 2 1 1
const char* btn_down = "11010110111101101011101011111111111111111111010110101";
// 2 1 2 4 1 4 1 20 2 1 1 2
const char* btn_left = "1101011011110101111010111111111111111111110110101011";
// 2 1 2 4 1 2 2 2 19 1 1 1 2 1
const char* btn_right = "110101101111010110110110111111111111111111101010101101";
// 2 1 2 5 2 2 1 18 2 1 1 1 1 2
const char* btn_bell = "110101101111101101101011111111111111111101101010101011";
// 2 1 2 5 4 2 4 2 1 1 4 2 1 2 1 2 1 2 2
const char* btn_clock = "11010110111110111101101111011010101111011010110101101011011";
// 2 1 2 7 3 19 1 1 1 2 2
const char* btn_target = "110101101111111011101111111111111111111010101011011";
// 2 1 2 6 1 2 19 3 1 1 3
const char* btn_s = "110101101111110101101111111111111111111011101010111";
// 2 1 2 7 3 1 18 1 1 1 2 1 1
const char* btn_max = "11010110111111101110101111111111111111110101010110101";
// 2 1 2 6 4 1 17 4 2 1 1
const char* btn_box = "110101101111110111101011111111111111111011110110101";
//                       2 1  2    4 1 1 1  2  2                 18  2 1 1  2 1
const char* btn_play = "1101011011110101010110110111111111111111111011010101101";
//                        2 1  2    4 1 1 1 22                      2 1    4
const char* btn_pause = "110101101111010101011111111111111111111110110101111";

enum Command {
    Idle = 0,
    Pause = 1,
    Go = 2,
    Home = 3,
};
volatile char cmd = Command::Idle;
void loop()
{
    for (;;) {
        const char* msg = nullptr;
        const char* blink = "ERR";
        switch (cmd) {
        case Command::Idle:
            continue;
        case Command::Home:
            msg = btn_home;
            blink = "HOME";
            break;
        case Command::Pause:
            msg = btn_pause;
            blink = "PAUSE";
            break;
        case Command::Go:
            msg = btn_play;
            blink = "GO";
            break;
        }
        cmd = Command::Idle;
        if (msg != nullptr) {
            tx_pwm(msg);
        }
        blink_string(blink);
    }
    // noInterrupts();
    // interrupts();
}

void i2c_handler(int bytes)
{
    while (Wire.available()) {
        cmd = Wire.read();
    }
}
