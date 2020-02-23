#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }
void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

int getop(char* s) {
  int i, c;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';

  if (!isdigit(c) && c != '.') { return c; }  // if not a digit, return

  i = 0;
  if (isdigit(c)) {  // get digits before '.'
    while (isdigit(s[++i] = c = getch_())) { }
  }
  if (c == '.') {    // get digits after decimal (if any)
    while (isdigit(s[++i] = c = getch_())) { }
  }
  s[i] = '\0';
  if (c != EOF) { ungetch_(c); }
  return NUMBER;      // return type is NUMBER, number stored in s
}

double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

void rpn(void) {
  int type;
  double op2, temp;
  char s[BUFSIZ];

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case '\n':    printf("\t%.8g\n", pop());  break;
      case NUMBER:  push(atof(s));              break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case 'c':     sp = 0;                     break; // clear
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;
      case '%':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "trying to mod by zero\n");  break;  }
        push(fmod(pop(), op2));
        break;
      case 'p': // print
        op2 = pop();
        printf("\t%.8g\n", op2);
        push(op2);
        break;
      case 'd': // duplicate
        op2 = pop();
        push(op2);
        push(op2);
        break;
      case 's': // swap
        temp = pop();
        op2 = pop();
        push(temp);
        push(op2);
        break;
      case 'e': return;
      default:      fprintf(stderr, "unknown command %s\n", s);  break;
    }
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
