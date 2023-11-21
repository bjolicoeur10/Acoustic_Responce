#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXG 2500.0
#define GAMP 32000
#define MAXGNUM 4
#define NUMREPEATS 3


void create_grads(int i, double base, double height, double **data) {
    int num_triangles = i;
    int grad_length = MAXG;
    int steps_per_triangle = grad_length / num_triangles;
    int is_positive;

    // Sets the amplitude of the first gradient in the train
    if (NUMREPEATS % 2 == 0) {
        is_positive = (i % 4 == 2 || i % 4 == 3) ? -1 : 1;
    } else {
        is_positive = (i % 4 == 2 || i % 4 == 3) ? 1 : -1;
    }

    // Loop over the same pulse freq for NUMREPEATS times
    for (int repeat = 0; repeat < NUMREPEATS; repeat++) {
        data[repeat] = (double *)malloc(grad_length * sizeof(double));

        // Get the actual waveform with num_triangle sub pulses
        for (int j = 0; j < num_triangles; j++) {
            double value = 0;
            int sign = is_positive;

            for (int k = 0; k < steps_per_triangle; k++) {
                data[repeat][j * steps_per_triangle + k] = -value;

                if (k < steps_per_triangle / 2) {
                    value += sign * (height / (steps_per_triangle / 2));
                } else {
                    value -= sign * (height / (steps_per_triangle / 2));
                }
            }

            // Alternate between positive and negative
            is_positive = -is_positive;
        }

        // Set last amplitude to 0
        data[repeat][grad_length - 1] = 0.0;
    }
}

int main() {
    for (int i = 1; i <= MAXGNUM; i++) {
        double base = MAXG;
        double height = (double)GAMP;
        double *dataArray[NUMREPEATS];

        create_grads(i, base, height, dataArray);

        for (int repeat = 0; repeat < NUMREPEATS; repeat++) {
            for (int j = 0; j < MAXG; j++) {
                printf("%.4f ", dataArray[repeat][j]);
            }
            printf("\n");

            free(dataArray[repeat]);
        }
    }

    return 0;
}
