# LSB Image Steganography

A command-line application developed in C that implements image steganography using the Least Significant Bit (LSB) technique. The program enables users to securely hide secret text inside BMP images and retrieve the hidden information when required.

## Features

* Encode secret text into a 24-bit BMP image.
* Decode and recover hidden information from a stego image.
* Verify user authenticity using a magic string mechanism.
* Perform image capacity validation before encoding.
* Support binary file operations for image processing.
* Generate a separate stego image containing the embedded message.
* Detect invalid files and incorrect decoding parameters.
* Modular implementation for better readability and maintenance.

## Encoding and Decoding Process

### Encoding

* Reads the source image and secret text file.
* Verifies whether the image can accommodate the secret data.
* Encodes the magic string, file information, and secret message into image pixels using the LSB method.
* Creates a new stego image containing the hidden data.

### Decoding

* Reads the stego image.
* Extracts and verifies the embedded magic string.
* Retrieves the hidden message from the image.
* Stores the recovered data in an output file.

## Working Principle

The application modifies the least significant bits of image pixel data to embed information without causing noticeable visual changes to the image. During decoding, the same bits are read and reconstructed to recover the original message.

## Project Structure

├── main.c        # Program entry and operation selection

├── encode.c      # Encoding implementation

├── encode.h      # Encoding declarations

├── decode.c      # Decoding implementation

├── decode.h      # Decoding declarations

├── common.h      # Common definitions and macros

├── types.h       # User-defined data types

├── beautiful.bmp # Source image

├── secret.txt    # Secret message input

├── stego.bmp     # Encoded output image

└── output.txt    # Decoded output file

## Technologies Used

* C Programming
* File Handling
* Structures
* Pointers
* Bitwise Operations
* String Manipulation
* Command-Line Arguments

## Key Learnings

* Learned the fundamentals of image steganography.
* Implemented data hiding using the LSB technique.
* Gained experience with binary image processing.
* Improved understanding of bitwise manipulation and file handling.
* Strengthened debugging and modular programming skills.
