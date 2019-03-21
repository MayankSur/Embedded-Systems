#ifndef CW2_input_hpp
#define CW2_input_hpp

/////////////////////// EVERYTHING TO DO WITH INPUT ////////////


//Character input buffer
char Buffer[20];
int buffer_index = 0;

RawSerial pc(SERIAL_TX, SERIAL_RX);
Queue<void, 8> inCharQ;

//New Key MUTEX Variable and NewKey variable
uint64_t newKey = 0;
Mutex newkey_mutex;


void serialISR(){
 uint8_t newChar = pc.getc();
 inCharQ.put((void*)newChar);
}

void incoming_message(){
    //Message to Start Input Queue
    makeMessage(4, 0);
    while(1){
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
       // Indicates end of commands
       if (newChar == '\r'){
            Buffer[buffer_index] = '\0';
            //Reset for the next command
           
           buffer_index = 0;
      
           switch(Buffer[0]){
                case 'K':
                    newkey_mutex.lock();
                    sscanf(Buffer, "K%x", &newKey);
                    //Print the whole Key - since it's 32 bits each so send in two bits 
                    newkey_mutex.unlock();
                    makeMessage(6,newKey >> 32);
                    makeMessage(7,newKey);
                    break;
                case 'T':
                    sscanf(Buffer, "T%u", &pwm_width);
                    motor.pulsewidth_us(pwm_width);
                    makeMessage(5,pwm_width);
                    break;
                case 'V':
                    //matches V, followed by 1-3 digits, followed by an optional decimal point then digit
                    sscanf(Buffer, "V%f",&newVelocity);
                    speedMaxInt = newVelocity;
                    velEnter = true;
                    makeMessage(10,speedMaxInt);
                    break;
                case 'R':
                    //matches Rotational Speed, followed by 1-3 digits, followed by an optional decimal point then digit
                    sscanf(Buffer, "R%f", &rotations);
                    //Sets the number of rotations extra
                    rotations += float(motor_position)/6;
                    rotEnter = true;
                    
                    //used to test the position of the rotation
//                    makeMessage(11, rotations);
//                    makeMessage(8, motor_position);
                    break;
                default:
                    break;

            }
            
       }
       

    }
}
/////////////////////// EVERYTHING TO DO WITH INPUT ////////////


#endif