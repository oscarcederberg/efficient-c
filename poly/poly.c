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
    int coef, expo, terms = 1;

    poly->terms = 0;
    poly->coefs = malloc(sizeof(int) * terms);
    poly->expos = malloc(sizeof(int) * terms);

    current_char = *string;
    while(1){
        coef = 1;
        expo = 0;
        if(current_char == '-'){
            coef = -1;
            string++; //skip space
            current_char = *(string++);
        }else if(current_char == '+'){
            string++; //skip space
            current_char = *(string++);
        }

        if(isdigit(current_char)){
            coef *= current_char-'0';
            current_char = *(string++);
            while(isdigit(current_char)){
                coef += coef * 10 + current_char - '0';
                current_char = *(string++);
            }
        }
        if(current_char == 'x'){
            expo = 1;
            current_char = *(string++);
            if(current_char == '^'){
                current_char = *(string++);
                expo = current_char-'0';
                current_char = *(string++);
                while(isdigit(current_char)){
                    expo += expo * 10 + current_char - '0';
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

        string++; //skip space
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
    int max_terms = p1->terms * p2->terms;
    int* coefs = calloc(max_terms, sizeof(int));
    int* expos = calloc(max_terms, sizeof(int));

    int  p1_terms = p1->terms;
    int* p1_coefs = p1->coefs;
    int* p1_expos = p1->expos;
    int  p2_terms = p2->terms;
    int* p2_coefs = p2->coefs;
    int* p2_expos = p2->expos;

    poly_t* product = malloc(sizeof(poly_t));
    product->terms = 0;
    product->coefs = calloc(max_terms, sizeof(int));
    product->expos = calloc(max_terms, sizeof(int));
    
    int x, y, i = 0;
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

    int coef, expo, terms = 0;
    for (x = 0; x < max_terms; x++)
    {
        coef = product->coefs[x];
        expo = product->expos[x];   
        for (y = 0; y < x + 1; y++)
        {
            if(y == x){
                terms++;
                coefs[y] = coef;
                expos[y] = expo;
                break;
            }
            if(expos[y] == expo){
                coefs[y] += coef;
                break;
            }
        }
    }

    //bubble sort with two arrays
    int largest_expo, last_expo = __INT_MAX__;
    int term = -1;
    for (size_t x = 0; x < terms; x++)
    {
        i = 0;    
        largest_expo = 0;
        for (y = 0; y < terms; y++)
        {
            expo = expos[y];
            if(expo > largest_expo && expo < last_expo){
                largest_expo = expo;
                i = y;
            }
        }

        coef = coefs[i];
        if(coef != 0){
            term++;
            product->coefs[term] = coef;
            product->expos[term] = largest_expo;
        }
        last_expo = largest_expo;
    }

    product->terms = term;
    product->coefs = realloc(product->coefs, sizeof(int) * term);
    product->expos = realloc(product->expos, sizeof(int) * term);

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
        
        if(coef < 0){
            printf("- ");
            coef *= -1;
        }else if(i != 0){
            printf("+ ");
        }

        if(coef != 0) {
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