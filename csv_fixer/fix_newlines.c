
#include <stdio.h>
#include <stdlib.h>

void fix_newlines(const char *in_filename, const char *out_filename) {
    FILE *in = fopen(in_filename, "rb");
    if (!in) {
        printf("Error: Cannot open input file '%s'\n", in_filename);
        return;
    }

    FILE *out = fopen(out_filename, "w"); // Text mode: '\n' -> System EOL (CRLF on Windows)
    if (!out) {
        printf("Error: Cannot open output file '%s'\n", out_filename);
        fclose(in);
        return;
    }

    int c;
    int prev_cr = 0; // Flag if previous char was '\r'

    while ((c = fgetc(in)) != EOF) {
        if (prev_cr) {
            // We just saw a '\r' in previous iteration.
            // We haven't written the newline for it yet, waiting to see if next is '\n'.
            if (c == '\n') {
                // It was "\r\n".
                fputc('\n', out);
                prev_cr = 0;
            } else {
                // It was just "\r" (Mac style or old).
                // Output the newline for that \r.
                fputc('\n', out);
                
                // Now process current char 'c'
                if (c == '\r') {
                    prev_cr = 1; // Current char is start of new sequence
                } else {
                    fputc(c, out);
                    prev_cr = 0;
                }
            }
        } else {
            if (c == '\r') {
                prev_cr = 1;
            } else if (c == '\n') {
                // Just '\n' (Unix style)
                fputc('\n', out);
            } else {
                fputc(c, out);
            }
        }
    }

    // Trailing \r
    if (prev_cr) {
        fputc('\n', out);
    }

    fclose(in);
    fclose(out);
    printf("Successfully converted '%s' to '%s' with normalized line endings.\n", in_filename, out_filename);
}

int main(int argc, char *argv[]) {
    char in_file[1024];
    char out_file[1024];

    if (argc < 2) {
        printf("Mode: Interactive\n");
        printf("Please enter the input CSV filename (e.g., input.csv): ");
        if (scanf("%1023s", in_file) != 1) {
            printf("Invalid input.\n");
            system("pause");
            return 1;
        }
        // Default output
        snprintf(out_file, sizeof(out_file), "%s_fixed.csv", in_file);
    } else {
        snprintf(in_file, sizeof(in_file), "%s", argv[1]);
        if (argc >= 3) {
            snprintf(out_file, sizeof(out_file), "%s", argv[2]);
        } else {
            snprintf(out_file, sizeof(out_file), "%s_fixed.csv", in_file);
        }
    }

    fix_newlines(in_file, out_file);
    
    printf("\nDone. Press any key to exit...\n");
    system("pause");
    return 0;
}
