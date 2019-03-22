#include "mbed.h"
#include "SHA256.h"
#include "CW2_bitcoin.hpp"
#include "CW2_input.hpp"
#include "CW2_motor.hpp"
#include "CW2_output.hpp"



//Declare all the functions being made
void bitcoin_gen();
void makeMessage(uint8_t code, int32_t data);
void output_message();

//Check these priorities and write them in report
Thread incoming_message_thread(osPriorityAboveNormal, 1024);;
Thread bitcoin_thread(osPriorityBelowNormal, 1024);;
Thread recieve_message_t (osPriorityAboveNormal, 1024);;
Thread motorCtrlT(osPriorityNormal, 1024);


//Value for speed max to be set via input
//value is abc.c, needs to be a float
volatile float newVelocity = 0; //10 times the desired value

//Main
int main() {

    
    makeMessage(0, 0);
    //Run the motor synchronisation
    orState = motorHome();
    makeMessage(3, orState);
    //orState is subtracted from future rotor state inputs to align rotor and motor states

    //Poll the rotor state and set the motor outputs accordingly to spin the motor
    // Improved the rotor by creating 3 interrupts for each of the positions.
    I1.rise(&motor_pos);
    I2.rise(&motor_pos);
    I3.rise(&motor_pos);
    
    I1.fall(&motor_pos);
    I2.fall(&motor_pos);
    I3.fall(&motor_pos);
    
    pc.attach(&serialISR);
    
    incoming_message_thread.start(incoming_message);
    //bitcoin_thread.start(bitcoin_gen);
    recieve_message_t.start(output_message);
    motorCtrlT.start(motorCtrl);
    
    motor.period_us(pwm);
    motor.pulsewidth_us(pwm_width);
    
}


