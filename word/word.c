#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int current_char;
char* current_word;
int current_word_count;
int allocated_size = 1;
char* longest_word;
int longest_word_count = 0;

int main(void)
{
	current_word = malloc(sizeof(char)*allocated_size);

	while(1){	
		current_word_count = 0;
		current_char = getchar();

		while(isalpha(current_char)){
			current_word_count++;
			
			if(current_word_count > allocated_size){
				allocated_size *= 2;
				current_word = realloc(current_word, sizeof(char)*allocated_size);
			}

			current_word[current_word_count - 1] = current_char;
			current_char = getchar();
		}

		if(current_word_count > longest_word_count){
			longest_word_count = current_word_count;
			if(longest_word_count == 0){
				longest_word = malloc(sizeof(char)*longest_word_count);
			}else{
				longest_word = realloc(longest_word, sizeof(char)*longest_word_count);
			}

			memcpy(longest_word, current_word, longest_word_count);
		}

		if(current_char == EOF){
			break;
		}
	}

	printf("%d characters in longest word: ", longest_word_count);
	for (size_t i = 0; i < longest_word_count; ++i)
	{
		printf("%c", longest_word[i]);
	}
	printf("\n");

	free(current_word);
	free(longest_word);
}
