#ifndef CW2_input_hpp
#define CW2_input_hpp


#include "CW2_output.hpp"

////EXTERNALS
extern volatile float newVelocity;
extern float pwm_width;
extern int motor_position;
extern float rotations;
extern float speedMaxInt; 
extern RawSerial pc;
extern PwmOut motor;
extern bool velEnter;
extern bool rotEnter;
extern float integral_error;
extern int R_O;

/////////////////////// EVERYTHING TO DO WITH INPUT ////////////


//TESTING FOR OUTPUT//


//Character input buffer
char Buffer[20];
int buffer_index = 0;


Queue<void, 8> inCharQ;

//New Key MUTEX Variable and NewKey variable
uint64_t newKey = 0;
Mutex newkey_mutex;


void serialISR(){
 uint8_t newChar = pc.getc();
 inCharQ.put((void*)newChar);
}

void incoming_message(){
    while(1){
        test.write(1);
        //Collects the Input
        osEvent newEvent = inCharQ.get();
        uint8_t newChar = (uint8_t)newEvent.value.p;

        //Checks for overflow
        
        if (buffer_index > 20){
            buffer_index =0;
        }
        else{
           Buffer[buffer_index] = newChar;
            buffer_index++;
       }

       //Need to deal with the new line character
       if (newChar == '\r'){
            Buffer[buffer_index] = '\0';
            //Reset for the next command
           
           buffer_index = 0;
      
           switch(Buffer[0]){
                case 'K':
                    newkey_mutex.lock();
                    sscanf(Buffer, "K%x", &newKey);
                    newkey_mutex.unlock();
                    makeMessage(4,newKey >> 32);
                    makeMessage(5,newKey);
                    break;
                case 'V':
                    //matches V, followed by 1-3 digits, followed by an optional decimal point then digit
                    sscanf(Buffer, "V%f",&newVelocity);
                    if ((newVelocity < 999.99f) && (newVelocity > 0)){ 
                    speedMaxInt = newVelocity*6;
                    integral_error= 0;
                    velEnter = true;
                    makeMessage(8,speedMaxInt);
                    }
                    break;
                case 'R':
                    //matches Rotational Speed, followed by 1-3 digits, followed by an optional decimal point then digit
                    sscanf(Buffer, "R%f", &rotations);
                    R_O = rotations;
                     if ((rotations < 999.99f) && (rotations > -999.99f)){ 

                    //Sets the number of rotations extra
                    rotations += float(motor_position)/6;
                    rotEnter = true;
                    makeMessage(9, R_O);
                    }
                    break;
                default:
                    break;

            }
            
       }


    }
}
/////////////////////// EVERYTHING TO DO WITH INPUT ////////////

#endif