#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "error.h"
#include "poly.h"

struct poly_t {
    int terms;
    int* coefs;
    int* expos;
};

int current_char;

poly_t*	new_poly_from_string(const char* string){
    poly_t* poly = malloc(sizeof(poly_t));
    int coef, expo, sign, terms = 1;

    poly->terms = 0;
    poly->coefs = malloc(sizeof(int) * terms);
    poly->expos = malloc(sizeof(int) * terms);

    current_char = *(string++);
    while(1){
        coef = 1;
        expo = 0;
        sign = 1;

        // check sign
        if(current_char == '-'){
            sign = -1;
            current_char = *(string++);
            if(current_char == ' '){
                current_char = *(string++); // skip space
            }
        }else if(current_char == '+'){
            current_char = *(string++);
            if(current_char == ' '){
                current_char = *(string++); // skip space
            }
        }

        // check coef
        if(isdigit(current_char)){
            coef = (current_char - '0');
            current_char = *(string++);
            while(isdigit(current_char)){
                coef = coef * 10 + (current_char - '0');
                current_char = *(string++);
            }
            coef *= sign;
        }

        // check expo
        if(current_char == 'x'){
            expo = 1;
            current_char = *(string++);
            if(current_char == '^'){
                current_char = *(string++);
                expo = (current_char - '0');
                current_char = *(string++);
                while(isdigit(current_char)){
                    expo = expo * 10 + (current_char - '0');
                    current_char = *(string++);
                }
            }
        }

        poly->coefs[terms - 1] = coef;
        poly->expos[terms - 1] = expo;

        if(current_char == '\0'){
            break;
        }

        terms++;
        poly->coefs = realloc(poly->coefs, sizeof(int) * terms);
        poly->expos = realloc(poly->expos, sizeof(int) * terms);

        current_char = *(string++);
    }

    poly->terms = terms;

    return poly;
}

void free_poly(poly_t* p){
    free(p->coefs);
    free(p->expos);
    free(p);
}

poly_t*	mul(poly_t* p1, poly_t* p2){
    int x, y, i = 0;

    int  p1_terms = p1->terms;
    int* p1_coefs = p1->coefs;
    int* p1_expos = p1->expos;
    int  p2_terms = p2->terms;
    int* p2_coefs = p2->coefs;
    int* p2_expos = p2->expos;

    int terms_max = p1_terms * p2_terms;

    int* coefs = (int*)calloc(terms_max, sizeof(int));
    int* expos = (int*)calloc(terms_max, sizeof(int));

    poly_t* product = malloc(sizeof(poly_t));
    product->terms = 0;
    product->coefs = (int*)calloc(terms_max, sizeof(int));
    product->expos = (int*)calloc(terms_max, sizeof(int));

    // multiply together terms
    for (x = 0; x < p1_terms; ++x)
    {
        int p1_coef = p1_coefs[x];
        int p1_expo = p1_expos[x];
        for (y = 0; y < p2_terms; ++y)
        {
            product->coefs[i] = p1_coef * p2_coefs[y];
            product->expos[i] = p1_expo + p2_expos[y];
            i++;
        }
    }

    // add together terms with same exponent
    int coef, expo, terms_after_add = 0;
    for (x = 0; x < terms_max; ++x){
        coef = product->coefs[x];
        expo = product->expos[x];
        for (y = 0; y < terms_after_add + 1; ++y){
            if(y == terms_after_add){
                coefs[y] = coef;
                expos[y] = expo;
                terms_after_add++;
                break;
            }else if(expos[y] == expo){
                coefs[y] += coef;
                break;
            }
        }
    }

    // bubble sort with two arrays
    int largest_expo, last_expo = __INT_MAX__;
    int terms_after_sort = 0;
    for (x = 0; x < terms_after_add; ++x){
        i = 0;
        largest_expo = 0;
        for (y = 0; y < terms_after_add; ++y)
        {
            expo = expos[y];
            if(expo >= largest_expo && expo < last_expo){
                largest_expo = expo;
                i = y;
            }
        }

        // remove term if coef is 0
        coef = coefs[i];
        if(coef != 0){
            product->coefs[terms_after_sort] = coef;
            product->expos[terms_after_sort] = largest_expo;
            terms_after_sort++;
        }
        last_expo = largest_expo;
    }

    product->terms = terms_after_sort;
    product->coefs = realloc(product->coefs, sizeof(int) * terms_after_sort);
    product->expos = realloc(product->expos, sizeof(int) * terms_after_sort);

    free(coefs);
    free(expos);
    return product;
}

void print_poly(poly_t* p){
    int coef, expo, terms = p->terms;

    for (size_t i = 0; i < terms; ++i)
    {
        coef = p->coefs[i];
        expo = p->expos[i];

        //printf("%dx^%d", coef, expo);

        if(coef < 0){
            printf("- ");
            coef *= -1;
        }else if(i != 0){
            printf("+ ");
        }

        if(coef != 1 || expo == 0) {
            printf("%d", coef);
        }

        if(expo > 0){
            printf("x");
        }
        if(expo > 1){
            printf("^%d", expo);
        }

        if(i != terms - 1){
            printf(" ");
        }else{
            printf("\n");
        }
    }
}