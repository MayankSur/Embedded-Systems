#ifndef CW2_bitcoin_hpp
#define CW2_bitcoin_hpp


/////////////////////// EVERYTHING TO DO WITH BITCOIN MINING ////////////




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
     // Need to use Mutex to protect the update of the key   
    
        if (*key != newKey){
            newkey_mutex.lock();
            *key = newKey;
            newkey_mutex.unlock();
            }
            
            
        // Instanciate an object
        SHA256::computeHash(&hash[0],&sequence[0],sizeof(sequence));
        //Searching for a nonce
        if ((hash[0] == 0) && hash[1] == 0){
            //pc.printf("Nonce Found: %x\n\r", *nonce);
            //Replacing with makemessage function
            makeMessage(1, *nonce);
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
            makeMessage(2, hash_count);
            hash_count = 0;
        }
    }
}

/////////////////////// EVERYTHING TO DO WITH BITCOIN MINING ////////////

#endif