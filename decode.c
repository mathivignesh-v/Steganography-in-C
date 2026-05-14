#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
#include "types.h"

char str[50];   // To store final output filename

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check for stego image file
    if (argv[2] == NULL)
    {
        printf("ERROR: No stego image provided\n");
        return e_failure;
    }

    decInfo->dest_image_fname = argv[2];

    // Check for output file name
    if (argv[3] == NULL)
    {
        // Default name "output"
        decInfo->output_fname = "output";
    }
    else
    {
        if(argv[3][0] != '.')
        {
           decInfo->output_fname = argv[3];
        }
        else
        {
            return e_failure;
        }
    }

    return e_success;
}

//open files 
Status open_files_for_decoding(DecodeInfo *decInfo)
{
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "r");
    if (decInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", decInfo->dest_image_fname);
        return e_failure;
    }
    printf("Stego image file opened successfully\n");
    return e_success;
}

// skip bmp header
Status skip_bmp_header(FILE *fptr_dest_image)
{
    if (fseek(fptr_dest_image, 54, SEEK_SET) != 0)
    {
        printf("Error skipping BMP header\n");
        return e_failure;
    }
    printf("BMP header skipped\n");
    return e_success;
}

// Decode one byte from LSBs of 8 bytes 
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char decoded_byte = 0;
    for (int i = 0; i < 8; i++)
    {
        decoded_byte = (decoded_byte << 1) | (image_buffer[i] & 1);
    }
    *data = decoded_byte;
    return e_success;
}

// Decode one int (4 bytes = 32 bits) from LSBs 
Status decode_int_from_lsb(int *size, char *image_buffer)
{
    int decoded_size = 0;
    for (int i = 0; i < 32; i++)
    {
        decoded_size = (decoded_size << 1) | (image_buffer[i] & 1);
    }
    *size = decoded_size;
    return e_success;
}

//decode magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char image_buffer[8], decoded_char;
    char temp[10];

    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_dest_image);
        decode_byte_from_lsb(&decoded_char, image_buffer);
        temp[i] = decoded_char;
    }
    temp[strlen(magic_string)] = '\0';

    printf("Magic string extracted: %s\n", temp);

    if (strcmp(temp, magic_string) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

//decode secrete file extn size
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer, 1, 32, decInfo->fptr_dest_image);
    decode_int_from_lsb(size, image_buffer);
    printf("Secret file extension size decoded: %d\n", *size);
    return e_success;
}

//decode secret file extn
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    char image_buffer[8], decoded_char;
    for (int i = 0; i <  decInfo->extn_size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_dest_image);
        decode_byte_from_lsb(&decoded_char, image_buffer);
        printf("decode: %c\n",decoded_char);

        file_extn[i] = decoded_char;
    }

    file_extn[decInfo->extn_size] = '\0';
    printf("Decoded extension: %s\n", file_extn);

    int i = 0;
    while (decInfo->output_fname[i])
    {
        if (decInfo->output_fname[i] != '.')
        {
            str[i] = decInfo->output_fname[i];
        }
        else
        {
            break;
        }
        i++;
    }
    str[i] = '\0';
    strcat(str, file_extn);
    decInfo->output_fname = str;
   printf("Output file: %s\n", decInfo->output_fname);

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output== NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open %s\n", decInfo->output_fname);
        return e_failure;
    }
    printf("output file opened successfully\n");


    printf("Output file: %s\n", decInfo->output_fname);
    printf("Secret file extension decoded: %s\n", file_extn);
    return e_success;
}

//decode secret file size
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer, 32, 1, decInfo->fptr_dest_image);
    decode_int_from_lsb((int *)file_size, image_buffer); // extract 32 bits

    decInfo->size_output_file = *file_size;  // store in struct
    printf("Secret file size decoded: %ld\n", *file_size);

    return e_success;
}

//decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;
    long i;

    printf("\nDecoded Secret Message:\n");

    for (i = 0; i <  decInfo->size_output_file; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_dest_image);
        decode_byte_from_lsb(&data, image_buffer);
        fputc(data, decInfo->fptr_output);
        printf("%c", data); 
    }

    printf("\nEnd of Secret Message\n");
    return e_success;
}

//do decoding
Status do_decoding(DecodeInfo *decInfo)
{
    printf("\n========== DECODING PROCESS STARTED ==========\n");

    printf("Step 1: Opening stego image file...\n");
    if (open_files_for_decoding(decInfo) == e_failure)
    {
        printf("ERROR: Failed to open stego image\n");
        return e_failure;
    }
    printf("✓ Stego image opened successfully\n\n");


    printf("Step 2: Skipping BMP header...\n");
    if (skip_bmp_header(decInfo->fptr_dest_image) == e_failure)
    {
        printf("ERROR: Failed to skip BMP header\n");
        return e_failure;
    }
    printf("✓ BMP header skipped\n\n");


    printf("Step 3: Decoding magic string...\n");
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        printf("ERROR: Magic string mismatch! Not a valid stego image\n");
        return e_failure;
    }
    printf("✓ Magic string verified\n\n");


    printf("Step 4: Decoding secret file extension size...\n");
    int extn_size = 0;
    if (decode_secret_file_extn_size(&extn_size, decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode extension size\n");
        return e_failure;
    }
    decInfo->extn_size = extn_size;
    printf("✓ Extension size decoded\n\n");


    printf("Step 5: Decoding secret file extension...\n");
    if (decode_secret_file_extn(decInfo->extn_output_file, decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode extension\n");
        return e_failure;
    }
    printf("✓ Extension decoded successfully\n\n");


    printf("Step 6: Decoding secret file size...\n");
    if (decode_secret_file_size(&decInfo->size_output_file, decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode secret file size\n");
        return e_failure;
    }
    printf("✓ Secret file size decoded\n\n");


    printf("Step 7: Decoding secret file data...\n");
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR: Failed to decode secret data\n");
        return e_failure;
    }
    printf("✓ Secret data extracted successfully\n\n");


    printf("------------------------------------------------\n");
    printf("   -->DECODING COMPLETED SUCCESSFULLY<--\n");
    printf("   -->Output File Created: %s<--\n", decInfo->output_fname);
    printf("------------------------------------------------\n\n");

    return e_success;
}