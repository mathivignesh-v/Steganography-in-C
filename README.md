# Steganography-in-C
# Steganography in C

A simple Steganography project developed in C to hide secret text messages inside BMP image files using the **LSB (Least Significant Bit)** technique. The hidden data can later be extracted without visibly changing the image.

## Features

* Encode secret text into BMP images
* Decode hidden text from images
* Uses LSB image steganography
* Command-line based application
* Simple and lightweight implementation

## Technologies Used

* C Programming
* File Handling
* Bitwise Operations
* Structures
* Command Line Arguments

## Project Structure

* `encode.c` → Hides secret data into image
* `decode.c` → Extracts hidden data
* `common.h` → Common declarations
* `types.h` → User-defined data types
* `main.c` → Main driver file

## How It Works

The project modifies the least significant bits of image pixels to store secret data. Since only the last bit is changed, the image appearance remains almost unchanged.

## Compilation

```bash
gcc *.c -o stego
```

## Usage

### Encode

```bash
./stego -e input.bmp secret.txt output.bmp
```

### Decode

```bash
./stego -d output.bmp decoded.txt
```

## Learning Outcomes

* Understanding of image steganography
* Practical use of file handling in C
* Bitwise manipulation techniques
* Working with BMP file formats

## Applications

* Secure communication
* Data hiding
* Cybersecurity projects
* Information protection

## Author

MATHI VIGNESH.V

