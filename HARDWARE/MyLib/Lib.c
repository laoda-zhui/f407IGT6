#include "Lib.h"



/**************************************************************************
函数功能：将单个字符变量用16进制转换成数字
入口参数：无
返回  值：数字
**************************************************************************/
uint8_t CharToHex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';      // '0'-'9' 转为 0-9
    if (c >= 'A' && c <= 'F') return c - 'A' + 10; // 'A'-'F' 转为 10-15
    if (c >= 'a' && c <= 'f') return c - 'a' + 10; // 'a'-'f' 转为 10-15
    return 0; // 如果不是十六进制字符，返回0
}




/**************************************************************************
函数功能：以下为数学运算解析器
入口参数：无
返回  值：无
**************************************************************************/

// 全局变量 a, b, c (卡1解出来的)
float var_a = 0;
float var_b = 0;
float var_c = 0;

void AssValue(float a, float b,float c)//给变量赋值
{
	var_a = a;
	var_b = b;
	var_c = c;
}

// 获取运算符优先级
int get_prio(char op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    if (op == '(') return 0;
    return -1;
}


// 执行一步运算
// 2. 基础运算单元
float Calc(float v1, float v2, char op)
{
    switch(op) {
        case '+': return v1 + v2;
        case '-': return v1 - v2;
        case '*': return v1 * v2;
        case '/': return (v2 != 0) ? (v1 / v2) : 0;
        case '^': return pow(v1, v2);
        default:  return 0;
    }
}

// 3. 核心计算函数 (纯字符串数字 可计算未知变量 支持两位数)
float Solve_Math_String(char *raw_input)
{
    float nums[20]; // 数字栈
    char  ops[20];  // 符号栈
    int   n_top = -1;
    int   o_top = -1;

    for (int i = 0; raw_input[i] != '\0'; i++)
    {
        char c = raw_input[i];

        // --- A. 如果是数字 (0-9) ---
        // 手动判断，不用 isdigit，更安全
        if (c >= '0' && c <= '9')
        {
            float val = 0;
            // 只要当前字符还是数字，就一直拼
            // 注意：这里我们用了一个小循环来处理多位数
            while (raw_input[i] >= '0' && raw_input[i] <= '9')
            {
                val = val * 10 + (raw_input[i] - '0');
                i++;
            }
            i--; // 关键：因为 for 循环还会 i++，所以这里要退一步，指向数字的最后一位
            nums[++n_top] = val;
        }

        // --- (可选) B. 如果是变量 a, b, c ---
        // 如果题目只有数字运算，这部分可以删掉

//        else if (c == 'a') nums[++n_top] = var_a;
//        else if (c == 'b') nums[++n_top] = var_b;
//        else if (c == 'c') nums[++n_top] = var_c;


        // --- C. 如果是左括号 ---
        else if (c == '(') {
            ops[++o_top] = c;
        }

        // --- D. 如果是右括号 ---
        else if (c == ')') {
            while (o_top >= 0 && ops[o_top] != '(') {
                float v2 = nums[n_top--];
                float v1 = nums[n_top--];
                char op = ops[o_top--];
                nums[++n_top] = Calc(v1, v2, op);
            }
            if (o_top >= 0) o_top--;
        }

        // --- E. 如果是运算符 ---
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
            while (o_top >= 0 && get_prio(ops[o_top]) >= get_prio(c)) {
                float v2 = nums[n_top--];
                float v1 = nums[n_top--];
                char op = ops[o_top--];
                nums[++n_top] = Calc(v1, v2, op);
            }
            ops[++o_top] = c;
        }

        // --- F. 干扰字符 (&, #, 空格...) ---
        else {
            continue; // 直接跳过
        }
    }

    // --- G. 扫尾 ---
    while (o_top >= 0) {
        float v2 = nums[n_top--];
        float v1 = nums[n_top--];
        char op = ops[o_top--];
        nums[++n_top] = Calc(v1, v2, op);
    }

    return (n_top >= 0) ? nums[n_top] : 0;
}

