#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double epsilon = 10e-6;

struct simplex_t {
    int         m;
    int         n;
    int*        var;
    double**    a;
    double*     b;
    double*     x;
    double*     c;
    double      y;
};

struct node_t {
    int         m;
    int         n;
    int         k;
    int         h;
    double      xh;
    double      ak;
    double      bk;
    double*     min;
    double*     max;
    double**    a;
    double*     b;
    double*     x;
    double*     c;
    double      z;
};

struct set_t {
    int             count;
    int             alloc;
    struct node_t** nodes;
};

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y);
double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h);
void pivot(struct simplex_t* s, int row, int col);
int initial(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);
void prepare(struct simplex_t* s, int k);
int select_nonbasic(struct simplex_t* s);
int init(struct simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);

struct node_t* initial_node(int m, int n, double** a, double* b, double* c);
struct node_t* extend(struct node_t* p, int m, int n, double** a, double* b, double* c, int k, double ak, double bk);
int is_integer(double* xp);
int integer(struct node_t* p);
void bound(struct node_t* p, struct set_t* h, double* zp, double* x);
int branch(struct node_t* q, double z);
void succ(struct node_t* p, struct set_t* h, int m, int n, double** a, double* b, double* c, int k, double ak, double bk, double* zp, double* x);
double intopt(int m, int n, double** a, double* b, double* c, double* x);

void free_node(struct node_t* p);

struct set_t* create_set();
void add(struct set_t* h, struct node_t* p);
int size(struct set_t* h);
struct node_t* pop(struct set_t* h);
void free_set(struct set_t* h);

