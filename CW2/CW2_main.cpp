#include "mbed.h"
#include "SHA256.h"

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
const int8_t lead = 2;  //2 for forwards, -2 for backwards

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
void makeMessage(uint8_t code, uint32_t data);
void output_message();

//Declaring the structure for messages
typedef struct {
  uint8_t    code; /* AD result of measured voltage */
  uint32_t data; /* A counter value               */
} message_t;

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

void motor_pos(){
    //Base position
    int8_t orState = 0;
    int8_t intState = readRotorState();
    motorOut((intState-orState+lead+6)%6); //+6 to make sure the remainder is positive
    
}


/////////////////////// EVERYTHING TO DO WITH INPUT ////////////

Thread incoming_message_thread;

//Initialising a new Serial Class to deal with inputs
RawSerial pc(SERIAL_TX, SERIAL_RX);
Queue<void, 8> inCharQ;

void serialISR(){
 uint8_t newChar = pc.getc();
 inCharQ.put((void*)newChar);
}

void incoming_message(){
    makeMessage(4, 0);
    while(1){
        osEvent newEvent = inCharQ.get();
        uint8_t newChar = (uint8_t)newEvent.value.p;
        //Used to test if the character inputted was working
       // makeMessage(5, newChar);

    }
}
/////////////////////// EVERYTHING TO DO WITH INPUT ////////////

/////////////////////// EVERYTHING TO DO WITH BITCOIN MINING ////////////

//Creating a new thread for the bitcoin function
Thread bitcoin_thread;

//Variables needed for the BitCoin

uint8_t sequence[] = {0x45,0x6D,0x62,0x65,0x64,0x64,0x65,0x64,
0x20,0x53,0x79,0x73,0x74,0x65,0x6D,0x73,
0x20,0x61,0x72,0x65,0x20,0x66,0x75,0x6E,
0x20,0x61,0x6E,0x64,0x20,0x64,0x6F,0x20,
0x61,0x77,0x65,0x73,0x6F,0x6D,0x65,0x20,
0x74,0x68,0x69,0x6E,0x67,0x73,0x21,0x20,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint64_t* key = (uint64_t*)((int)sequence + 48);
uint64_t* nonce = (uint64_t*)((int)sequence + 56);
uint8_t hash[32];

void bitcoin_gen(){
    
    //Counter for number of hashes:
    uint8_t nonce_count = 0;
    uint8_t hash_count = 0;
    uint8_t hash_count_total = 0;
    
    //Time to help compare the hash rate
    int previous_time = 0;
    int current_time = 0;
    
    //Start a timer
    Timer count;
    count.start();
    
 //Start the while loop
    while(1){
    // Instanciate an object
    SHA256::computeHash(&hash[0],&sequence[0],sizeof(sequence));
    //Searching for a nonce
    if ((hash[0] == 0) && hash[1] == 0){
        //pc.printf("Nonce Found: %x\n\r", *nonce);
        //Replacing with makemessage function
        //makeMessage(1, *nonce);
        nonce_count++;
        }
    *nonce += 1;
    hash_count++;
    hash_count_total++;
    
    //Time calculation for 1 second rate
    
    current_time = count.read();

    //Some error with regards to nonce
    
    if ((current_time - previous_time) >= 1){
        previous_time = current_time;

        //pc.printf("Hash Rate : %d\n\r", hash_count);
        //makeMessage(2, hash_count);
        hash_count = 0;
    }
    }
}

/////////////////////// EVERYTHING TO DO WITH BITCOIN MINING ////////////



/////////////////////// EVERYTHING TO DO WITH OUTPUT MESSAGES ////////////
//Start to design the mailbox
Mail<message_t, 16> out_message;


//Create a thread to output messages
Thread recieve_message_t;


//Create a function that allocates space for a message on the mail box
void makeMessage(uint8_t code, uint32_t data){
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
                pc.printf("New Character inputted: %c\n\r", o_message->data);
                break;
            default:
                pc.printf("======================Unexpected Input!====================");
                    break;
            
        }
        out_message.free(o_message);
        }
    }


/////////////////////// EVERYTHING TO DO WITH OUTPUT MESSAGES ////////////
    
//Main
int main() {
    int8_t orState = 0;    //Rotot offset at motor state 0
    
    //Initialise the serial port
    //Serial pc(SERIAL_TX, SERIAL_RX);
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
    bitcoin_thread.start(bitcoin_gen);
    recieve_message_t.start(output_message);
    
    
}
