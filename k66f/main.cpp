#include <cstdio>
#include <cstring>
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "PinNames.h"
#include "PwmOut.h"
#include "ThisThread.h"
#include "Thread.h"
#include "mbed.h"
#include "SRF05.h"

// Motor
#define FULL_SPEED 1.0f
#define MIN_SPEEED 0.7f
#define DEFAULT 0.85f


PwmOut motorL(D5); // enA
DigitalOut leftFront(D6); // in1
DigitalOut leftRear(D7); // in2

PwmOut motorR(D8); // enB
DigitalOut rightFront(D9); // in3
DigitalOut rightRear(D10); // in4

class Motor {        
    public:
    Motor() {
        motorL.period(0.0005); // Set the PWM period to 0.5 milliseconds (2000 Hz)
        motorR.period(0.0005); // Set the PWM period to 0.5 milliseconds (2000 Hz)
        printf("motor running\r\n");
    }

    void move(float speed, float turn = 0.0f, int time_ms = 0) {
        float k = (0.3*turn)/0.90;

        float leftSpeed = DEFAULT - k; 
        float rightSpeed = DEFAULT + k;
        
        // Clamp speeds to [-1, 1]
        leftSpeed = std::min(std::max(leftSpeed, -1.0f), 1.0f); 
        rightSpeed = std::min(std::max(rightSpeed, -1.0f), 1.0f); 
        
        // Set motor directions
        leftFront = leftSpeed > 0 ? 1 : 0;
        leftRear = leftSpeed < 0 ? 1 : 0;
        rightFront = rightSpeed < 0 ? 1 : 0;
        rightRear = rightSpeed > 0 ? 1 : 0;
        
        // Set motor speeds
        motorL.write(std::abs(leftSpeed));
        motorR.write(std::abs(rightSpeed));
        
        // Delay if a time is specified
        if (time_ms > 0) {
            ThisThread::sleep_for(chrono::milliseconds(time_ms));
            // printf("sleeping..\n");
            stop(); // Optionally stop motors after moving
        }
    }

    void stop(){
        leftFront = 0;
        leftRear = 0;
        rightFront = 0;
        rightRear = 0;
    }

};


// Global
#define UART_TX PTC4
#define UART_RX PTC3
static BufferedSerial serial_port(UART_TX, UART_RX, 9600);
Motor motor;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
bool motorStop = false;

// Thread Func
void ultrasonic();
void ldr();
void moveCar();


// Ultrasonic Global
SRF05 srf(PTA1, PTA9);
PwmOut buzzer(PTC8);
Mutex distanceMutex;
volatile float sensorDistance = 0.0;

// LDR Global
DigitalIn sensorIn(D2);
DigitalOut lightout(PTA27);


// Helper
bool parse(const char* command, float &speed, float &turn, int &time);
void interruptStopMotor(){ 
    motor.stop();
}
void sensor_thread();
void uartSend(const char* msg);


int main()
{
    // Threads
    Thread movement;
    Thread sensor_ultrasonic;
    Thread sensor_light;
    

    movement.start(moveCar);
    sensor_ultrasonic.start(ultrasonic);
    sensor_light.start(ldr);

    queue.dispatch_forever(); 
}

// Gets data from esp32, use comma as delimiter and moves the motor accordingly
bool parse(const char* command, float &speed, float &turn, int &time) {
    int parsedCount = sscanf(command, "%f, %f, %d", &speed, &turn, &time);
    return (parsedCount == 3);
}

void moveCar(){
    char buffer[101]; // Buffer for storing up to 100 characters + null terminator
    DigitalOut led(PTC9);
   
    printf("Started...\r\n");
    for(int i = 0; i < 3; i++){
        led = !led;
        ThisThread::sleep_for(500ms);
    }

    float speed = 0.0f, turn = 0.0f; int time = 0;

    while (true) {
        if(serial_port.readable()){

            int bytesRead = serial_port.read(buffer, 100);
        
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; // Null-terminate the received string
                ThisThread::sleep_for(300ms);

                if (bytesRead > 1){
                    speed = 0.0f; turn = 0.0f; time = 0;
                    
                    if (parse(buffer, speed, turn, time)){
                        motor.move(speed, turn, time); 
                    }
                }
            
            }
        }         
    }
}


// Thread for sensor
void sensor_thread() { 

    while (true) {
        // Read distance and store it safely
        float localDistance = 0;
        distanceMutex.lock();
        sensorDistance = srf.read();  // Assume this function returns a float representing the distance
        localDistance = sensorDistance;
        distanceMutex.unlock();

        // Sends data to the ESP,which is updated in the web dashboard
        int lightStatus = lightout.read();
        char buffer[64];
        
        int bytes_written = snprintf(buffer, sizeof(buffer), "%d,%.2f\n", lightStatus, localDistance);
        if (bytes_written > 0) {
            serial_port.write(buffer, bytes_written); // Send the buffer over serial
            ThisThread::sleep_for(20ms);
        }
            
        ThisThread::sleep_for(30ms);
    }
}

void ultrasonic(){
    Thread sensorThread;
    sensorThread.start(sensor_thread);

    Timer timer;
    buzzer.period(1.0 / 261.63);
    buzzer.write(0.5);
    
    while (true) {
        float localDistance;

        // Safely copy distance
        distanceMutex.lock();
        localDistance = sensorDistance;
        distanceMutex.unlock();
        //printf("reading data %3.3f \r\n", localDistance);

        if(localDistance < 20.0){
            queue.call(interruptStopMotor);
        }
        else if (localDistance < 25.0) {
            buzzer.period(1.0 / 261.63);  // Set buzzer to C4
            buzzer.write(0.7);  // 50% duty cycle
            ThisThread::sleep_for(200ms);
            buzzer.write(0);  // Turn off the buzzer
        } else if (localDistance < 35.0) {
            buzzer.period(1.0 / 329.63);  // Set buzzer to E4
            buzzer.write(0.7);  // 50% duty cycle
            ThisThread::sleep_for(400ms);
            buzzer.write(0);  // Turn off the buzzer
        } else {
            buzzer.write(0);  // Ensure the buzzer is off
        }
    }
}

void ldr(){
    while (true) {
        if (sensorIn.read() == 1) {
            //printf("read 1\r\n");
            lightout.write(0);
        } else {
            //printf("read 0\r\n");
            lightout.write(1);
        }

        ThisThread::sleep_for(500ms);

    }
}