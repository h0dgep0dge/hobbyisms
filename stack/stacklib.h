/*********************************************************************************\
* Headers for Robbie Mckennie's (probably terrible) implementation of LIFO stacks *
\*********************************************************************************/

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
void int_stack_stop(struct int_stack *stack);
int int_stack_push(struct int_stack *stack,int val);
int *int_stack_pop(struct int_stack *stack,int *r);
int *int_stack_peek(struct int_stack *stack,int *r);

struct char_stack *char_stack_init(struct char_stack *stack,unsigned int size);
void int_stack_stop(struct int_stack *stack);
int char_stack_push(struct char_stack *stack,char val);
char *char_stack_pop(struct char_stack *stack,char *r);
char *char_stack_peek(struct char_stack *stack,char *r);
