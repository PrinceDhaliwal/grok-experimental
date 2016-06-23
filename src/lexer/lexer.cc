#include "lexer/lexer.h"

Lexer::Lexer(const char *file, bool f) {
    file_ = true;
    std::ifstream infile(file);
    file_name_ = file;
    int length = 0;
    if (infile.fail()) {
      ERROR(std::string, "cannot open file");
      f = false;
      err_msg_ = file + std::string(": file doesn't exist\n");
      return;
    }
    infile.seekg(0, infile.end);
    length = (size_t)infile.tellg();
    infile.seekg(0, infile.beg);
    code_.resize(length + 2, ' ');
    infile.read(&*(code_.begin()), length);
    infile.close();
    size_ = length;
    end_ = length;
    seek_ = 0;
    tok_ = nullptr;
    eos_ = false;
}

Token *Lexer::NextToken() {
    char ch, ch2, ch3;
    TokenType tok;
    Position pos;

    if (!buffer_.empty()) {
      Token *tok = buffer_.back();
      buffer_.pop_back();
      return tok;
    }

    ch = NextCharacter();
    // skip whitespaces
    while (ch != EOF && (ch == ' ' || ch == '\n' || ch == '\t'))
      ch = NextCharacter();

    // skip comments
    while (ch != EOF &&
           ((ch == '/' && LookAhead() == '/') || (ch == '/' && LookAhead() == '*')))
      StripComments(ch);

    // note the current position as we are about to parse
    // a valid(may be) token from this position
    pos = position_;

    if (ch != EOF) {

      // handle keywords, identifiers and numbers
      while (isalnum(ch) || ch == '_')
        return Characterize(ch, pos);

      if (LookAhead() == EOF || LookAhead() == '\0' || LookAhead() == ' ' ||
          LookAhead() == '\t' || LookAhead() == '\n')
        goto one;
      // else ch is a symbol
      ch2 = NextCharacter();

      if (LookAhead() == EOF || LookAhead() == '\0' || LookAhead() == ' ' ||
          LookAhead() == '\t' || LookAhead() == '\n')
        goto two;
      ch3 = NextCharacter();

      // check if there is a three character symbol
      tok = ThreeCharacterSymbol(ch, ch2, ch3);
      if (tok != INVALID) {
        return new Token(std::string({ch, ch2, ch3}), tok,
                         TOKENS[tok].precedance_, pos);
      }
      // as we have gone ahead one character so time to go back
      GoBack();
    two:
      // so now check whether we are at position of two character symbol
      tok = TwoCharacterSymbol(ch, ch2);
      if (tok != INVALID) {
        return new Token(std::string({ch, ch2}), tok, TOKENS[tok].precedance_,
                         pos);
      }

      // No! Let's go back one character
      GoBack();

    one:
      // Only possibility left
      tok = OneCharacterSymbol(ch);
      if (tok != INVALID) {
        return new Token(std::string({ch}), tok, TOKENS[tok].precedance_, pos);
      } else // oops! something went wrong, token is invalid!
        return new Token(std::string({ch}), INVALID, 0, pos);
    }

    // finally reached at the end of the given string
    return new Token(std::string({ch}), EOS, -1, pos);
}

void Lexer::StripComments(char &ch) { // skips all the comments from the string
  char last = '\0';
  // skip the comments - single line comment
  if (ch != EOF && (ch == '/' && LookAhead() == '/')) {
    ch = NextCharacter();
    while (ch != '\n' || ch == EOF)
      ch = NextCharacter();
    ch = NextCharacter();
  }

  // multiline comment, /* */ type
  if (ch != EOF && (ch == '/' && LookAhead() == '*')) {
    last = ch;
    ch = NextCharacter();
    while (ch != EOF && !(last == '*' && ch == '/')) {
      last = ch;
      ch = NextCharacter();
    }
    ch = NextCharacter();
  }

  while (ch != EOF && (ch == ' ' || ch == '\n' || ch == '\t'))
    ch = NextCharacter();
}

void Lexer::GoBack() { // go back one character back
  --seek_;
  if (position_.col_ - 1 < 0) { // we don't want our col_ to be negative
    position_.row_--;           // needed
    position_.col_ = lastColNumber_;
  } else { // okay, so simply decrement col_ member
    position_.col_--;
  }
}

char Lexer::NextCharacter() { // returns the next character from the string
  if (seek_ <= end_) { // if eos_ flag has not been set
    char ch = code_[seek_++];
    if (ch == '\0') { // if we are not processing EOF file character
      ch = EOF;
      eos_ = true;
    }
    if (ch == '\n') { // change the position
      position_.row_++;
      lastColNumber_ = position_.col_;
      position_.col_ = 0;
    } else {
      position_.col_++;
    }
    return ch;
  }
  return EOF;
}

Token *Lexer::Characterize(char ch,
             Position &pos)
{
  std::string buffer;
  if (isdigit(ch))
    return ParseNumber(ch, pos);

  if (isalpha(ch) || ch == '_' || ch == '$')
    return ParseIdentifierOrKeyWord(ch, pos);

  // if none of both then we have to return a invalid token
  return new Token(std::string(""), INVALID, -1, pos);
}

