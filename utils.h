//Copyright Marin Iulia 2022
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structura pixel
typedef struct {
	double *box;
} pixel;

// definim structura Imagine
typedef struct {
	int rows;
	int cols;
	unsigned char maxval;
	unsigned int type;
	unsigned int color;
	char magic_number[3];
	pixel **photo;
} image;

//citeste restul pana la \n pentru a nu incurca

void clean_up(void);

//aduce valorile in intervalul 0-255 pentru a respecta formatul pozelor

double clamp(double a, image b);

//verifiica existenta comentariilor in poze si trece peste cirirea lor

void check_comment(FILE *filename);

void read_photo(image *a, FILE *filename);

void read_photo_binary(image *a, FILE *filename);

void LOAD(char *filename, image *a, int *loaded);

void EXIT(int loaded, image *a);

void print_photo(image *a, FILE *filename);

void print_photo_binary(image *a, FILE *filename);

void SAVE(image *a, char *unde_salvam);

//verificam integritatea argumentelor

int invader(char *argument);

void SELECT(image a, int *x1, int *y1, int *x2, int *y2, int loaded);

pixel **aloc_matrice(int x, int y, int nr_pixeli);

void CROP(image *a, int x1, int y1, int x2, int y2);

//functie care roteste o bucata din poza

void rotate_baby_one_more_time(image *a, int *x1, int *y1, int *x2, int *y2);

//functie care roteste toata poza

void spin_me_right_round(image *a, int *x1, int *y1, int *x2, int *y2);

void ROTATE(image *a, int *x1, int *y1, int *x2, int *y2);

//verifica rama

void walking_on_thin_ice(int *x1, int *y1, int *x2, int *y2, image a);
//atribuie kernerurile specifice fiecarui filtru

void choose_your_fighter(int filtru, double kernel[][3][3]);

void cica_modularizez(pixel ***copie, image a, int x1, int y1, int x2, int y2,
					  double kernel[3][3]);

void APPLY(image *a, int x1, int y1, int x2, int y2);
