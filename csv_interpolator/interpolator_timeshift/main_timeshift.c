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

    FILE *out = fopen("output_interpolated.csv", "w");
    if (!out) {
        printf("Error creating output file.\n");
        system("pause");
        return 1;
    }

    // Write header
    fprintf(out, "time;X;Y;Z;Roll;Pitch;yaw\n");

    // Store interpolated data in memory first
    static Point interpolated_data[MAX_POINTS];
    int interpolated_count = 0;
    
    // Always start from 0.00
    double t_start = 0.0;
    double t_end = data[point_count - 1].time;
    
    int current_idx = 0;
    
    for (double t = t_start; t <= t_end; t += TARGET_DT) {
        if (interpolated_count >= MAX_POINTS) break;
        
        Point *p_out = &interpolated_data[interpolated_count];
        p_out->time = t;

        if (t < data[0].time) {
            // If current time is before the first recorded time, use the first recorded values (hold)
            Point p0 = data[0];
            p_out->x = p0.x;
            p_out->y = p0.y;
            p_out->z = p0.z;
            p_out->roll = p0.roll;
            p_out->pitch = p0.pitch;
            p_out->yaw = p0.yaw;
        } else {
            // Find surrounding points
            while (current_idx < point_count - 1 && data[current_idx + 1].time < t) {
                current_idx++;
            }
            
            Point p0 = data[current_idx];
            Point p1 = data[current_idx + 1];
            
            p_out->x = interpolate(t, p0.time, p0.x, p1.time, p1.x);
            p_out->y = interpolate(t, p0.time, p0.y, p1.time, p1.y);
            p_out->z = interpolate(t, p0.time, p0.z, p1.time, p1.z);
            p_out->roll = interpolate(t, p0.time, p0.roll, p1.time, p1.roll);
            p_out->pitch = interpolate(t, p0.time, p0.pitch, p1.time, p1.pitch);
            p_out->yaw = interpolate(t, p0.time, p0.yaw, p1.time, p1.yaw);
        }
        interpolated_count++;
    }

    // Now write to file with time shift
    // We want to shift everything EXCEPT Yaw by 200ms (20 steps at 0.01s)
    // The output should extend by 200ms
    
    int shift_steps = 20; // 0.20s / 0.01s = 20 steps
    int total_output_steps = interpolated_count + shift_steps;

    for (int i = 0; i < total_output_steps; i++) {
        double current_time = i * TARGET_DT;
        
        // Yaw Index: Direct mapping 0..end, then hold last value
        int idx_yaw = i;
        if (idx_yaw >= interpolated_count) {
            idx_yaw = interpolated_count - 1;
        }
        
        // Other Index: Shifted by 20 steps. 
        // 0..19 -> use index 0 (hold start)
        // 20..end -> use index i-20
        int idx_other = i - shift_steps;
        if (idx_other < 0) {
            idx_other = 0;
        }
        if (idx_other >= interpolated_count) {
             // Should effectively match the loop limit, but safe guard
             idx_other = interpolated_count - 1;
        }

        fprintf(out, "%.3f;%.5f;%.5f;%.5f;%.5f;%.5f;%.5f\n", 
                current_time, 
                interpolated_data[idx_other].x, 
                interpolated_data[idx_other].y, 
                interpolated_data[idx_other].z, 
                interpolated_data[idx_other].roll, 
                interpolated_data[idx_other].pitch, 
                interpolated_data[idx_yaw].yaw);
    }

    fclose(out);
    printf("Interpolation with shift complete. Saved to output_interpolated.csv\n");
    
    system("pause");
    return 0;
}
