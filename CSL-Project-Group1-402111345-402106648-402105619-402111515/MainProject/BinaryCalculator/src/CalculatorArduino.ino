/**
 * Binary Calculator for Arduino!
 *
 * Authors:
 *  1. Seyed Ahmad Mousavi Awal - 402106648
 *  2. Arman Tahmasebizadeh - 402111345
 *  3. Seyed AmirHossein MousaviFard - 402111515
 *  4. Reza Eslami Abyaneh - 402105619
 **/

// Includes:
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// PINs:
#define BUTTON_1 2
#define BUTTON_0 3
#define BUTTON_OPEN_PARENTHESIS 4
#define BUTTON_CLOSE_PARENTHESIS 5
#define BUTTON_PLUS 6
#define BUTTON_MINUS 7
#define BUTTON_DIVIDE 8
#define BUTTON_MULTIPLY 9
#define BUTTON_BACKSPACE 10
#define BUTTON_QUIT 11
#define BUTTON_EQUAL 12

// Maximum stack size
#define MAX_STACK 100

LiquidCrystal_I2C lcd(0x27, 20, 4);

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
void pushNumber(IntStack *stack, long int value);
long int popNumber(IntStack *stack);
void pushChar(CharStack *stack, char value);
char popChar(CharStack *stack);
int isOperator(char c);
int precedence(char op);
long int applyOperation(long int a, long int b, char op);
long int shuntingYardExpression(const char *expression);
char* convertBinaryInputToDecimalInput(char* input);
int haveErrorParantesesOrDividing(char* expression);
int weHaveErrorInExpression(char* expression);
long int writeOnLCD();
void writeOnLCD(String result);
void writeExpression(String decimal);
void upadateExpressionAfterEqual();
int readButton();
void letsExit();

// Global variable to hold the current input expression
char currentInput[500] = "";

void pushNumber(IntStack *stack, long int value) {
    /**
     * push long long int in stack
     **/
    stack->data[++stack->top] = value;
}

long int popNumber(IntStack *stack) {
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

long int applyOperation(long int a, long int b, char op) {
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
                return __LONG_MAX__;
            }
            return a / b;
        }
    }
    return 0;
}

long int shuntingYardExpression(char* expression) {
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
    IntStack values;
    CharStack operators;
    values.top = -1;
    operators.top = -1;

    for (int i = 0; expression[i] != '\0'; i++) {
        if (isdigit(expression[i])) {
            long int value = 0;
            while (isdigit(expression[i])) {
                value = value * 10 + (expression[i] - '0');
                i++;
            }
            pushNumber(&values, value);
            i--;
        } else if (isOperator(expression[i])) {
            while (operators.top >= 0 && precedence(operators.data[operators.top]) >= precedence(expression[i])) {
                long int b = popNumber(&values);
                long int a = popNumber(&values);
                char op = popChar(&operators);
                pushNumber(&values, applyOperation(a, b, op));
            }
            pushChar(&operators, expression[i]);
        } else if (expression[i] == '(') {
            pushChar(&operators, expression[i]);
        } else if (expression[i] == ')') {
            while (operators.data[operators.top] != '(') {
                long int b = popNumber(&values);
                long int a = popNumber(&values);
                char op = popChar(&operators);
                pushNumber(&values, applyOperation(a, b, op));
            }
            popChar(&operators);
        }
    }
    while (operators.top >= 0) {
        long int b = popNumber(&values);
        long int a = popNumber(&values);
        char op = popChar(&operators);
        pushNumber(&values, applyOperation(a, b, op));
    }

    return popNumber(&values);
}

char* convertBinaryInputToDecimalInput(char* input) {
    /**
     * This function is for converting Binary Input to Decimal
     */
    static char converted[200];
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isspace(input[i])) continue;
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
    if (j == 0) {
        converted[0] = '0';
        converted[1] = '\0';
    } else converted[j] = '\0';
    return converted;
}

void letsExit() {
    /**
     * This function is for exit from Arduino
     */
    lcd.off();
    while (1);
}

int haveErrorParantesesOrDividing(char* expression) {
    /**
     * This function is for finding some errors in expression
     * 
     * This function will return:
     *      1. dividing by zero
     *      2. we have unmatched paranteses
     *      3. we have open paranteses
     *      0. NO Error!
     **/
    IntStack values;
    CharStack operators;
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
            pushNumber(&values, value);
            i--;
        } else if (isOperator(expression[i])) {
            while (operators.top >= 0 && precedence(operators.data[operators.top]) >= precedence(expression[i])) {
                long int b = popNumber(&values);
                long int a = popNumber(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_MAX__) return 1;
                pushNumber(&values, applyOperation(a, b, op));
            }
            pushChar(&operators, expression[i]);
        } else if (expression[i] == '(') {
            openParenthesisCount++;
            pushChar(&operators, expression[i]);
        } else if (expression[i] == ')') {
            openParenthesisCount--;
            if (openParenthesisCount < 0) return 2;
            while (operators.data[operators.top] != '(') {
                long int b = popNumber(&values);
                long int a = popNumber(&values);
                char op = popChar(&operators);
                if (applyOperation(a, b, op) == __LONG_MAX__) return 1;
                pushNumber(&values, applyOperation(a, b, op));
            }
            popChar(&operators);
        }
    }

    if (openParenthesisCount != 0) return 3;
    return 0;
}

