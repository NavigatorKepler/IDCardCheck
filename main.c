#define READBUF 256
#define STRMAX  32
#define LISTMAX 3600
#define OUTOFRANGE listptr+1

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

struct POSITION {
    uint32_t code;
    uint8_t local[64];
} __preread, __current, position_list[LISTMAX];

int8_t readbuf[READBUF];

size_t listptr=0;

uint32_t LoadList() {
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

uint32_t searchIndex(uint32_t code) {
    uint32_t imax=listptr;
    uint32_t imin=0;
    uint32_t index=(imax + imin) / 2;
    __current=position_list[index];
    while (imin < imax) {
        // printf("code=%d, imin=%d, imax=%d, ptr=%d\r", code, imin, imax, index);
        if ((imax - imin) / 2 < 1) {
            return OUTOFRANGE;
        } else if (__current.code == code) {
            // printf("\n");
            return index;
        } else if (__current.code > code) {
            imax -= (imax - imin) / 2;
        } else if (__current.code < code) {
            imin += (imax - imin) / 2;
        }
        index=(imax + imin) / 2;
        __current=position_list[index];
    }
    if (__current.code == code) {
        // printf("\n");
        return index;
    } else {
        return OUTOFRANGE;
    }
}

void main() {
    LoadList();

    uint8_t  c;
    uint8_t  stopflag=0;
    uint8_t   IDtemp[STRMAX]={};
    uint32_t strlength=0;
    uint32_t _T1, _T2;
    uint32_t location=0, biry=0, birm=0, bird=0, cnt=0, chk=0;
    uint32_t realLocationID1, realLocationID2, realLocationID3;

    uint8_t  weight[17] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    char  ZtoM[11] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'};

    // 用于验证二分查找
    // for (int i=0;i<listptr;i++) {
    //     int32_t searchCode = searchIndex(position_list[i].code);
    //     if (searchCode != i) {
    //         printf("在%d位置发生错误, 实际区号为%d，查找到的序号是%d，区号是%d\n", i, position_list[i].code, searchCode, position_list[searchCode].code);
    //     };
    // }

    while (1) {
        stopflag=0;location=0;
        biry=0;birm=0;bird=0;
        cnt=0;chk=0;
        memset(IDtemp, 0, sizeof(uint8_t)*STRMAX);
        printf("请输入18位身份证号:");
        scanf("%18s", IDtemp);
        while ((c = fgetc(stdin)) != '\n' && c != EOF);

        for (strlength=0;IDtemp[strlength]!='\0'&&strlength<STRMAX;strlength++);
        if (strlength<18) {
            printf("身份证长度不足18位!\n");
            continue;
        }
        for (uint32_t i=0;i<17;i++) {
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

        uint32_t res=chk%11;
        if (ZtoM[res] != IDtemp[17]) {
            printf("输入的身份证%s不合法!\n",IDtemp);
            stopflag=1;
        }
        if (stopflag == 1) continue;

        for (uint32_t i=0;i<6;i++) {
            location*=10;
            location+=IDtemp[i]-'0';
        }
        for (uint32_t i=0;i<4;i++) {
            biry*=10;
            biry+=IDtemp[i+6]-'0';
        }
        for (uint32_t i=0;i<2;i++) {
            birm*=10;
            birm+=IDtemp[i+10]-'0';
        }
        for (uint32_t i=0;i<2;i++) {
            bird*=10;
            bird+=IDtemp[i+12]-'0';
        }
        for (uint32_t i=0;i<3;i++) {
            cnt*=10;
            cnt+=IDtemp[i+14]-'0';
        }

        _T1 = 0;
        _T2 = 0;
        _T1 = location/10000;
        _T2 = location/100;
        _T1 *= 10000;
        _T2 *= 100;
        realLocationID1 = searchIndex(_T1);
        realLocationID2 = searchIndex(_T2);
        realLocationID3 = searchIndex(location);
        if (realLocationID3 == OUTOFRANGE || realLocationID2 == OUTOFRANGE || realLocationID1 == OUTOFRANGE) {
            printf("身份证的地区无效!\n");
            stopflag=1;
        } else {
            printf("地区代码:%d; %s, %s, %s\n", location, position_list[realLocationID1].local, position_list[realLocationID2].local, position_list[realLocationID3].local);
        }
        if (stopflag == 1) continue;
        printf("出生于:%d年%d月%d日\n", biry, birm, bird);
        if (cnt%2) {
            printf("性别:男\n");
        } else {
            printf("性别:女\n");
        }
    }
}
