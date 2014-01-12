#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "stacklib.h"

int weight();

int main() {
	struct char_stack operators;
	struct int_stack numbers;
	struct char_stack digits;
	int i,c,m;
	char b;
	char equation[50] = "(5 * 3 + 8) - 22";
	if(char_stack_init(&operators,100) == NULL) return 1;
	if(int_stack_init(&numbers,100) == NULL) return 1;
	if(char_stack_init(&digits,100) == NULL) return 1;
	for(i = 0;equation[i] != '\0';i++) {
		if(equation[i] == ' ');
		else if(isdigit(equation[i])) {
			c = 0;
			for(;isdigit(equation[i]);i++) char_stack_push(&digits,equation[i]);
			i--;
			for(m = 1;char_stack_pop(&digits,&b);m *= 10) c += m*(b-48);
			int_stack_push(&numbers,c);
		}
		else {
			char_stack_push(&operators,equation[i]);
		}
	}
	for(;int_stack_pop(&numbers,&c);) printf("%i \n",c);
	for(;char_stack_pop(&operators,&b);) printf("%c \n",b);
	// printf("%i \n",strint("55"));
	return 0;
}