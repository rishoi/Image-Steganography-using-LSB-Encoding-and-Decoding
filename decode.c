#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"

/* Open files */
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR opening stego image\n");
        return e_failure;
    }
    return e_success;
}

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8];
    char str[3];

    for (int i = 0; i < 2; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        str[i] = decode_byte_from_lsb(buffer);
    }
    str[2] = '\0';

    if (strcmp(str, MAGIC_STRING) == 0)
        return e_success;
    else
        return e_failure;
}

/* Decode extension size */
Status decode_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);

    decInfo->extn_size = decode_size_from_lsb(buffer);
    return e_success;
}

/* Decode extension */
Status decode_extn(DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->extn[i] = decode_byte_from_lsb(buffer);
    }
    decInfo->extn[decInfo->extn_size] = '\0';

    return e_success;
}

/* Decode file size */
Status decode_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);

    decInfo->file_size = decode_size_from_lsb(buffer);
    return e_success;
}

/* Decode file data */
Status decode_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    sprintf(decInfo->output_fname, "output%s", decInfo->extn);

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR opening output file\n");
        return e_failure;
    }

    for (int i = 0; i < decInfo->file_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        ch = decode_byte_from_lsb(buffer);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}

/* Decode 1 byte */
char decode_byte_from_lsb(char *buffer)
{
    char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = ch | ((buffer[i] & 1) << (7 - i));
    }

    return ch;
}

/* Decode 32-bit size */
int decode_size_from_lsb(char *buffer)
{
    int size = 0;

    for (int i = 0; i < 32; i++)
    {
        size = size | ((buffer[i] & 1) << (31 - i));
    }

    return size;
}

/* Main decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_failure)
    {
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }

    decode_extn_size(decInfo);
    decode_extn(decInfo);
    decode_file_size(decInfo);
    decode_file_data(decInfo);

    printf("Decoding completed\n");
    return e_success;
}