#include <stdio.h>

int main(int argc, char **argv)
{
  // printf("Hello World!\n");
  // printf("a icin deger girin\n");
  char *file_path = "/home/usr1/riscv-isa-sim/"
                    "a_tets_faruk/spike_link/log/dummy.txt";
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
