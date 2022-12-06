#include <stdio.h>
#include <stdlib.h>

void print(int m, int n, double** a, double* b, double* c) {
    printf("%14s = %10d\n%14s = %10d\n", "m", m, "n", n);
    printf("%14s = ", "max z");
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
            printf("%10.3lf*x_%d", a[i][j], j);
            if(j != n - 1){
                printf(" + ");
            } else {
                printf(" \u2264 %10.3lf", b[i]);
            }
        }
        printf("\n");
    }
}

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
    b = (double*)calloc(m, sizeof(double));
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

    print(m, n, a, b, c);

    free(b);
    for (size_t i = 0; i < m; i++)
    {
        free(a[i]);
    }
    free(a);
    free(c);

    return 0;
}
