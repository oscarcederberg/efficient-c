#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CHARS 64

struct word_t {
    size_t length;
    char *string;
};

struct list_t {
    struct word_t *word;
    int count;
    struct list_t *next;
};

struct list_t  *first;

void free_word(struct word_t *word) {
    free(word->string);
    free(word);
}

void free_list() {
    struct list_t* temp;
    struct list_t *entry = first;

    while (entry) {
        temp = entry->next;
        free_word(entry->word);
        free(entry);
        entry = temp;
    }
}

void print_word(struct word_t *word) {
    for (size_t i = 0; i < word->length; ++i)
    {
        printf("%c", word->string[i]);
    }
}

void print_list() {
    struct list_t *entry = first;
    while(entry) {
        printf("{word: %s, length: %lu, count: %d}\n", entry->word->string, entry->word->length, entry->count);
        entry = entry->next;
    }
}

struct list_t *find(struct word_t *word) {
    struct list_t *entry = first;

    while (entry) {
        if (entry->word->length == word->length && strncmp(entry->word->string, word->string, word->length) == 0) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

void increment(struct word_t *word) {
    struct list_t *entry = find(word);

    if (entry) {
        entry->count += 1;

        if (entry->count == 1)
            printf("added ");
        else
            printf("counted ");
        print_word(word);
        printf("\n");
        free_word(word);
    } else {
        entry = (struct list_t *) malloc(sizeof(struct list_t));
        entry->word = word;
        entry->count = 1;
        entry->next = first;
        first = entry;

        printf("added ");
        print_word(word);
        printf("\n");
    }
}

void delete(struct word_t *word) {
    struct list_t *entry = find(word);
    printf("trying to delete ");
    print_word(word);

    if (entry && entry->count > 0) {
        entry->count = 0;
        printf(": deleted\n");
    } else {
        printf(": not found\n");
    }

    free_word(word);
}

struct word_t *get_word() {
    unsigned int length;
    struct word_t *word = calloc(1, sizeof(struct word_t));
    int current_char = getchar();

    for (length = 0; !isspace(current_char) && current_char != EOF; length++) {
        word->string = (char *) realloc(word->string, (length + 2) * sizeof(char));
        word->string[length] = current_char;
        current_char = getchar();
    }

    if (length == 0) {
        free_word(word);
        return NULL;
    }

    word->string[length] = '\0';
    word->length = length;
    return word;
}

int is_prime(unsigned int number) {
    if (number == 1) {
        return 0;
    }

    unsigned int max = sqrt(number);

    for (unsigned int i = 2; i <= max; i++) {
        if (number % i == 0) {
            return 0;
        }
    }

    return 1;
}

int compare_words(struct word_t *first, struct word_t *second) {
    int length = first->length > second->length ? first->length : second->length;
    return strncmp(first->string, second->string, length);
}

struct list_t *find_most_frequent_entry() {
    struct list_t *most_frequent = first;
    struct list_t *entry = first->next;

    while (entry) {
        if (entry->count == most_frequent->count && compare_words(entry->word, most_frequent->word) < 0) {
            most_frequent = entry;
        } else if (entry->count > most_frequent->count) {
            most_frequent = entry;
        }

        entry = entry->next;
    }

    return most_frequent;
}

int main(void) {
    unsigned int current_line = 0;
    struct word_t *word = get_word();

    while (word) {
        current_line++;
        if (is_prime(current_line)) {
            delete(word);
        } else {
            increment(word);
        }

        word = get_word();
    }

    struct list_t *entry = find_most_frequent_entry();
    printf("result: %s %u\n", entry->word->string, entry->count);

    free_list();
}