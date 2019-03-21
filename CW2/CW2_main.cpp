#include "mbed.h"
#include "SHA256.h"
#include "CW2_input.hpp"
#include "CW2_bitcoin.hpp"
#include "CW2_output.hpp"



//Photointerrupter input pins
#define I1pin D3
#define I2pin D6
#define I3pin D5

//Incremental encoder input pins
#define CHApin   D12
#define CHBpin   D11

//Motor Drive output pins   //Mask in output byte
#define L1Lpin D1           //0x01
#define L1Hpin A3           //0x02
#define L2Lpin D0           //0x04
#define L2Hpin A6          //0x08
#define L3Lpin D10           //0x10
#define L3Hpin D2          //0x20

#define PWMpin D9

//Motor current sense
#define MCSPpin   A1
#define MCSNpin   A0

//Mapping from sequential drive states to motor phase outputs
/*
State   L1  L2  L3
0       H   -   L
1       -   H   L
2       L   H   -
3       L   -   H
4       -   L   H
5       H   L   -
6       -   -   -
7       -   -   -
*/
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};

//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};  
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

//Phase lead to make motor spin
int8_t lead = 2;  //2 for forwards, -2 for backwards

//Status LED
DigitalOut led1(LED1);

//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);

//Motor Drive outputs
DigitalOut L1L(L1Lpin);
DigitalOut L1H(L1Hpin);
DigitalOut L2L(L2Lpin);
DigitalOut L2H(L2Hpin);
DigitalOut L3L(L3Lpin);
DigitalOut L3H(L3Hpin);

//Declare all the functions being made
void bitcoin_gen();
void makeMessage(uint8_t code, int32_t data);
void output_message();

//Check these priorities and write them in report
Thread incoming_message_thread(osPriorityAboveNormal, 1024);;
Thread bitcoin_thread(osPriorityBelowNormal, 1024);;
Thread recieve_message_t (osPriorityAboveNormal, 1024);;
Thread motorCtrlT(osPriorityNormal, 1024);


//Declaring the structure for messages
typedef struct {
  uint8_t    code; /* AD result of measured voltage */
  int32_t data; /* A counter value               */
} message_t;

//Start to design the mailbox
Mail<message_t, 16> out_message;


//Value for speed max to be set via input
//value is abc.c, needs to be a float
volatile float newVelocity = 0; //10 times the desired value


////////////Controling the motor PWM ACTIVATE ////////////////

//Define a PWMOut Class
PwmOut motor(PWMpin);

int motor_position = 0;
uint8_t previous_state = 0 ;
float velocity = 0;
float prev_velocity;
int lastPosition = 0;
uint32_t orState = 0;
int8_t sign;

int pwm = 2000;
float pwm_width = 2000;


//These need to be set by the user
float rotations = 0;
float speedMaxInt = 0;

#define K_PS 8.0
#define K_IS 10.0
#define K_PR 30.0
#define K_DR 38.0
#define PWM_LIMIT 2000.0


bool velEnter = false;
bool rotEnter = false;

float error;
float torque;

float error_term;
float diff_error = 0;
float previous_error = 0;
float integral_error = 0;

float v,r;


//////////////////////////////////////////////////////////////

//Set a given drive state
void motorOut(int8_t driveState){
    
    //Lookup the output byte from the drive state.
    int8_t driveOut = driveTable[driveState & 0x07];
      
    //Turn off first
    if (~driveOut & 0x01) L1L = 0;
    if (~driveOut & 0x02) L1H = 1;
    if (~driveOut & 0x04) L2L = 0;
    if (~driveOut & 0x08) L2H = 1;
    if (~driveOut & 0x10) L3L = 0;
    if (~driveOut & 0x20) L3H = 1;
    
    //Then turn on
    if (driveOut & 0x01) L1L = 1;
    if (driveOut & 0x02) L1H = 0;
    if (driveOut & 0x04) L2L = 1;
    if (driveOut & 0x08) L2H = 0;
    if (driveOut & 0x10) L3L = 1;
    if (driveOut & 0x20) L3H = 0;
    }
    
    //Convert photointerrupter inputs to a rotor state
