#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_fname[50];
    FILE *fptr_output;

    int extn_size;
    char extn[20];

    long file_size;

} DecodeInfo;

/* Function prototypes */

Status do_decoding(DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_extn_size(DecodeInfo *decInfo);
Status decode_extn(DecodeInfo *decInfo);
Status decode_file_size(DecodeInfo *decInfo);
Status decode_file_data(DecodeInfo *decInfo);
char decode_byte_from_lsb(char *buffer);
int decode_size_from_lsb(char *buffer);

#endif