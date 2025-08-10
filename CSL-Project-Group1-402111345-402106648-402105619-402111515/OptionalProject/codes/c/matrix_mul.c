#include <stdio.h>
#include <time.h>

void multiplyMatrices(int n, int matrix1[n][n], int matrix2[n][n], int result[n][n]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i][j] = 0;
            for (int k = 0; k < n; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void printMatrix(int n, int matrix[n][n]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    
    int n;
    scanf("%d", &n);

    int matrix1[n][n], matrix2[n][n], result[n][n];

    // Input first matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &matrix1[i][j]);
        }
    }

    // Input second matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &matrix2[i][j]);
        }
    }

    // Repeat the operation multiple times
    int start_time = clock();
    int iterations = 10; // Number of iterations for better timing accuracy
    

    for (int i = 0; i < iterations; i++) {
        multiplyMatrices(n, matrix1, matrix2, result);
    }

    int end_time = clock();

    // Calculate the average time per iteration
    double total_time = (double)(end_time - start_time);
    double average_time = total_time / iterations;
    // Output the result (only after the last iteration)
    printMatrix(n, result);
    // Output the time taken
    printf("Time taken for one matrix multiplication: %.10f millis\n", average_time);

    return 0;
}
