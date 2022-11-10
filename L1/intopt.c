#include <stdio.h>
#include <stdlib.h>

void print(int constraints_amount, int decision_vars_amount, double** matrix, double* vector_1, double* vector_2) {
    printf("%14s = %10d\n%14s = %10d\n", "m", constraints_amount, "n", decision_vars_amount);
    printf("%14s = ", "max z");
    for (size_t i = 0; i < constraints_amount; ++i)
    {
        printf("%10.3lf*x_%d", vector_1[i], i);
        if(i != constraints_amount - 1){
            printf(" + ");
        }
    }
    printf("\n");
    for (size_t i = 0; i < constraints_amount; ++i)
    {
        for (size_t j = 0; j < decision_vars_amount; ++j)
        {
            printf("%10.3lf*x_%d", matrix[i][j], j);
            if(j != decision_vars_amount - 1){
                printf(" + ");
            } else {
                printf(" \u2264 %10.3lf", vector_2[i]);
            }
        }
        printf("\n");
    }
}

int main(){
    int constraints_amount;
    int decision_vars_amount;
    double** matrix;
    double* vector_1;
    double* vector_2;

    //
    // Scan input
    //
    scanf("%d", &constraints_amount);
    scanf("%d", &decision_vars_amount);

    matrix = (double**)calloc(constraints_amount, sizeof(double*));
    vector_1 = (double*)calloc(constraints_amount, sizeof(double));
    vector_2 = (double*)calloc(decision_vars_amount, sizeof(double));
    for (size_t i = 0; i < constraints_amount; ++i)
    {
        scanf("%lf", &vector_1[i]);
    }
    for (size_t i = 0; i < constraints_amount; ++i)
    {
        matrix[i] = (double*)calloc(decision_vars_amount, sizeof(double));
        for (size_t j = 0; j < decision_vars_amount; ++j)
        {
            scanf("%lf",&matrix[i][j]);
        }
    }
    for (size_t i = 0; i < decision_vars_amount; ++i)
    {
        scanf("%lf", &vector_2[i]);
    }

    print(constraints_amount, decision_vars_amount, matrix, vector_1, vector_2);

    free(vector_1);
    for (size_t i = 0; i < constraints_amount; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
    free(vector_2);

    return 0;
}
