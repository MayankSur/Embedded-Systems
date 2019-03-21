#ifndef CW2_output_hpp
#define CW2_output_hpp


RawSerial pc(SERIAL_TX, SERIAL_RX);

//Externals
extern int motor_position;


/////////////////////// EVERYTHING TO DO WITH OUTPUT MESSAGES ////////////

//Declaring the structure for messages
typedef struct {
  uint8_t    code; /* AD result of measured voltage */
  int32_t data; /* A counter value               */
} message_t;

//Start to design the mailbox
Mail<message_t, 16> out_message;


//Create a function that allocates space for a message on the mail box
void makeMessage(uint8_t code, int32_t data){
    message_t *message = out_message.alloc();
    message->code = code;
    message->data = data;
    out_message.put(message);
    }

void output_message(){
    while(1){
        osEvent evt = out_message.get();
        message_t *o_message = (message_t*)evt.value.p;
            
        switch(o_message->code){
            case 0:
                pc.printf("========================= Welcome! ===========================\n\r");
                break;
            case 1:
                pc.printf("Nonce Found: %x\n\r", o_message->data);
                break;
            case 2:
                pc.printf("Hashrate per second: %d\n\r", o_message->data);
                break;
            case 3:
                pc.printf("Rotor origin: %x\n\r", o_message->data);
                break;
            case 4:
                pc.printf("========================= Input Thread ON! ===========================\n\r");
                break;
            case 5:
                pc.printf("Torque Changed %u\n\r", o_message->data);
                break;
            //Both for printing the key    
            case 6:
                pc.printf("New Key: %x", o_message->data);
                break;
            case 7:
                pc.printf("%x\n\r", o_message->data);
                break;
            case 8:
                pc.printf("Motor Position%d\n\r", motor_position);
                break;
            case 9:
                pc.printf("Motor Velocity%d\n\r", (o_message->data)/6);
                break;
            case 10:
                pc.printf("New Motor Velocity %d\n\r",o_message->data);
                break;
            case 12:
                pc.printf("Current PWM value %u\n\r",o_message->data);
                break;
            case 11:
                pc.printf("No. Rotations %d\n\r",o_message->data);
                break;
            case 13:
                pc.printf("New Torque %d\n\r",o_message->data);
                break;
            case 14:
                pc.printf("New Integral %d\n\r",o_message->data);
                break;
            default:
                pc.printf("======================Unexpected Input!====================");
                    break;
            
        }
        out_message.free(o_message);
        }
    }


/////////////////////// EVERYTHING TO DO WITH OUTPUT MESSAGES ////////////

#endif