/**********************************************************************\
* Robbie Mckennie's (probably terrible) implementation of a LIFO stack *
\**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "stacklib.h"

struct int_stack *int_stack_init(struct int_stack *stack,unsigned int size) {
	memset(stack,0,sizeof(struct int_stack));
	if((stack->stack = malloc(size*sizeof(int))) == NULL) return NULL;
	stack->size = size;
	stack->offset = 0;
	return stack;
}

void int_stack_stop(struct int_stack *stack) {
	free(stack->stack);
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
	memset(stack,0,sizeof(struct char_stack));
	if((stack->stack = malloc(size*sizeof(char))) == NULL) return NULL;
	stack->size = size;
	stack->offset = 0;
	return stack;
}

void char_stack_stop(struct char_stack *stack) {
	free(stack->stack);
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