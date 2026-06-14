#ifndef PROTON_SUMO_H
#define PROTON_SUMO_H

#include <Arduino.h>

typedef void (*TacticFunction)();

class ProtonSumo {
public:
    void begin();
    void run();

    void registerTactic(uint8_t slot, TacticFunction tactic);

    bool isRunning();

    // Motor control
    void setMotors(char leftDir, int leftPower,
                   char rightDir, int rightPower);

    void stop();

    void forward(int speed);
    void backward(int speed);
    void turnLeft(int speed);
    void turnRight(int speed);

    // Sensors
    bool leftEdgeDetected();
    bool rightEdgeDetected();
    bool edgeDetected();
    float distance();
    bool opponentDetected(float threshold = 30);

    // High level behavior
    void search();
    void escapeEdge();

    // Sensor toggles
    void enableEdgeSensor(bool state);
    void enableUltrasonic(bool state);

    // Minimize blocking delays while still checking IR commands
    void smartDelay(unsigned long ms);

private:
    void checkIR();

    TacticFunction tactics[8];

    bool running = false;
    uint8_t selectedTactic = 0;

    bool edgeEnabled = true;
    bool ultrasonicEnabled = true;
};

extern ProtonSumo Sumo;

#endif