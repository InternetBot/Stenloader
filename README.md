# Steganography Shellcode Loader

This project demonstrates that a payload can be hidden inside a BMP file. 
I made this after completing the first few modules on Maldev Academy. The 
`.rsrc` section module, where they show how payloads can be stored in PE 
resources, kind of reminded me of my college CTF days where flags used to 
be hidden inside image metadata. I did some research and realized I could 
write a payload directly into a `.bmp` file. I am pretty sure this can be 
done with any image format but for this project I focused on bitmap images.

## How It Works
![HighLevelFlow](images/research_shellcode.png)

## XOR Encryption
XOR encryption is pretty straightforward. The same function both encrypts 
and decrypts the shellcode applying XOR twice with the same key restores 
it to the original value. In our case the key is `0xBD`. The key can be 
changed but I just stuck with `0xBD` due to its nature which will make more 
sense as we go further. Also possibly try to avoid using `0xFE` as your key 
since that is what is used to clear the LSB (Least Significant Bit) during 
the embedding process.

## BMP File
A BMP file has 3 sections but we only care about the third one which is the 
Pixel Data. The three sections are the File Header, DIB Header, and Pixel Data.

Reference: https://engineering.purdue.edu/ece264/16au/hw/HW13

At some point in the code you will see me reference the pixel data offset 
from the header. The reason for that is the pixel data offset is stored at 
byte 10 in the File Header. So at byte 10 it basically says "hey, the pixel 
data starts at byte 54" and the code jumps straight there. Check the snippet 
below. 
```c
DWORD dwPixelOffset = *(DWORD*)(pBmpBuffer + 10);
// dwPixelOffset = 54
```
##LSB Stenography
Now this is the fun part. LSB steganography hides data inside the Least 
Significant Bit of each pixel byte. Remember a pixel byte is made up of 
8 bits so changing the very last bit makes a difference that is completely 
invisible to the human eye. So let's say we have a pixel byte like this:

![lsb](images/lsb.png)

We can change the Least Significant Bit from 1 to 0 and there will be no 
visual change to the image whatsoever.

![HighLevelFlow](images/lsb1.png)
