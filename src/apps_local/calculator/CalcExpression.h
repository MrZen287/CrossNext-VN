#pragma once

#include "CalcEngine.h"

namespace calc {

class ExpressionEngine {
 public:
  const char* display() const { return display_; }
  const char* pending() const { return expressionLabel_; }
  bool hasError() const { return error_; }

  void press(Key key) {
    if (key == Key::None) return;
    if (key == Key::ClearAll || (error_ && key == Key::ClearEntry)) {
      *this = {};
      return;
    }
    if (key == Key::Equals) {
      evaluate();
      return;
    }

    char input = 0;
    if (isDigit(key)) input = static_cast<char>('0' + digitValue(key));
    if (key >= Key::Add && key <= Key::Div) input = "+-*/"[static_cast<int>(key) - static_cast<int>(Key::Add)];
    if (key == Key::Dot) input = '.';
    if (key == Key::OpenGroup) input = '(';
    if (key == Key::CloseGroup) input = ')';
    if (key == Key::Percent) input = '%';

    if (solved_ && (input || key == Key::Negate) && input != ')') {
      solved_ = false;
      length_ = 0;
      if (binary(input) || input == '%' || key == Key::Negate) {
        seed_ = answer_;
        append('@');
      }
      expression_[length_] = '\0';
    }
    error_ = solved_ = false;
    if (key == Key::Backspace && length_) expression_[--length_] = '\0';
    if (key == Key::ClearEntry) {
      length_ = entryStart();
      expression_[length_] = '\0';
    }
    if (key == Key::Negate) toggleSign();
    if (input) insert(input);
    refresh();
  }

 private:
  static constexpr size_t kMaxExpression = 40;
  static constexpr int kMaxGroups = 8;

  static bool digit(char c) { return c >= '0' && c <= '9'; }
  static bool binary(char c) { return c && std::strchr("+-*/", c); }
  static bool operandEnd(char c) { return digit(c) || c == '.' || c == ')' || c == '%' || c == '@'; }

  struct Operation {
    char symbol = 0;
    decNumber right{};
  };

  struct Value {
    decNumber number{};
    bool percentage = false;
  };

  struct Parser {
    const char* next;
    const decNumber& seed;
    decContext context{};
    Operation last{};
    bool allowUnclosed = false;
    bool valid = true;

    Parser(const char* text, const decNumber& seed, bool allowUnclosed) : next(text), seed(seed), allowUnclosed(allowUnclosed) {
      decContextDefault(&context, DEC_INIT_BASE);
      context.digits = kWorkingDigits;
      context.emax = kMaxExponent;
      context.emin = -kMaxExponent;
      context.round = DEC_ROUND_HALF_UP;
      context.traps = 0;
    }

    decNumber apply(char symbol, const decNumber& left, const decNumber& right) {
      decNumber out;
      switch (symbol) {
        case '+': decNumberAdd(&out, &left, &right, &context); break;
        case '-': decNumberSubtract(&out, &left, &right, &context); break;
        case '*': decNumberMultiply(&out, &left, &right, &context); break;
        default: decNumberDivide(&out, &left, &right, &context); break;
      }
      return out;
    }

    Value sum(int depth) {
      Value left = term(depth);
      while (valid && (*next == '+' || *next == '-')) {
        const char symbol = *next++;
        Value right = term(depth);
        if (!valid) break;
        if (right.percentage) right.number = apply('*', left.number, right.number);
        last = {symbol, right.number};
        left = {apply(symbol, left.number, right.number), false};
      }
      return left;
    }

    Value term(int depth) {
      Value left = primary(depth);
      while (valid && (*next == '*' || *next == '/')) {
        const char symbol = *next++;
        const Value right = primary(depth);
        if (!valid) break;
        last = {symbol, right.number};
        left = {apply(symbol, left.number, right.number), false};
      }
      return left;
    }

    Value primary(int depth) {
      const bool negative = *next == '-';
      if (negative || *next == '+') ++next;
      Value value;
      if (*next == '(' && depth < kMaxGroups) {
        ++next;
        value = sum(depth + 1);
        if (*next == ')')
          ++next;
        else if (*next || !allowUnclosed)
          valid = false;
      } else if (*next == '@') {
        ++next;
        value.number = seed;
      } else {
        const char* start = next;
        while (digit(*next)) ++next;
        if (*next == '.') {
          ++next;
          while (digit(*next)) ++next;
        }
        const size_t length = next - start;
        if (!length || (length == 1 && *start == '.')) {
          valid = false;
          return value;
        }
        char number[kMaxEntryDigits + 2]{};
        if (length >= sizeof(number)) {
          valid = false;
          return value;
        }
        std::memcpy(number, start, length);
        decNumberFromString(&value.number, number, &context);
      }
      if (negative) decNumberMinus(&value.number, &value.number, &context);
      if (*next == '%') {
        ++next;
        decNumber hundred;
        decNumberFromInt32(&hundred, 100);
        value = {apply('/', value.number, hundred), true};
      }
      return value;
    }
  };

  void append(char c) {
    if (length_ == kMaxExpression) return;
    expression_[length_++] = c;
    expression_[length_] = '\0';
  }

