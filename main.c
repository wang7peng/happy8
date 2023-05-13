#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcat strlen
#include <time.h>

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
  return ret;
}

void setOverlap(int arr[]) {
  FILE *fp;
  char buffer[50];
  
  // get time format, e.g 2023-0520
  fp = popen("date -d'1 day ago' +%Y-%m%d", "r");
  fgets(buffer, sizeof(buffer), fp);
  
  int eleToday = getElementToday(buffer+7);
  
  char cmdReadOverlap[50] = "more config/overlap* | grep ";
  strcat(cmdReadOverlap, buffer);
  // printf("run cmd: %s", cmdReadOverlap);
  fp = popen(cmdReadOverlap, "r");
  fgets(buffer, sizeof(buffer), fp);
  printf("record: %s", buffer);

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
        if (arr[1] != arr[2] && arr[1] != arr[2] && arr[1]!=arr[3])
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
  }
  
  arr[2][0] = srcArr[2][0] + 1;
  arr[2][1] = srcArr[2][1];
  arr[2][2] = srcArr[2][2] - 1;
  
}

int checkBigTotal(int (*arr)[3]){
  int totalBig = 0;
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 3; col++)
      if (arr[row][col] > 40) totalBig++;
  }
  printf("totalBig: %d\n", totalBig);
  
  if ((totalBig * 2 - 15) > 4 || (15 - 2 * totalBig) > 4) {
    printf("abandon: not average between big and small numbers!\n");
    return 0;
  } else {
    printf("this ok!\n");
    return 1;
  }
}

int main(int argc, char** argv) {
  int row[5][3] = {0};
  srand((unsigned)time(NULL));
  
  int avoidLastRes[6] = {0};
  
  setOverlap(avoidLastRes);
  for (int i = 0; i < 6; i++)
    printf("%-3d", avoidLastRes[i]);
  putchar(10);
  
  int row2[5][3] = {0};
  while (1) {
    for (int i = 0; i < 5; i++) {
      getOneRow(row[i]);
      if (existSame(row[i], sizeof(row[i])/4, avoidLastRes, 6))
    	  i--; // again
    }

    
    algo_getArrImage(row2, row, 3);
  
    if (checkBigTotal(row) && checkBigTotal(row2))
      break; // bigNum - smallNum <= 4
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
