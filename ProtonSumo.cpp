#include "ProtonSumo.h"
#include <IRremote.h>

// ================= PINS =================
#define IR_RECEIVE_PIN 8

#define TRIG_PIN 2
#define ECHO_PIN 3

#define MOTOR_L_FWD 10
#define MOTOR_L_BWD 9

#define MOTOR_R_FWD 5
#define MOTOR_R_BWD 6

// =============== INSTANCE ===============
ProtonSumo Sumo;

// =============== INIT ====================
void ProtonSumo::begin()
{
    pinMode(MOTOR_L_FWD, OUTPUT);
    pinMode(MOTOR_L_BWD, OUTPUT);

    pinMode(MOTOR_R_FWD, OUTPUT);
    pinMode(MOTOR_R_BWD, OUTPUT);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(A0, INPUT);
    pinMode(A1, INPUT);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    stop();

    for (int i = 0; i < 8; i++)
        tactics[i] = nullptr;

    edgeEnabled = true;
    ultrasonicEnabled = true;
}

// =============== REGISTRATION ===========
void ProtonSumo::registerTactic(uint8_t slot, TacticFunction tactic)
{
    if (slot < 1 || slot > 8) return;
    tactics[slot - 1] = tactic;
}

// =============== RUN ====================
bool ProtonSumo::isRunning()
{
    return running;
}

void ProtonSumo::checkIR()
{
    if (!IrReceiver.decode())
        return;

    uint8_t cmd = IrReceiver.decodedIRData.command;

    if (cmd >= 0x01 && cmd <= 0x08)
    {
        selectedTactic = cmd;
    }

    if (cmd == 0x09)
    {
        running = true;
        digitalWrite(LED_BUILTIN, HIGH);
    }

    if (cmd == 0x0A)
    {
        running = false;
        stop();
        digitalWrite(LED_BUILTIN, LOW);
    }

    IrReceiver.resume();
}

void ProtonSumo::run()
{
    checkIR();

    if (!running) return;
    if (selectedTactic == 0) return;

    TacticFunction t = tactics[selectedTactic - 1];
    if (t != nullptr)
        t();
}

// ================= MOTORS =================
void ProtonSumo::setMotors(char lDir, int lPower,
                           char rDir, int rPower)
{
    lPower = constrain(lPower, 0, 100);
    rPower = constrain(rPower, 0, 100);

// NAPOMENA: Minimalna brzina motora je 55% od maksimalne brzine motora

    if (lPower > 0 && lPower < 55) lPower = 55;
    if (rPower > 0 && rPower < 55) rPower = 55;

    int pwmL = map(lPower, 0, 100, 0, 255);
    int pwmR = map(rPower, 0, 100, 0, 255);

    analogWrite(MOTOR_L_FWD, 0);
    analogWrite(MOTOR_L_BWD, 0);
    analogWrite(MOTOR_R_FWD, 0);
    analogWrite(MOTOR_R_BWD, 0);

    delayMicroseconds(50);


    // LEFT
    if (lDir == 'F')
    {
        analogWrite(MOTOR_L_FWD, pwmL);
        analogWrite(MOTOR_L_BWD, LOW);
    }
    else
    {
        analogWrite(MOTOR_L_BWD, pwmL);
        analogWrite(MOTOR_L_FWD, LOW);
    }

    // RIGHT
    if (rDir == 'F')
    {
        analogWrite(MOTOR_R_FWD, pwmR);
        analogWrite(MOTOR_R_BWD, LOW);
    }
    else
    {
        analogWrite(MOTOR_R_BWD, pwmR);
        analogWrite(MOTOR_R_FWD, LOW);
    }
}

void ProtonSumo::stop()
{
    analogWrite(MOTOR_L_FWD, 0);
    analogWrite(MOTOR_L_BWD, 0);
    analogWrite(MOTOR_R_FWD, 0);
    analogWrite(MOTOR_R_BWD, 0);
}

void ProtonSumo::forward(int s)  { setMotors('F', s, 'F', s); }
void ProtonSumo::backward(int s) { setMotors('B', s, 'B', s); }
void ProtonSumo::turnLeft(int s) { setMotors('B', s, 'F', s); }
void ProtonSumo::turnRight(int s){ setMotors('F', s, 'B', s); }

// ================= EDGE SENSOR ============
void ProtonSumo::enableEdgeSensor(bool state)
{
    edgeEnabled = state;
}

bool ProtonSumo::leftEdgeDetected()
{
    if (!edgeEnabled)
        return false;

    return !digitalRead(A0);
}

bool ProtonSumo::rightEdgeDetected()
{
    if (!edgeEnabled)
        return false;

    return !digitalRead(A1);
}

bool ProtonSumo::edgeDetected()
{
    return leftEdgeDetected() || rightEdgeDetected();
}
// ================= ULTRASONIC =============
void ProtonSumo::enableUltrasonic(bool state)
{
    ultrasonicEnabled = state;
}

float ProtonSumo::distance()
{
    if (!ultrasonicEnabled)
        return -1;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0)
        return -1;

    return duration * 0.034 / 2.0;
}

bool ProtonSumo::opponentDetected(float threshold)
{
    float d = distance();
    if (d < 0) return false;
    return d < threshold;
}

// ================= BEHAVIOR ================
void ProtonSumo::search()
{
    turnLeft(70);
}

void ProtonSumo::smartDelay(unsigned long ms)
{
    unsigned long start = millis();

    while (millis() - start < ms)
    {
        checkIR();

        if (!running)
        {
            stop();
            return;
        }

        delay(1);
    }
}

void ProtonSumo::escapeEdge()
{
    bool left = leftEdgeDetected();
    bool right = rightEdgeDetected();

    if (left && right)
    {
        backward(100);
        smartDelay(150);
        stop();
        return;
    }

    if (left)
    {
    backward(100);
    smartDelay(100);

    if (!running)
        return;

    turnRight(100);
    smartDelay(50);

    stop();
    }

    if (right)
    {
        backward(100);
        smartDelay(100);

        if (!running)
            return;

        turnLeft(100);
        smartDelay(50);

        stop();
        return;
    }
}