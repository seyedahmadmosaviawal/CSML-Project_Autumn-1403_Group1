/**
 * Binary Calculator that work in Terminal!
 * we should connect this code to our simulator
 * 
 * Authors:
 *  1. Seyed Ahmad Mousavi Awal - 402106648
 *  2. Arman Tahmasebizadeh - 402111345
 *  3. Seyed AmirHossein MousaviFard - 402111515
 *  4. Reza Eslami Abyaneh - 402105619
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK 100

// Stack for numbers
typedef struct {
    long long int data[MAX_STACK];
    int top;
} IntStack;

// Stack for operators
typedef struct {
    char data[MAX_STACK];
    int top;
} CharStack;

// Function prototypes
void pushInt(IntStack *stack, long long int value);
long long int popInt(IntStack *stack);
void pushChar(CharStack *stack, char value);
char popChar(CharStack *stack);
int isOperator(char c);
int precedence(char op);
long long int applyOperation(long long int a, long long int b, char op);
long long int shuntingYardExpression(const char *expression);
long long int binaryToDecimal(const char *binary);
void letsPrintString(char* wantPrint);
void letsExit();
char* convertBinaryInputToDecimalInput(char* input);


void pushInt(IntStack *stack, long long int value) {
    /**
     * push long long int in stack
     **/
    stack->data[++stack->top] = value;
}

long long int popInt(IntStack *stack) {
    /**
     * pop long long int from stack
     **/
    return stack->data[stack->top--];
}

void pushChar(CharStack *stack, char operation) {
    /**
     * push char op in stack
     **/
    stack->data[++stack->top] = operation;
}

char popChar(CharStack *stack) {
    /**
     * pop char from stack
     **/
    return stack->data[stack->top--];
}

int isOperator(char c) {
    /**
     * this function is for knowing opertaors
     **/
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    /**
     * Determine operator precedence for algorithm
     **/
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

long long int applyOperation(long long int a, long long int b, char op) {
    /**
     * this function is for apply operation
     * output of this function is (long long int)
     * if we have Error(Divide by zero) the output will be
     * __LONG_LONG_MAX__
     **/
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': {
            if (b == 0) {
                letsPrintString("Error! (Divide by zero)\n");
                return __LONG_LONG_MAX__; // Return __LONG_LONG_MAX__
            }
            return a / b;
        }
    }
    return 0;
}

long long int shuntingYardExpression(const char *expression) {
    /**
     * This is the pivot function!
     * we use "Shunting Yard" Algorithm
     * (https://en.wikipedia.org/wiki/Shunting_yard_algorithm)
     * this is an algorithm for parsing arithmetical or logical expressions
     * we use two stack in this algorithm one is for numbers
     * another is for operations.
     * 
     * if we have error in applying operation we have that output is: __LONG_LONG_MAX__
     **/
    IntStack values = {.top = -1};
    CharStack operators = {.top = -1};
    int openParenthesisCount = 0;



    for (int i = 0; expression[i] != '\0'; i++) {
        if (isdigit(expression[i])) {
            long long int value = 0;
            while (isdigit(expression[i])) {
                value = value * 10 + (expression[i] - '0');
                i++;
            }
            pushInt(&values, value);
            i--;
        } else if (isOperator(expression[i])) {
            while (operators.top >= 0 && precedence(operators.data[operators.top]) >= precedence(expression[i])) {
                long long int b = popInt(&values);
                long long int a = popInt(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_LONG_MAX__) return __LONG_LONG_MAX__;
                pushInt(&values, applyOperation(a, b, op));
            }
            pushChar(&operators, expression[i]);
        } else if (expression[i] == '(') {
            openParenthesisCount++;
            pushChar(&operators, expression[i]);
        } else if (expression[i] == ')') {
            openParenthesisCount--;
            if (openParenthesisCount < 0) {
                letsPrintString("Error! (Mismatched parentheses)\n");
                return __LONG_LONG_MAX__;
            }
            while (operators.data[operators.top] != '(') {
                long long int b = popInt(&values);
                long long int a = popInt(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_LONG_MAX__) return __LONG_LONG_MAX__;
                pushInt(&values, applyOperation(a, b, op));
            }
            popChar(&operators);
        }
    }
    
    if (openParenthesisCount != 0) {
        letsPrintString("Error! (Unmatched opening parentheses)\n");
        return __LONG_LONG_MAX__;
    }

    while (operators.top >= 0) {
        long long int b = popInt(&values);
        long long int a = popInt(&values);
        char op = popChar(&operators);
        if (applyOperation(a, b, op) == __LONG_LONG_MAX__) return __LONG_LONG_MAX__;
        pushInt(&values, applyOperation(a, b, op));
    }

    return popInt(&values);
}

