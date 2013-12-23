#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "stacklib.h"

int *strint(const char *num,int *r);

int main() {
	struct char_stack operators;
	struct int_stack numbers;
	struct char_stack digits;
	int i,c,m;
	char b;
	char equation[] = "5*3+8-22";
	if(char_stack_init(&operators,100) == NULL) return 1;
	if(int_stack_init(&numbers,100) == NULL) return 1;
	if(char_stack_init(&digits,100) == NULL) return 1;
	for(i = 0;equation[i] != '\0';i++) {
		// if(isdigit(equation[i])) {
			// c = 0;
			// for(;isdigit(equation[i]);i++) char_stack_push(&digits,equation[i]);
			// for(m = 1;char_stack_pop(&digits,&b);m *= 10) c += m*(b-48);
			// int_stack_push(&numbers,c);
		// }
		// else char_stack_push(&operators,equation[i]);
		printf("%c \n",equation[i]);
	}
	// for(;int_stack_pop(&numbers,&c);) printf("%i \n",c);
	// printf("%i \n",strint("55"));
	return 0;
}

int *strint(const char *num,int *r) {
	struct char_stack digits;
	if(char_stack_init(&digits,20) == NULL) return NULL;
	int i;
	char b;
	*r = 0;
	for(i = 0;isdigit(num[i]);i++) char_stack_push(&digits,num[i]);
	for(i = 1;char_stack_pop(&digits,&b);i *= 10) *r += i*(b-48);
	char_stack_stop(&digits);
	return r;
}