int weHaveErrorInExpression(char* expression) {
    /**
     * This function is for finding All errors in expression
     * 
     * This function will return:
     *      1. dividing by zero
     *      2. we have unmatched paranteses
     *      3. we have open paranteses
     *      4. if we have two operator that are left to each other we have error
     *      5. if last char is not digit we can't calculate
     *      0. NO Error!
     **/

    int len = strlen(expression);
    // NO Error in len = 0:
    if (len == 0) return 0;
    
    // if we have two operator that are left to each other we have error:
    for (int i = 0; i < len - 1; i++) {
        if (isOperator(expression[i]) && isOperator(expression[i + 1])) return 4;
    }

    // if last char is not digit we can't calculate:
    if (expression[len - 1] != '1' && expression[len - 1] != '0' && expression[len - 1] != ')') return 5;

    return haveErrorParantesesOrDividing(expression);
}

long int writeOnLCD() {
    /**
     * This function is for writing on LCD
     * also send the information in serial for UI
     */
    int errorCode = weHaveErrorInExpression(currentInput);
    if (errorCode) {
        if (errorCode == 5) {
            writeOnLCD("");

            Serial.println("Result: ");
            Serial.println("Expression:" + String(currentInput));
            return __LONG_MAX__;
        }
        writeOnLCD("Error code=");
        lcd.write(errorCode - 1 + '1');

        Serial.println("Result:Error code= " + String(errorCode));
        Serial.println("Expression:" + String(currentInput));
        return __LONG_MAX__;
    }

    char* decimal = convertBinaryInputToDecimalInput(currentInput);
    lcd.clear();
    // write expression:
    writeExpression(decimal);
    // write result:
    lcd.setCursor(0, 3);
    lcd.print("Result:");
    lcd.setCursor(8, 3);
    long res = shuntingYardExpression(decimal);
    lcd.print(res);

    Serial.println("Result:" + String(res));
    Serial.println("Expression:" + String(currentInput));

    return res;
}

void writeOnLCD(String result) {
    char* decimal = convertBinaryInputToDecimalInput(currentInput);
    lcd.clear();
    // write expression:
    writeExpression(decimal);
    // write result:
    lcd.setCursor(0, 3);
    lcd.print("Result:");
    lcd.setCursor(8, 3);
    lcd.print(result); 
}

void writeExpression(String decimal) {
    lcd.setCursor(0, 0);
    lcd.print("Bin:");
    lcd.setCursor(5, 0);
    lcd.print(currentInput);
    
    lcd.setCursor(0, 1);
    lcd.print("Dec:");
    lcd.setCursor(5, 1);
    lcd.print(decimal);
}

void upadateExpressionAfterEqual() {
    if (weHaveErrorInExpression(currentInput)) return;

    char* decimal = convertBinaryInputToDecimalInput(currentInput);
    long int result = shuntingYardExpression(decimal);
    long int res = result;
    char justSave[100];
    int i = 0;
    while(result != 0) {
        justSave[i] = result % 2 + '0';
        result /= 2;
        i++;
    }
    for (int j = i - 1; j >= 0; j--) {
        currentInput[i - 1 - j] = justSave[j];
    }
    currentInput[i] = '\0';
    sprintf(justSave, "%ld", res);
    writeOnLCD(justSave);
    
    Serial.println("Result:" + String(res));
    Serial.println("Expression:" + String(currentInput));
}

int readButton() {
    if (digitalRead(BUTTON_1) == LOW) return '1';
    if (digitalRead(BUTTON_0) == LOW) return '0';
    if (digitalRead(BUTTON_OPEN_PARENTHESIS) == LOW) return '(';
    if (digitalRead(BUTTON_CLOSE_PARENTHESIS) == LOW) return ')';
    if (digitalRead(BUTTON_PLUS) == LOW) return '+';
    if (digitalRead(BUTTON_MINUS) == LOW) return '-';
    if (digitalRead(BUTTON_DIVIDE) == LOW) return '/';
    if (digitalRead(BUTTON_MULTIPLY) == LOW) return '*';
    if (digitalRead(BUTTON_BACKSPACE) == LOW) return 'z';
    if (digitalRead(BUTTON_QUIT) == LOW) return 'q';
    if (digitalRead(BUTTON_EQUAL) == LOW) return '=';
    return 0; // No button pressed
}



void setup() {
    Serial.begin(9600);
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(1, 1);
    lcd.print("Binary  Calculator");

    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_0, INPUT_PULLUP);
    pinMode(BUTTON_OPEN_PARENTHESIS, INPUT_PULLUP);
    pinMode(BUTTON_CLOSE_PARENTHESIS, INPUT_PULLUP);
    pinMode(BUTTON_PLUS, INPUT_PULLUP);
    pinMode(BUTTON_MINUS, INPUT_PULLUP);
    pinMode(BUTTON_DIVIDE, INPUT_PULLUP);
    pinMode(BUTTON_MULTIPLY, INPUT_PULLUP);
    pinMode(BUTTON_BACKSPACE, INPUT_PULLUP);
    pinMode(BUTTON_QUIT, INPUT_PULLUP);
    pinMode(BUTTON_EQUAL, INPUT_PULLUP);
}

void loop() {
    char input = readButton();
    delay(100);
    if (input) {
        delay(300);
        if (input == 'q') {
            letsExit();
        } else if (input == 'z') {
            if (strlen(currentInput) > 0) {
                currentInput[strlen(currentInput) - 1] = '\0'; // Remove last character
                writeOnLCD();
            } else {
                writeOnLCD("NO Character");
            }
        } else if (input == '=') {
            upadateExpressionAfterEqual();
        } else if (isdigit(input) || isOperator(input) || input == '(' || input == ')') {
            int len = strlen(currentInput);
            currentInput[len] = input;
            currentInput[len + 1] = '\0';
            writeOnLCD();
        } else {
            writeOnLCD("Error code=5");
        }
    }

    // Handle data received from Serial:
    if (Serial.available()) {
        String received = Serial.readStringUntil('\n');
        received.trim();
        received.toCharArray(currentInput, received.length() + 1);
        writeOnLCD();
    }
    delay(150);
}