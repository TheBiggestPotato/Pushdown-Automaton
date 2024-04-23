#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int top;
    unsigned capacity;
    char* array;
} Stack;

Stack* createStack(unsigned capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (char*)malloc(stack->capacity* sizeof(char));
    return stack;
}

int isFull(Stack* stack) {

    return stack->top == stack->capacity - 1;
}

int isEmpty(Stack* stack) {

    return stack->top == -1;
}

void push(Stack* stack, int element) {

    if(!isFull(stack)) {
            stack->array[++stack->top] = element;
    } else {
        return;
    }
}

char pop(Stack* stack) {
    if(!isEmpty(stack)) {
        return stack->array[stack->top--];
    }
}

typedef struct {
    char state;
    char nextState;
    char inputSymbol;
    char stackSymbol;
    char result[5];
} Transition;

typedef struct {
    char states[100];
    char alphabet[50];
    char stackAlphabet[50];
    Transition transitions[300];
    char initialState;
    char z0; // first symbol in the stack
    char finalStates[100];
} PushdownAutomaton;

PushdownAutomaton generateAutoamatonFromFile(FILE* file) {

    PushdownAutomaton automaton;
    char inputCh;

    while (!feof(file)) {
        inputCh = fgetc(file);
        printf("%c", inputCh);

    }

    return automaton;
}

int main() {

    printf("hello");

    FILE* file;

    

    file = fopen("input.txt", "r");

    if (file == NULL) {
        printf("file reading error! \n");
    }

    generateAutoamatonFromFile(file);

    fclose(file);
    return 0;
}
