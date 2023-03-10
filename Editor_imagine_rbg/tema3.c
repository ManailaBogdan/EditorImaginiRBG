//313CB_Manaila_Bogdan
#include <stdio.h>
#include "bmp_header.h"
#include <string.h>
#include <stdlib.h>

//Structure pixel

typedef struct {

    unsigned char B;
    unsigned char G;
    unsigned char R;
}RGB;   

//Functie calcul padding

int padding (int width){

    int pad = width % 4;
    
    return pad; 

}

//Sir de cifre -> numar intreg

unsigned int nr_char_to_int(char *s){

    unsigned int nr = 0;

    for(int i = 0; i < strlen(s); i ++)
        nr = nr * 10 + s[i] - 48;

    return nr;

}

//Functie de schimbare a 2 numere

void swap(int *a, int *b){

    int aux;

    aux = *a;
    *a = *b;
    *b = aux;
}

//Functia edit

void edit(char *path, bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat){

    //Deschidere fisier
   FILE *in;
   in = fopen(path, "rb");

   if (in == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s\n", path);
        
    }

    //Salvare in memorie

    fread(fh, sizeof(bmp_fileheader), 1, in);

    fread(ih, sizeof(bmp_infoheader), 1, in);

    int pad = padding(ih->width);

    for(int i = 0; i < ih->height; i ++){

        fread(mat[i], sizeof(RGB), ih->width, in);

        fseek(in, pad, SEEK_CUR);
    }


    //Inchidere fisier
    fclose(in);
   
}

//Functia save

void save(char *path, bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat){


    //Deschidere fisier
   FILE *out;
   out = fopen(path, "wb");

   if (out == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s\n", path);
        
    }
       
       //Transcriere din memorie in fisier
    fwrite(fh, sizeof(bmp_fileheader), 1, out);

    fwrite(ih, sizeof(bmp_infoheader), 1, out);

    int pad = padding(ih->width);

    unsigned char *p = calloc(1, sizeof(unsigned char));

    for(int i = 0; i < ih->height; i ++){

        fwrite(mat[i], sizeof(RGB), ih->width, out);

        fwrite(p, sizeof(unsigned char), pad, out);
    }
    
    //Inchidere fisier
    fclose(out);

}


//
void insert(char *path, bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat, unsigned int y, unsigned int x){

    //Deschidere fisier
    FILE *new;
    new = fopen(path, "rb");

     if (new == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s\n", path);
        
    }

    //Citirea in memorie a datelor din fisiere
    bmp_fileheader *fh_new = malloc(1 * sizeof(bmp_fileheader));
    bmp_infoheader *ih_new = malloc(1 * sizeof(bmp_infoheader));
   

    fread(fh_new, sizeof(bmp_fileheader), 1, new);
    fread(ih_new, sizeof(bmp_infoheader), 1, new);
    
    //Suprapunere poze

    int pad = padding(ih_new->width);
    

    int width;

        if(ih_new->width <= ih->width)
            width = ih_new->width;
        else
            width = ih->width;

            
    for(int i = x; i < ih->height && i - x < ih_new->height; i ++){


        fread(mat[i] + y, sizeof(RGB), width, new);

        fseek(new, pad, SEEK_CUR);
       
        
    }

    //Eliberare memorie
    free(fh_new);
    free(ih_new);

    //Inchidere fisier
    fclose(new);
}

//Functie ce coloreaza un patrat de latura data cu o cuoare data

void fcolor(bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat, int x, int y, int l, RGB c){

    l /= 2;
    int a, b;
  
    if(x - l < 0)
        a = 0;
    else
        a = x - l;

    if(y - l < 0)
        b = 0;
    else
        b = y - l;

    for(int i = a; i <= x + l && i < ih->height; i ++)
        for(int j = b; j <= y + l && j < ih->width; j ++){

            mat[i][j].B = c.B;
            mat[i][j].G = c.G;
            mat[i][j].R = c.R;

        }

}

//Functie ce deseneaza o linie intre 2 puncte date

void draw_line(bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat, RGB color, int lenght, int y1, int x1, int y2, int x2){

    int i, x, y;
    
    fcolor(fh, ih, mat, x1, y1, lenght, color);
    fcolor(fh, ih, mat, x2, y2, lenght, color);
 
    
    if(abs(y2 - y1) > abs(x2 - x1)){

        if( y1 < y2)

        for(i = y1 + 1; i < y2; i ++){

            x = ((i - y1) * (x2 - x1) + x1 * (y2 - y1)) / (y2 - y1);
        
            fcolor(fh, ih, mat, x, i, lenght, color);

        }

        else
        for(i = y1 - 1; i > y2; i --){

            x = ((i - y1) * (x2 - x1) + x1 * (y2 - y1)) / (y2 - y1);

            fcolor(fh, ih, mat, x, i, lenght, color);

        }

    }

    else{

        if(abs(y2 - y1) <=  abs(x2 - x1)){

        if( x1 < x2)

        for(i = x1 + 1; i < x2; i ++){

            
            y = ((i - x1) * (y2 - y1) + y1 * (x2 - x1)) / (x2 - x1);
            
            fcolor(fh, ih, mat, i, y, lenght, color);

        }
        else

        for(i = x1 - 1; i > x2; i --){

            
            y = ((i - x1) * (y2 - y1) + y1 * (x2 - x1)) / (x2 - x1);
            
            fcolor(fh, ih, mat, i, y, lenght, color);

        }

        }    


    }

}

//Functia fill

