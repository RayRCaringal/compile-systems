#include <stdlib.h>
#include <stdio.h>
/*
Logical Operators {AND,OR,NOT} {true,false} (Case Sensitive)
Arithmethic Operators {0,1...9} {-, +, *, /}
/0 = Terminator Null 
*/

typedef int bool;
#define true 1
#define false 0


bool expressionNotEnded; //True if expression has not ended, false otherwise 
bool firstOperator; //True if first operator has been found, false otherwise
int arith = 0, logical = 0; //Global Counters for Expression types

//Forward Declarations for expectations 
void expectArithOperator(char ** arrTok, int sizeTok, int expression, int pos);
void expectArithOperand(char ** arrTok, int sizeTok, int expression, int pos);
void expectLogOperator(char ** arrTok, int sizeTok, int expression, int pos);
void expectLogOperand(char ** arrTok, int sizeTok, int expression, int pos);
  

typedef enum types{
  logOperator,logOperand,arithOperator,arithOperand, unknown, missing
}type;

int strLen(char * string){
  int count = 0;
  while(string[count] != '\0'){
    count++;
  }
  return count;
}

int strComp(char * string, char * cmp){
  if(strLen(string) != strLen(cmp)){
    return 1;
  }
  int i = 0; 
  while(string[i] != '\0' && cmp[i] != '\0'){
    if(string[i] != cmp[i]){
      return 1;
    }
    i++;
  }
  return 0;
}

type tokenClassifier(char * token, int expression){
  type t;
  if(strLen(token) == 0){ // for ""
    printf("Error: Scan error in expression %d: Missing Expression\n", expression);
    t = missing;
  }else if(strLen(token) == 1){
    char test = token[0];
    switch(test){
        case '-':
        case '+':
        case '*':
        case '/':
      t = arithOperator;
      break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
      t = arithOperand;
      break;
      case ' ':
        if(expression > 0){
          t = missing;
          break;
        }else{
          t = unknown;
          break;
        }
      default :
        t = unknown;
        break;
    };
  }else{
    if(strComp(token, "true") == 0|| strComp(token, "false") == 0){
      t = logOperand;
    }else if (strComp(token, "AND") == 0|| strComp(token, "OR") == 0 || strComp(token, "NOT") == 0){
      t = logOperator;
    }else{
      t = unknown;
    }
  }
    return t;
} 


//Count how many expressions there are. We count "1+2;" as 2 expressions. 
int countExpress(char* line){
int count = 1, i = 0; 
while(line[i] != '\0'){
  if(line[i] == ';'){
    count++;
  }
  i++;
}
return count;
}

//Won't count multiple spaces so "1  2" is "1" delim "" "2"
int countTokens(char * expr){
  int count = 0, i = 0;
  bool prev = false; //If false dont count 
  while(expr[i] != '\0'){
    if(prev == true && expr[i] == ' '){
      count++;
      prev = false;
    }
    if(expr[i] != ' '){
      prev = true;
    }else{
      if(prev == false){
        prev = true;
      }else{
        prev = false;
      }
    }
    i++;
  }
  if(expr[0] == ' '){ //For expressions > 0
    count++;
  }
  
  if(expr[i] != ' '){ 
    count++;
  }
  
  return count;
}

//Takes argv[1] and returns it as an array of Expressions
char ** intoExpress(char * line, int sizeExpr){
 int i, x = 0, y = 0; ;
 char ** ret =  malloc(sizeExpr * sizeof(char *));
  for(i = 0; i < sizeExpr; i++){
   ret[i] = malloc(256 * sizeof(char));
  }
  i = 0;
  while(line[i] != '\0'){
    if(line[i] != ';'){
      ret[x][y] = line[i];
      y++;
    }else{
      y = 0;
      x++;
    }
    i++;
  }
  return ret;
}

//Takes an Expression and returns an array of Tokens 
char ** intoTokens(char * expr, int sizeTok, int expression){
  int i, check = false, x = 0, y = 0;
  char ** ret =  malloc(sizeTok * sizeof(char *));
  for(i = 0; i < sizeTok; i++){
   ret[i] = malloc(26 * sizeof(char));
  }
  if(expression > 0 && expr[0] != ' ' && strLen(expr) != 0){
       printf("Error: Scan error in expression %d: Incomplete Expression expected space.\n \"%s\"\n", expression, expr);
  }
  
  i = 0;
  while(expr[i] != '\0'){
    if(expr[i] != ' '){
      ret[x][y] = expr[i];
      y++;
      check = false;
    }else if(check == true){// previous was space 
     ret[x][y]  = ' ';
     x++;
     check = false;
    }else if(expression > 0 && i == 0){
     ret[x][y]  = ' ';
     x++;
    }else{// previous was not space 
      y = 0;
      x++;
     check = true;
    }
    i++;
  }
  return ret;
}

