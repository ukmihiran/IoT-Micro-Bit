#include "main.hpp"


/*
 Function: SelectMode
 Operation: Handle the Send and Reciever Mode
 Input: N/A
 Outputr: Return 1 (Sender)
          Return 2 (Reciever)
*/
int select_Mode()
{
    uBit.serial.send("Press A button for Sender | Press B button For Reciever\n");

    while(true)
    {
        if(uBit.buttonA.isPressed())
        {
            return 1;

        }else if(uBit.buttonB.isPressed())
        {
            return 2;
        }
    }

    return 0;
}


/*
 Function: SaltGen()
 Operation: Generate Salt Value to Create DPK key
 Input: pointer variable salt
 Output: Salt Value 
*/
void SaltGen(char *salt)
{
        //Salt Characters
    const char *saltingletters = "abcdefghijklmnopqrstuvwyxz""ABCDEFGHIJKLMNOPQRSTUVWYXZ""0123456789";
    
    //Generate random number between 0 and 62, assign character based on output.
    for (int i = 0; i < SaltLength; i = i + 1){
        salt[i] = saltingletters[(int)(62.0 * rand()/(RAND_MAX + 1.0))];
    }
}


/*
Function: CreateDPK()
Operation: Create a SHA256 hash key for AES encryption using a secret & salt.
Input: Pointer variable secret, salt, dpk
Output: dpk hash value
*/
void CreateDPK(char *Secret, char *salt, char *dpk)
{
    char dpkInput[34] = {0};
    char temp[65];
    
    //Concatanate Password And Salt dpk = (Secret + salt)
    strcpy(dpkInput, Secret);
    strcat(dpkInput, salt);

    uBit.serial.send("Concat Password & Salt: ");
    uBit.serial.send(dpkInput);
    uBit.serial.send("\n");

    //Generate Sha256 Hash
    string dpkHash = SHA256(dpkInput); 
    //String to Char
    strcpy(temp, dpkHash.c_str());

    //Debug Sha256 Hash - Sha256 Produce 32byte output. AES128 needs a 16 byte key
    string Resize_Dpk = dpkHash;
    Resize_Dpk.resize(16);

    strcpy(dpk, Resize_Dpk.c_str()); //string to char

    uBit.serial.send("DPK Generated : ");
    uBit.serial.send(temp);
    uBit.serial.send("\n");
   // uBit.serial.send("Resize DPK : ");
   // uBit.serial.send(dpk);

}

/*
 Function: SendMode()
 Operation: Send commands to the other device
 Input: N/A
 Output: send encrypted commands to the reciever
*/
void SendMode()
{
    uBit.serial.send("Send Mode Activated: \n");

    //Calling to SaltGen Function
    char salt[SaltLength];

    SaltGen(salt);

    uBit.serial.send("Salt Generated:  ");
    uBit.serial.send(salt);
    uBit.serial.send("\n");
    
    //DPK Generate
    char dpk[7];
    CreateDPK(Secret, salt, dpk);

    //Send Salt Value to the Reciever
    uBit.radio.enable();
    
    uBit.sleep(500);
    //char encrypted_comm[100];

    uBit.serial.send("-----Command Mode------\n");

    while (1)
    {
        if (uBit.buttonAB.isPressed())
        {
            uBit.serial.send("Sending command 1: \n");

            SendCommand(command1, salt, dpk);

        }else if (uBit.buttonA.isPressed())
        {
            uBit.serial.send("Sending command 2: \n");
            SendCommand(command2, salt, dpk); 

        } else if (uBit.buttonB.isPressed())
        {
            uBit.serial.send("Sending command 3: \n");
            SendCommand(command3, salt, dpk);
        }
        
        uBit.sleep(500);
    }
}


/*
    Function: SendCommand
    Operation: Send Command to the Reciever
    Input: The command that will be sent to the reciever
*/
void SendCommand(const char *command, char *salt, char *dpk)
{
    char enc[100];
    char enc_dpk[28];

    uBit.serial.send("\n Command PlainText: ");
    uBit.serial.send(command);
    uBit.serial.send("\n");

    //AES 128bit Encryption
    string comm = AES_Encrypt(command, dpk); 

    //String to Char
    strcpy(enc, comm.c_str()); 

    uBit.serial.send("\n Encrypted command: ");
    uBit.serial.send(enc);
    uBit.serial.send("\n");

    //Concat encryption message + Salt
    strcpy(enc_dpk, enc);  
    strcat(enc_dpk, salt);

    uBit.serial.send("Encrypted Command + salt: \n");
    uBit.serial.send(enc_dpk);
    uBit.serial.send("\n");

    //Send data
    uBit.radio.datagram.send(enc_dpk);
}


