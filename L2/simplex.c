#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double epsilon = 10e-6;

struct simplex_t{
    int         m;
    int         n;
    int*        vars;
    double**    a;
    double*     b;
    double*     x;
    double*     c;
    double      y;
};

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y);

double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* vars, int h);

void pivot(struct simplex_t* s, int row, int col);

int initial(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* vars);

int select_nonbasic(struct simplex_t* s);

int init(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* vars);


int main() {
    return 0;
}

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y) {
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* vars, int h) {
    struct simplex_t    s;
    int                 i, row, col;
    
    if (!initial(&s, m, n, a, b, c, x, y, vars)) {
        free(s.vars);
        return NAN; //not a number.
    }
    
    col = select_nonbasic(&s);
    while (col  >= 0) {
        row = -1;
        for (size_t i = 0; i < m; ++i)
        {
            if (a[i][col] > epsilon && 
            (row < 0 || b[i] / a[i][col] < b[row] / a[row][col])) {
                row = i;
            }
        }

        if (row < 0) {
            free(s.vars);
            return INFINITY; //unbounded.
        }

        pivot(&s, row, col);
        col = select_nonbasic(&s);
    }

    if (h == 0) {
        for (size_t i = 0; i < n; ++i) {
            if (s.vars[i] < n) {
                x[s.vars[i]] = 0;
            }
        }        
        for (size_t i = 0; i < m; ++i) {
            if (s.vars[n + i] < n) {
                x[s.vars[n + i]] = s.b[i];
            }
        } 
        free(s.vars);    
    } else {
        for (size_t i = 0; i < n; ++i) {
            x[i] = 0;
        }        
        for (size_t i = 0; i < n+m; ++i) {
            x[i] = s.b[i-n];
        }
    }

    return s.y;
}

void pivot(struct simplex_t* s, int row, int col) {
    double**        a = s->a;
    double*         b = s->b;
    double*         c = s->c;
    int             m = s->m;
    int             n = s->n;
    int             i, j, t;

    t = s->vars[col];
    s->vars[col] = s->vars[n+row];
    s->vars[n+row] = t;
    s->y = s->y + c[col] * b[row] / a[row][col];

    for (i = 0; i < n; ++i) {
        if (i != col) {
            c[i] = c[i] - c[col] * a[row][i] / a[row][col];
        }
    }
    c[col] = -c[col] / a[row][col];
    
    for (i = 0; i < m; ++i) {
        if (i != row) {
            b[i] = b[i] - a[i][col] * b[row] / a[row][col];
        }
    }

    for (i = 0; i < m; ++i) {
        if (i != row) {
            for (j = 0; j < n; ++j) {
                if (j != col) {
                    a[i][j] = a[i][j] - a[i][col] * a[row][j] / a[row][col];
                }
            }
        }
    }

    for (i = 0; i < m; ++i) {
        if (i != row) {
            a[i][col] = -a[i][col] / a[row][col];
        }
    }
    
    for (i = 0; i < n; ++i) {
        if (i != col) {
            a[row][i] = -a[row][i] / a[row][col];
        }
    }

    b[row] = b[row] / a[row][col];
    a[row][col] = 1 / a[row][col];
}

int initial(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* vars) {
    int             i, j, k;
    double          w;

    k = init(s, m, n, a, b, c, x, y, vars);
    
    if (b[k] >= 0) {
        return 1; //feasible.
    }

    //assume b[k] >= 0.
    /*
    prepare(s,k);
    n = s->n;
    s->y = xsimplex(m,n, s->a, s->b, s->c, s->x, 0, s->vars, 1);

    for (i = 0; i < m+n; ++i) {
        if (s->vars[i] == m+n+1) {
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
        //x_{n+m} is basic. find good nonbasic.
        for (j = k = 0; k < n; ++k) {
            if (fabs(s->a[i - n][k]) > fabs(s->a[i - n][j])) {
                j = k;
            }
        }
        pivot(s, i - n, j);
        i = j;
    }

    if (i < n-1) {
        //x_{n+m} is nonbasic and not last. swap columns i and n-1.
        k = s->vars[i];
        s->vars[i] = s->vars[n-1];
        s->vars[i-1] = k;
        for (k = 0; k < m; ++k) {
            w = s->a[k][n-1];
            s->a[k][n-1] = s->a[k][i];
            s->a[k][i] = w;
        }
    } else {
        //x_{n+m} is nonbasic and last. forget it.
    }

    free(s->c);
    s->c = c;
    s->y = y;

    for (k = n-1; k < n+m-1; ++k) {
        s->vars[k] = s->vars[k+1];
    }

    n = s->n = s->n - 1;
    double t[n];

    for (k = 0; k < n; ++k) {
        for (j = 0; j < n; ++j) {
           if (k == s->vars[j]) {
               //x_k is nonbasic. add c_k.
               t[j] = t[j] + s->c[k];
               goto next_k;
           } 
        }
        for (j = 0; j < m; ++j) {
           if (s->vars[n+j] == k) {
               //x_k is at row j.
               break;
           } 
        }

        s->y = s->y + s->c[k] * s->b[j];

        for (i = 0; i < n; ++i) {
            t[i] = t[i] - s->c[k] * s->a[j][i];
        }
    next_k:;
    }

    for (i = 0; i < n; ++i) {
        s->c[i] = t[i];
    }

    free(t);
    free(s->x);

    return 1;
    */
}

int select_nonbasic(struct simplex_t* s) {
    int i;
    for (i = 0; i < s->n; ++i) {
        if (s->c[i] > epsilon) {
            return i;
        }
    }
    return -1;
}

int init(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* vars) {
    int i, k;

    s = malloc(sizeof(struct simplex_t));

    s->m = m;
    s->n = n;
    s->a = a;
    s->b = b;
    s->c = c;
    s->x = x;
    s->y = y;
    s->vars = vars;

    if (s->vars == NULL) {
        s->vars = malloc(sizeof(int) * (m+n+1));
        for (i = 0; i < m+n; ++i) {
            s->vars[i] = i;
        }
    }

    for (k = 0, i = 1; i < m; ++i) {
        if (b[i] < b[k]) {
            k = i;
        }
    }

    return k;
}