#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include <stdlib.h>

#include <GL/glut.h>

#ifdef _WIN32
#include <W.indows.h>
#else
#include <unistd.h>
#endif

#define WINDOW_SIZE_X 250
#define WINDOW_SIZE_Y 370
#define CALC_X 10
#define CALC_Y 10
#define BUTTON_WIDTH 50
#define GAP 10
#define BTN_STR_MAX_LEN 14



struct button {
	int x1, y1, x2, y2;
	int state;
	char str[BTN_STR_MAX_LEN + 4];
};

typedef struct button BUTTON;
BUTTON btnArray[5][4];
BUTTON btnScreen;
char expression[40];
char disp[BTN_STR_MAX_LEN];

void drawstring(float x, float y, const char *string) {
	const char *c;
	glRasterPos2f(x, y);

	for (c = string; *c != '\0'; c++) {
		glColor3f(1, 1, 1);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}


BUTTON initButton(int x11, int y11, int x22, int y22, char *str1) {
	BUTTON btn;
	btn.x1 = x11;
	btn.y1 = y11;
	btn.x2 = x22;
	btn.y2 = y22;
	btn.state = 1;
	strcpy(btn.str, str1);

	return btn;
}

void draw(BUTTON btn) {

	glColor3f(1, 1, 1);
	
	drawstring(btn.x1 + GAP, btn.y1 + GAP, btn.str);

	glBegin(GL_LINE_LOOP);
	glVertex2d(btn.x1, btn.y1);
	glVertex2d(btn.x1, btn.y2);
	glVertex2d(btn.x2, btn.y2);
	glVertex2d(btn.x2, btn.y1);
	glEnd();	
}

BUTTON togglestate(BUTTON btn) {
	btn.state = (btn.state == 1) ? 0 : 1;
	return btn;
}

int insidebutton(BUTTON btn, int x, int y) {
	if (x > btn.x1 && x < btn.x2 && y > btn.y1 && y < btn.y2)
		return 1;
	else return 0;
}

struct Stack {
	int top;
	unsigned capacity;
	int* array;
};

struct Stack* createStack(unsigned capacity) { 
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));

	if (!stack) {
		return NULL;
	}

	stack->top = -1;
	stack->capacity = capacity;

	stack->array = (int*) malloc(stack->capacity * sizeof(int));
	return stack;
}

int isEmpty(struct Stack* stack) {
	return stack->top == -1 ;
}

char peek(struct Stack* stack) { 
	return stack->array[stack->top];
}

char pop(struct Stack* stack) { 
	if (!isEmpty(stack)) {
		return stack->array[stack->top--];
	}
	return '$';
}

void push(struct Stack* stack, char op) { 
	stack->array[++stack->top] = op; 
}

int fnIsNum(char ch) { 
	return (ch >= '0' && ch <= '9');
}

int fnPrecedence(char ch) { 
	switch (ch) 
	{
		case '$':
			return -1;
		case '(':
			return 0;
		case '+':
		case '-':
			return 1;

		case '*':
		case '/':
			return 2;

		case '^':
			return 3; 
	} 
	return -1; 
} 
  
void fnInfixToPostfix(char* exp) { 
	int i = 0, k = -1;

	struct Stack* stack = createStack(strlen(exp));
	if(!stack) {
		strcpy(exp, "ERROR");
	}

	push(stack, '$');

	char myExp[strlen(exp) + 10];

	while(exp[i] != '\0') {

		if (fnIsNum(exp[i])) {
			while(fnIsNum(exp[i])) {
				myExp[++k] = exp[i++];
			}
			myExp[++k] = ' ';			
		}
		else if (exp[i] == '(') {
			push(stack, exp[i++]);
		}

		else if (exp[i] == ')') {
			while (!isEmpty(stack) && peek(stack) != '(') {
				myExp[++k] = pop(stack);
			}
			i++;
			pop(stack);
		}
		else {
			while (fnPrecedence(exp[i]) <= fnPrecedence(peek(stack))) {
				myExp[++k] = pop(stack);
				myExp[++k] = ' ';
			}
			push(stack, exp[i++]);
		}
	}

	if(peek(stack) == '$') {
		k++;
	}
	else {
		while (peek(stack) != '$') {
			myExp[++k] = pop(stack);
			myExp[++k] = ' ';
		}
	}
	myExp[k] = '\0';

	strcpy(exp, myExp);
}

void fnEvaluatePostfix(char* exp) { 
	// Create a stack of capacity equal to expression size
	struct Stack* stack = createStack(strlen(exp));
	int i;

	// See if stack was created successfully
	if (!stack) {
		strcpy(exp, "ERROR");
		return;
	} 

	// Scan all characters one by one
	for (i = 0; exp[i]; ++i) {
		//if the character is blank space then continue
		if(exp[i]==' ')continue;

		// If the scanned character is an
		// operand (number here),extract the full number
		// Push it to the stack.
		else if (fnIsNum(exp[i])) {
			float num=0, decimalPart=0;

			//extract full number
			while(fnIsNum(exp[i])) {
				num=num*10 + (int)(exp[i]-'0');
				i++;
			}
			i--;


			//push the element in the stack
			
			push(stack,num);
		}

		// If the scanned character is an operator, pop two
		// elements from stack apply the operator
		else {
			int val1 = pop(stack);
			int val2 = pop(stack);

			switch(exp[i]) {
				case '+': push(stack, val2 + val1); break;
				case '-': push(stack, val2 - val1); break;
				case '*': push(stack, val2 * val1); break;
				case '/': push(stack, val2/val1); break;
			}
		}
	}
	sprintf(exp, "%d", pop(stack));
}

void fnEvaluateExpression() {
	fnInfixToPostfix(expression);
	fnEvaluatePostfix(expression);
} 