void letsPrintString(char* wantPrint) {
    /**
     * this function is for printing anywhere
     * now we use this function for coding better
     * then when want to add simulators and go to main work,
     * we can just modify this function!
     **/
    printf("%s", wantPrint);
}

void letsPrintNumber(long long int wantPrint) {
    /**
     * print numbers in this function
     **/
    printf("%lld\n", wantPrint);
}

void letsExit(){
    /**
     * exit the program.
     **/
    exit(0);
}

char* convertBinaryInputToDecimalInput(char* input) {
    /**
     * As you see,
     * This function is for convert binary input to decimal
     **/
    char* converted = (char*) malloc(1000 * sizeof(char));
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) {
            long long int value = 0;
            while (isdigit(input[i])) {
                value = value * 2 + (input[i] - '0');
                i++;
            }
            char justValue[100];
            sprintf(justValue, "%lld", value);
            for (unsigned int k = 0; k < strlen(justValue); k++) {
                converted[j] = justValue[k];
                j++;
            }
            i--;
        }
        else {
            converted[j] = input[i];
            j++;   
        }
    }
    converted[j] = '\0';
    return converted;
}

int main() {
    /**
     * Main Function
     * 
     * NOTE: we use 'z' input for backspace!
     **/
    char input[100];
    char currentInput[1000] = "";

    letsPrintString("Enter binary input (use operators +, -, *, /, (, ) and '=' to evaluate, 'q' to quit and 'z' for backspace):\n");

    while (1) {
        letsPrintString("Your Input Character: ");
        scanf("%s", input);

        if (strcmp(input, "q") == 0) {
            letsExit();
        } else if(strcmp(input, "z") == 0) {
            if (strlen(currentInput) > 1) {
                currentInput[strlen(currentInput) - 1] = '\0';
                
                char* justPrint = convertBinaryInputToDecimalInput(currentInput);
                letsPrintString("Current Expression: ");
                letsPrintString(justPrint);
                letsPrintString("\n");

                long long int result = shuntingYardExpression(justPrint);
                if (result == __LONG_LONG_MAX__) continue;
                letsPrintString("Current Result = ");
                letsPrintNumber(result);
            }
        } else if (strcmp(input, "=") == 0) {
            char* justPrint = convertBinaryInputToDecimalInput(currentInput);
            letsPrintString("Current Expression: ");
            letsPrintString(justPrint);
            letsPrintString("\n");

            long long int result = shuntingYardExpression(justPrint);
            if (result == __LONG_LONG_MAX__) continue;
            letsPrintString("Result = ");
            letsPrintNumber(result);

            // Reset the expression
            currentInput[0] = '\0';
        } else if (isOperator(input[0]) || input[0] == '(' || input[0] == ')') {
            // Check for invalid operator sequences or misplaced parentheses
            size_t len = strlen(currentInput);
            if (len > 0 && (isOperator(currentInput[len - 1]) || currentInput[len - 1] == '(') && input[0] != '(') {
                letsPrintString("Error! (Invalid operator sequence or misplaced parentheses)\n");
                continue;
            }
            if (input[0] == ')' && (len == 0 || currentInput[len - 1] == '(')) {
                letsPrintString("Error! (Misplaced closing parenthesis)\n");
                continue;
            }
            
            strcat(currentInput, input);
            letsPrintString("Current Expression: ");
            char* justPrint = convertBinaryInputToDecimalInput(currentInput);
            letsPrintString(justPrint);
            letsPrintString("\n");

            long long int intermediateResult = shuntingYardExpression(justPrint);
            if (intermediateResult != __LONG_LONG_MAX__) {
                printf("Current Result: %lld\n", intermediateResult);
            }
        } else if (input[0] == '0' || input[0] == '1'){
            strcat(currentInput, input);
            letsPrintString("Current Expression: ");
            char* justPrint = convertBinaryInputToDecimalInput(currentInput);
            letsPrintString(justPrint);
            letsPrintString("\n");
            long long int intermediateResult = shuntingYardExpression(justPrint);
            if (intermediateResult != __LONG_LONG_MAX__) {
                printf("Current Result: %lld\n", intermediateResult);
            }
        } else {
            letsPrintString("Error! (bad character)\n");
        }
    }

    return 0;
}