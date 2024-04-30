#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 100
#define MAX_ALPHABET 50
#define MAX_TRANSITIONS 300

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
    char states[MAX_STATES];
    char alphabet[MAX_ALPHABET];
    char stackAlphabet[MAX_ALPHABET];
    Transition transitions[MAX_TRANSITIONS];
    char initialState;
    char z0; // first symbol in the stack
    char finalStates[MAX_STATES];
    int transitionCount;
    int finalStateCount;
} PushdownAutomaton;

void readFromFile(const char* filename, PushdownAutomaton* pda) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int isTransitionSection = 0;

    while (fgets(line, sizeof(line), file)) {
        if (isTransitionSection) {

            parseTransitions(file, pda);
            break;
        } else {

            switch (line[0]) {
                case 's':
                    if (strstr(line, "states:") != NULL) {
                        parseStates(line, pda);
                    } else if (strstr(line, "stack alphabet:") != NULL) {
                        parseStackAlphabet(line, pda);
                    }
                    break;
                case 'i':
                    if (strstr(line, "initial state:") != NULL) {
                        parseInitialState(line, pda);
                    } else if (strstr(line, "input alphabet:") != NULL) {
                        parseInputAlphabet(line, pda);
                    }
                    break;
                case 'f':
                    if (strstr(line, "final states:") != NULL) {
                        parseFinalStates(line, pda);
                    }
                    break;
                case 't':
                    if (strstr(line, "transitions:") != NULL) {
                        isTransitionSection = 1;
                    }
                    break;
                default:

                    break;
            }
        }
    }

    fclose(file);

    printf("States: ");
    for (int i = 0; i < MAX_STATES && pda->states[i] != '\0'; i++) {
        printf("%c ", pda->states[i]);
    }
    printf("\n");

    printf("Initial state: %c\n", pda->initialState);

    printf("Final states: ");
    for (int i = 0; i < MAX_STATES && pda->finalStates[i] != '\0'; i++) {
        printf("%c ", pda->finalStates[i]);
    }
    printf("\n");

    printf("Input alphabet: ");
    for (int i = 0; i < MAX_ALPHABET && pda->alphabet[i] != '\0'; i++) {
        printf("%c ", pda->alphabet[i]);
    }
    printf("\n");

    printf("Stack alphabet: ");
    for (int i = 0; i < MAX_ALPHABET && pda->stackAlphabet[i] != '\0'; i++) {
        printf("%c ", pda->stackAlphabet[i]);
    }
    printf("\n");

    printf("Transitions:\n");
    for (int i = 0; i < pda->transitionCount; i++) {
        Transition* transition = &pda->transitions[i];
        printf("State %c, Input %c, Next state %c, Stack top %c, Result %s\n",
               transition->state, transition->inputSymbol, transition->nextState,
               transition->stackSymbol, transition->result);
    }
}


void parseStates(const char* line, PushdownAutomaton* pda) {
    int pos = strchr(line, ':') - line + 1;
    char* token = strtok((char*)line + pos, ", ");
    int index = 0;
    while (token) {
        pda->states[index++] = token[0];
        token = strtok(NULL, ", ");
    }
}

void parseInitialState(const char* line, PushdownAutomaton* pda) {
    int pos = strchr(line, ':') - line + 2;
    pda->initialState = line[pos];
}

void parseInputAlphabet(const char* line, PushdownAutomaton* pda) {
    int pos = strchr(line, ':') - line + 1;
    char* token = strtok((char*)line + pos, ", ");
    int index = 0;
    while (token) {
        pda->alphabet[index++] = token[0];
        token = strtok(NULL, ", ");
    }
}

void parseStackAlphabet(const char* line, PushdownAutomaton* pda) {
    int pos = strchr(line, ':') - line + 1;
    char* token = strtok((char*)line + pos, ", ");
    int index = 0;
    while (token) {

        if (token[0] >= ' ' && token[0] <= '~') {

            pda->stackAlphabet[index++] = token[0];
        } else {
            printf("Warning: Invalid character '%c' found in stack alphabet\n", token[0]);
        }
        token = strtok(NULL, ", ");
    }

    pda->stackAlphabet[index] = '\0';
}

void parseFinalStates(const char* line, PushdownAutomaton* pda) {
    int pos = strchr(line, ':') - line + 1;
    char* token = strtok((char*)line + pos, ", ");
    int index = 0;
    while (token) {

        pda->finalStates[index++] = token[0];
        token = strtok(NULL, ", ");
    }
    pda->finalStates[index] = '\0';
    pda->finalStateCount = index;
}


