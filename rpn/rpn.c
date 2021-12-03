#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE		(10)
#define DEBUG			(0)

int error_occured = 0;
int program_finished = 0;
int current_line = 1;

int stack[MAX_SIZE];
int current_index = -1;

int isFull();
int isEmpty();
int peek(int* out);
int pop(int* out);
int push(int in);

int isFull(){
	return current_index == MAX_SIZE - 1;
}

int isEmpty(){
	return current_index == -1;
}

int peek(int* out){
	if(isEmpty()){
		return 0;
	}
	
	*out = stack[current_index];
	return 1;
}

int pop(int* out){
	if(isEmpty()){
		return 0;
	}
	
	*out = stack[current_index];
	current_index--;
	return 1;
}

int push(int in){
	if(isFull()){
		return 0;
	}

	current_index++;
	stack[current_index] = in;
	return 1;
}

void error(){
	error_occured = 1;
	current_index = -1;
	current_line++;
}

int main(void){
	int current_char = 0, parsed_int = 0, print_line = 1;

	int term_1, term_2;

	while(!program_finished){
		if(error_occured){
			while(current_char != '\n'){
				current_char = getchar();
			}
			error_occured = 0;
		}

		print_line = 1;

		while(!error_occured){
			current_char = getchar();
			if(print_line){
				if(current_char != EOF){
					printf("line %d: ", current_line);
					print_line = 0;
				}else{
					program_finished = 1;
					break;
				}
			}

			#if DEBUG
			if(current_char == '\n'){
				printf("[\\n=10]");
			}else{	
				printf("[%c=%d]",current_char, current_char);
			}
			#endif
			
			if(isdigit(current_char)){
				if(isFull()){
					printf("error at %c\n", current_char);
					error();
					break;
				}

				parsed_int = current_char - '0';
				current_char = getchar();

				while(isdigit(current_char)){
					parsed_int = parsed_int * 10 + (current_char - '0');

					current_char = getchar();
				}
				
				if(!push(parsed_int)){
					printf("error at %d\n", parsed_int);
					error();
					break;
				}
			}

			if(current_char != ' '){	
				if(current_char == '\n'){
					if(!pop(&term_1) || !isEmpty()){
						printf("error at \\n\n");
						error();
						break;
					}else{
						printf("%d\n", term_1);
					}
					
					current_line++;
					break;
				}else if(current_char == EOF){
					program_finished = 1;
					break;
				}else{
					if(!pop(&term_2) || !pop(&term_1)){
						printf("error at %c\n", current_char);
						error();
						break;
					}

					switch(current_char){
						case '+':
							push(term_1 + term_2);
							break;
						case '-':
							push(term_1 - term_2);
							break;
						case '*':
							push(term_1 * term_2);
							break;
						case '/':
							if(term_2 == 0){
								printf("error at /\n");
								error();
							}else{
								push(term_1 / term_2);
							}
							break;
						default:
							printf("error at %c\n", current_char);
							error();
							break;
					}
				}
			}
		}
	}

	return 0;
}