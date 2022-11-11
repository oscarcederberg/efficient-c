#include <stdio.h>
#include <stdlib.h>

int main(){
    int m;
    int n;
    double** a;
    double* b;
    double* c;

    //
    // Scan input
    //
    scanf("%d", &m);
    scanf("%d", &n);

    a = (double**)calloc(m, sizeof(double*));
    b = (double*)calloc(1, sizeof(double));
    c = (double*)calloc(n, sizeof(double));
    for (size_t i = 0; i < n; ++i)
    {
        scanf("%lf", &c[i]);
    }
    for (size_t i = 0; i < m; ++i)
    {
        a[i] = (double*)calloc(n, sizeof(double));
        for (size_t j = 0; j < n; ++j)
        {
            scanf("%lf",&a[i][j]);
        }
    }
    for (size_t i = 0; i < m; ++i)
    {
        scanf("%lf", &b[i]);
    }

    //
    // Print input
    //
    printf("m = %10d\nn = %10d\n", m, n);
    printf("max %10s = ", "z");
    for (size_t i = 0; i < m; ++i)
    {
        printf("%10.3lf*x_%d", c[i], i);
        if(i != m - 1){
            printf(" + ");
        }
    }
    printf("\n");
    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            printf("%10.3lf*x_%d", a[i][j], i);
            if(j != n - 1){
                printf(" + ");
            } else {
                printf(" \u2264 %10.3lf", b[i]);
            }
        }
        printf("\n");
    }

    return 0;
}