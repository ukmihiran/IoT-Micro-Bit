# IoT-Micro-Bit
Lightweight Authentic Wireless Communications for Micro:Bit  IoT Device

## About Project
UWE coursework assessment for IoT security to implement an authentication system and encrypted wireless communications between two Micro:bits using the nRF radio (uBit.radio) and C/C++ programming language.

The program has 3 commands and a single secret code that will be shared with other device. A ‘command’ here is defined as any action that can be performed using a Micro:bit device. the sender can select the command using the Micro:bit buttons and the system will encrypt the command with AES (aes_enc) and transmit it to the other Micro:bit. 

The second Micro:bit (receiver) can receive the encrypted message decrypt it (aes_dec), then authentically identify the “command” and execute it.

## Functions

### Sender
- Generate a random salt.
- Generate a data encryption key using the SHA hash function algorithm applied to the shared secret and salt as follows: dpk=sha256(secret +salt)
- Use AES to encrypted the command with dpk: cipher=aes_enc(command ,dpk). The command here is the message that identify the command.
- Send (cipher, salt) to the receiver Micro:bit via the radio.

### Receiver

- Receive cipher and salt.
- Generate a data encryption key using SHA hash of the shared secret and salt, dpk=sha256 (secret+salt).
- Decrypt the cipher, command=aes_dec(cipher, dpk)
- Run the command