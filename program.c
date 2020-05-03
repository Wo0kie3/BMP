#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

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

typedef struct __attribute__((__packed__)) {
        unsigned char   b; //rgbtBlue;
        unsigned char   g; //rgbtGreen;
        unsigned char   r; //rgbtRed;
} PIX;


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


  /// RGB HISTOGRAM ///

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
    int divi = size / 3;
    int blue, green, red;
    int padding = width % 4;
    unsigned char *data = malloc(sizeof(unsigned char) * size);

    fread(data, sizeof(unsigned char), size, fileBMP);
    fclose(fileBMP);

        //NEW FILE CREATING
    char *newfilename;
    printf("Type new file name with .bmp:\n");
    scanf("%s", newfilename);

    FILE *newfile;
    newfile = fopen(newfilename,"wb");
    fwrite(&bitmapFileHeader,1,sizeof(bitmapFileHeader),newfile);
    fwrite(&bitmapInfoHeader,1,sizeof(bitmapInfoHeader),newfile);
        //fwrite(data,sizeof(WORD),bitmapInfoHeader.biSizeImage,newfile);

    //for(int row = 0; row < height; row++){}
    //for(int row = height - 1; row >= 0; row--)
    for(int row = 0; row < height; row++){
      for(int col = 0; col < width; col++) {
        int p = (row * width + col) * 3 + row * padding;
        //printf("%d %d %d\n", data[p+0], data[p+1], data[p+2]);
        int grey_val = 0;
        for(int k = 0; k < 3; k++){
          grey_val += data[p+k];
          color_val = data[p + k];
          color_val /= 16;
          hist[k][color_val]+=1;
        }
        grey_val = grey_val/3;
        for(int k=0;k<3;k++){
          fwrite(&grey_val,1,1,newfile);
        }
        //GREYs
      }
    //HISTOGRAM OUTPUT
    }
    fclose(newfile);



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
  }
  ///NEW FILE CREATION

  return 0;
}
