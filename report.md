# Project Report
Oscar Cederberg (os7138ce-s)

## Profiling Tools
These are the following tools used to gather data mentioned in this report:

- `gprof`: Software counters give a more precise count of invocations and time taken for each function than `operf`.
- `gcov`: With `gcov -b` it gives an annotated overview of which branches are taken in the source code, similar to `opannotate`.
- `valgrind`: `valgrind --tol=cachegrind` to see cache-related data.

## Result
```
 %   cumulative   self              self     total
 time   seconds   seconds    calls  ms/call  ms/call  name
 92.2      17.89    17.89   987802     0.02     0.02  pivot [7]
  5.7      19.00     1.11    30081     0.04     0.62  xsimplex <cycle 1> [6]
  1.2      19.23     0.23    30081     0.01     0.02  initial <cycle 1> [8]
  0.7      19.36     0.13   990401     0.00     0.00  select_nonbasic [10]
  0.1      19.38     0.02   180916     0.00     0.00  is_integer [11]
  0.1      19.40     0.02    30081     0.00     0.00  init [12]
  0.1      19.41     0.01    15014     0.00     0.02  prepare [9]
  0.0      19.41     0.00    15067     0.00     0.00  free_node [15]
  0.0      19.41     0.00    15067     0.00     1.29  simplex [5]
  0.0      19.41     0.00    15066     0.00     0.00  extend [16]
  0.0      19.41     0.00    15066     0.00     1.29  succ [3]
  0.0      19.41     0.00    12443     0.00     0.00  integer [13]
  0.0      19.41     0.00    12422     0.00     0.00  branch [14]
  0.0      19.41     0.00     9089     0.00     0.00  add [17]
  0.0      19.41     0.00     7534     0.00     0.00  size [18]
  0.0      19.41     0.00     7533     0.00     0.00  pop [19]
  0.0      19.41     0.00       21     0.00     0.00  bound [20]
  0.0      19.41     0.00        1     0.00     0.00  create_set [21]
  0.0      19.41     0.00        1     0.00     0.00  free_set [22]
  0.0      19.41     0.00        1     0.00     0.00  initial_node [23]
  0.0      19.41     0.00        1     0.00 19410.00  intopt [1]
```
Figure 1: `gprof` output.

```
function pivot called 987802 returned 100% blocks executed 100%
   987802:  246:void pivot(struct simplex_t* s, int row, int col) {
   987802:  247:    double** a = s->a;
   987802:  248:    double* b = s->b;
   987802:  249:    double* c = s->c;
   987802:  250:    int m = s->m;
   987802:  251:    int n = s->n;
        -:  252:    int i, j, t;
        -:  253:
   987802:  254:    t = s->var[col];
   987802:  255:    s->var[col] = s->var[n + row];
   987802:  256:    s->var[n + row] = t;
   987802:  257:    s->y = s->y + c[col] * b[row] / a[row][col];
        -:  258:
 21281013:  259:    for (i = 0; i < n; i++) {
branch  0 taken 95%
branch  1 taken 5% (fallthrough)
 20293211:  260:        if (i != col) {
branch  0 taken 95% (fallthrough)
branch  1 taken 5%
 19305409:  261:            c[i] = c[i] - c[col] * a[row][i] / a[row][col];
        -:  262:        }
        -:  263:    }
   987802:  264:    c[col] = -c[col] / a[row][col];
        -:  265:
 33422538:  266:    for (i = 0; i < m; i++) {
branch  0 taken 97%
branch  1 taken 3% (fallthrough)
 32434736:  267:        if (i != row) {
branch  0 taken 97% (fallthrough)
branch  1 taken 3%
 31446934:  268:            b[i] = b[i] - a[i][col] * b[row] / a[row][col];
        -:  269:        }
        -:  270:    }
        -:  271:
 33422538:  272:    for (i = 0; i < m; i++) {
branch  0 taken 97%
branch  1 taken 3% (fallthrough)
 32434736:  273:        if (i != row) {
branch  0 taken 97% (fallthrough)
branch  1 taken 3%
677905422:  274:            for (j = 0; j < n; j++) {
branch  0 taken 95%
branch  1 taken 5% (fallthrough)
646458488:  275:                if (j != col) {
branch  0 taken 95% (fallthrough)
branch  1 taken 5%
615011554:  276:                    a[i][j] = a[i][j] - a[i][col] * a[row][j] / a[row][col];
        -:  277:                }
        -:  278:            }
        -:  279:        }
        -:  280:    }
        -:  281:
 33422538:  282:    for (i = 0; i < m; i++) {
branch  0 taken 97%
branch  1 taken 3% (fallthrough)
 32434736:  283:        if (i != row) {
branch  0 taken 97% (fallthrough)
branch  1 taken 3%
 31446934:  284:            a[i][col] = -a[i][col] / a[row][col];
        -:  285:        }
        -:  286:    }
        -:  287:
 21281013:  288:    for (i = 0; i < n; i++) {
branch  0 taken 95%
branch  1 taken 5% (fallthrough)
 20293211:  289:        if (i != col) {
branch  0 taken 95% (fallthrough)
branch  1 taken 5%
 19305409:  290:            a[row][i] = a[row][i] / a[row][col];
        -:  291:        }
        -:  292:    }
        -:  293:
   987802:  294:    b[row] = b[row] / a[row][col];
   987802:  295:    a[row][col] = 1 / a[row][col];
   987802:  296:}
```
Figure 2: gcov -b output of `pivot`.