Token *
Lexer::ParseNumber(char ch,
            Position &pos) { // parses the number from the current position
  std::string buffer;        // buffer to store the parsed number
  while (isdigit(ch)) {
    buffer += ch;
    ch = NextCharacter();
  }
  // possible floating point number
  if (ch == '.' || ch == 'e' || ch == 'E') {
    buffer += ch;
    ch = NextCharacter();
    while (isdigit(ch)) {
      buffer += ch;
      ch = NextCharacter();
    }
  }
  // we've gone one character ahead, so go back
  GoBack();

  return new Token(buffer, DIGIT, -1, pos);
}

Token *Lexer::ParseIdentifierOrKeyWord(
    char ch,
    Position &
        pos) { // parses the identifier or a keyword from the current position
  std::string buffer;
  // possibly it is an identifier
  while (isalnum(ch) || ch == '_' || ch == '$') {
    buffer += ch;
    ch = NextCharacter();
  }
  // again went a character ahead
  GoBack();
  // check for KeyWord
  if (buffer[0] != '_' || buffer[0] != '$') {
    for (int i = LET; i <= RET; i++)
      if (buffer[0] == TOKENS[i].value_[0] && buffer == TOKENS[i].value_)
        return new Token(buffer, i, 0, pos);
  }
  return new Token(buffer, IDENT, -1, pos);
}

// although not required because of no use
Token *Lexer::ParseStringLiteral(
    char ch, Position &pos) { // parses the string literal from the string
  std::string str = "";
  if (ch == '"') { // string surrounded by "
    ch = NextCharacter();
    while (ch != '"') {
      str += ch;
      ch = NextCharacter();
    }
  } else if (ch == '\'') { // string surrounded by '
    ch = NextCharacter();
    while (ch != '\'') {
      str += ch;
      ch = NextCharacter();
    }
  }
  return new Token(str, STRING, -1, pos);
}

// this function should be used instead of above function
// when we found a token of " or ' that means we are about
// to parse the string from the current position
std::string Lexer::GetStringLiteral() { // returns all the string until it finds "

  // create a buffer
  std::string str = "";
  char ch = NextCharacter();
  while (ch != EOF && (ch != '"' && ch != '\'')) {
    str += ch; // fill the buffer
    ch = NextCharacter();
  }
  return str;
}

TokenType Lexer::OneCharacterSymbol(
    char ch) { // returns the type of token from the given symbol
  switch (ch) {
  case '(':
    return LPAR;
  case ')':
    return RPAR;
  case '{':
    return LBRACE;
  case '}':
    return RBRACE;
  case '[':
    return LSQB;
  case ']':
    return RSQB;
  case ':':
    return COLON;
  case ';':
    return SCOLON;
  case '"':
  case '\'':
    return STRING;
  case '.':
    return DOT;
  case ',':
    return COMMA;
  case '?':
    return CONDITION;
  case '+':
    return PLUS;
  case '-':
    return MINUS;
  case '/':
    return DIV;
  case '*':
    return MUL;
  case '%':
    return MOD;
  case '>':
    return GT;
  case '<':
    return LT;
  case '=':
    return ASSIGN;
  case '^':
    return XOR;
  case '|':
    return BOR;
  case '&':
    return BAND;
  case '!':
    return NOT;
  case '~':
    return BNOT;
  default:
    return INVALID;
  }
}

TokenType
Lexer::TwoCharacterSymbol(char ch1,
                   char ch2) { // returns the type of symbol of two characters
  switch (ch2) {
  case '=':
    switch (ch1) {
    case '+':
      return PLUSEQ;
    case '-':
      return MINUSEQ;
    case '*':
      return MULEQ;
    case '/':
      return DIVEQ;
    case '%':
      return MODEQ;
    case '^':
      return XOREQ;
    case '&':
      return BANDEQ;
    case '|':
      return BOREQ;
    case '>':
      return GTE;
    case '<':
      return LTE;
    case '=':
      return EQUAL;
    case '!':
      return NOTEQ;
    }
    return INVALID;
  case '|':
    if (ch1 == '|')
      return OR;
    return INVALID;
  case '&':
    if (ch1 == '&')
      return AND;
    return INVALID;
  case '+':
    if (ch1 == '+')
      return INC;
    return INVALID;
  case '-':
    if (ch1 == '-')
      return DEC;
    return INVALID;
  case '<':
    if (ch1 == '<')
      return SHL;
    return INVALID;
  case '>':
    if (ch1 == '>')
      return SHR;
  default:
    return INVALID;
  }
}

bool Lexer::IsFile() const { return file_; }
std::string Lexer::GetFileName() { return file_name_; }

TokenType Lexer::ThreeCharacterSymbol(
    char ch1, char ch2,
    char ch3) { // returns the type of symbol of three characters
  switch (ch3) {
  case '=':
    if (ch1 == '>' && ch2 == '>')
      return SHREQ;
    if (ch1 == '<' && ch2 == '<')
      return SHLEQ;
  default:
    return INVALID;
  }
}

void Lexer::PutBack(Token *tok) { // put in the buffer for one token lookahead
                           // required by parser
  buffer_.push_back(tok);
}

// move to next token
void Lexer::advance()
{
  tok_ = NextToken();
}

double Lexer::GetNumber()
{
  double result = 0.0;
  try {
    result = std::stod(tok_->GetValue());
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return result;
}