void print_simplex(struct simplex_t* s) {
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

void print_node(struct node_t* p) {
    printf("%14s = %10d\n%14s = %10d\n", "m", p->m, "n", p->n);
    printf("%14s = %10d\n%14s = %10d\n", "h", p->h, "k", p->k);
    printf("%14s = %10.3lf\n%14s = %10.3lf\n%14s = %10.3lf\n", "xh", p->xh, "ak", p->ak, "bk", p->bk);
    printf("%14s = ", "max z");
    for (size_t i = 0; i < p->n + 1; i++)
    {
        printf("%10.3lf*x_%ld", p->c[i], i);
        if(i != p->n){
            printf(" + ");
        }
    }
    printf("\n");
    for (size_t i = 0; i < p->m + 1; i++)
    {
        for (size_t j = 0; j < p->n + 1; j++)
        {
            printf("%10.3lf*x_%ld", p->a[i][j], j);
            if(j != p->n){
                printf(" + ");
            } else {
                printf(" \u2264 %10.3lf", p->b[i]);
            }
        }
        printf("\n");
    }
    printf("%14s = ", "min");
    for (size_t i = 0; i < p->n + 1; i++)
    {
        printf("%10.3lf", p->min[i]);
        if(i != p->n){
            printf(", ");
        }
    }
    printf("\n");
    printf("%14s = ", "max");
    for (size_t i = 0; i < p->n + 1; i++)
    {
        printf("%10.3lf", p->max[i]);
        if(i != p->n){
            printf(", ");
        }
    }
    printf("\n");
    printf("%14s = %10.3lf\n", "z", p->z);
    printf("--------------------------\n");
}

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y) {
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h) {
    struct simplex_t*   s = (struct simplex_t*)calloc(1, sizeof(struct simplex_t));
    int                 i, row, col;

    if (!initial(s, m, n, a, b, c, x, y, var)) {
        free(s->var);
        free(s);
        return NAN; //not a number.
    }

    while ((col = select_nonbasic(s)) >= 0) {
        row = -1;
        for (i = 0; i < m; i++) {
            if (a[i][col] > epsilon &&
            (row < 0 || b[i] / a[i][col] < b[row] / a[row][col])) {
                row = i;
            }
        }

        if (row < 0) {
            free(s->var);
            free(s);
            return INFINITY; //unbounded.
        }

        pivot(s, row, col);
        //print_simplex(s);
    }

    if (h == 0) {
        for (i = 0; i < n; i++) {
            if (s->var[i] < n) {
                x[s->var[i]] = 0;
            }
        }
        for (i = 0; i < m; i++) {
            if (s->var[n + i] < n) {
                x[s->var[n + i]] = s->b[i];
            }
        }
        free(s->var);
    } else {
        for (i = 0; i < n; i++) {
            x[i] = 0;
        }
        for (i = n; i < n + m; i++) {
            x[i] = s->b[i - n];
        }
    }

    double result = s->y;
    free(s);
    return result;
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
            //printf("|%lf| > |%lf|\n", s->a[i - n][k], s->a[i - n][j]);
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
    double* t = (double*)calloc(n, sizeof(double));

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

    free(t);
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

struct node_t* initial_node(int m, int n, double** a, double* b, double* c) {
    int i;

    struct node_t* p = (struct node_t*)calloc(1, sizeof(struct node_t));
    p->a = (double**)calloc(m + 1, sizeof(double*));
    for (i = 0; i < m + 1; i++) {
        p->a[i] = (double*)calloc(n + 1, sizeof(double));
    }
    p->b = (double*)calloc(m + 1, sizeof(double));
    p->c = (double*)calloc(n + 1, sizeof(double));
    p->x = (double*)calloc(n + 1, sizeof(double));
    p->min = (double*)calloc(n, sizeof(double));
    p->max = (double*)calloc(n, sizeof(double));
    p->m = m;
    p->n = n;

    for (i = 0; i < m; i++) {
        memcpy(p->a[i], a[i], n * sizeof(double));
    }
    memcpy(p->b, b, m * sizeof(double));
    memcpy(p->c, c, n * sizeof(double));

    for (i = 0; i < n; i++) {
        p->min[i] = -INFINITY;
        p->max[i] = +INFINITY;
    }

    return p;
}

struct node_t* extend(struct node_t* p, int m, int n, double** a, double* b, double* c, int k, double ak, double bk) {
    struct node_t* q = (struct node_t*)calloc(1, sizeof(struct node_t));
    int i, j;

    q->k = k;
    q->ak = ak;
    q->bk = bk;

    if (ak > 0 && p->max[k] < INFINITY) {
        q->m = p->m;
    } else if (ak < 0 && p->min[k] > 0) {
        q->m = p->m;
    } else {
        q->m = p->m + 1;
    }

    q->n = p->n;
    q->h = -1;

    q->a = (double**)calloc(q->m + 1, sizeof(double*));
    for (i = 0; i < q->m + 1; i++) {
        q->a[i] = (double*)calloc(q->n + 1, sizeof(double));
    }
    q->b = (double*)calloc(q->m + 1, sizeof(double));
    q->c = (double*)calloc(q->n + 1, sizeof(double));
    q->x = (double*)calloc(q->n + 1, sizeof(double));
    q->min = (double*)calloc(n, sizeof(double));
    q->max = (double*)calloc(n, sizeof(double));

    memcpy(q->min, p->min, n * sizeof(double));
    memcpy(q->max, p->max, n * sizeof(double));
    for (i = 0; i < m; i++) {
        memcpy(q->a[i], a[i], n * sizeof(double));
    }
    memcpy(q->b, b, m * sizeof(double));
    memcpy(q->c, c, n * sizeof(double));

    if (ak > 0) {
        if (q->max[k] == INFINITY || bk < q->max[k]) {
            q->max[k] = bk;
        }
    } else if (q->min[k] == -INFINITY || -bk > q->min[k]) {
        q->min[k] = -bk;
    }

    for (i = m, j = 0; j < n; j++) {
        if (q->min[j] > -INFINITY) {
            q->a[i][j] = -1;
            q->b[i] = -q->min[j];
            i++;
        }
        if (q->max[j] < INFINITY) {
            q->a[i][j] = 1;
            q->b[i] = q->max[j];
            i++;
        }
    }
    return q;
}

int is_integer(double* xp) {
    double x = *xp;
    double r = lround(x);

    if (fabs(r - x) < epsilon) {
        *xp = r;
        return 1;
    } else {
        return 0;
    }
}

int integer(struct node_t* p) {
    int i;

    for (i = 0; i < p->n; i++) {
        if (!is_integer(&(p->x[i]))) {
              return 0;
        }
    }
    return 1;
}

void bound(struct node_t* p, struct set_t* h, double* zp, double* x) {
    if (p->z > *zp) {
        *zp = p->z;
        memcpy(x, p->x, (p->n + 1) * sizeof(double));

        for (int i = 0; i < h->alloc; i++) {
            if (!h->nodes[i] || h->nodes[i]->z >= p->z) {
                continue;
            }

            free_node(h->nodes[i]);
            h->nodes[i] = NULL;
            h->count--;
        }
    }
}

int branch(struct node_t* q, double z) {
    double min, max;
    int h;

    if (q->z < z) {
        return 0;
    }

    for (h = 0; h < q->n; h++) {
        if (!is_integer(&(q->x[h]))) {
            if (q->min[h] == -INFINITY) {
                min = 0;
            } else {
                min = q->min[h];
            }

            max = q->max[h];

            if (floor(q->x[h]) < min || ceil(q->x[h]) > max) {
                continue;
            }

            q->h = h;
            q->xh = q->x[h];

            // for (int i = 0; i < q->m + 1; i++) {
            //     free(q->a[i]);
            // }
            // free(q->a);
            // free(q->b);
            // free(q->c);
            // free(q->x);

            return 1;
        }
    }

    return 0;
}

void succ(struct node_t* p, struct set_t* h, int m, int n, double** a, double* b, double* c, int k, double ak, double bk, double* zp, double* x) {
    struct node_t* q = extend(p, m, n, a, b, c, k, ak, bk);

    if (q == NULL) {
        return;
    }

    q->z = simplex(q->m, q->n, q->a, q->b, q->c, q->x, 0);

    if (isfinite(q->z)) {
        if (integer(q)) {
            bound(q, h, zp, x);
        } else if (branch(q, *zp)) {
            add(h, q);
            return;
        }
    }

    free_node(q);
}

double intopt(int m, int n, double** a, double* b, double* c, double* x) {
    struct node_t* p = initial_node(m, n, a, b, c);
    struct set_t* h = create_set();
    add(h, p);

    double z = -INFINITY;
    p->z = simplex(p->m, p->n, p->a, p->b, p->c, p->x, 0);

    if (integer(p) || !isfinite(p->z)) {
        z = p->z;
        if (integer(p)) {
            memcpy(x, p->x, (p->n + 1) * sizeof(double));
        }
        free_node(p);
        free_set(h);
        return z;
    }

    branch(p, z);

    while (size(h) > 0) {
        struct node_t* p = pop(h);
        succ(p, h, m, n, a, b, c, p->h, 1, floor(p->xh), &z, x);
        succ(p, h, m, n, a, b, c, p->h, -1, -ceil(p->xh), &z, x);
        free_node(p);
    }

    free_set(h);

    if (z == -INFINITY) {
        return NAN;
    } else {
        return z;
    }
}

void free_node(struct node_t* p) {
    for (int i = 0; i < p->m + 1; i++) {
        free(p->a[i]);
    }
    free(p->a);
    free(p->b);
    free(p->c);
    free(p->x);
    free(p->min);
    free(p->max);
    free(p);
}

struct set_t* create_set() {
    struct set_t* h = (struct set_t*)calloc(1, sizeof(struct set_t));
    h->alloc = 10;
    h->count = 0;
    h->nodes = (struct node_t**)calloc(h->alloc, sizeof(struct node_t*));

    for (int i = 0; i < h->alloc; i++) {
        h->nodes[i] = NULL;
    }

    return h;
}

void add(struct set_t* h, struct node_t* p) {
    int i;

    if (h->count < h->alloc) {
        for (i = 0; i < h->alloc; i++) {
            if ((h->nodes)[i] == NULL) {
                h->nodes[i] = p;
                h->count++;
                return;
            }
        }
    } else {
        h->alloc = h->alloc * 2;
        h->nodes = (struct node_t**)realloc(h->nodes, h->alloc * sizeof(struct node_t*));
        for (i = h->count; i < h->alloc; i++) {
            h->nodes[i] = NULL;
        }
        h->nodes[h->count] = p;
        h->count++;
    }
}

int size(struct set_t* h) {
    return h->count;
}

struct node_t* pop(struct set_t* h) {
    struct node_t* p;
    for (int i = 0; i < h->alloc; i++) {
        if ((h->nodes)[i]) {
            p = h->nodes[i];
            h->nodes[i] = NULL;
            h->count--;
            break;
        }
    }

    return p;
}


void free_set(struct set_t* h) {
    free(h->nodes);
    free(h);
}
