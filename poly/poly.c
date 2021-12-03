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
    int terms = 1;
    int* coefs = malloc(sizeof(int) * terms);
    int* expos = malloc(sizeof(int) * terms);

    poly_t* product = malloc(sizeof(poly_t));

    

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
        }
    }  
}