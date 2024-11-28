#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 512           /* Image width, fixed at 512 pixels */
#define HEIGHT 512          /* Image height, fixed at 512 pixels */
#define MAX_GRAY 255        /* Maximum grayscale value for PGM images */

/* Function Prototypes */
int readPGMText(const char *filename, int *width, int *height, int *maxGray, unsigned char **pixels);
int writePGMText(const char *filename, int width, int height, int maxGray, unsigned char *pixels);
int writePGMBinary(const char *filename, int width, int height, int maxGray, unsigned char *pixels);
int embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, unsigned char *stegoPixels, int numPixels);
int extractLSB(unsigned char *stegoPixels, unsigned char *extractedCoverPixels, unsigned char *extractedSecretPixels, int numPixels);

int main() {
    int width, height, maxGray;  /* Variables to store image dimensions and max grayscale value */
    unsigned char *coverPixels = NULL, *secretPixels = NULL, *stegoPixels = NULL;
    unsigned char *extractedCoverPixels = NULL, *extractedSecretPixels = NULL;

    /* Read cover image in PGM text format */
    if (readPGMText("baboon.pgm", &width, &height, &maxGray, &coverPixels) != 0) {
        fprintf(stderr, "Failed to read cover image\n");
        return 1;
    }

    /* Read secret image in PGM text format */
    if (readPGMText("farm.pgm", &width, &height, &maxGray, &secretPixels) != 0) {
        fprintf(stderr, "Failed to read secret image\n");
        free(coverPixels);  /* Free memory allocated for the cover image */
        return 1;
    }

    int numPixels = width * height;  /* Total number of pixels in the image */

    /* Allocate memory for stego and extracted images */
    stegoPixels = (unsigned char *)malloc(numPixels);
    extractedCoverPixels = (unsigned char *)malloc(numPixels);
    extractedSecretPixels = (unsigned char *)malloc(numPixels);

    /* Check for successful memory allocation */
    if (!stegoPixels || !extractedCoverPixels || !extractedSecretPixels) {
        fprintf(stderr, "Memory allocation failed\n");
        free(coverPixels);
        free(secretPixels);
        return 1;
    }

    /* Embed secret image into cover image using LSB */
    if (embedLSB(coverPixels, secretPixels, stegoPixels, numPixels) != 0) {
        fprintf(stderr, "Failed to embed secret image\n");
        free(coverPixels);
        free(secretPixels);
        free(stegoPixels);
        free(extractedCoverPixels);
        free(extractedSecretPixels);
        return 1;
    }

    /* Write the stego image to a binary PGM file */
    if (writePGMBinary("stego_image_bin.pgm", width, height, maxGray, stegoPixels) != 0) {
        fprintf(stderr, "Failed to write stego image\n");
    }

    /* Extract the secret image from the stego image */
    if (extractLSB(stegoPixels, extractedCoverPixels, extractedSecretPixels, numPixels) != 0) {
        fprintf(stderr, "Failed to extract secret image\n");
    }

    /* Write the extracted secret image to a text format PGM file */
    if (writePGMText("extracted_secret.pgm", width, height, maxGray, extractedSecretPixels) != 0) {
        fprintf(stderr, "Failed to write extracted secret image\n");
    }

    /* Free all allocated memory */
    free(coverPixels);
    free(secretPixels);
    free(stegoPixels);
    free(extractedCoverPixels);
    free(extractedSecretPixels);

    return 0;
}

/* Function to read a PGM image in text format */
int readPGMText(const char *filename, int *width, int *height, int *maxGray, unsigned char **pixels) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char buffer[256];
    /* Check for valid PGM format (P2 for ASCII) */
    if (!fgets(buffer, sizeof(buffer), file) || strncmp(buffer, "P2", 2) != 0) {
        fprintf(stderr, "Invalid PGM format\n");
        fclose(file);
        return 1;
    }

    /* Skip comments in the file */
    do {
        fgets(buffer, sizeof(buffer), file);
    } while (buffer[0] == '#');

    /* Read image width, height, and maximum grayscale value */
    sscanf(buffer, "%d %d", width, height);
    fscanf(file, "%d", maxGray);

    int numPixels = (*width) * (*height);
    /* Allocate memory for image pixels */
    *pixels = (unsigned char *)malloc(numPixels);
    if (!*pixels) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    /* Read pixel values */
    for (int i = 0; i < numPixels; i++) {
        int pixelValue;
        fscanf(file, "%d", &pixelValue);
        (*pixels)[i] = (unsigned char)pixelValue;
    }

    fclose(file);
    return 0;
}

/* Function to write a PGM image in text format */
int writePGMText(const char *filename, int width, int height, int maxGray, unsigned char *pixels) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    /* Write PGM file header */
    fprintf(file, "P2\n%d %d\n%d\n", width, height, maxGray);

    /* Write pixel values */
    for (int i = 0; i < width * height; i++) {
        fprintf(file, "%d ", pixels[i]);
        if ((i + 1) % width == 0) {  /* New line after each row of pixels */
            fprintf(file, "\n");
        }
    }

    fclose(file);
    return 0;
}

/* Function to write a PGM image in binary format */
int writePGMBinary(const char *filename, int width, int height, int maxGray, unsigned char *pixels) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    /* Write PGM file header in binary format */
    fprintf(file, "P5\n%d %d\n%d\n", width, height, maxGray);

    /* Write pixel data */
    fwrite(pixels, sizeof(unsigned char), width * height, file);

    fclose(file);
    return 0;
}

/* Function to embed secret image into cover image using LSB */
int embedLSB(unsigned char *coverPixels, unsigned char *secretPixels, unsigned char *stegoPixels, int numPixels) {
    for (int i = 0; i < numPixels; i++) {
        unsigned char coverNibble = (coverPixels[i] >> 4) << 4;  /* Extract upper 4 bits of cover pixel */
        unsigned char secretNibble = (secretPixels[i] >> 4);     /* Extract upper 4 bits of secret pixel */
        stegoPixels[i] = coverNibble | secretNibble;             /* Combine nibbles to form stego pixel */
    }
    return 0;
}

/* Function to extract the secret image from the stego image */
int extractLSB(unsigned char *stegoPixels, unsigned char *extractedCoverPixels, unsigned char *extractedSecretPixels, int numPixels) {
    for (int i = 0; i < numPixels; i++) {
        extractedCoverPixels[i] = (stegoPixels[i] >> 4) << 4;    /* Extract upper 4 bits for cover */
        extractedSecretPixels[i] = (stegoPixels[i] & 0x0F) << 4; /* Extract lower 4 bits for secret */
    }
    return 0;
}
    