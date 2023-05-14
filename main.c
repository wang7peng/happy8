#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcat strcmp strcpy strlen
#include <time.h>
#include <unistd.h>

// avoid choosing this 5 ball, they rarely appear in each night result. :(
typedef struct {
  char time[10];
  int balloldest;
  int ball2;
  int ball3;
  int ball4;
  int ball5;
  int ballmaybe;
}ColdballRecord;

const
char* getLastNightDate() {
  FILE *fp;
  static char timeStr[10];
  
  // get time format, e.g 2023-0520
  fp = popen("date -d'1 day ago' +%Y-%m%d", "r");
  fgets(timeStr, sizeof(timeStr), fp);
  return timeStr;
}

ColdballRecord getColdfromFile() {
  FILE *fp;
  ColdballRecord br = {"2023-0520", 0, 0, 0, 0, 0};
    
  char buf[30];  
  char yesterday[10];
  strcpy(yesterday, getLastNightDate());
  
  fp = fopen("./config/cold.txt", "r");
  if (fp == NULL) {
    printf("No such file recorded cold balls!\n");
    return br;
  }
  //rewind(fp);

  int goon = 1;
  int count = 0;
  while(goon) {
    fgets(buf, 30, fp);
    sscanf(buf, "%s %d %d %d %d %d %d", 
      br.time, &br.balloldest, &br.ball2, &br.ball3, &br.ball4, &br.ball5, &br.ballmaybe);

    // only need one record, stop searching once found it. 
    if (!strcmp(br.time, yesterday)) {
      printf("cold recoid: %s", buf);
      goon = 0; 
    }
    if (count++ == 99) {
      printf("Warn: No such day record!\n");
      goon = 0;
    }
  }
  fclose(fp);

  return br;
}

void storeString2Arr(char *str, int arr[]) {
  int count = 0;
  int indexValue = 0;
  
  for (int i = 0; i < strlen(str); ++i) { // len: 11
    if (*(str+i) > 47 && *(str+i) < 58) {
      indexValue *= 10;
      indexValue += *(str+i) - 48;
    } else {
      arr[count++] = indexValue;
      indexValue = 0; // reset
    }
  }
  // printf("count: %d\n", count);
}

int getElementToday(char *str) {
  int ret = 0;
  
  if (*str-48) {
     ret = (*str-48) * 10 + *(str+1) - 48;
  } else {
     ret = *(str+1) - 48; 
  }
  return ret + 1; // ret is yesterday!
}

void setOverlap(int arr[]) {
  FILE *fp;
  char buffer[50];
  
  // get time format, e.g 2023-0520
  fp = popen("date -d'1 day ago' +%Y-%m%d", "r");
  fgets(buffer, sizeof(buffer), fp);
  
  int eleToday = getElementToday(buffer+7);
  printf("ball today: %3d\n", eleToday);
  
  char cmdReadOverlap[50] = "more config/overlap* | grep ";
  strcat(cmdReadOverlap, buffer);
  // printf("run cmd: %s", cmdReadOverlap);
  fp = popen(cmdReadOverlap, "r");
  fgets(buffer, sizeof(buffer), fp);
  printf("repe record: %s", buffer);

  storeString2Arr(buffer + 10, arr);
  arr[5] = eleToday;
  
  pclose(fp);
}

int existSame(int arr[], int len, int src[], int src_len) {
  puts("-- -- -- -- -- -- -- -- -- --");
  for (int i = 0; i < len; i++) {
    printf("new[%d]: %-6d", i, arr[i]); 
    for (int j = 0; j < src_len; j++) {
      printf("%3d", src[j]);
      if (arr[i] == src[j]) {
      	printf(" x \n");
      	return 1;
      }
    }
    printf(" √ \n");
  }

  return 0;
}

// get 3 numbers from scale [1, 80]
int getelement(int max) {

  int avoidArr[5] = {33, 40, 41, 55};
  int ret = 0;
  
  while(1) {
    ret = rand() % (max + 1) + 1;
    
    int i = 0;
    for (i = 0; i < 5; i++) {
	if (ret == avoidArr[i]) 
	    break;
    }
    if (i==5) break;
  }
  
  return ret;
} 


void getOneRow(int arr[]) {
  while(1) {
        arr[0] = getelement(80);
        arr[1] = getelement(80);
        arr[2] = getelement(80);
        if (arr[0] != arr[1] && arr[0] != arr[2] && arr[1]!=arr[2])
        break;
  }

}

void algo_getArrImage(int (*arr)[3], // priority: 1-[ , 2-*
                      int (*srcArr)[3], 
                      int col) {
  for (int i = 0; i < col; ++i) {
    arr[0][i] = (srcArr[0][i] + 3) % 80;
    arr[1][i] = (srcArr[1][i] + 2) % 80;
    arr[3][i] = (srcArr[3][i] - 2 + 80) % 80;
    arr[4][i] = (srcArr[4][i] - 3 + 80) % 80;
    arr[i+1][0] = (arr[i][0] + srcArr[i+1][1]) % 80;
  }
  arr[2][0] = (srcArr[2][0] + 23) % 80;
  arr[2][1] = srcArr[2][1];
  arr[2][2] = (srcArr[2][2] - 23 + 80) % 80;
}

int checkBigTotal(int (*arr)[3]){
  int totalBig = 0;
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 3; col++)
      if (arr[row][col] > 40) totalBig++;
  }
  printf("totalBig: %d\n", totalBig);
  
  // big:small just in {6:9, 7:8, 8:7, 9:6} 
  if ((totalBig * 2 - 15) > 3 || (totalBig * 2 - 15) < -3) {
    printf("abandon: not average between big and small numbers!\n");
    return 0;
  } else {
    printf("this ok!\n");
    return 1;
  }
}

int main(int argc, char** argv) {
  int row[5][3] = {0};
  int row2[5][3] = {0};
  
  srand((unsigned)time(NULL));
  
  int avoidLastRes[6] = {0};
  setOverlap(avoidLastRes);
  ColdballRecord ar = getColdfromFile();
  int avoidCold[6] = {ar.balloldest, ar.ball3, ar.ball4, ar.ball5, ar.ball2, ar.ballmaybe};

  for (int i = 0; i < 6; i++)
    printf("%-3d%-3d", avoidLastRes[i], avoidCold[i]);
  putchar(10);

  while (1) {
    // A ticket at most has 5 different bets 
    int i;
    for (i = 0; i < 5; i++) {
      getOneRow(row[i]);

      if (existSame(row[i], sizeof(row[i])/4, avoidLastRes, 6) ||
           existSame(row[i], sizeof(row[i])/4, avoidCold, 6))
    	  i--; // again
    }
  
    
    algo_getArrImage(row2, row, 3);

    for (i = 0; i < 5; i++) {
      if (existSame(row2[i], sizeof(row2[i])/4, avoidLastRes, 3) ||
        existSame(row2[i], sizeof(row2[i])/4, avoidCold, 4)) {
        sleep(1);
        getOneRow(row2[i]);
        i--; // otherwise again
      }
    }
 
    if (checkBigTotal(row) && checkBigTotal(row2))
      break; 
  }

  char *hr = "-- -- --";
  printf("\n %s Today I Choose %s \n", hr, hr);
  for (size_t i = 0; i < 5; i++) {
    for (size_t j = 0; j < 3; j++) {
      printf("%3d", row[i][j]);
    }
    printf("%c%c", 9, 9);
 
    for (size_t j = 0; j < 3; j++) {
      printf("%3d", row2[i][j]);
    }
    putchar(10);
  }
  return 0;
}
