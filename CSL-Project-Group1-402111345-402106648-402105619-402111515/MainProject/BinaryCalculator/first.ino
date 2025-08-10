/**
 * Binary Calculator for Arduino!
 *
 * Authors:
 *  1. Seyed Ahmad Mousavi Awal - 402106648
 *  2. Arman Tahmasebizadeh - 402111345
 *  3. Seyed AmirHossein MousaviFard - 402111515
 *  4. Reza Eslami Abyaneh - 402105619
 **/

#include <Arduino.h>

// Maximum stack size
#define MAX_STACK 100

// Stack for numbers
typedef struct {
    long int data[MAX_STACK];
    int top;
} IntStack;

// Stack for operators
typedef struct {
    char data[MAX_STACK];
    int top;
} CharStack;

// Function prototypes
void pushInt(IntStack *stack, long int value);
long int popInt(IntStack *stack);
void pushChar(CharStack *stack, char value);
char popChar(CharStack *stack);
int isOperator(char c);
int precedence(char op);
long int applyOperation(long int a, long int b, char op);
long int shuntingYardExpression(const char *expression);
char* convertBinaryInputToDecimalInput(char* input);

// Arduino specific functions
void letsPrintString(const char* wantPrint);
void letsPrintNumber(long int wantPrint);
void letsExit();

// Global variable to hold the current input expression
char currentInput[500] = "";

void pushInt(IntStack *stack, long int value) {
    stack->data[++stack->top] = value;
}

long int popInt(IntStack *stack) {
    return stack->data[stack->top--];
}

void pushChar(CharStack *stack, char operation) {
    stack->data[++stack->top] = operation;
}

char popChar(CharStack *stack) {
    return stack->data[stack->top--];
}

int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

long int applyOperation(long int a, long int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': {
            if (b == 0) {
                letsPrintString("Error! (Divide by zero)\n");
                return __LONG_MAX__;
            }
            return a / b;
        }
    }
    return 0;
}

long int shuntingYardExpression(const char *expression) {
    IntStack values;
    CharStack operators;

    // Initialize stacks
    values.top = -1;
    operators.top = -1;

    int openParenthesisCount = 0;

    for (int i = 0; expression[i] != '\0'; i++) {
        if (isdigit(expression[i])) {
            long int value = 0;
            while (isdigit(expression[i])) {
                value = value * 10 + (expression[i] - '0');
                i++;
            }
            pushInt(&values, value);
            i--;
        } else if (isOperator(expression[i])) {
            while (operators.top >= 0 && precedence(operators.data[operators.top]) >= precedence(expression[i])) {
                long int b = popInt(&values);
                long int a = popInt(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_MAX__) return __LONG_MAX__;
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
                return __LONG_MAX__;
            }
            while (operators.data[operators.top] != '(') {
                long int b = popInt(&values);
                long int a = popInt(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_MAX__) return __LONG_MAX__;
                pushInt(&values, applyOperation(a, b, op));
            }
            popChar(&operators);
        }
    }

    if (openParenthesisCount != 0) {
        letsPrintString("Error! (Unmatched opening parentheses)\n");
        return __LONG_MAX__;
    }

    while (operators.top >= 0) {
        long int b = popInt(&values);
        long int a = popInt(&values);
        char op = popChar(&operators);
        if (applyOperation(a, b, op) == __LONG_MAX__) return __LONG_MAX__;
        pushInt(&values, applyOperation(a, b, op));
    }

    return popInt(&values);
}

char* convertBinaryInputToDecimalInput(char* input) {
    static char converted[200];
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i])) {
            long int value = 0;
            while (isdigit(input[i])) {
                value = value * 2 + (input[i] - '0');
                i++;
            }
            char justValue[10];
            sprintf(justValue, "%ld", value);
            for (unsigned int k = 0; k < strlen(justValue); k++) {
                converted[j] = justValue[k];
                j++;
            }
            i--;
        } else {
            converted[j] = input[i];
            j++;
        }
    }
    converted[j] = '\0';
    return converted;
}

void letsPrintString(const char* wantPrint) {
    Serial.print(wantPrint);
}

void letsPrintNumber(long int wantPrint) {
    Serial.print(wantPrint);
    Serial.print("\n");
}

void letsExit() {
    while (1);
}

void printCurrentResult() {
    letsPrintString("Current Expression: ");
    letsPrintString(currentInput);
    letsPrintString("\n");
    char* justPrint = convertBinaryInputToDecimalInput(currentInput);
    letsPrintString("Current Result = ");
    letsPrintNumber(shuntingYardExpression(justPrint));
}

void setup() {
    Serial.begin(9600);
    letsPrintString("Enter binary input (use operators +, -, *, /, (, ) and '=' to evaluate, 'q' to quit and 'z' for backspace):\n");
}

void loop() {
    if (Serial.available()) {
        char input = Serial.read();
        if (input == 'q') {
            letsExit();
        } else if (input == 'z') {
            if (strlen(currentInput) > 0) {
                currentInput[strlen(currentInput) - 1] = '\0'; // Remove last character
                if (currentInput[strlen(currentInput) - 1] == '1' || currentInput[strlen(currentInput) - 1] == '0') printCurrentResult();
                else {
                    letsPrintString("Current Expression: ");
                    letsPrintString(currentInput);
                    letsPrintString("\n");
                }
            } else {
                letsPrintString("Error! (No characters to delete)\n");
            }
        } else if (input == '=') {
            printCurrentResult();
            currentInput[0] = '\0';
        } else if (isdigit(input) || isOperator(input) || input == '(' || input == ')') {
            size_t len = strlen(currentInput);
            if (len > 0 && isOperator(currentInput[len - 1]) && isOperator(input)) {
                letsPrintString("Error! (Invalid operator sequence)\n");
                return;
            }
            currentInput[len] = input;
            currentInput[len + 1] = '\0';
            if (isdigit(input)) printCurrentResult();
            else {
                letsPrintString("Current Expression: ");
                letsPrintString(currentInput);
                letsPrintString("\n");
            }
        } else {
            letsPrintString("Error! (Invalid character)\n");
        }
    }
}