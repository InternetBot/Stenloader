# Steganography Shellcode Loader

## Background

While working through Maldev Academy, a section covering payload storage in the `.rsrc`
section caught my attention. The `.rsrc` section is where Windows executables store
resources like icons and images, and the idea that a payload could be hidden there was
interesting to me.

It connected back to CTF competitions I participated in during college where steganography
challenges came up regularly. At the time it never occurred to me that the same concept
could be applied to payload delivery. This project is my attempt to understand how that
works in practice.

## What It Does

Takes a shellcode payload, encrypts it using XOR, and embeds the encrypted bytes into the
LSB of a BMP image file. The image looks visually identical to the original.

## What i have done so far

- Xor Encryption :Done:
- Write to LSB :Done:
- Read from LSB :Nope:
- Execute :Nope:
