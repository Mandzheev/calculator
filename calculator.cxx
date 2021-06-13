#include <cmath>
#include <iostream>
using namespace std;

/******************************************************/

void error(string s) { throw runtime_error(s); }

/******************************************************/
const char number = '8';
const char quit = 'q';
const char print = '=';

class Token {
public:
  char kind;
  double value;
  Token(char ch) // make a Token from a char
      : kind(ch), value(0) {}
  Token(char ch, double val) // make a Token from a char and a double
      : kind(ch), value(val) {}
};

/******************************************************/

class Token_stream {
public:
  Token_stream();
  Token get();
  void putback(Token t);

private:
  bool full;
  Token buffer; // keep a Token put back using putback()
};

/******************************************************/

// Indicate that the buffer is empty
Token_stream::Token_stream()
    : full(false), buffer(0) // no Token in buffer
{}

/******************************************************/

// Puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t) {
  if (full)
    error("putback() into a full buffer");
  buffer = t;  // copy t to buffer
  full = true; // buffer is full
}

/******************************************************/

Token Token_stream::get() {
  if (full) {
    full = false; // remove token from buffer
    return buffer;
  }

  char ch;
  cin >> ch;

  switch (ch) {
  case print:
  case quit:
  case '(':
  case ')': // 'q' for quit
  case '+':
  case '-':
  case '*':
  case '/':
    return Token{ch};
  case '.': // a floating-point-literal can start with a dot
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    cin.putback(ch); // put digit into the input stream
    double val;
    cin >> val; // read a floating-point number
    return Token{number, val};
  }
  default:
    error("Bad token");
  }
}

/******************************************************/

Token_stream ts; // provides get() and putback()

/******************************************************/

double term();

// + and -
double expression() {

  double left = term(); // read and evaluate term()

  Token t = ts.get(); 

  while (true) {
    switch (t.kind) {
    case '+':
      left += term();
      t = ts.get();
      break;

    case '-':
      left -= term();
      t = ts.get();
      break;
    default:
      ts.putback(t); // put t back into the token stream
      return left;   // finally: return the answer
    }
  }
}

/******************************************************/

// numbers and parentheses
double primary() {

  Token t = ts.get();

  switch (t.kind) {
  case '(': // handle '('expression')'
  {
    double d = expression();
    t = ts.get();
    if (t.kind != ')')
      error("')'expected");
    return d;
  }
  case number:
    return t.value;
  case '-':
    return -primary();
  case '+':
    return primary();
  default:
    error("primary expected");
  }
}

/******************************************************/

// * and /
double term() {
  double left = primary();

  Token t = ts.get(); // get the next token from token stream

  while (true) {
    switch (t.kind) {
    case '*':
      left *= primary();
      t = ts.get();
      break;
    case '/': {
      double d = primary();
      if (d == 0)
        error("divide by zero");
      left /= d;
      t = ts.get();
      break;
    }
    case '%':
    default:
      ts.putback(t);
      return left;
    }
  }
}

/******************************************************/
void calculate() {
  double val = 0;

  while (cin) {
    Token t = ts.get();

    if (t.kind == quit)
      break;
    if (t.kind == print)
      cout << "=" << val << '\n';
    else
      ts.putback(t);
    val = expression();
  }
}
/******************************************************/

int main() try {
  cout << "Simple calculator.\n";
  cout << "[q]quit, [=]result\n";
  cout << "Enter expressions:\n";

  calculate();
  return 0;
} catch (runtime_error &e) {
  cerr << e.what() << '\n';
  return 1;
} catch (...) {
  cerr << "exception \n";
  return 2;
}