//Checks if an operator has been already found, increments global counter for the corresponding type
void expressionType(type t){
  if(firstOperator == false){
    if(t == arithOperator){
      arith++; 
    }else{
      logical++;
    }
    firstOperator = true;
  }
}

//Feeds the array of Tokens into functions that decide what the expression expects next.
void feedTokens(char ** arrTok, int sizeTok, int expression, int pos ){
  type t;
  while(pos < sizeTok){ //Loops until t is a valid type or we run out of tokens
    t = tokenClassifier(arrTok[pos], expression);
    if(t == missing){
      pos++;
    }else if(t != unknown){
      break;
    }else{
      printf("Error: Parse Error in expression %d: Unknown Identifier\n\"%s\".\n", expression, arrTok[pos]);
      pos++;
    }
  }

   if(t == arithOperand){
    if(expressionNotEnded == true){
      printf("Error: Parse Error in expression %d: Unexpected Operand \n\"%s\".\n",expression, arrTok[pos]);
    }
    expectArithOperator(arrTok,sizeTok, expression,pos);
  }else if(t == arithOperator){
    printf("Error: Parse Error in expression %d: Unexpected Operator \n\"%s\".\n",expression, arrTok[pos]);
    expressionType(t);
    expectArithOperand(arrTok,sizeTok, expression,pos);
  }else if(t == logOperator){
    expressionType(t);
    if(strComp(arrTok[pos], "NOT") == 1){ //If not NOT
      printf("Error: Parse Error in expression %d: Unexpected Operator \n\"%s\".\n",expression, arrTok[pos]);
    }else if(expressionNotEnded == true){
      printf("Error: Parse Error in expression %d: Unexpected Operator \n\"%s\".\n",expression, arrTok[pos]);
    }
     expectLogOperand(arrTok,sizeTok, expression,pos);
    }else if(t == logOperand){
      if(expressionNotEnded == true){
        printf("Error: Parse Error in expression %d: Unexpected Operand \n\"%s\".\n",expression, arrTok[pos]);
      }
      expectLogOperator(arrTok,sizeTok, expression,pos);
  }
  }

//Feeds the array of expressions into functions to create an array of tokens and then a function to feed those tokens
void feedExpr(char ** arrExpr, int sizeExpr){
  int i, sizeTok;
  for(i = 0; i < sizeExpr; i++){
    sizeTok = countTokens(arrExpr[i]);
    char ** arrTok = intoTokens(arrExpr[i],sizeTok, i);
    feedTokens(arrTok,sizeTok, i, 0);
    expressionNotEnded = false;
    firstOperator = false;
    free(arrTok);
  }
}

//For Expression not ended error, takes up less space 
void endofExpression(char ** arrTok, int expression, int pos){
  if(expressionNotEnded == false) {
        printf("Error: Parse error in expression %d: Expression not ended\n\"%s\"\n",expression, arrTok[pos]);
        expressionNotEnded = true;
      }
}

// Expecting Functions, look for the current expectation and decide the next following type.

