#include "TaskSlove.h"


/* 通用解密模板1 - 寻找 < 和 > 内的数据 */
char E01_Results[8][20];
uint8_t E01_Count;
void SloveTemplate1(void)
{
    char TempBuf[20]; // 缓冲区稍微开大点
    // 1. 数据源拷贝 (根据实际长度修改 memcpy 大小)
    memcpy(TempBuf, READ_RFID, 16);
    TempBuf[16] = '\0';

    char *pStart = NULL;
    char *pEnd = NULL;
    E01_Count = 0; // 清空索引计数器

    // 2. 循环寻找所有的结束符 '>'
    while ((pEnd = strchr(TempBuf, '}')) != NULL)
    {
        pStart = NULL; // 每次循环必须重置！

        // 3. 从 '>' 位置开始，往回（向左）找最近的一个 '<'
        for (char *p = pEnd - 1; p >= TempBuf; p--)
        {
            if (*p == '{')
            {
                pStart = p;
                break;
            }
        }

        if (pStart != NULL)
        {
            // 4. 计算有效数据长度
            // 长度 = (结束符地址) - (起始符地址) - 1
            int len = pEnd - pStart - 1;

            if (len > 0 && len < 20) // 增加边界保护
            {
                // 提取数据 (跳过开头的 '<')
                memcpy(E01_Results[E01_Count], pStart + 1, len);
                E01_Results[E01_Count][len] = '\0'; // 封口

                E01_Count++;
                if (E01_Count >= 8) break; // 防止数组溢出
            }

            // 5. 抹除这一段 (<Data>)
            // 长度 = (结束符地址 - 起始符地址) + 1
            memset(pStart, ' ', pEnd - pStart + 1);
        }
        else
        {
            // 6. 没找到开头，只抹除结尾，防止死循环
            *pEnd = ' '; // strchr 返回的是单个字符指针，直接赋值即可
        }
    }
}




/* 通用解密模板2 - 寻找 $$ 和 && 内的数据 */
char E02_Results[8][20]; // 结果存放
uint8_t E02_Count;
void SloveTemplate2(void)
{
    char TempBuf[20]; // 稍微开大一点，防止溢出
    // 注意：如果是处理长字符串，TempBuf 要足够大。如果是 RFID 16字节，这里 17 就够。
    // 但你的例子里字符串明显可能超过 16 字节，建议开大点。

    // 假设 InputData 是你要处理的源数据
    // 如果源数据只有16字节，那这里没问题。如果是长字符串，请改大 memcpy 长度
    memcpy(TempBuf, READ_RFID, 16);
    TempBuf[16] = '\0';

    char *pStart = NULL;
    char *pEnd = NULL;
    E02_Count = 0;  // 清空索引计数器

    while ((pEnd = strstr(TempBuf, "&&")) != NULL)
    {
        pStart = NULL; // 每次循环前要重置 pStart

        // 1. 从 '&&' 往前找最近的 '$$'
        // 注意：循环条件 p >= TempBuf + 1，防止 p-1 越界
        for(char *p = pEnd - 1; p >= TempBuf + 1; p--)
        {
            if(*p == '$' && *(p-1) == '$')  // 找到连续的两个 $
            {
                pStart = p - 1; // 【关键修正】pStart 要指向第一个 $
                break;
            }
        }

        if (pStart != NULL)
        {
            // 2. 计算有效数据长度
            // 总长 = (结束符地址) - (起始符地址)
            // 数据长 = 总长 - 4 (两个$$, 两个&&)
            int len = (pEnd - pStart) - 2;

            if (len > 0 && len < 20) // 增加边界保护
            {
                // 3. 提取数据 (跳过开头的 $$)
                memcpy(E02_Results[E02_Count], pStart + 2, len);
                E02_Results[E02_Count][len] = '\0'; // 加上结束符

                E02_Count++;
                if(E02_Count >= 8) break; // 防止数组溢出
            }

            // 4. 抹除这一段 ($$Data&&)
            // 长度 = (pEnd - pStart) + 2 (两个&)
            memset(pStart, ' ', (pEnd - pStart) + 2);
        }
        else
        {
            // 5. 没找到开头，只抹除结尾，防止死循环
            memset(pEnd, ' ', 2);
        }
    }
}

