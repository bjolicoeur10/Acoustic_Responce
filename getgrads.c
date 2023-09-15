#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

void createEquilateralTriangle(int baseLength, double height, int interval, int *numSteps, double **t, double **values) {
    *numSteps = baseLength / interval + 1;
    *t = (double *)malloc(sizeof(double) * (*numSteps));
    *values = (double *)malloc(sizeof(double) * (*numSteps));
    
    for (int i = 0; i < *numSteps; i++) {
        (*t)[i] = i * interval;
    }
    
    int halfNumSteps = *numSteps / 2;
    
    for (int i = 0; i < halfNumSteps; i++) {
        (*values)[i] = i * (height / halfNumSteps);
    }
    
    for (int i = halfNumSteps; i < *numSteps; i++) {
        (*values)[i] = height - (i - halfNumSteps) * (height / halfNumSteps);
    }
}

int main() {
    int timeLimit = 32000;
    int interval = 4;
    int numRepeats = 3;
    int maxTriangles = 100;
    double maxSteepestSlopeCorr = 150.0;  // mT/m/ms
    double maxGradientHeight = 50.0;      // mT/m
    int addedZerosPerRepeat = 1000;
    double *t_all = NULL;
    double *values_all = NULL;
    bool aboveXAxis = true;
    int numTriangles = 1;
    double desiredHeight = maxGradientHeight;
    double steepestSlope, steepestSlopeCorr;
    int min_length;

    while (1) {
        int triangleLength = timeLimit / numTriangles;
        int numSteps, repeatIdx;
        double *t, *values;

        for (repeatIdx = 0; repeatIdx <= numRepeats; repeatIdx++) {
            createEquilateralTriangle(triangleLength, desiredHeight, interval, &numSteps, &t, &values);
            double *t_shifted = (double *)malloc(sizeof(double) * numSteps);

            for (int i = 0; i < numSteps; i++) {
                t_shifted[i] = (t_all ? t_all[numSteps - 1] + interval : 0.0) + t[i];
            }

            if (aboveXAxis) {
                for (int i = 0; i < numSteps; i++) {
                    values_all = (double *)realloc(values_all, (min_length + 1) * sizeof(double));
                    values_all[min_length] = values[i];
                    min_length++;
                }
            } else {
                for (int i = 0; i < numSteps; i++) {
                    values_all = (double *)realloc(values_all, (min_length + 1) * sizeof(double));
                    values_all[min_length] = -values[i];
                    min_length++;
                }
            }

            for (int i = 0; i < addedZerosPerRepeat; i++) {
                t_all = (double *)realloc(t_all, (min_length + 1) * sizeof(double));
                values_all = (double *)realloc(values_all, (min_length + 1) * sizeof(double));
                t_all[min_length] = t_all[min_length - 1] + interval;
                values_all[min_length] = 0.0;
                min_length++;
            }

            aboveXAxis = !aboveXAxis;
            free(t_shifted);
            free(t);
            free(values);
        }

        steepestSlope = 0.0;

        for (int i = 0; i < min_length - 1; i++) {
            double slope = fabs((values_all[i + 1] - values_all[i]) / interval);
            steepestSlope = max(steepestSlope, slope);
        }

        steepestSlopeCorr = steepestSlope * 10.0 * 1000.0;  // mT/m/ms

        if (steepestSlopeCorr > maxSteepestSlopeCorr) {
            desiredHeight = (maxSteepestSlopeCorr / steepestSlopeCorr) * desiredHeight;
            if (desiredHeight > maxGradientHeight) {
                desiredHeight = maxGradientHeight;
            }
        }

        numTriangles++;

        if (numTriangles > maxTriangles) {
            break;
        }
    }

    printf("Steepest Slope: %.6f\n", steepestSlopeCorr);

    // Clean up allocated memory
    free(t_all);
    free(values_all);

    return 0;
}
