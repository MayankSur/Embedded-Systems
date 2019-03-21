#ifndef CW2_motor_hpp
#define CW2_motor_hpp



extern Thread motorCtrlT;
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
int R_O;
//SPEED
#define K_PS 8.0
#define K_IS 10.5

//ROTATION
#define K_PR 30.0
#define K_DR 38.0
#define PWM_LIMIT 2000.0


bool velEnter = false;
bool rotEnter = false;

float error_speed;
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
    //Put the motor in drive state 2 and wait for it to stabilise
    motorOut(2);
    wait(3.0);
    
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
        error_speed = speedMaxInt - abs(velocity);

        //Sign Calculation
        sign = (velocity<0) ? -1 : 1;  
        
                
        //Only consider the integral part when close to the target velocity
        if (error_speed < 0.15*speedMaxInt){
            //Integral Calculation
             integral_error += ((error_speed));
            if((integral_error > 15)){ integral_error = 15;}
            if((integral_error < -15)){ integral_error = -15;}
        }
                
      //Set e_s then set kp then torque
        torque = (K_PS*(error_speed) +  (K_IS*integral_error))* sign;
        //Check for positive/negative lead
        lead = (torque <0) ? -2: 2;

        //Output for PWM
        output = (abs(torque) < 2000) ? (torque) : 2000;
        return abs(output);

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
                  
            //makeMessage(13, integral_error);
            makeMessage(9, velocity);
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
            if (speedMaxInt == 0){
                pwm_width = 2000;
            }
            else{
        pwm_width = velocityControl();
        }
        }
        if (rotEnter){
            if (R_O == 0){
            lead = 2;
            }
            else{
            pwm_width = rotationControl();
            }
        }
        else if(velEnter&& rotEnter) {
            v = velocityControl();
            r = rotationControl();
            pwm_width =(velocity >= 0) ? min(r, v) : max(r, v);
          }

        lastPosition = motor_position;
        }
    }


#endif