/* 通用解密模板3 - 寻找 %%% 和 &&& 内的数据 */
char E03_Results[8][20]; // 结果存放
uint8_t E03_Count;

void SloveTemplate3(void)
{
    char TempBuf[32]; // 3个字符占位较多，建议缓冲区开大点

    // 假设 READ_RFID 是源数据，拷贝 30 字节 (留 2 字节给 \0 和防溢出)
    // 请根据实际数据长度修改 memcpy 的大小
    memcpy(TempBuf, READ_RFID, 16);
    TempBuf[30] = '\0';

    char *pStart = NULL;
    char *pEnd = NULL;
    E03_Count = 0;

    // 1. 循环寻找所有的结束符 "&&&"
    while ((pEnd = strstr(TempBuf, "&&&")) != NULL)
    {
        pStart = NULL;

        // 2. 从 '&&&' 往前找最近的 '%%%'
        // 注意：循环条件 p >= TempBuf + 2，防止 p-2 越界
        for (char *p = pEnd - 1; p >= TempBuf + 2; p--)
        {
            // 判断连续三个 %
            if (*p == '%' && *(p-1) == '%' && *(p-2) == '%')
            {
                pStart = p - 2; // 【关键】pStart 要指向第一个 %
                break;
            }
        }

        if (pStart != NULL)
        {
            // 3. 计算有效数据长度
            // 总长 = (结束符地址) - (起始符地址)
            // 数据长 = 总长 - 6 (三个%, 三个&)
            int len = (pEnd - pStart) - 3;

            if (len > 0 && len < 20)
            {
                // 提取数据 (跳过开头的 %%%)
                memcpy(E03_Results[E03_Count], pStart + 3, len);
                E03_Results[E03_Count][len] = '\0'; // 封口

                E03_Count++;
                if (E03_Count >= 8) break;
            }

            // 4. 抹除这一段 (%%%Data&&&)
            // 长度 = (结束符地址 - 起始符地址) + 3 (三个&)
            memset(pStart, ' ', (pEnd - pStart) + 3);
        }
        else
        {
            // 5. 没找到开头，只抹除结尾 "&&&" (3个字符)
            memset(pEnd, ' ', 3);
        }
    }
}

/* 通用解密模板4 - 寻找字符串的 0-9数据 */
void SloveTemplate4(char *pResult)
{
    // 1. 准备源数据
    // 注意：假设源数据在全局变量 READ_RFID 中，且长度固定为 16
    char TempBuf[50];
    memcpy(TempBuf, READ_RFID, 16);
    TempBuf[49] = '\0'; // 封口，防止越界

    uint8_t count = 0;

    // 2. 遍历提取数字
    for (int i = 0; TempBuf[i] != '\0'; i++)
    {
        // 判断是否为数字 '0' ~ '9'
        if (TempBuf[i] >= '0' && TempBuf[i] <= '9')
        {
            pResult[count] = TempBuf[i]; // 存入外部数组
            count++;
        }
    }

    // 3. 封口
    pResult[count] = '\0';

}

/* 通用解密模板5 - 寻找字符串的 0-9 A-F数据 */
void SloveTemplate5(char *pInput,char *pResult)
{
    // 1. 安全保护：如果传入的是空指针，直接返回
    if (pInput == NULL || pResult == NULL) return 0;

    uint8_t count = 0;

    // 2. 直接遍历输入字符串，直到遇到结束符 '\0'
    // 这样就不用管它是 16 字节还是 50 字节了，通吃！
    for (int i = 0; pInput[i] != '\0'; i++)
    {
        char c = pInput[i];

        // 3. 判断是否为 16 进制有效字符
        // 包含 0-9, A-F, 以及小写 a-f (可选，比赛通常是大写)
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'))
        {
            pResult[count] = c;
            count++;

            // 可选：为了绝对安全，可以加个结果数组的长度保护
            // if (count >= MAX_RESULT_LEN - 1) break;
        }
    }

    // 4. 在结果末尾添加结束符，使其成为一个标准的 C 字符串
    pResult[count] = '\0';

    return count; // 返回个数，方便后续判断是否提取成功
}


