#include <stdio.h>
#include "bmp_header.h"
#include <string.h>
#include <stdlib.h>

typedef struct {

    unsigned char B;
    unsigned char G;
    unsigned char R;
}RGB;
int padding (int width){

    int pad = 4 - ((3 * width) % 4);
    if(pad == 4)
        return 0;
    else
        return pad ; 

}

int edit(char *path, bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat){

    FILE *in = fopen(path, "rb");

    if (in == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s\n", path);
        return -1;
    }

    
    
    
    fread(fh, sizeof(bmp_fileheader), 1, in);
    fread(ih, sizeof(bmp_infoheader), 1, in);
    
    int pad = (ih->biSizeImage - (ih->height * ih->width * 3)) / ih->height ;
    printf("%d %d %d %d\n",pad, ih->biSizeImage, ih->height, ih->width);
    
    
    for(int i = 0; i < ih->height; i ++){

        
        fread(mat[i], sizeof(RGB), ih->width, in);
       
        fseek(in, pad, SEEK_CUR);
    }
  
       


    

    fclose(in);
   
}

int save(char *path, bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat){

    FILE *in = fopen(path, "wb");
    
    if (in == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s\n", path);
        return -1;
    }
    
    fwrite(&fh, sizeof(bmp_fileheader), 1, in);
      
    fwrite(&ih, sizeof(bmp_infoheader), 1, in);

     int pad = (ih->biSizeImage - (ih->height * ih->width * 3)) / ih->height ;
    unsigned char *p = calloc(1, sizeof(unsigned char));
   
    for(signed int i = 0; i < ih->height; i ++){
       
        fwrite(&(mat[i]), sizeof(RGB), ih->width, in);
        fwrite(&p, sizeof(unsigned char), pad, in);

        
       
        
    }

    fclose(in);

}

void quit(bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat){

    free(fh);
    free(ih);
    free(mat);
    exit(0);
    
}

int main(){

    bmp_fileheader *fh = calloc(1, sizeof(bmp_fileheader));
    bmp_infoheader *ih = calloc(1, sizeof(bmp_infoheader));;
    RGB **mat = calloc(65535, sizeof(RGB *));
    
    for(int i = 0; i < 65535; i ++){

        mat[i] = calloc(65535, sizeof(RGB));
        
    }
     

    char input[10001];
    int lenght;

    while(fgets(input, sizeof(input), stdin)){

        lenght = strlen(input);
       if(input[lenght - 1] == 10)
            input[lenght - 1] = 0;

       
        char *path = calloc(1001, sizeof(char));
        path = strtok(input, " ");
        
        char *function = calloc(101, sizeof(char));
        strcpy(function, path);
        path = strtok(NULL, " ");

      

        if(strcmp(function, "edit") == 0)
            edit(path, fh, ih, mat);
           
        
        if(strcmp(function, "save") == 0)
            save(path, fh, ih, mat);

        if(strcmp(function, "quit") == 0)
            quit( fh,  ih,  mat);
        


    }


    return 0;
}
    