/*
    Function: onData()
    Operation: Start recieving data from the sender. split the encrypted postion and salt value from received data.
    Input: Microbit event data.
    Output: Plaintext command.
*/
void onData(MicroBitEvent)
{
    uBit.serial.send("\n Start receiving Data\n");

    // Receive the message from the radio
    PacketBuffer b = uBit.radio.datagram.recv();

    // Obtain the length of the recived data
    int packet_len = b.length();

    // Get a pointer to byte array
    uint8_t *buf  = b.getBytes();

    //Add Null terminate the string.
    buf[packet_len] = '\0';

    //Read Buffer data
    char* data = reinterpret_cast<char*>(buf);

    uBit.serial.send("Data Recieved: ");
    uBit.serial.send(data);
    uBit.serial.send("\n");

    //Split Salt and Encrypoted message
    char encrypt_data[100], salt[6];

    data_split(data, 16, encrypt_data, salt); //split index is 16

    //Create DPK
    char dpk[7];
    CreateDPK(Secret, salt, dpk);

    //AES 128bit Encryption
    string comm = AES_Decrypt(encrypt_data, dpk);

    char command[100];
    strcpy(command, comm.c_str()); 

    uBit.serial.send("Encrypted Command: ");
    uBit.serial.send(encrypt_data);
    uBit.serial.send("\n");

    uBit.serial.send("Salt: ");
    uBit.serial.send(salt);
    uBit.serial.send("\n");

    uBit.serial.send("Command is: ");
    uBit.serial.send(command);
    uBit.serial.send("\n");

    // uBit.serial.send("Data Len: ");
    // uBit.serial.send(packet_len);

    //Services trigger
    if (strcmp(command, command1) == 0)
    {
        Fan_request();

    } else if (strcmp(command, command2) == 0)
    {
        compass();
    } else  if (strcmp(command, command3) == 0)
    {
        led_bulb();
    }

}

/*
    Function: Fan_request()
    Operation: Trigger fan service. the moter start to spin slowly then faster until it reaches a maximum speed.
    Input: N/A
    Output: Microbit pin0 electrical output to trigger fan
*/
void Fan_request()
{
    //initialise fans.
    uBit.serial.send("Fan ON\n");

    int Duty = 0;

    while (Duty < 1023) 
    {
        uBit.io.P0.setAnalogValue(Duty);
        Duty = Duty + 1;

        uBit.sleep(10);
    }
        while (Duty > 0)
    {
        uBit.io.P0.setAnalogValue(Duty);
        Duty = Duty - 1;
        uBit.sleep(10);
    }
}


/*
 Function Name: compass
 Operation: detect cardinal directions
 Input: N/A
 Output: Directions
*/
void compass()
{
  //initialise compass.
  int loop = 1;
  uBit.compass.calibrate();

  //Loop and display where the Microbit is currently pointing

  while(loop){

    //Exit functionality.
    if(uBit.buttonB.isPressed()){
      loop = 0;
    }

    //Currently only works for the four cardinal directions.
    int degrees = uBit.compass.heading();
        if (degrees < 45)
        {
            uBit.display.print("N");
        }
        else if (degrees <  135)
        {
            uBit.display.print("E");
        }
        else if (degrees < 225)
        {
            uBit.display.print("S");
        }
        else if (degrees < 315)
        {
            uBit.display.print("W");
        }
        else
        {
            uBit.display.print("N");
        }
        
        uBit.sleep(100);
  }
  uBit.display.scroll("EXIT");
 
}

/*
    Function Name: led_bulb()
    Operation: Turn On Led Bulb 
    Input: N/A
    Output: N/A
*/
void led_bulb()
{
    int loop = 1;

    uBit.io.P2.setDigitalValue(1); //turn on LED bulb

    while (1 == loop)
    {
        //Exit functionality.
        if(uBit.buttonA.isPressed()){
        uBit.io.P2.setDigitalValue(0); //Turn Off LED bulb
        loop = 0; //Exit Loop

        uBit.sleep(100);
    }
    
    }
     
}


/*
    Function: data_split()
    Operation: Split data buffer into salt and encrypt message
    Input: Data varibale, index number, variable to store encrypt data, variable to store Salt
    Output: Encrypt data and Salt value
*/
void data_split(char *s, int index, char *first, char *second)
{
    //Get length of the string
    int length = strlen(s);

    if (index < length)
    {
        for (int i = 0; i < index; i++) // first data postion (encrypted data)
        {
            first[i] = s[i]; //copy data up untill the index
            first[index] = '\0'; //terminate the index
            
        for (int i = index; i <= length; i++) //second data postion (Salt value)
        {
            second[i - index] = s[i]; // copy data to the second pointer array
        }
        
        }   
    }
}

/*
 Function: RecievMode()
 Operation: listening data comming from sender
 Input: N/A
 Output: N/A
*/
void ReceivMode()
{
    uBit.serial.send("Reciever Mode Activated: \n");

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    // Wait while Salt received from sender
    while(1) {
    uBit.sleep(300); // keep waiting to receive sault
    }

}

/*
 Function Name: Main()
 Operation: main is where execution begins
 Input: N/A
 Output: Mode selection (Sender || Receiver)
*/
int main()
{
    uBit.init();
    uBit.radio.setGroup(10);

    //Mode Select
    int mode = select_Mode();

    //Set Mode
    if(mode == 1)
    {
        SendMode();

    }else if(mode == 2)
    {
        ReceivMode();
    }

    release_fiber();
}

