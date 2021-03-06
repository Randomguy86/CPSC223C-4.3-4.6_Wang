#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100
#define FUNC 1

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
  i = 0;
  if (islower(c)) {
    while (islower(s[++i] = c = getch_())) { }
    s[i] = '\0';
    if (c != EOF) { ungetch_(c); }
    if (strlen(s) > 0) { return FUNC; }
  }

  if (!isdigit(c) && c != '.') { return c; }  // if not a digit, return

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

void mathfunc(char * s) {
  double op2;
  if (strcmp(s, "sin") == 0)
      push(sin(pop()));
  else if (strcmp(s, "exp") == 0)
      push(exp(pop()));
  else if (strcmp(s, "pow") == 0) {
      op2 = pop();
      push(pow(pop(), op2));
    }
  else {
     fprintf(stderr, "The command inputted is not available\n");
    }
}

void rpn(void) {
  int type, i, v = 0;
  double op2, temp, solution;
  char s[BUFSIZ];
  double var[26];

  for (i = 0; i < 26; i++) {
    var[i] = 0.0;
  }

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case NUMBER:  push(atof(s));              break;
      case FUNC:    mathfunc(s);                break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;
      case '%':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "trying to mod by zero\n");  break;  }
        push(fmod(pop(), op2));
        break;
      case '!': // print
        op2 = pop();
        printf("\t%.8g\n", op2);
        push(op2);
        break;
      case '@': // duplicate
        op2 = pop();
        push(op2);
        push(op2);
        break;
      case '#': // swap
        temp = pop();
        op2 = pop();
        push(temp);
        push(op2);
        break;
      case '=':
        pop();
        if ( v >= 'A' && v <= 'Z' )
            var[v - 'A'] = pop();
        else
          fprintf(stderr, "Error: no variable\n");
        break;
      case '\n':
        solution = pop();
        printf("\t%.8g\n", solution);
        break;
      case '$': return;
      default:
        if ( type >= 'A' && type <= 'Z' )
          push(var[type - 'A']);
        else if (type == 's')
          push(solution);
        else
          fprintf(stderr, "unknown command %s\n", s);
        break;
    }
    v = type;
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