//Expression currently expects an Arithmethic Operator next
void expectArithOperator(char ** arrTok, int sizeTok, int expression, int pos){
  if(pos+1 >= sizeTok){
    printf("Error: Scan error in expression %d: Missing Operator.\n",expression);
    return;
  }
  type t = tokenClassifier(arrTok[pos+1], expression);
  if( t == arithOperand){
    printf("Error: Parse error in expression %d: Missing Operator. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
    expectArithOperator(arrTok, sizeTok, expression, pos+1);  
    return;
  }else if(t == logOperand){
    printf("Error: Parse error in expression %d: Missing Operator. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
    expectLogOperator(arrTok, sizeTok, expression, pos+1);
    return;
  }else if(t == logOperator){
    expressionType(t);
    printf("Error: Parse error in expression %d: Type mismatch. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
    expectLogOperand(arrTok, sizeTok, expression, pos+1);
    return;
  }else if(t == unknown){
    printf("Error: Parse error in expression %d: Unknown Operator\n\"%s\"\n",expression, arrTok[pos+1]);
    expectArithOperand(arrTok, sizeTok, expression, pos+1);
    return;
  }
  
  if(pos+1 >sizeTok){
    printf("Error: Scan error in expression %d: Missing Operand.\n",expression);
  }else{
    expressionType(t);
    expectArithOperand(arrTok, sizeTok, expression, pos+1);
  }

}

//Expression currently expects an Arithmethic Operand next
void expectArithOperand(char ** arrTok, int sizeTok, int expression, int pos){
    if(pos+1 >= sizeTok){
    printf("Error: Scan error in expression %d: Missing Operand.\n",expression);
    return;
   }  
    type t = tokenClassifier(arrTok[pos+1], expression);
    if(t == arithOperator){
      expressionType(t);
      printf("Error: Parse error in expression %d: Missing Operand. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
      expectArithOperand(arrTok, sizeTok, expression, pos+1);
      return;
    }else if(t == logOperand){
      printf("Error: Parse error in expression %d: Type mismatch. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
      if(pos+2 < sizeTok){
       endofExpression(arrTok,expression,pos+1);
       expectLogOperator(arrTok, sizeTok, expression, pos+1);
       return;  
      }
    }else if(t == logOperator){
      expressionType(t);
      printf("Error: Parse error in expression %d: Missing operand. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
      expectLogOperand(arrTok, sizeTok, expression, pos+1);
      return;
    }else if(t == unknown){
      printf("Error: Parse error in expression %d: Unknown Operand\n\"%s\"\n",expression, arrTok[pos+1]);
    }
  
    if(pos+2 < sizeTok){
       endofExpression(arrTok,expression,pos+1);
       feedTokens(arrTok, sizeTok, expression, pos+2);
      }
}

//Expression currently expects a Logical Operator next
void expectLogOperator(char ** arrTok, int sizeTok, int expression, int pos){
   if(pos+1 >= sizeTok){
    printf("Error: Scan error in expression %d: Missing Operator.\n",expression);
    return;
  }
   type t = tokenClassifier(arrTok[pos+1], expression);
   if(t == arithOperator){
     expressionType(t);
     printf("Error: Parse error in expression %d: Type mismatch. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
     expectArithOperand(arrTok, sizeTok, expression, pos+1);
     return;
  }else if( t == arithOperand){
    printf("Error: Parse error in expression %d: Missing Operator. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
    expectArithOperator(arrTok, sizeTok, expression, pos+1);  
    return;
  }else if(t == logOperand){
     printf("Error: Parse error in expression %d: Missing Operator. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
     expectLogOperator(arrTok, sizeTok, expression, pos+1);
     return;
  }else if(t == unknown){
     printf("Error: Parse error in expression %d: Unknown Operator\n\"%s\"\n",expression, arrTok[pos+1]);
  }
  
  if(pos+1 >sizeTok){
    printf("Error: Scan error in expression %d: Missing Operand.\n",expression);
  }else{
    expressionType(t);
    expectLogOperand(arrTok, sizeTok, expression, pos+1);
  }
}

//Expression currently expects a Logical Operand next
void expectLogOperand(char ** arrTok, int sizeTok, int expression, int pos){
    if(pos+1 >= sizeTok){
    printf("Error: Scan error in expression %d: Missing Operand.\n",expression);
    return;
   }  
  type t = tokenClassifier(arrTok[pos+1], expression);
  if(t == arithOperator){
    expressionType(t);
    printf("Error: Parse error in expression %d: Missing Operand. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
    expectArithOperand(arrTok, sizeTok, expression, pos+1);
    return;
  }else if(t == arithOperand){
     printf("Error: Parse error in expression %d: Type mismatch. Unexpected Operand\n\"%s\"\n",expression, arrTok[pos+1]);
     if(pos+2 < sizeTok){
       endofExpression(arrTok,expression,pos+1);
       expectArithOperator(arrTok, sizeTok, expression, pos+1);
       return;  
      }
  }else if(t == logOperator){
    expressionType(t);
    printf("Error: Parse error in expression %d: Missing operand. Unexpected Operator\n\"%s\"\n",expression, arrTok[pos+1]);
    expectLogOperand(arrTok, sizeTok, expression, pos+1);
    return;
  }else if(t == unknown){
    printf("Error: Parse error in expression %d: Unknown Operand\n\"%s\"\n",expression, arrTok[pos+1]);
  }
    if(pos+2 < sizeTok){
     endofExpression(arrTok,expression,pos+1);
     feedTokens(arrTok, sizeTok, expression, pos+2);
    }
}


int main(int argc, char** argv) {
 
  if(argc < 2){
    printf("Not enough arguments\n");
    return 0;
  }else if(argc > 2){
    printf("Too many arguments\n");
    return 0;
  }
  int sizeExpr = countExpress(argv[1]);
  char** arrExpress = intoExpress(argv[1], sizeExpr);
  expressionNotEnded = false;
  firstOperator = false;
  feedExpr(arrExpress, sizeExpr);
  printf("Found %d expressions: %d Logical and %d Arithmethic\n", sizeExpr, logical, arith);
  free (arrExpress);

  return 0;
  
  
}