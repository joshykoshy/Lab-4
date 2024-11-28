#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Function prototypes */
void read_and_filter_file(const char *input_filename, const char *output_filename);
int is_float(const char *str);

int main() {
    const char *input_filename = "data.txt";      /* Source file containing mixed data */
    const char *output_filename = "valid_floats.txt"; /* Destination file for valid floats */

    read_and_filter_file(input_filename, output_filename);

    return 0;
}

/* 
   This function reads the input file, filters out invalid float values,
   and writes valid ones to an output file.
*/
void read_and_filter_file(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    FILE *output_file = fopen(output_filename, "w");

    /* Check if files were opened successfully */
    if (!input_file || !output_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char value_buffer[100]; /* Buffer to hold values read from the file */
    int invalid_count = 0;  /* Counter for invalid float entries */

    /* Read values from the file until the end is reached */
    while (!feof(input_file)) {
        fseek(input_file, 0, SEEK_CUR); /* Advance file pointer to the current position */

        if (fscanf(input_file, "%s", value_buffer) == 1) {
            /* Check if the value is a valid float */
            if (is_float(value_buffer)) {
                fprintf(output_file, "%s\n", value_buffer); /* Write valid float to output file */
            } else {
                invalid_count++; /* Increment invalid count for non-float values */
            }
        } else if (ferror(input_file)) {
            perror("Error reading file");
            fclose(input_file);
            fclose(output_file);
            exit(EXIT_FAILURE);
        }
    }

    /* Display the number of invalid float values found */
    printf("Number of invalid float values: %d\n", invalid_count);

    /* Close both input and output files */
    fclose(input_file);
    fclose(output_file);
}

/* Function to determine if a string represents a valid float.*/
int is_float(const char *str) {
    char *end_pointer;
    strtod(str, &end_pointer);
    return *end_pointer == '\0'; /* Returns true if no invalid characters remain */
}
