/**********************************************************************\
* Robbie Mckennie's (probably terrible) implementation of a LIFO stack *
\**********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct int_stack {
	int *stack;
	unsigned int size;
	unsigned int offset;
};

struct char_stack {
	int *stack;
	unsigned int size;
	unsigned int offset;
};

struct int_stack *int_stack_init(struct int_stack *stack,unsigned int size);
int int_stack_push(struct int_stack *stack,int val);
int *int_stack_pop(struct int_stack *stack,int *r);
int *int_stack_peek(struct int_stack *stack,int *r);

struct char_stack *char_stack_init(struct char_stack *stack,unsigned int size);
int char_stack_push(struct char_stack *stack,char val);
char *char_stack_pop(struct char_stack *stack,char *r);
char *char_stack_peek(struct char_stack *stack,char *r);

int main() {
	struct char_stack test;
	int i,c=0;
	char buf;
	char str[] = "345332";
	memset(&test,0,sizeof(test));
	if(char_stack_init(&test,100) == NULL) return 1;
	for(i = 0;str[i] != '\0';i++) char_stack_push(&test,str[i]);
	for(i = 1;char_stack_pop(&test,&buf);i *= 10) c += i*(buf-48);
	printf("%i \n",c);
	return 0;
}

struct int_stack *int_stack_init(struct int_stack *stack,unsigned int size) {
	if((stack->stack = malloc(size*sizeof(int))) == NULL) return NULL;
	stack->size = size;
	stack->offset = 0;
	return stack;
}

int int_stack_push(struct int_stack *stack,int val) {
	if(stack->offset >= stack->size) {
		errno = ENOMEM;
		return -1;
	}
	stack->stack[stack->offset] = val;
	stack->offset++;
	return 0;
}

int *int_stack_pop(struct int_stack *stack,int *r) {
	if(stack->offset <= 0) {
		return NULL;
	}
	stack->offset--;
	*r = stack->stack[stack->offset];
	return r;
}

int *int_stack_peek(struct int_stack *stack,int *r) {
	if(stack->offset <= 0) {
		return NULL;
	}
	*r = stack->stack[stack->offset];
	return r;
}

struct char_stack *char_stack_init(struct char_stack *stack,unsigned int size) {
	if((stack->stack = malloc(size*sizeof(char))) == NULL) return NULL;
	stack->size = size;
	stack->offset = 0;
	return stack;
}

int char_stack_push(struct char_stack *stack,char val) {
	if(stack->offset >= stack->size) {
		errno = ENOMEM;
		return -1;
	}
	stack->stack[stack->offset] = val;
	stack->offset++;
	return 0;
}

char *char_stack_pop(struct char_stack *stack,char *r) {
	if(stack->offset <= 0) {
		return NULL;
	}
	stack->offset--;
	*r = stack->stack[stack->offset];
	return r;
}

char *char_stack_peek(struct char_stack *stack,char *r) {
	if(stack->offset <= 0) {
		return NULL;
	}
	*r = stack->stack[stack->offset];
	return r;
}