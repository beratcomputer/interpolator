#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_POINTS 100000
#define TARGET_DT 0.01

typedef struct {
    double time;
    double x, y, z;
    double roll, pitch, yaw;
} Point;

Point data[MAX_POINTS];
int point_count = 0;

void read_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        system("pause");
        exit(1);
    }

    char line[1024];
    // Read header
    if (fgets(line, sizeof(line), file)) {
        // Skip header
    }

    while (fgets(line, sizeof(line), file) && point_count < MAX_POINTS) {
        Point *p = &data[point_count];
        // Format: time;X;Y;Z;Roll;Pitch;yaw
        // Note: Using %lf for double
        if (sscanf(line, "%lf;%lf;%lf;%lf;%lf;%lf;%lf", 
            &p->time, &p->x, &p->y, &p->z, &p->roll, &p->pitch, &p->yaw) == 7) {
            point_count++;
        }
    }
    fclose(file);
    printf("Read %d points.\n", point_count);
}

double interpolate(double t, double t0, double v0, double t1, double v1) {
    if (t1 == t0) return v0;
    return v0 + (v1 - v0) * (t - t0) / (t1 - t0);
}

int main() {
    char filename[256];
    printf("Enter input CSV filename: ");
    scanf("%255s", filename);

    read_csv(filename);

    if (point_count < 2) {
        printf("Not enough data points.\n");
        system("pause");
        return 1;
    }

    FILE *out = fopen("output_interpolated_standard.csv", "w");
    if (!out) {
        printf("Error creating output file.\n");
        system("pause");
        return 1;
    }

    // Write header
    fprintf(out, "time;X;Y;Z;Roll;Pitch;yaw\n");

    // Always start from 0.00
    double t_start = 0.0;
    double t_end = data[point_count - 1].time;
    
    int current_idx = 0;
    
    for (double t = t_start; t <= t_end; t += TARGET_DT) {
        double x, y, z, roll, pitch, yaw;

        if (t < data[0].time) {
            // If current time is before the first recorded time, use the first recorded values (hold)
            Point p0 = data[0];
            x = p0.x;
            y = p0.y;
            z = p0.z;
            roll = p0.roll;
            pitch = p0.pitch;
            yaw = p0.yaw;
        } else {
            // Find surrounding points
            while (current_idx < point_count - 1 && data[current_idx + 1].time < t) {
                current_idx++;
            }
            
            Point p0 = data[current_idx];
            Point p1 = data[current_idx + 1];
            
            x = interpolate(t, p0.time, p0.x, p1.time, p1.x);
            y = interpolate(t, p0.time, p0.y, p1.time, p1.y);
            z = interpolate(t, p0.time, p0.z, p1.time, p1.z);
            roll = interpolate(t, p0.time, p0.roll, p1.time, p1.roll);
            pitch = interpolate(t, p0.time, p0.pitch, p1.time, p1.pitch);
            yaw = interpolate(t, p0.time, p0.yaw, p1.time, p1.yaw);
        }
        
        fprintf(out, "%.3f;%.5f;%.5f;%.5f;%.5f;%.5f;%.5f\n", 
                t, x, y, z, roll, pitch, yaw);
    }

    fclose(out);
    printf("Standard interpolation complete. Saved to output_interpolated_standard.csv\n");
    
    system("pause");
    return 0;
}
