/*
Name: Risha A
Admission ID:25040A_011

Description:

This project implements Image Steganography using C language to hide and retrieve secret data inside a BMP image using LSB technique.

Encoding:
The program takes a source BMP image and a secret file as input. 
It embeds the secret data into the image by modifying the least significant bits and generates a stego image.

Decoding:
The program extracts the hidden data from the stego image. 
It retrieves the secret file by reading the encoded bits and reconstructing the original data.

The project uses file handling, structures, and bitwise operations, along with proper validation of inputs.
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("ERROR: Insufficient arguments\n");
        printf("Usage: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("       ./a.out -d stego.bmp\n");
        return 1;
    }

    OperationType op = check_operation_type(argv[1]);  //        //step 1-> call check_operation_type(argv[1])


    if (op == e_encode)
    {
        printf("INFO: Encoding selected\n");            //e_encode -> do encoding


        EncodeInfo encInfo;        //(e_encode)  step 2-> declare strcture variable EncodeInfo encInfo;


        if (read_and_validate_encode_args(argv, &encInfo) == e_failure)  //step 3 -> call read_and_validate_encode_args(argv, &encInfo)

        {
            printf("ERROR: Invalid encode arguments\n");              // e_failure -> print error msg stop the program

            return 1;
        }
        printf("INFO: Arguments validated successfully\n");             // e_success -> print success msg and goto next step


        if (do_encoding(&encInfo) == e_failure)        //step 4 -> check  do_encoding(&encInfo) is returning e_success or not

        {
            printf("ERROR: Encoding failed\n");
            return 1;
        }

        printf("INFO: Encoding completed successfully\n");
    }
    else if (op == e_decode)
    {
        printf("INFO: Decoding selected\n");

        if (argc < 3)
        {
            printf("ERROR: Please provide stego image\n");
            return 1;
        }

        DecodeInfo decInfo;

        decInfo.stego_image_fname = argv[2];

        if (do_decoding(&decInfo) == e_failure)
        {
           printf("ERROR: Decoding failed\n");
           return 1;
        }

       printf("INFO: Decoding completed successfully\n");
    }
    else
    {
        printf("ERROR: Unsupported operation\n");            //e_unsupported -> print error msg stop program

        printf("Usage: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("       ./a.out -d stego.bmp\n");
    }

    return 0;
}

OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)    // step 1-> check it is -e or not

    {
        return e_encode;            // t -> return e_encode

    }

    if (strcmp(symbol, "-d") == 0)    // step 2-> check it is -d or not

    {
        return e_decode;            // t -> return e_decode

    }

    return e_unsupported;    // step 3-> return e_unsupported

}