```
$ valgrind --tool=cachegrind --I1=65536,1,128 --D1=32768,2,128 --LL=1048576,8,128 ./a.out < i
==21155== Cachegrind, a cache and branch-prediction profiler
==21155== Copyright (C) 2002-2017, and GNU GPL'd, by Nicholas Nethercote et al.
==21155== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==21155== Command: ./a.out
==21155==
result: 633.000000
==21155==
==21155== I   refs:      55,485,148,066
==21155== I1  misses:           676,583
==21155== LLi misses:             1,815
==21155== I1  miss rate:           0.00%
==21155== LLi miss rate:           0.00%
==21155==
==21155== D   refs:      26,308,143,182  (22,972,654,851 rd   + 3,335,488,331 wr)
==21155== D1  misses:        18,959,273  (    17,565,897 rd   +     1,393,376 wr)
==21155== LLd misses:           164,447  (        82,234 rd   +        82,213 wr)
==21155== D1  miss rate:            0.1% (           0.1%     +           0.0%  )
==21155== LLd miss rate:            0.0% (           0.0%     +           0.0%  )
==21155==
==21155== LL refs:           19,635,856  (    18,242,480 rd   +     1,393,376 wr)
==21155== LL misses:            166,262  (        84,049 rd   +        82,213 wr)
==21155== LL miss rate:             0.0% (           0.0%     +           0.0%  )
```
Figure 3: valgrind output.

## Discussion
### gprof
`pivot` and `select_nonbasic` are the function called most of the time by a large majority. However, the program spends over 90% of its time executing `pivot`. Because the program is inside `pivot` so often, any optimizations should be focused around `pivot`. Optimizing any other function would give negligeable improvements.

Optimizing by changing the data structures used in the programs might give an overall uplift, but hard to say from the output of `gprof` as it is purely focused on function performance.

### gcov
The output of annotation was limited to `pivot` as it is the main function of interest in optimizing. It involves a lot of nested `for`-loops and `if`-cases, which leads to a lot of branching. Any restructuring which would lead to less branching would be helpful in optimizing the function. If possible, restructuring of the nested for-loops to focus on vectorization would lead to better cache efficency.

## valgrind
From the first implementation at `intopt` there is already a clear good use of cache memory. This is might related to the use of an pointer array instead of linked-list for the implementation of set in `intopt`. This should lead to less cache misses, as the entirety of the set can be loaded into cache at once and less jumping in memory.