  int unmatchedGroups() const {
    int depth = 0;
    for (size_t i = 0; i < length_; ++i) {
      if (expression_[i] == '(') ++depth;
      if (expression_[i] == ')') --depth;
    }
    return depth;
  }

  size_t entryStart() const {
    size_t p = length_;
    if (p && expression_[p - 1] == '%') --p;
    if (p && expression_[p - 1] == ')') {
      int depth = 1;
      --p;
      while (p && depth) {
        const char c = expression_[--p];
        if (c == ')') ++depth;
        if (c == '(') --depth;
      }
    } else {
      while (p && (digit(expression_[p - 1]) || expression_[p - 1] == '.' || expression_[p - 1] == '@')) --p;
    }
    if (p && expression_[p - 1] == '-' && (p == 1 || !operandEnd(expression_[p - 2]))) --p;
    return p;
  }

  void toggleSign() {
    const size_t p = entryStart();
    if (p < length_ && expression_[p] == '-' && (p == 0 || !operandEnd(expression_[p - 1]))) {
      std::memmove(expression_ + p, expression_ + p + 1, length_-- - p);
    } else if (length_ < kMaxExpression) {
      std::memmove(expression_ + p + 1, expression_ + p, length_ - p + 1);
      expression_[p] = '-';
      ++length_;
    }
  }

  void insert(char input) {
    const char last = length_ ? expression_[length_ - 1] : '\0';
    if (binary(input)) {
      if (operandEnd(last)) append(input);
      else if (input == '-' && (!last || last == '(' || last == '*' || last == '/')) append(input);
      else if (length_ >= 2 && binary(last) && operandEnd(expression_[length_ - 2])) expression_[length_ - 1] = input;
      return;
    }
    if (input == '%') {
      if (operandEnd(last) && last != '%') append(input);
      return;
    }
    if (input == ')') {
      if (unmatchedGroups() > 0 && operandEnd(last)) append(input);
      return;
    }
    if (input == '(' || last == ')' || last == '%' || last == '@') {
      if (length_ + 1 >= kMaxExpression || (input == '(' && unmatchedGroups() >= kMaxGroups)) return;
      if (operandEnd(last)) append('*');
    }
    if (digit(input) || input == '.') {
      int digits = 0;
      for (size_t i = length_; i && (digit(expression_[i - 1]) || expression_[i - 1] == '.'); --i) {
        if (input == '.' && expression_[i - 1] == '.') return;
        if (digit(expression_[i - 1])) ++digits;
      }
      if (digit(input) && digits >= kMaxEntryDigits) return;
    }
    append(input);
  }

  void updateLabel() {
    char* out = expressionLabel_;
    for (const char* p = expression_; *p; ++p) {
      const char* glyph = nullptr;
      if (*p == '*') glyph = "\xc3\x97";
      if (*p == '/') glyph = "\xc3\xb7";
      if (*p == '@') {
        Parser parser("", seed_, false);
        char number[kTextMax];
        formatResult(seed_, parser.context, number, sizeof(number));
        glyph = number;
        std::strcpy(out, glyph);
        out += std::strlen(glyph);
      } else if (glyph) {
        std::strcpy(out, glyph);
        out += 2;
      } else {
        *out++ = *p;
      }
    }
    *out = '\0';
  }

  void refresh() {
    updateLabel();
    if (!length_) {
      std::strcpy(display_, "0");
      return;
    }
    if (std::strspn(expression_ + (expression_[0] == '-' ? 1 : 0), "0123456789.") ==
        length_ - (expression_[0] == '-' ? 1 : 0)) {
      std::snprintf(display_, sizeof(display_), "%s", expression_);
      return;
    }
    Parser parser(expression_, seed_, true);
    const Value value = parser.sum(0);
    if (parser.valid && !*parser.next && !(parser.context.status & DEC_Errors))
      formatResult(value.number, parser.context, display_, sizeof(display_));
    else
      display_[0] = '\0';
  }

  void evaluate() {
    if (!length_ || error_ || (solved_ && !repeat_.symbol)) return;
    Parser parser(expression_, seed_, true);
    const decNumber value = solved_ ? parser.apply(repeat_.symbol, answer_, repeat_.right) : parser.sum(0).number;
    const char* error = nullptr;
    if (parser.context.status & (DEC_Division_by_zero | DEC_Division_undefined))
      error = "DIVIDE BY 0";
    else if (parser.context.status & (DEC_Overflow | DEC_Underflow))
      error = "OVERFLOW";
    else if (!solved_ && (!parser.valid || *parser.next || (parser.context.status & DEC_Errors)))
      error = "BAD INPUT";
    if (error) {
      std::strcpy(display_, error);
      error_ = true;
      return;
    }
    if (!solved_) repeat_ = parser.last;
    answer_ = value;
    formatResult(answer_, parser.context, display_, sizeof(display_));
    solved_ = true;
  }

  char expression_[kMaxExpression + 1]{};
  char expressionLabel_[2 * kMaxExpression + kTextMax]{};
  char display_[kTextMax] = "0";
  size_t length_ = 0;
  decNumber seed_{}, answer_{};
  Operation repeat_{};
  bool solved_ = false, error_ = false;
};

}  // namespace calc
