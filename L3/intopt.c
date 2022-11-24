#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double epsilon = 10e-6;

struct simplex_t{
    int         m;
    int         n;
    int*        var;
    double**    a;
    double*     b;
    double*     x;
    double*     c;
    double      y;
};

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y);
double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h);
void pivot(struct simplex_t* s, int row, int col);
int initial(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);
void prepare(struct simplex_t* s, int k);
int select_nonbasic(struct simplex_t* s);
int init(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);

void print(struct simplex_t* s) {
    printf("%14s = %10d\n%14s = %10d\n", "m", s->m, "n", s->n);
    printf("%14s = ", "max z");
    for (size_t i = 0; i < s->n; i++)
    {
        printf("%10.3lf*x_%ld", s->c[i], i);
        if(i != s->n - 1){
            printf(" + ");
        }
    }
    printf("\n");
    for (size_t i = 0; i < s->m; i++)
    {
        for (size_t j = 0; j < s->n; j++)
        {
            printf("%10.3lf*x_%ld", s->a[i][j], j);
            if(j != s->n - 1){
                printf(" + ");
            } else {
                printf(" \u2264 %10.3lf", s->b[i]);
            }
        }
        printf("\n");
    }
    printf("--------------------------\n");
}

int main() {
    int m;
    int n;
    double** a;
    double* b;
    double* c;
    double* x;
    double y;
    size_t i;

    //
    // Scan input
    //
    scanf("%d", &m);
    scanf("%d", &n);

    a = (double**)calloc(m + n, sizeof(double*));
    b = (double*)calloc(m + n, sizeof(double));
    c = (double*)calloc(n + 1, sizeof(double));
    x = (double*)calloc(n + m + 1, sizeof(double));

    for (i = 0; i < n; i++) {
        scanf("%lf", &c[i]);
    }
    for (i = 0; i < m; i++) {
        a[i] = (double*)calloc(n + 1, sizeof(double));
        for (size_t j = 0; j < n; j++) {
            scanf("%lf",&a[i][j]);
        }
    }
    for (i = 0; i < m; i++) {
        scanf("%lf", &b[i]);
    }
    y = 0;
    printf("result: %lf\n", simplex(m, n, a, b, c, x, y));

    free(b);
    for (i = 0; i < m; i++) {
        free(a[i]);
    }
    free(a);
    free(c);
    free(x);
}

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y) {
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h) {
    struct simplex_t   s;
    int                 i, row, col;

    if (!initial(&s, m, n, a, b, c, x, y, var)) {
        free(s.var);
        return NAN; //not a number.
    }

    while ((col = select_nonbasic(&s)) >= 0) {
        row = -1;
        for (i = 0; i < m; i++) {
            if (a[i][col] > epsilon &&
            (row < 0 || b[i] / a[i][col] < b[row] / a[row][col])) {
                row = i;
            }
        }

        if (row < 0) {
            free(s.var);
            return INFINITY; //unbounded.
        }

        pivot(&s, row, col);
        //print(s);
    }

    if (h == 0) {
        for (i = 0; i < n; i++) {
            if (s.var[i] < n) {
                x[s.var[i]] = 0;
            }
        }
        for (i = 0; i < m; i++) {
            if (s.var[n + i] < n) {
                x[s.var[n + i]] = s.b[i];
            }
        }
        free(s.var);
    } else {
        for (i = 0; i < n; i++) {
            x[i] = 0;
        }
        for (i = n; i < n + m; i++) {
            x[i] = s.b[i - n];
        }
    }

    return s.y;
}

void pivot(struct simplex_t* s, int row, int col) {
    double** a = s->a;
    double* b = s->b;
    double* c = s->c;
    int m = s->m;
    int n = s->n;
    int i, j, t;

    t = s->var[col];
    s->var[col] = s->var[n + row];
    s->var[n + row] = t;
    s->y = s->y + c[col] * b[row] / a[row][col];

    for (i = 0; i < n; i++) {
        if (i != col) {
            c[i] = c[i] - c[col] * a[row][i] / a[row][col];
        }
    }
    c[col] = -c[col] / a[row][col];

    for (i = 0; i < m; i++) {
        if (i != row) {
            b[i] = b[i] - a[i][col] * b[row] / a[row][col];
        }
    }

    for (i = 0; i < m; i++) {
        if (i != row) {
            for (j = 0; j < n; j++) {
                if (j != col) {
                    a[i][j] = a[i][j] - a[i][col] * a[row][j] / a[row][col];
                }
            }
        }
    }

    for (i = 0; i < m; i++) {
        if (i != row) {
            a[i][col] = -a[i][col] / a[row][col];
        }
    }

    for (i = 0; i < n; i++) {
        if (i != col) {
            a[row][i] = a[row][i] / a[row][col];
        }
    }

    b[row] = b[row] / a[row][col];
    a[row][col] = 1 / a[row][col];
}

