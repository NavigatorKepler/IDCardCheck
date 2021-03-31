#define READBUF 256
#define LISTMAX 3600

#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct POSITION {
    uint64_t code;
    uint8_t local[64];
} __preread, __current, position_list[LISTMAX];

uint8_t readbuf[READBUF];

size_t listptr=0;

int LoadList() {
    FILE* fp=fopen("IDcard.txt", "r");
    if (fp == NULL) {
        return 1;
    }
    while (fgets(readbuf, READBUF, fp) != NULL) {
        sscanf(readbuf, "%d|%s", &__preread.code, &__preread.local);
        position_list[listptr]=__preread;
        listptr++;
    }

    // 用于验证文件是否正常加载
    // for (int i=0;i<listptr;i++) {
    //     printf("%d|%s\n", position_list[i].code, position_list[i].local);
    // }

    fclose(fp);
    return 0;
}

int64_t searchIndex(uint64_t code) {
    size_t imax=listptr;
    size_t imin=0;
    size_t index=(imax + imin) / 2;
    __current=position_list[index];
    while (imin < imax) {
        if (__current.code == code) {
            return index;
        } else if (__current.code > code){
            imax -= (imax - imin) / 2;
        } else if (__current.code < code){
            imin += (imax - imin) / 2;
        }
        index=(imax + imin) / 2;
        __current=position_list[index];
    }
    return -1;
}

void main() {
    LoadList();

    char c;
    char stopflag=0;
    char IDtemp[19];
    uint64_t location, biry, birm, bird, cnt, chk;

    uint8_t weight[] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    uint8_t ZtoM[] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'};

    // 用于验证二分查找
    // for (int i=0;i<listptr;i++) {
    //     int64_t searchCode = searchIndex(position_list[i].code);
    //     if (searchCode != i) {
    //         printf("在%d位置发生错误, 实际区号为%d，查找到的序号是%d，区号是%d\n", i, position_list[i].code, searchCode, position_list[searchCode].code);
    //     };
    // }

    while (1) {
        stopflag=0;
        printf("请输入18位身份证号:");
        scanf("%18s", &IDtemp[0]);
        while ((c = fgetc(stdin)) != '\n' && c != EOF);
        if (strlen(IDtemp) < 18) {
            printf("身份证长度不正确!\n");
            stopflag=1;
        }
        if (stopflag == 1) continue;


        for (char i=0;i<17;i++) {
            if (IDtemp[i]-'0'<0 || IDtemp[i]-'9'>9) {
                printf("输入中混入了非法字符!\n");
                stopflag=1;
                break;
            } else {
                chk += (IDtemp[i]-'0')*weight[i];
            }
        }
        if (stopflag == 1) continue;

        if (IDtemp[17] == 'x') IDtemp[17]='X';
        if (IDtemp[17] != 'X' && (IDtemp[17]-'0'<0 || IDtemp[17]-'9'>9)) {
            printf("输入中混入了非法字符!\n");
            stopflag=1;
        }
        if (stopflag == 1) continue;

        char res=chk%11;
        if (ZtoM[res] != IDtemp[17]) {
            // printf("%s\n",ZtoM[res]);
            printf("输入的身份证%s不合法!\n",IDtemp);
            stopflag=1;
        }
        if (stopflag == 1) continue;

    }
}