/* 通用解密模板6 - 将0-9和A-Z字母 进行排序(字数和字母顺序可以调换,A-Z字母也可以换) */
void SloveTemplate6_Sort(char *pInput, char *pResult)
{
    // 保护：防止空指针
    if (pInput == NULL || pResult == NULL) return;

    uint8_t NumBuf[128] = {0}; // 桶计数数组
    uint8_t Index = 0;

    // 1. 统计频率
    // 遍历传入的 pInput
    for(uint16_t i = 0; pInput[i] != '\0'; i++)
    {
        // 过滤非 0-F 的字符 (只统计数字和字母)
        if((pInput[i] >= '0' && pInput[i] <= '9') ||
           (pInput[i] >= 'A' && pInput[i] <= 'Z'))
        {
            NumBuf[(uint8_t)pInput[i]]++;
        }
    }

    // 2顺序输出 (按照先数字、后字母的顺序)

    // 提取只出现 1 次的数字 0-9
    for(uint16_t i = '0'; i <= '9'; i++)
    {
        if(NumBuf[i] == 1)
        {
            pResult[Index++] = (char)i;
        }
    }

    // 提取只出现 1 次的字母 A-Z
    for(uint16_t i = 'A'; i <= 'Z'; i++)
    {
        if(NumBuf[i] == 1)
        {
            pResult[Index++] = (char)i;
        }
    }

    // 封口
    pResult[Index] = '\0';


//	// 2. 排序输出(排序出现过版本:)
//    for(uint16_t i = '0'; i <= '9'; i++)
//    {
//        if(NumBuf[i] >= 1)
//        {
//            pResult[Index++] = (char)i;
//        }
//    }
//
//    // 提取只出现 1 次的字母 A-Z
//    for(uint16_t i = 'A'; i <= 'Z'; i++)
//    {
//        if(NumBuf[i] == 1)
//        {
//            pResult[Index++] = (char)i;
//        }
//    }
//
//    // 封口
//    pResult[Index] = '\0';


}






/* 通用解密模板7 - 将str1和str2的最长公共子串 保存到result */
void Find_LCS_Two(char *str1, char *str2, char *result)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int max_len = 0;
    int end_index = 0; // 记录最长子串在 str1 中的结束位置

    // 暴力遍历法（或者动态规划，比赛用暴力法代码短）
    // 矩阵 dp[len1+1][len2+1] 可能会爆栈，所以我们用滑动窗口或者简化版动态规划

    // 考虑到单片机栈空间有限，这里使用“空间优化版”暴力匹配
    for (int i = 0; i < len1; i++)
    {
        for (int j = 0; j < len2; j++)
        {
            int k = 0;
            // 从当前位置开始匹配，看能匹配多长
            while ((i + k < len1) && (j + k < len2) && (str1[i + k] == str2[j + k]))
            {
                k++;
            }
            // 如果这次匹配的长度比之前的都长，记录下来
            if (k > max_len)
            {
                max_len = k;
                end_index = i + k; // 记录在 str1 中的结束位置
            }
        }
    }

    // 提取子串
    if (max_len > 0)
    {
        // 这里的逻辑是：从结束位置往前倒推 max_len 个字符
        // 比如 str1="ABCDE", end_index=3 (指向C的后面), max_len=3
        // 就要把 ABC 拷出来
        memcpy(result, &str1[end_index - max_len], max_len);
        result[max_len] = '\0';
    }
    else
    {
        result[0] = '\0'; // 没找到
    }
}














