#include "stack.h"

int stack_push(STACK *stack, unsigned char data)
{
	if(stack->stack_top >= 99) return 0;
	else {
		stack->stack[stack->stack_top] = data;
		stack->stack_top++;
		return 1;
	}
	
}


int stack_pop(STACK *stack)
{
	if(stack->stack_top == 0) return 0;
	else {
		stack->stack[stack->stack_top] = 0;
		stack->stack_top--;
		return 1;
	}
}


int stack_read_top(STACK *stack)
{
	return stack->stack[stack->stack_top];
}