int fnCheckExpression() {
	int i, j, l, t = 0;

	l = strlen(expression);

	for(i=0;i<l;i++) {
		if(expression[i] == '(')
			t++;
		else if(expression[i] == ')')
			t--;
		else if(expression[i] == '*' || expression[i] == '/' || expression[i] == '+'|| expression[i] == '-' || expression[i] == '%') {
			if(((expression[i-1] >= '0' && expression[i-1] <= '9') && (expression[i+1] >= '0' && expression[i+1] <= '9'))
				|| (expression[i-1] == ')' && expression[i+1] == '(')
				|| ((expression[i-1] >= '0' && expression[i-1] <= '9') && expression[i+1] == '(')
				|| (expression[i-1] == ')' && (expression[i+1] >= '0' && expression[i+1] <= '9'))
				);
			else {
				break;
			}
		}
		if (t < 0) {
			return 0;
		}
	}
	if(i == l && t == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

void fnDelete() {
	expression[strlen(expression) - 1] = '\0';
}

void mouse(int btn, int state, int x, int y) {
	y = WINDOW_SIZE_Y - y;

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		int i, j;

		if(!strcmp(expression, "Length Exceeded") || !strcmp(expression, "ERROR")) {
			strcpy(expression,"");
		}

		if (insidebutton(btnArray[0][0], x, y)) {		//CLEAR BUTTON
			strcpy(expression, "");
			btnArray[0][0] = togglestate(btnArray[0][0]);
		}
		else if(insidebutton(btnArray[0][2], x, y)) {		//POWER OFF BUTTON
			exit(0);
		}
		else if(insidebutton(btnArray[0][3], x, y)) {		//EQUALS BUTTON
			if(fnCheckExpression()) {
				fnEvaluateExpression();
				if (strlen(expression) > BTN_STR_MAX_LEN) {
					strcpy(expression, "Length Exceeded");
				}
			}
			else {
				strcpy(expression, "ERROR");
			}
			btnArray[0][3] = togglestate(btnArray[0][3]);
		}
		else if(insidebutton(btnArray[4][2], x, y)) {		//DELETE BUTTON
			if(strlen(expression)) {
				fnDelete();
			}
			btnArray[2][4] = togglestate(btnArray[2][4]);
		}
		else {							//OTHER BUTTONS
			for(i = 0; i < 5; i++) {
				for(j = 0; j < 4; j++) {
					if (insidebutton(btnArray[i][j], x, y)) {
						strcat(expression, btnArray[i][j].str);
						btnArray[i][j] = togglestate(btnArray[i][j]);
					}
				}
			}
		}
		printf("%s\n", expression);	
	}

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
	}

	glutPostRedisplay();
				
}

void fnBtnNames() {
	strcpy(btnArray[0][0].str, "C");
	strcpy(btnArray[0][1].str, "0");
	strcpy(btnArray[0][2].str, "off");
	strcpy(btnArray[0][3].str, "=");

	strcpy(btnArray[1][0].str, "1");
	strcpy(btnArray[1][1].str, "2");
	strcpy(btnArray[1][2].str, "3");
	strcpy(btnArray[1][3].str, "+");

	strcpy(btnArray[2][0].str, "4");
	strcpy(btnArray[2][1].str, "5");
	strcpy(btnArray[2][2].str, "6");
	strcpy(btnArray[2][3].str, "-");

	strcpy(btnArray[3][0].str, "7");
	strcpy(btnArray[3][1].str, "8");
	strcpy(btnArray[3][2].str, "9");
	strcpy(btnArray[3][3].str, "*");

	strcpy(btnArray[4][0].str, "(");
	strcpy(btnArray[4][1].str, ")");
	strcpy(btnArray[4][2].str, "del");
	strcpy(btnArray[4][3].str, "/");
}

void fnInitButtons() {
	int i, j;

	for (i = 0; i < 5; i++) {
		for (j = 0; j < 4; j++) {
			btnArray[i][j] = initButton(CALC_X + (BUTTON_WIDTH + GAP) * j, CALC_Y + (BUTTON_WIDTH + GAP) * i, CALC_X + BUTTON_WIDTH + (BUTTON_WIDTH + GAP) * j, CALC_Y + BUTTON_WIDTH + (BUTTON_WIDTH + GAP) * i, "str");
		}
	}
	fnBtnNames();		
}

void fnDrawButtons() {
	int i, j;

	for (i = 0; i < 5; i++) {
		for (j = 0; j < 4; j++) {
			draw(btnArray[i][j]);
		}
	}
}

void fnCalcDisplay() {
	if(strlen(expression) >= BTN_STR_MAX_LEN) {
		int i = BTN_STR_MAX_LEN;
		while(i--) {
			disp[BTN_STR_MAX_LEN - i] = expression[strlen(expression) - i];
		}
		disp[BTN_STR_MAX_LEN] = '\0';
	}
	else {
		strcpy(disp, expression);
	}
		
	btnScreen = initButton(CALC_X + (BUTTON_WIDTH + GAP) * 0, CALC_Y + (BUTTON_WIDTH + GAP) * 5, CALC_X + BUTTON_WIDTH + (BUTTON_WIDTH + GAP) * 3, CALC_Y + BUTTON_WIDTH + (BUTTON_WIDTH + GAP) * 5, disp);
	draw(btnScreen);
}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fnCalcDisplay();
	fnDrawButtons();
	glFlush();
	glutSwapBuffers();
}


int main(int argc, char **argv) {
	fnInitButtons();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow("CALCULATOR");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutPostRedisplay();
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_SIZE_X, 0, WINDOW_SIZE_Y);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_COLOR_MATERIAL);
	glutMainLoop();

	return 0;
}