int initial(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var) {
    int             i, j, k;
    double          w;

    k = init(s, m, n, a, b, c, x, y, var);

    if (b[k] >= 0) {
      return 1; //feasible.
    }

    prepare(s, k);
    n = s->n;
    s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);

    for (i = 0; i < m + n; i++) {
        if (s->var[i] == m + n - 1) {
            if (fabs(s->x[i]) > epsilon) {
                free(s->x);
                free(s->c);
                return 0; //infeasible.
            } else {
                break;
            }
        }
    }

    if (i >= n) {
        // x_{n+m} is basic. find good nonbasic.
        for (j = k = 0; k < n; k++) {
            printf("|%lf| > |%lf|\n", s->a[i - n][k], s->a[i - n][j]);
            if (fabs(s->a[i - n][k]) > fabs(s->a[i - n][j])) {
                j = k;
            }
        }
        pivot(s, i - n, j);
        i = j;
    }

    if (i < n - 1) {
        //x_{n+m} is nonbasic and not last. swap columns i and n-1.
        k = s->var[i];
        s->var[i] = s->var[n - 1];
        s->var[n - 1] = k;
        for (k = 0; k < m; k++) {
            w = s->a[k][n - 1];
            s->a[k][n - 1] = s->a[k][i];
            s->a[k][i] = w;
        }
    } else {
        //x_{n+m} is nonbasic and last. forget it.
    }

    free(s->c);
    s->c = c;
    s->y = y;

    for (k = n - 1; k < n + m - 1; k++) {
        s->var[k] = s->var[k + 1];
    }

    n = s->n = s->n - 1;
    double t[n];

    int next_k;
    for (k = 0; k < n; k++) {
        next_k = 0;
        for (j = 0; j < n; j++) {
           if (k == s->var[j]) {
               //x_k is nonbasic. add c_k.
               t[j] = t[j] + s->c[k];
               next_k = 1;
               break;
           }
        }

        if (next_k)
            continue;

        for (j = 0; j < m; j++) {
           if (s->var[n + j] == k) {
               //x_k is at row j.
               break;
           }
        }

        s->y = s->y + s->c[k] * s->b[j];

        for (i = 0; i < n; i++) {
            t[i] = t[i] - s->c[k] * s->a[j][i];
        }
    }

    for (i = 0; i < n; i++) {
        s->c[i] = t[i];
    }

    free(s->x);

    return 1;
}

void prepare(struct simplex_t* s, int k) {
    int m = s->m;
    int n = s->n;
    int i;

    // make room for x_{m + n} at s.var[n] by moving s.var[n..n+m-1] one
    // step to the right.
    for (i = m + n; i > n; i--) {
        s->var[i] = s->var[i - 1];
    }

    s->var[n] = m + n;
    // add x_{m + n} to each constraint

    n = n + 1;
    for (i = 0; i < m; i++) {
        s->a[i][n - 1] = -1;
    }

    s->x = (double*)calloc(m + n, sizeof(double));
    s->c = (double*)calloc(n, sizeof(double));
    s->c[n - 1] = -1;
    s->n = n;
    pivot(s, k, n - 1);
}

int select_nonbasic(struct simplex_t* s) {
    int i;
    for (i = 0; i < s->n; i++) {
        if (s->c[i] > epsilon) {
            return i;
        }
    }
    return -1;
}

int init(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var) {
    int i, k;

    s->m = m;
    s->n = n;
    s->a = a;
    s->b = b;
    s->c = c;
    s->x = x;
    s->y = y;
    s->var = var;

    if (s->var == NULL) {
        s->var = (int*)calloc(m + n + 1, sizeof(int));
        for (i = 0; i < m + n; i++) {
            s->var[i] = i;
        }
    }

    for (k = 0, i = 1; i < m; i++) {
        if (b[i] < b[k]) {
            k = i;
        }
    }

    return k;
}
