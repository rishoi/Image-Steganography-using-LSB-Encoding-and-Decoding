#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)

{
    if (strstr(argv[2], ".bmp") != NULL)            //step 1 -> check the source file having .bmp or not
    {
        encInfo->src_image_fname = argv[2];         // t -> store file name to structure mem (encInfo -> src_image_fname = argv[2])

    }
    else
    {
        printf("ERROR: Source file is not BMP format\n");
        return e_failure;                            // f -> print error msg and return e_failure

    }

    if (strchr(argv[3], '.') != NULL)          //step 2 -> check secret file having extn or not

    {
        encInfo->sec_fname = argv[3];          // t -> store file name to structure mem (encInfo -> sec_fname = argv[3])


        /* Extract extension and store in structure */
        strcpy(encInfo->extn_secret_file, strchr(argv[3], '.'));
    }
    else
    {
        printf("ERROR: Secret file does not have extension\n");
        return e_failure;                     // f -> print error msg and return e_failure

    }

    if (argv[4] != NULL)                       // step 3 -> check argv[4] != NULL or not

    {
        if (strstr(argv[4], ".bmp") != NULL)           // t -> check the file name is having .bmp or not

        {
            encInfo->stego_image_fname = argv[4]; // t -> store file name to structure mem (encInfo -> stego_image_fname = argv[4])

        }
        else
        {
            printf("ERROR: Output file must be .bmp\n");
            return e_failure;                        // f -> print error msg and return e_failure

        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";// f -> give one default name to output file(encInfo -> stego_image_fname = "stego.bmp")

    }

    return e_success;       //return e_success

}
   
Status open_files(EncodeInfo *encInfo)
{
    // Source Image
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        printf("ERROR: Unable to open source image\n");
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->sec_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        printf("ERROR: Unable to open secret file\n");
        return e_failure;
    }

    // Stego file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open output file\n");
        return e_failure;
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);   //step 1-> encInfo -> image_capacity = call get_image_size_for_bmp(encInfo -> fptr_src_image)


    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);   //step 2-> encoInfo -> size_secret_file = call get_file_size(encInfo -> fptr_secret)


    if (encInfo->image_capacity > (54 + (strlen(MAGIC_STRING) * 8) + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))   //step 3 -> check encInfo -> image_capacity > 54 + (strlen(MAGIC_STRING)*8) + 32 + 32 + 32 + (encoInfo -> size_secret_file * 8)

    {
        return e_success;        // t -> return e_success

    }
    else
    {
        printf("ERROR: Insufficient image capacity\n");
        return e_failure;        // f -> return e_failure

    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];

    rewind(fptr_src_image);    // rewind source file pointer to 0th pos


    fread(buffer, 54, 1, fptr_src_image);    // using fread read 54 bytes of header from source file


    fwrite(buffer, 54, 1, fptr_dest_image);    // using fwrite write 54 bytes of header to dest file


    return e_success;    // return e_success

}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)

{
    char buffer[8];    // step 1-> declare char buffer[8]

    int i, len = strlen(magic_string);

    for (i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);    // step 2-> read 8 bytes from source image store into buffer


        encode_byte_to_lsb(magic_string[i], buffer);    // step 3-> call encode_bytes_to_lsb(magic_string[i], buffer)


        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);    // step 4-> write the buffer to dest file -> repeat this upto length of data times

    }

    return e_success;    // return e_success

}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];    // step 1-> declare char buffer[32]


    fread(buffer, 32, 1, encInfo->fptr_src_image);    // step 2-> read 32 bytes from source image store into buffer


    encode_size_to_lsb(size, buffer);    // step 3-> call encode_size_to_lsb(size, buffer)


    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);    // step 4-> write the buffer to dest file


    return e_success;    // return e_success

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)

{
    char buffer[8];    // step 1-> declare char buffer[8]

    int i, len = strlen(file_extn);

    for (i = 0; i < len; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);    // step 2-> read 8 bytes from source image store into buffer


        encode_byte_to_lsb(file_extn[i], buffer);    // step 3-> call encode_bytes_to_lsb(file_extn[i], buffer)


        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);    // step 4-> write the buffer to dest file -> repeat this upto length of file_extn_size times

    }

    return e_success;    // return e_success

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)

{
    char buffer[32];    // step 1-> declare char buffer[32]


    fread(buffer, 32, 1, encInfo->fptr_src_image);    // step 2-> read 32 bytes from source image store into buffer


    encode_size_to_lsb(file_size, buffer);    // step 3-> call encode_size_to_lsb(file_size, buffer)


    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);    // step 4-> write the buffer to dest file


    return e_success;    // return e_success

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[8];    // step 2-> declare char buffer[8]

    int i;

    rewind(encInfo->fptr_secret);

    fread(encInfo->secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);    // step 1 -> read secret data from the secret file store into encInfo -> secret_data;


    for (i = 0; i < encInfo->size_secret_file; i++)    // step 3-> read 8 bytes from source image store into buffer

    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);        //Read 8 bytes from source image


        encode_byte_to_lsb(encInfo->secret_data[i], buffer);    // step 4-> call encode_bytes_to_lsb(secret_data[i], buffer)


        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);    // step 5-> write the buffer to dest file -> repeat this upto length of secret_data size times

    }

    return e_success;    // return e_success

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src) > 0)     //Copy remaining bytes until EOF 

    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;    // return e_success

}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }

    return e_success;   // return e_success
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }

    return e_success;    // return e_success
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)     // step 1-> check open_files(encInfo) is returning e_success or not

    {
        printf("ERROR: Failed to open files\n");
        return e_failure;                    //e_failure -> print error msg and return e_failure
    }
    printf("INFO: Files opened successfully\n");

    if (check_capacity(encInfo) == e_failure)       // step 2-> check check_capacity(encInfo) is returning e_success or not

    {
        printf("ERROR: Insufficient capacity\n");
        return e_failure;        //e_failure -> print error msg and return e_failure

    }
    printf("INFO: Capacity check passed\n");

    encInfo->secret_data = malloc(encInfo->size_secret_file);

    copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image);    // step 3-> copy_bmp_header(encInfo -> fptr_src_image, encInfo ->fptr_dest_image)

    printf("INFO: BMP header copied successfully\n");          // print success msg


    encode_magic_string(MAGIC_STRING, encInfo);    // step 4-> call encode_magic_string(MAGIC_STRING, encInfo)

    printf("INFO: Magic string encoded successfully\n");        // print success msg


    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo);   // step 5-> call encode_secret_file_extn_size(strlen(encInfo ->extn_secret_file), encInfo)

    printf("INFO: Secret file extension size encoded\n");        // print success msg


    encode_secret_file_extn(encInfo->extn_secret_file,encInfo);   // step 6-> call  encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)

    printf("INFO: Secret file extension encoded\n");        // print success msg


    encode_secret_file_size(encInfo->size_secret_file,encInfo);  // step 7-> call encode_secret_file_size(encInfo -> size_secret_file, encInfo)

    printf("INFO: Secret file size encoded\n");        //print success msg


    encode_secret_file_data(encInfo);    // step 8-> call encode_secret_file_data(encInfo)

    printf("INFO: Secret file data encoded\n");        //print success msg


    /* Step 9: Copy remaining image data */
    copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image);  // step 9-> call copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)

    printf("INFO: Remaining image data copied\n");         //print success msg


    printf("INFO: Encoding completed successfully\n");    // step 10 -> return e_success

    free(encInfo->secret_data);

    return e_success;
}