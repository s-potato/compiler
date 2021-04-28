/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
  {
    readChar();
  }
  
}

void skipComment() {
  // TODO
  int times = 0, commentClose = 0;
  while((!commentClose) && (currentChar != EOF)){
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_TIMES))
    {
      times = 1;
    } else if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)){
      if (times){
        commentClose = 1;
      } else
        times = 0;
    } else
      times = 0;
    readChar();
  }
  if (!commentClose) {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
}

Token* readIdentKeyword(void) {
  // TODO
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 0;
  token->string[count++] = currentChar;
  readChar();
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    if (count <= MAX_IDENT_LEN)
    {
      token->string[count++] = currentChar;
    }
    readChar();
  }
  if (count > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, lineNo, colNo);
    return token;
  }

  token->string[count] = '\0';
  token->tokenType = checkKeyword(token->string);

  if (token->tokenType == TK_NONE)
    token->tokenType = TK_IDENT;

  return token;
}

Token* readNumber(void) {
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;
  token->string[count++] = currentChar;
  readChar();
  int period_count = 0;
  /*edit*/
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD))
  {
    if (charCodes[currentChar] == CHAR_PERIOD){
      period_count++;
      if (period_count > 1)
      {
        error(ERR_INVALIDSYMBOL, lineNo, colNo);
        token->tokenType = TK_NONE;
        return token;
      }
    }
    token->string[count++] = currentChar;
    readChar();
  }
  if (charCodes[token->string[count-1]] == CHAR_PERIOD){
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      token->tokenType = TK_NONE;
      return token;
  }
  if (period_count) {
    token->tokenType = TK_FLOAT;
  }
  token->string[count] = '\0';
  token->value = atoi(token->string);

  return token;
}

Token* readConstChar(void) {
  // TODO
  int ln = lineNo, cn = colNo;
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar();
  if(currentChar == EOF){
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    token->tokenType = TK_NONE;
    return token;
  }
  token->string[0] = currentChar;
  token->string[1] = '\0';
  readChar();
  if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SINGLEQUOTE))
  {
    readChar();
    return token;
  } else {
    error(ERR_INVALIDCHARCONSTANT, ln, cn);
    token->tokenType = TK_NONE;
    return token;
  }
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS:/*edit*/
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_ASSIGN_PLUS, ln, cn);
    } else
      return makeToken(SB_PLUS, ln, cn);
    // ....
    // TODO
    // ....
  case CHAR_MINUS:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_ASSIGN_MIMUS, ln, cn);
    } else
      return makeToken(SB_MINUS, ln, cn);
  case CHAR_TIMES:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_ASSIGN_TIMES, ln, cn);
    } else
      return makeToken(SB_TIMES, ln, cn);
  case CHAR_SLASH:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_ASSIGN_SLASH, ln, cn);
    } else
      return makeToken(SB_SLASH, ln, cn);
  case CHAR_LT:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else
      return makeToken(SB_LT, ln, cn);
  case CHAR_GT:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else
      return makeToken(SB_GT, ln, cn);
  case CHAR_EXCLAIMATION:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else{
      error(ERR_INVALIDSYMBOL, ln, cn);
      return makeToken(TK_NONE, ln, cn);
    }
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COLON:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)){
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else
      return makeToken(SB_COLON, ln, cn);
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  case CHAR_LPAR:
    ln = lineNo; cn = colNo;
    readChar();
    if((currentChar != EOF) && (charCodes[currentChar] == CHAR_TIMES)){
      readChar();
      skipComment();
      return getToken();
    } else if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_PERIOD))
    {
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    } else
      return makeToken(SB_LPAR, ln, cn);
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token; 
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;
  /*add*/
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  /*add*/
  case KW_FLOAT: printf("KW_FLOAT\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  /*add*/
  case SB_ASSIGN_MIMUS: printf("SB_ASSIGN_MIMUS\n"); break;
  case SB_ASSIGN_PLUS: printf("SB_ASSIGN_PLUS\n"); break;
  case SB_ASSIGN_TIMES: printf("SB_ASSIGN_TIMES\n"); break;
  case SB_ASSIGN_SLASH: printf("SB_ASSIGN_SLASH\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



