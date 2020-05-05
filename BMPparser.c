/*
Project no. 2 "BMP file parser"
KONRAD SZEWCZYK 145466 and JERZY LUKASZEWICZ 145458

Our program mainly support bitmap files only, for other file types it can work incorrectly!

Every use of this program prints information about this file like size width, height etc.
It also create simple histogram of RGBs color percentage

For files with Compression == 0 and BitCount == 24 it supports
Executable commands:
  1) ./parseBMP file-to-decrypt.bmp then chosing Y or y
        - decrypt hidden messege in file and prints it out
  2) ./parseBMP source-file.bmp new-file-name.bmp
        - make a grey image of source files
  3) ./parseBMP source-file.bmp new-file-name.bmp "messege to hide"
        - hides a messege inside bitmap file
*/




#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <math.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

typedef struct __attribute__((__packed__)) {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct __attribute__((__packed__)) {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

int potega(int base, int power){
  int result = 1;
  for(int i = 1; i <= power;i++){
    result *= base;
  }
  return result;
}


int main(int argc, char const *argv[]) {
  FILE *fileBMP;
  BITMAPFILEHEADER bitmapFileHeader;
  BITMAPINFOHEADER bitmapInfoHeader;

  fileBMP = fopen(argv[1],"rb");
  if (fileBMP == 0){
    return 0;
  }

  /// READING FROM FILE ///

  fread(&bitmapFileHeader, sizeof(unsigned char), sizeof(bitmapFileHeader),fileBMP);
  fread(&bitmapInfoHeader, sizeof(unsigned char), sizeof(bitmapInfoHeader),fileBMP);

  /// INFO SECTION ///

  //filehead
  printf("BITMAPFILEHEADER:\n");
  printf("bfType:         0x%X\n", bitmapFileHeader.bfType);
  printf("bfSize:         %u\n", bitmapFileHeader.bfSize);
  printf("bfReserved1:    0x%X\n", bitmapFileHeader.bfReserved1);
  printf("bfReserved2:    0x%X\n", bitmapFileHeader.bfReserved2);
  printf("bfOffBits:      %u\n", bitmapFileHeader.bfOffBits);
  //fileinfo
  printf("\nBITMAPINFOHEADER:\n");
  printf("biSize:         %u\n", bitmapInfoHeader.biSize);
  printf("biWidth:        %d\n", bitmapInfoHeader.biWidth);
  printf("biHeight:       %d\n", bitmapInfoHeader.biHeight);
  printf("biPlanes:       %u\n", bitmapInfoHeader.biPlanes);
  printf("biBitCount      %u\n", bitmapInfoHeader.biBitCount);
  printf("biCompression   %u\n", bitmapInfoHeader.biCompression);
  printf("biSizeImage     %u\n", bitmapInfoHeader.biSizeImage);
  printf("biXPelsPerMeter %d\n", bitmapInfoHeader.biXPelsPerMeter);
  printf("biYPelsPerMeter %d\n", bitmapInfoHeader.biYPelsPerMeter);
  printf("biClrUsed       %u\n", bitmapInfoHeader.biClrUsed);
  printf("biClrImportant  %u\n\n", bitmapInfoHeader.biClrImportant);


  /// RGB HISTOGRAM and (GREY IMAGE or HIDEN MESSEGE)///

  if(bitmapInfoHeader.biBitCount != 24 || bitmapInfoHeader.biCompression != 0){
    printf("Histogram calculation is unsupported!");
  }
  else{
    int size = bitmapFileHeader.bfSize;
    int height = bitmapInfoHeader.biHeight;
    int width = bitmapInfoHeader.biWidth;

    //HISTOGRAM RGB TAB

    float hist[3][16];
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 16; j++){
        hist[i][j] = 0;
      }
    }
    int color_val = 0;
    //int divi = size / 3;
    int divi = height * width;
    int padding = width % 4;
    unsigned char *data = malloc(sizeof(unsigned char) * size);

    //RGB HISTOGRAM
    fread(data, sizeof(unsigned char), size, fileBMP);
    fclose(fileBMP);
    // counting values
    for(int row = 0; row < height; row++){
      for(int col = 0; col < width; col++) {
        int p = (row * width + col) * 3 + row * padding;
        for(int k = 0; k < 3; k++){
          color_val = data[p + k];
          color_val /= 16;
          hist[k][color_val]+=1;
        }
      }
    }
    //printing RGB HISTOGRAM
    char *rgb[3] = {"BLUE","GREEN","RED"};
    for(int i = 2; i >= 0; i--){
      printf("%s:\n",rgb[i]);
      for(int j = 0; j < 16; j++){
        int a1,a2;
        a1 = j * 16;
        a2 = (j+1) * 16 -1;
        float value = hist[i][j]*100/divi;
        printf("  %d-%d: %0.2f%%\n",a1,a2, value);
      }
    }


    //ENCRYPTING MESSE


    if(argc == 4){
      FILE *newfile;
      newfile = fopen(argv[2],"wb");
      fwrite(&bitmapFileHeader,1,sizeof(bitmapFileHeader),newfile);
      fwrite(&bitmapInfoHeader,1,sizeof(bitmapInfoHeader),newfile);
      int messege_len = strlen(argv[3]);
      char text[messege_len+1];
      strcpy(text,argv[3]);
      int mes_tab [messege_len+1][8];
      for(int i=0;i<messege_len+1;i++){
        for(int j=0;j<8;j++){
          mes_tab[i][j] = 0;
        }
      }
      //BINARY FORM OF MESSEGE LENGTH
      int n = messege_len;
      for(int i=0;n>0;i++){
        int a = n%2;
        if(a%2==1){
          mes_tab[0][i]=1;
        }
        else{
          mes_tab[0][i]=0;
        }

        n=n/2;
      }
      //CHARACTER TO BINARY FORM
      for(int i = 0; i< messege_len;i++){
        char c = text[i];
        for (int j = 7; j >= 0; j--){
            int k = potega(2,j);
            if(c >= k){
              c-=k;
              mes_tab[i+1][j] = 1;
            }
            else{
              mes_tab[i+1][j] = 0;
            }
          }
        }
      //WRITING FRIST BYTEs TO FILE
      for(int i=0;i<68;i++){
        fwrite(&data[i],1,1,newfile);
      }
      //ENCRYPTING
      int count =0;
      for(int i = 0;i<messege_len+1;i++){
        for(int b = 0;b<8;b++){
          unsigned char bit = mes_tab[i][b];
          unsigned char byte = data[i*8+b+68];
          unsigned char newbyte = (byte & 0xFE) | bit;
          fwrite(&newbyte,1,1,newfile);
          count += 1;
        }
      }
      for(int i = count; i < height*width*3;i++){
        fwrite(&data[i+68],1,1,newfile);
      }
      fclose(newfile);
    }

    else if(argc == 3){
    // GREY IMAGE

    //NEW FILE CREATING

      FILE *newfile;
      newfile = fopen(argv[2],"wb");
      fwrite(&bitmapFileHeader,1,sizeof(bitmapFileHeader),newfile);
      fwrite(&bitmapInfoHeader,1,sizeof(bitmapInfoHeader),newfile);

    //GREY IMAGE

      for(int row = 0; row < height; row++){
        for(int col = 0; col < width; col++) {
          int p = (row * width + col) * 3 + row * padding;
          int grey_val = 0;
          //CALCULATING GREY VALUE
          for(int k = 0; k < 3; k++){
            grey_val += data[p+k];
          }
          grey_val = grey_val/3;
          for(int k=0;k<3;k++){
            fwrite(&grey_val,1,1,newfile);
          }
        }
      }
      fclose(newfile);
    }

    else{
      //decoding
      printf("Decode steganography? [y/n]\n");
      char ans;
      scanf("%c", &ans);
      if(ans == 121 || ans == 89){ //121 = y     89 = Y

        FILE *decryptFile;
        BITMAPFILEHEADER decryptbitmapFileHeader;
        BITMAPINFOHEADER decryptbitmapInfoHeader;
        decryptFile = fopen(argv[1],"rb");
        if (decryptFile == 0){
          free(data);
          return 0;
        }

        /// READING FROM FILE TO DECRYPT///

        fread(&decryptbitmapFileHeader, sizeof(unsigned char), sizeof(decryptbitmapFileHeader),decryptFile);
        fread(&decryptbitmapInfoHeader, sizeof(unsigned char), sizeof(decryptbitmapInfoHeader),decryptFile);
        unsigned char *decryptdata = malloc(sizeof(unsigned char) * size);
        fread(decryptdata, sizeof(unsigned char), size, decryptFile);
        fclose(decryptFile);
        int dec_len = 0;
        for(int i = 68; i<76; i++){
          unsigned char byte = data[i];
          unsigned char bin = byte & 0x01;
          int pot = bin * potega(2,i-68);
          dec_len += pot;
        }
        printf("Decrypted messege:\n");
        for(int i = 0;i<dec_len;i++){
          int znak[8];
          for(int j=0;j<8;j++){
            int px = decryptdata[i*8 + j+76];
            znak[j] = px & 0x01;
          }
          char text[8] ="";
          for(int k=0;k<8;k++){
            if(znak[k]==1){
              text[8-k-1]='1';
            }
            else{
              text[8-k-1]='0';
            }
            char c = strtol(text, 0, 2); //converting binary number to character
            printf("%c", c);
          }
        }
        printf("\n");
        free(decryptdata);
      }
      else{
        free(data);
        return 0;
      }
    }
    free(data);
  }
  return 0;
}
