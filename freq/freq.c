#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct list_t {
    char* word;
    size_t length;
    int count;
    struct list_t* next;
};

struct list_t* first;
int current_line;
int current_char;
char* current_word;
size_t current_word_count;
size_t allocated_size = 1;

void print(char* word, int length) {
    for (size_t i = 0; i < length; ++i)
    {
        printf("%c", word[i]);
    }
}

struct list_t* find(char* word, size_t length) {
    struct list_t* entry = first;

    while (entry) {
        if (entry->length == length && strncmp(entry->word, word, length)) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

void increment(char* word, size_t length) {
    struct list_t* entry = find(word, length);

    if (entry) {
        entry->count += 1;

        if (entry->count == 1)
            printf("added ");
        else
            printf("counted ");
        print(word, length);
        printf("\n");
    } else {
        entry = malloc(sizeof(struct list_t));
        entry->word = malloc(length * sizeof(char));
        entry->length = length;
        entry->count = 1;
        entry->next = first;
        first = entry;

        printf("added ");
        print(word, length);
        printf("\n");
    }
}

void decerment(char* word, size_t length) {
    struct list_t* entry = find(word, length);
    printf("trying to delete ");
    print(word, length);

    if (entry && entry->count > 0) {
        entry->count = 0;
        printf(": deleted\n");
    } else {
        printf(": not found\n");
    }
}

int main(void) {
    current_line = 1;

	while (1) {
		current_char = getchar();

        if(current_char == EOF){
			break;
		}

        current_word = malloc(allocated_size * sizeof(char));
		current_word_count = 0;

		while(current_char != '\n'){
			if(current_word_count > allocated_size){
				allocated_size *= 2;
				current_word = realloc(current_word, sizeof(char)*allocated_size);
			}

			current_word[current_word_count - 1] = current_char;
			current_char = getchar();
		}

	}
}