# Lab 6 notes
## optimization
```
$ size --common *.o
   text    data     bss     dec     hex filename
  19044     272 5093552 5112868  4e0424 0.o
  11695     320 5093552 5105567  4de79f 1.o
  12939     320 5093552 5106811  4dec7b 2.o
  35371     296 5093552 5129219  4e4403 3.o
  35371     296 5093552 5129219  4e4403 4.o
  35371     296 5093552 5129219  4e4403 5.o
   9258     272 5093552 5103082  4dddea s.o
```

```
$ for x in s 0 1 2 3
> do
> gcc -O$x -o $x mipsx.c tbr.s timebase.c
> ./$x
> done
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     301     462    1167     769    1014    1371      54     413      84     325
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     674    1408    2641    1180    1016    3662     136    1083     191    1619
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     226     404    1015     769    1010     801      55     424     101     378
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     238     324     869     154     189     969      45     466      77     311
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     221     323     732      61      58     986      45     466      73     154
```

## nm
`T` are symbols that are in the text/code section.

`U` are for undefined symbols.

`G` are for symbols in a initialized data section for small objects.

`C` are for common symbols which are uninitialized data.

## ftree-vectorize
`gcc -ftree-vectorize -O3 -mcpu=power8 -o vectorized mipsx.c tbr.s timebase.c`

`%s/stvx//gn`: 5\
`%s/vmaddfp//gn`: 0\
`%s/vperm//gn`: 0\
`%s/lvx//gn`: 6\

## pgo
```
$ gcc -O3 -fprofile-generate mipsx.c tbr.s timebase.c; ./a.out
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     349     636    1324      61      58    1032      50     427      79     160
```

```
$ gcc -O3 -fprofile-use mipsx.c tbr.s timebase.c -mcpu=power8; ./a.out
    Perm  Towers  Queens   Intmm      Mm  Puzzle   Quick  Bubble    Tree     FFT
     242     242     455      44      54     697      46     468      79     237
```

## link optimization
```
0000000000000640 <main>:
 640:   02 00 4c 3c     addis   r2,r12,2
 644:   c0 78 42 38     addi    r2,r2,30912
 648:   a6 02 08 7c     mflr    r0
 64c:   f8 ff e1 fb     std     r31,-8(r1)
 650:   0d 00 80 38     li      r4,13
 654:   0a 00 60 38     li      r3,10
 658:   10 00 01 f8     std     r0,16(r1)
 65c:   91 ff 21 f8     stdu    r1,-112(r1)
 660:   e1 01 00 48     bl      840 <p0>
 664:   00 00 00 60     nop
 668:   fe ff 82 3c     addis   r4,r2,-2
 66c:   d0 8a 84 38     addi    r4,r4,-30000
 670:   78 1b 7f 7c     mr      r31,r3
 674:   78 1b 65 7c     mr      r5,r3
 678:   01 00 60 38     li      r3,1
 67c:   85 ff ff 4b     bl      600 <00000047.plt_call.__printf_chk@@GLIBC_2.17>
 680:   18 00 41 e8     ld      r2,24(r1)
 684:   70 00 21 38     addi    r1,r1,112
 688:   78 fb e3 7f     mr      r3,r31
 68c:   10 00 01 e8     ld      r0,16(r1)
 690:   f8 ff e1 eb     ld      r31,-8(r1)
 694:   a6 03 08 7c     mtlr    r0
 698:   20 00 80 4e     blr
 69c:   00 00 00 00     .long 0x0
 6a0:   00 00 00 01     .long 0x1000000
 6a4:   80 01 00 00     .long 0x180
```

```
0000000000000640 <main>:
 640:   02 00 4c 3c     addis   r2,r12,2
 644:   c0 78 42 38     addi    r2,r2,30912
 648:   a6 02 08 7c     mflr    r0
 64c:   fe ff 82 3c     addis   r4,r2,-2
 650:   17 00 a0 38     li      r5,23
 654:   90 8a 84 38     addi    r4,r4,-30064
 658:   01 00 60 38     li      r3,1
 65c:   10 00 01 f8     std     r0,16(r1)
 660:   a1 ff 21 f8     stdu    r1,-96(r1)
 664:   9d ff ff 4b     bl      600 <00000121.plt_call.__printf_chk@@GLIBC_2.17>
 668:   18 00 41 e8     ld      r2,24(r1)
 66c:   60 00 21 38     addi    r1,r1,96
 670:   17 00 60 38     li      r3,23
 674:   10 00 01 e8     ld      r0,16(r1)
 678:   a6 03 08 7c     mtlr    r0
 67c:   20 00 80 4e     blr
 680:   00 00 00 00     .long 0x0
 684:   00 00 00 01     .long 0x1000000
 688:   80 00 00 00     .long 0x80
```

inlined function-procedure.

## clang vectorize
```
$ clang -O3 clang-matmul-old.c tbr.s timebase.c -mcpu=power8 && ./a.out
T = 3.133 s
```

```
$ clang -O3 clang-matmul.c tbr.s timebase.c -mcpu=power8 && ./a.out
T = 0.001 s
```

removed `a[i][j] = 0`, and changed `+=` to just `=`.

## compilers
```
clang: T = 3.136 s
gcc: T = 1.352 s
ibm: T = 0.243 s
nvidia: T = 0.761 s
```
