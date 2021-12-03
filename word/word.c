#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int cc;
char* cw;
int cws;
int a = 1;
char* lw;
int lws = 0;

int main(void)
{
	int pf = 0;

	cw = malloc(sizeof(char)*a);

	while(!pf){	
		cws = 0;
		cc = getchar();

		while(isalpha(cc)){
			cws++;
			
			if(cws > a){
				a *= 2;
				cw = realloc(cw, sizeof(char)*a);
			}

			cw[cws - 1] = cc;
			cc = getchar();
		}

		if(cws > lws){
			lws = cws;
			if(lws == 0){
				lw = malloc(sizeof(char)*lws);
			}else{
				lw = realloc(lw, sizeof(char)*lws);
			}

			memcpy(lw, cw, lws);
		}

		if(cc == EOF){
			pf = 1;
			break;
		}
	}

	printf("%d characters in longest word: ", lws);
	for (size_t i = 0; i < lws; ++i)
	{
		printf("%c", lw[i]);
	}
	printf("\n");

	free(cw);
	free(lw);
}