void parseTransitions(FILE* file, PushdownAutomaton* pda) {
    char line[256];
    int index = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline character from the end of the line
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }

        // Ignore empty lines
        if (len == 0) {
            continue;
        }

        // Tokenize the line and parse the transition
        char* token = strtok(line, " ");
        if (token == NULL) {
            continue;
        }

        // Create a transition structure
        Transition transition;
        
        // Read the current state
        transition.state = token[0];

        // Read the input symbol and expected stack top
        token = strtok(NULL, " (");
        if (token == NULL || strlen(token) != 1) {
            continue;
        }
        transition.inputSymbol = token[0];

        // Read the stack symbol
        token = strtok(NULL, " ");
        if (token == NULL || strlen(token) != 1) {
            continue;
        }
        transition.stackSymbol = token[0];

        // Read the next state
        token = strtok(NULL, " )");
        if (token == NULL || strlen(token) != 1) {
            continue;
        }
        transition.nextState = token[0];

        // Read the result (symbols to push onto the stack)
        token = strtok(NULL, " ");
        if (token == NULL) {
            continue;
        }
        strncpy(transition.result, token, sizeof(transition.result) - 1);
        transition.result[sizeof(transition.result) - 1] = '\0';

        // Add the transition to the PDA
        pda->transitions[index++] = transition;
    }

    // Update the transition count
    pda->transitionCount = index;
}


int isFinalState(PushdownAutomaton* pda, char state) {
    for (int i = 0; i < pda->finalStateCount; i++) {
        if (pda->finalStates[i] == state) {
            return 1; // True
        }
    }
    return 0; // False
}


int simulatePDA(PushdownAutomaton* pda, const char* input) {
// Initialize the stack and push the initial stack symbol onto it
    Stack* stack = createStack(MAX_ALPHABET);
    push(stack, pda->z0);
    
    // Set the current state to the initial state
    char currentState = pda->initialState;
    
    // Iterate through each symbol in the input string
    for (int i = 0; input[i] != '\0'; i++) {
        char inputSymbol = input[i];
        char stackTop = isEmpty(stack) ? '\0' : stack->array[stack->top];
        int transitionFound = 0;
        
        // Search for a valid transition based on the current state, input symbol, and stack top
        for (int j = 0; j < pda->transitionCount; j++) {
            Transition* transition = &pda->transitions[j];
            
            // Check if the current state, input symbol, and stack symbol match the transition
            if (transition->state == currentState &&
                transition->inputSymbol == inputSymbol &&
                transition->stackSymbol == stackTop) {
                
                // Update the current state
                currentState = transition->nextState;
                
                // Perform stack operation according to the transition's result
                // Pop the stack if the result is empty ("e"), otherwise push each symbol in the result onto the stack
                if (strcmp(transition->result, "e") == 0) {
                    pop(stack);
                } else {
                    // First, pop the stack to remove the stack symbol
                    pop(stack);
                    
                    // Push each character in the transition result onto the stack
                    for (int k = strlen(transition->result) - 1; k >= 0; k--) {
                        push(stack, transition->result[k]);
                    }
                }
                
                // Transition found, break the loop
                transitionFound = 1;
                break;
            }
        }
        
        // If no valid transition is found, reject the input string
        if (!transitionFound) {
            free(stack->array);
            free(stack);
            return 0; // Input string is rejected
        }
    }
    
    // Check if the current state is a final state and the stack is empty
    printf("__%d__", isFinalState(pda, currentState));
    printf("__%d__", isEmpty(stack));
    int isAccepted = isFinalState(pda, currentState) && isEmpty(stack);
    
    // Free the stack memory
    free(stack->array);
    free(stack);
    
    // Return whether the input string is accepted or rejected
    return isAccepted ? 1 : 0;
}


int main() {
    PushdownAutomaton pda;

    readFromFile("C:\\Users\\Andrei\\Documents\\Facultate\\Unibuc\\Anul III\\Sem II\\Fundamentele Proiectarii Compilatoarelor\\proiect_2\\input.txt", &pda);

    const char* inputString = "(())()";

    
    int result = simulatePDA(&pda, inputString);
    
    if (result) {
        printf("The input string \"%s\" is accepted by the PDA.\n", inputString);
    } else {
        printf("The input string \"%s\" is rejected by the PDA.\n", inputString);
    }
    
    return 0;
}
