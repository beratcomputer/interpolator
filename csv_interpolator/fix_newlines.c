
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
    if (argc < 2) {
        printf("Usage: %s <input_file> [output_file]\n", argv[0]);
        printf("If output_file is not specified, uses '<input_filename>_fixed.csv'\n");
        return 1;
    }

    const char *in_file = argv[1];
    char out_file[1024];

    if (argc >= 3) {
        snprintf(out_file, sizeof(out_file), "%s", argv[2]);
    } else {
        // Construct default output name
        snprintf(out_file, sizeof(out_file), "%s_fixed.csv", in_file);
        // Quick hack to remove extension from mid-string if happened? No, keep it simple.
        // If input is "file.csv", output is "file.csv_fixed.csv". 
        // A bit ugly but safe.
        // Let's try to insert before extension if possible
        /*
        char *dot = strrchr(in_file, '.');
        if (dot) {
            int base_len = dot - in_file;
            snprintf(out_file, sizeof(out_file), "%.*s_fixed%s", base_len, in_file, dot);
        }
        */
    }

    fix_newlines(in_file, out_file);
    return 0;
}
