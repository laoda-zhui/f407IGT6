#ifndef MYLIB_LIB_H_
#define MYLIB_LIB_H_



#include "main.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include <ctype.h>


/*字符串处理工具*/
uint8_t CharToHex(char c);

/*数学运算通用工具*/
void AssValue(float a, float b,float c);
float Solve_Math_String(char *raw_input);
#endif /* MYLIB_LIB_H_ */