inline int8_t readRotorState(){
    return stateMap[I1 + 2*I2 + 4*I3];
    }

//Basic synchronisation routine    
int8_t motorHome() {
    //Put the motor in drive state 0 and wait for it to stabilise
    motorOut(0);
    wait(2.0);
    
    //Get the rotor state
    return readRotorState();
}

 //Upgrade motor_pos to work for the Motor ISR 
void motor_pos(){
    //Base position
    int8_t intState = readRotorState();
    motorOut((intState-orState+lead+6)%6); //+6 to make sure the remainder is positive
    
    //Cases for comparing values from 0-6 or 
    if (intState - previous_state == 5) motor_position--;
    else if (intState - previous_state == -5) motor_position++;
    else motor_position += (intState - previous_state);
    previous_state = intState;
}

void motorCtrlTick(){
    //Sets a signal for the thread
    motorCtrlT.signal_set(0x1);
    }
int output = 0;
int velocityControl(){
        output = 0;
        // Error term
        error = speedMaxInt - abs(velocity)
        
        //Case for speed entered being 0 - Set speed to maximum
        
        if (speedMaxInt == 0){
            //Full power 
            output = 2000;
            return output;
            }
            
        sign = (velocity<0) ? -1 : 1;  
        integral_error += K_IS*((error));
        
        if(integral_error > 600) integral_error = 600;
        if(integral_error < (-600)) integral_error =-600;
        makeMessage(14, integral_error);
                   
      //Set e_s then set kp then torque
        torque = (K_PS*(error) + integral_error) * sign;

        //Check for positive/negative lead
        //Sets the lead based on the maximum speed inputted
        lead = (torque <0) ? -2: 2;
        
        //Output for PWM
        output = (abs(torque) < 4000) ? (torque/2) : 2000;
        return output;

    }
    
    
int rotationControl(){
    error_term = rotations - (float(motor_position)/6);  
      
    diff_error = error_term - previous_error; 
    //Check this 
    integral_error = (error_term + previous_error)/2;
    
    previous_error = error_term;
    
    output = (K_PR*error_term) + (K_DR*diff_error);
    
    lead = (output <0) ? -2: 2;
    
    output = (abs(output) < 4000) ? (output/2) : 2000;
    
    return abs(output);
    
    }
    

void motorCtrl(){
    //Creates a ticker to be used to triggere interrupts in a timed interval
    Ticker motorCtrlTicker;
    motorCtrlTicker.attach_us(&motorCtrlTick, 100000);
    //Counter for the 10 iterations
    int i = 0;
    while(1){
        motorCtrlT.signal_wait(0x1);
        
        //Potential may need to use critical region for this calculation
        
        if(i < 10){
            i++;
        }else{
            i = 0;
            //Outputs the velocity and motor position every 10 iterations 
            makeMessage(9, velocity);

            if (velocity == 0){
                pwm_width = 2000;
                }
            makeMessage(8, motor_position);
            
        //Needs to be done last
        }
 //calculation for velocity + Critcal Section used since the value of motor position can change due to an intterupt
        core_util_critical_section_enter();
        velocity = (motor_position - lastPosition)*10;
        core_util_critical_section_exit();
        prev_velocity = velocity;
        //Need to call motor velocity 
        if (velEnter){
        pwm_width = velocityControl();
        }
        if (rotEnter){
        pwm_width = rotationControl();
        }
        else if(velEnter&& rotEnter) {
        v = velocityControl();
        r = rotationControl();
        pwm_width =(velocity >= 0) ? min(r, v) : max(r, v);
          }

        
        lastPosition = motor_position;
        }
    }
 
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