void fill(bmp_fileheader *fh, bmp_infoheader *ih, RGB **mat, int y, int x, RGB color){

        
        RGB origine;
        origine.B = mat[x][y].B;
        origine.G = mat[x][y].G;
        origine.R = mat[x][y].R;
        if(origine.B == color.B && origine.G == color.G && origine.R == color.R)
        {int i = 0;}
        
        else{
        mat[x][y].B = color.B;
        mat[x][y].G = color.G;
        mat[x][y].R = color.R;

        if(origine.B == mat[x + 1][y].B && origine.G == mat[x + 1][y].G && origine.R == mat[x + 1][y].R)
            
            { fill(fh, ih, mat, y, x + 1, color);}

         if(x - 1 >= 0)
        if(origine.B == mat[x - 1][y].B && origine.G == mat[x - 1][y].G && origine.R == mat[x - 1][y].R)
           
            fill(fh, ih, mat, y, x - 1, color);

        if(origine.B == mat[x][y + 1].B && origine.G == mat[x][y + 1].G && origine.R == mat[x][y + 1].R)
            
            fill(fh, ih, mat, y + 1, x, color);
        
         if(y - 1 >= 0)
        if(origine.B == mat[x][y - 1].B && origine.G == mat[x][y - 1].G && origine.R == mat[x][y - 1].R)
    
            fill(fh, ih, mat, y - 1, x, color);

        }



}


int main(){

    //Citire instructiune
    bmp_fileheader *fh = malloc(1 * sizeof(bmp_fileheader));
    bmp_infoheader *ih = malloc(1 * sizeof(bmp_infoheader));;
    RGB **mat = malloc(65535 * sizeof(RGB *));
    
    for(int i = 0; i < 65535; i ++){

        mat[i] = malloc(65535 * sizeof(RGB));
        
    }

     RGB color;
     unsigned int lenght;

    char *input = malloc(1001 * sizeof(char));
    int l;

    while(fgets(input, 10001 * sizeof(input), stdin)){

        l = strlen(input);
       if(input[l - 1] == 10)
            input[l - 1] = 0;
         
      
        char *path = malloc(1001 * sizeof(char));
        path = strtok(input, " ");
        
        char *function = malloc(101 * sizeof(char));
        strcpy(function, path);
        path = strtok(NULL, " ");
        
        //Determinare instructiune
        
        if(strcmp(function, "insert") == 0){
            
           
            char *path_new = calloc(1001, sizeof(char));
                strcpy(path_new, path);
            
            path = strtok(NULL, " ");
            unsigned int y = nr_char_to_int(path);

            path = strtok(NULL, " ");
            unsigned int x = nr_char_to_int(path);

            insert(path_new, fh, ih, mat, y, x);

        }

        if(strcmp(function, "set") == 0){
            

           if(strcmp(path, "draw_color") == 0){

               

               path = strtok(NULL, " ");
               color.R = nr_char_to_int(path);

               path = strtok(NULL, " ");
               color.G = nr_char_to_int(path);

               path = strtok(NULL, " ");
               color.B = nr_char_to_int(path);

           }

           else{

               path = strtok(NULL, " ");
               lenght = nr_char_to_int(path);

           }
        
        }

       if(strcmp(function, "draw") == 0){

            unsigned y1, x1, y2, x2, y3, x3, w, h;

            if(strcmp(path, "line") == 0){

                path = strtok(NULL, " ");
                y1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               x1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               y2 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               x2 = nr_char_to_int(path);

                draw_line(fh, ih, mat, color, lenght, y1, x1, y2, x2);

            }

            if(strcmp(path, "rectangle") == 0){
                
                path = strtok(NULL, " ");
                y1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               x1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               w = nr_char_to_int(path);

               path = strtok(NULL, " ");
               h = nr_char_to_int(path);

                draw_line(fh, ih, mat, color, lenght, y1, x1, y1 + w, x1);
                draw_line(fh, ih, mat, color, lenght, y1, x1, y1 , x1 + h);
                draw_line(fh, ih, mat, color, lenght, y1, x1 + h, y1 + w , x1 + h);
                draw_line(fh, ih, mat, color, lenght, y1 + w, x1, y1 + w , x1 + h);

            }

            if(strcmp(path, "triangle") == 0){
                
                path = strtok(NULL, " ");
                y1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               x1 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               y2 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               x2 = nr_char_to_int(path);

               path = strtok(NULL, " ");
               y3 = nr_char_to_int(path);
 
               path = strtok(NULL, " ");
               x3 = nr_char_to_int(path);
 
                draw_line(fh, ih, mat, color, lenght, y1, x1, y2, x2);
                draw_line(fh, ih, mat, color, lenght, y1, x1, y3 , x3);
                draw_line(fh, ih, mat, color, lenght, y2, x2, y3, x3);

            }

        }

        if(strcmp(function, "fill") == 0){
            
            int x, y;
            y = nr_char_to_int(path);
 
            path = strtok(NULL, " ");
            x = nr_char_to_int(path);
           
            fill(fh, ih, mat, y, x, color);


        }
    
        if(strcmp(function, "edit") == 0)
            edit(path, fh, ih, mat);
           
        
        if(strcmp(function, "save") == 0)
            save(path, fh, ih, mat);

        if(strcmp(function, "quit") == 0){
              for(int i = 0; i < ih->height; i ++)
            free(mat[i]);
            free(fh);
            free(ih);
            free(mat);  
            free(function);
            free(input);
            free(path);
            exit(0);  


        }
            
        


    }
    

    return 0;
}
    

