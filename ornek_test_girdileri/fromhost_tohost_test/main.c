#include <stdio.h>

int main(int argc, char **argv)
{
  // printf("Hello World!\n");
  // printf("a icin deger girin\n");
  #ifndef DUMMY_TXT_PATH
  #define DUMMY_TXT_PATH "dummy.txt"
  #warning DUMMY_TXT_PATH is not defined. Using default path: dummy.txt
  #endif
  const char *file_path = DUMMY_TXT_PATH;
  int a, b;
  printf("dosya aciliyor: %s\n", file_path);

  FILE *input_file = fopen(file_path, "r");

  fscanf(input_file, "%d", &b);
  printf("dosyadan okunan sayi: %d\n", b);

  printf("dosyadan okunan sayiyi "
         "carpmak icin deger girin\n");
  scanf("%d", &a);
  a = a * b;

  printf("carpim sonucu %s dosyasina yaziliyor\n", file_path);

  fclose(input_file);

  FILE *fp;
  fp = fopen(file_path, "w");
  fprintf(fp, "%d", a);
  fclose(fp);
}
