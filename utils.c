//Copyright Marin Iulia 2022
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

//citeste restul pana la \n pentru a nu incurca
void clean_up(void)
{
	char curatam_in_urma_nu_lasam;
	do {
		fscanf(stdin, "%c", &curatam_in_urma_nu_lasam);
	} while (curatam_in_urma_nu_lasam != '\n');
}

//aduce valorile in intervalul 0-255 pentru a respecta formatul pozelor

double clamp(double a, image b)
{
	if (a < 0)
		return 0;
	if (a > b.maxval)
		return b.maxval;
	return a;
}

//verifiica existenta comentariilor in poze si trece peste cirirea lor

void check_comment(FILE *filename)
{
	char gotcha;
	fscanf(filename, "%c", &gotcha);
	if (gotcha == '#')
		clean_up();
	else
		fseek(filename, -1, SEEK_CUR);
}

void read_photo(image *a, FILE *filename)
{
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	unsigned char imprumut[nr_pixels];
	// aloc spatiu pentru linii
	a->photo = (pixel **)malloc(a->rows * sizeof(pixel *));
	if (!a->photo) {
		fprintf(stderr, "malloc() failed\n");
		return;
	}
	// aloc pt fiecare linie,nr de coloane
	for (int i = 0; i < a->rows; i++) {
		a->photo[i] = malloc(a->cols * sizeof(pixel));

		if (!a->photo[i]) {
			fprintf(stderr, "malloc() failed\n");

			//elibereaza memeoria alocata pe liniile anterioarr
			for (int j = 0; j < i; j++)
				free(a->photo[j]);
			free(a->photo);
			return;
		}

		int culori_necesare = 1;
		if (a->color == 2)
			culori_necesare = 3;
		for (int j = 0; j < a->cols; j++) {
			//aloc nr de casute necesare pentru scrierea unui pixel
			a->photo[i][j].box = (double *)malloc(culori_necesare
			* sizeof(double));
			for (int k = 0; k < nr_pixels; k++) {
				//le citim intr-un vector temporar pentru a face conversia mai
				//usor de la char la double
				fscanf(filename, "%hhu", &imprumut[k]);
				a->photo[i][j].box[k] = (double)imprumut[k];
			}
		}
	}
}

void read_photo_binary(image *a, FILE *filename)
{
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	unsigned char imprumut[nr_pixels];

	//alocam spatiu pentru linii

	a->photo = (pixel **)malloc(a->rows * sizeof(pixel *));

	//verificam daca imaginea poate fi citita

	if (!a->photo) {
		fprintf(stderr, "malloc() failed\n");
		return;
	}

	// alocam fiecare linie
	for (int i = 0; i < a->rows; i++) {
		a->photo[i] = malloc(a->cols * sizeof(pixel));

		if (!a->photo[i]) {
			fprintf(stderr, "malloc() failed\n");

			// eliberam memoria alocata pe liniile precedente

			for (int j = 0; j < i; j++)
				free(a->photo[j]);

			free(a);

			return;
		}

		for (int j = 0; j < a->cols; j++) {
			a->photo[i][j].box = (double *)malloc(nr_pixels * sizeof(double));

			//le citim intr-un vector temporar pentru a face conversia mai usor
			//de la char la double

			fread(imprumut, sizeof(unsigned char), nr_pixels, filename);
			for (int k = 0; k < nr_pixels; k++)
				a->photo[i][j].box[k] = (double)imprumut[k];
		}
	}
}

void LOAD(char *filename, image *a, int *loaded)
{ //deschidem fisierul
	FILE *in = fopen(filename, "rb");
	// verificam deschiderea fisierului
	if (!in) {
		fprintf(stdout, "Failed to load %s\n", filename);
		*loaded = 0;
		return;
	}
	check_comment(in);
	fscanf(in, "%s", a->magic_number);
	check_comment(in);
	fscanf(in, "%d%d", &a->cols, &a->rows);
	check_comment(in);
	fscanf(in, "%hhu", &a->maxval);
	check_comment(in);
	// le aflam culoarea

	if (strcmp(a->magic_number, "P2") == 0 ||
		strcmp(a->magic_number, "P5") == 0)
		a->color = 1; // gri;
	if (strcmp(a->magic_number, "P3") == 0 ||
		strcmp(a->magic_number, "P6") == 0)
		a->color = 2; // color

	//le aflam tipul
	if (strcmp(a->magic_number, "P2") == 0 ||
		strcmp(a->magic_number, "P3") == 0) {
		a->type = 0; // ASCII
	}
	if (strcmp(a->magic_number, "P5") == 0 ||
		strcmp(a->magic_number, "P6") == 0) {
		a->type = 1; // BINAR
	}

	if (a->type == 1) { //citim binar
		if (!in) {
			printf("Failed to load %s\n", filename);
			return;
		}
		// ducem curosrul la matrice
		fseek(in, 1, SEEK_CUR);
		read_photo_binary(a, in);
		fprintf(stdout, "Loaded %s\n", filename);
		*loaded = 1;
	} else if (a->type == 0) {
		//citim ascii
		read_photo(a, in);
		fprintf(stdout, "Loaded %s\n", filename);
		*loaded = 1;
	} else {
		printf("Cannot load %s", filename);
		*loaded = 0;
	}
	//inchidem fisierul
	fclose(in);
}

void EXIT(int loaded, image *a)
{
	//daca nu e incarcata o poza
	if (loaded == 0) {
		fprintf(stdout, "No image loaded\n");
		return;
	}
		//eliberam memoria
		for (int i = 0; i < a->rows; i++) {
			for (int j = 0; j < a->cols; j++)
				free(a->photo[i][j].box);
			free(a->photo[i]);
		}
		free(a->photo);
}

void print_photo(image *a, FILE *filename)
{
	//alfam numarul de pixeli
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	for (int i = 0; i < a->rows; i++) {
		for (int j = 0; j < a->cols; j++) {
			for (int k = 0; k < nr_pixels; k++) {
				//facem conveersia inapoi la char
				char haidee = round(a->photo[i][j].box[k]);
				fprintf(filename, "%hhu ", haidee);
			}
		}
		fprintf(filename, "\n");
	}
}

void print_photo_binary(image *a, FILE *filename)
{
	//alfam numarul de pixeli
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	for (int i = 0; i < a->rows; i++) {
		for (int j = 0; j < a->cols; j++) {
			for (int k = 0; k < nr_pixels; k++) {
				//facem trecerea inapoi la char
				char haidee = round(a->photo[i][j].box[k]);
				fwrite(&haidee, sizeof(char), 1, filename);
			}
		}
	}
}

void free_the_prisoner(image *a)
{
	for (int i = 0; i < a->rows; i++) {
		for (int j = 0; j < a->cols; j++)
			free(a->photo[i][j].box);
		free(a->photo[i]);
	}
	free(a->photo);
}

void SAVE(image *a, char *unde_salvam)
{
	char mofturi[10];
	fgets(mofturi, 10, stdin);
	FILE *filename = fopen(unde_salvam, "wb");
	if ((strcmp(mofturi, "\n") == 0) ||
		((mofturi[0] == ' ') && (mofturi[1] != 'a'))) {
		//schimbam tipul si magic word ca sa facem citirea bine binar
		a->type = 1;
		if (a->color == 1)
			fprintf(filename, "P5\n");
		else
			fprintf(filename, "P6\n");
		fprintf(filename, "%d %d\n", a->cols, a->rows);
		fprintf(filename, "%d\n", a->maxval);
		print_photo_binary(a, filename);
	} else { //schimbam tipul si magic word ca sa facem citirea bine ascii
		a->type = 0;
		if (a->color == 1)
			fprintf(filename, "P2\n");
		else
			fprintf(filename, "P3\n");
		fprintf(filename, "%d %d\n", a->cols, a->rows);
		fprintf(filename, "%d\n", a->maxval);
		print_photo(a, filename);
	}
	fclose(filename);
	fprintf(stdout, "Saved %s\n", unde_salvam);
}

//verificamintegritatea argumentelor

int invader(char *argument)
{
	char *cuvant = strtok(argument, "\n ");
	int nr_arg = 0;

	while (cuvant) {
		if (strcmp(cuvant, "ALL") == 0) {
			return 1;
			//ALL
		} else if ((cuvant[0] >= 'a' && cuvant[0] <= 'z') ||
		(cuvant[0] >= 'A' && cuvant[0] <= 'Z')) {
			return -1;
			//comanda invalida
		} else if (cuvant[0] == '-') {
			return -2;
			//invalid set
		} else {
			nr_arg++;
		}
		cuvant = strtok(NULL, "\n ");
	}
	if (nr_arg < 4)
		return -1;
	//daca sunt toate ok
	return 0;
}

void SELECT(image a, int *x1, int *y1, int *x2, int *y2, int loaded)
{
	int x1tmp, x2tmp, y1tmp, y2tmp, imprumut;
	char *coordonata, argument[30], aux[30];
	//citim toata linia
	fgets(argument, 30, stdin);
	//strtok distruge stringul asa ca facem o copie
	strcpy(aux, argument);

	if (loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int is_it_or_is_it_not = invader(argument);
	if (is_it_or_is_it_not == -2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (is_it_or_is_it_not == -1) {
		printf("Invalid command\n");
		return;
	}
	if (is_it_or_is_it_not == 1) {
		*x1 = 0;
		*y1 = 0;
		*y2 = a.rows;
		*x2 = a.cols;
		printf("Selected ALL\n");
		return;
	}
	coordonata = strtok(aux, " ");
	x1tmp = atoi(coordonata);
	coordonata = strtok(NULL, " ");
	y1tmp = atoi(coordonata);
	coordonata = strtok(NULL, " ");
	x2tmp = atoi(coordonata);
	coordonata = strtok(NULL, " ");
	y2tmp = atoi(coordonata);
	//le asezam in ordine crescatoare
	if (x1tmp > x2tmp) {
		imprumut = x1tmp;
		x1tmp = x2tmp;
		x2tmp = imprumut;
	}
	if (y1tmp > y2tmp) {
		imprumut = y1tmp;
		y1tmp = y2tmp;
		y2tmp = imprumut;
	}
	//cateva conditii matemetice pentru coordonate a.i. sa fie valide
	if (y2tmp > a.rows || x2tmp > a.cols || x2tmp == 0 || y2tmp == 0) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x2tmp - x1tmp == 0 || y2tmp - y1tmp == 0) {
		printf("Invalid set of coordinates\n");
		return;
	}

	//abia dupa ce suntem siguri ca sunt valide,le inlocuim
	*x1 = x1tmp;
	*x2 = x2tmp;
	*y1 = y1tmp;
	*y2 = y2tmp;

	printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
}

pixel **aloc_matrice(int x, int y, int nr_pixeli)
{
	pixel **nou = (pixel **)malloc(x * sizeof(pixel *));

	// verificam daca poate fi citita
	if (!nou) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}

	// aloc fiecare linie
	for (int i = 0; i < x; i++) {
		nou[i] = malloc(y * sizeof(pixel));

		if (!nou[i]) {
			fprintf(stderr, "malloc() failed\n");
			// eliberam liniile alocate pana in acest moment
			for (int j = 0; j < i; j++)
				free(nou[j]);
			free(nou);

			return NULL;
		}

		for (int j = 0; j < y; j++) { //alocam nr de elem necesare
			nou[i][j].box = (double *)malloc(nr_pixeli * sizeof(double));
		}
	}
	return nou;
}

void CROP(image *a, int x1, int y1, int x2, int y2)
{
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	//aloca o matrice de dimensiunile noi
	pixel **nou = aloc_matrice(y2 - y1, x2 - x1, nr_pixels);

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < nr_pixels; k++)
				//completam copia cu elementele din cadrul selectiei
				//din matricea veche
				nou[i - y1][j - x1].box[k] = a->photo[i][j].box[k];
	//eliberam ce era in a
	free_the_prisoner(a);
	//legam noua matrice de a
	a->photo = nou;
	a->rows = y2 - y1;
	a->cols = x2 - x1;
}

//functie care roteste o bucata din poza

void rotate_baby_one_more_time(image *a, int *x1, int *y1, int *x2, int *y2)
{
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	pixel **nou = aloc_matrice(*y2 - *y1, *x2 - *x1, nr_pixels);
	for (int i = *y1; i < *y2; i++) {
		for (int j = *x1; j < *x2; j++) {
			for (int k = 0; k < nr_pixels; k++)
				nou[i - *y1][j - *x1].box[k] = a->photo[i][j].box[k];
		}
	}
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			for (int k = 0; k < nr_pixels; k++)
				a->photo[i][j].box[k] = nou[*x2 - j - 1][i - *y1].box[k];
	for (int i = *y1; i < *y2; i++) {
		for (int j = *x1; j < *x2; j++)
			free(nou[i - *y1][j - *x1].box);
		free(nou[i - *y1]);
	}
	free(nou);
}

//functie care roteste toata poza

void spin_me_right_round(image *a, int *x1, int *y1, int *x2, int *y2)
{
	// printf("LIKE A ROCKET BABY RIGHT ROUND");
	int nr_pixels = 1;
	if (a->color == 2)
		nr_pixels = 3;
	pixel **nou = aloc_matrice(a->cols, a->rows, nr_pixels);

	for (int i = 0; i < a->cols; i++) {
		for (int j = 0; j < a->rows; j++) {
			for (int k = 0; k < nr_pixels; k++)
				nou[i][j].box[k] = a->photo[j][a->cols - i - 1].box[k];
		}
	}
	//eliberam ce era in a
	free_the_prisoner(a);
	//legam noua matrice de a
	a->photo = nou;
	//inversam dimensiunile
	int aut = a->rows;
	a->rows = a->cols;
	a->cols = aut;
	//facem select pentru toata poza
	*x1 = 0;
	*y1 = 0;
	*x2 = a->cols;
	*y2 = a->rows;
}

void ROTATE(image *a, int *x1, int *y1, int *x2, int *y2)
{
	int unghi;
	int nr_rotiri;
	scanf("%d", &unghi);
	if (unghi == 0) {
		fprintf(stdout, "Rotated %d\n", unghi);
		return;
	}
	if (unghi % 90 != 0) {
		fprintf(stdout, "Unsupported rotation angle\n");
		return;
	}
	if ((*x2 - *x1) != (*y2 - *y1)) {
		int full = 0;
		if ((*x2 - *x1) == a->cols && (*y2 - *y1) == a->rows)
			full = 1;
		if (full == 0) {
			fprintf(stdout, "The selection must be square\n");
			return;
		}

		if (unghi == 360 || unghi == -360)
			nr_rotiri = 0;
		if (unghi == 180 || unghi == -180)
			nr_rotiri = 2;
		if (unghi == 90 || unghi == -270)
			nr_rotiri = 3;
		if (unghi == 270 || unghi == -90)
			nr_rotiri = 1;
		for (int i = 0; i < nr_rotiri; i++)
			spin_me_right_round(a, x1, y1, x2, y2);

		fprintf(stdout, "Rotated %d\n", unghi);
		return;
	} //calculam de cate ori trebuie sa rotim in functie de unghiuri
	if (unghi == 360 || unghi == -360)
		nr_rotiri = 0;
	if (unghi == 180 || unghi == -180)
		nr_rotiri = 2;
	if (unghi == 90 || unghi == -270)
		nr_rotiri = 1;
	if (unghi == 270 || unghi == -90)
		nr_rotiri = 3;
	for (int i = 0; i < nr_rotiri; i++)
		rotate_baby_one_more_time(a, x1, y1, x2, y2);

	fprintf(stdout, "Rotated %d\n", unghi);
}

//verifica rama

void walking_on_thin_ice(int *x1, int *y1, int *x2, int *y2, image a)
{
	// in cazul in care rama e continuta in select,le modificam valoarea a.i.
	//sa nu intre in for -> nu li se aplica filtrul
	if (*y1 == 0)
		*y1 = *y1 + 1;

	if (*x1 == 0)
		*x1 = *x1 + 1;

	if (*y2 == a.rows)
		*y2 = *y2 - 1;

	if (*x2 == a.cols)
		*x2 = *x2 - 1;
}

//atribuie kernerurile specifice fiecarui filtru

void choose_your_fighter(int filtru, double kernel[][3][3])
{
	if (filtru == 1) {
		(*kernel)[0][0] = -1;
		(*kernel)[0][1] = -1;
		(*kernel)[0][2] = -1;
		(*kernel)[1][0] = -1;
		(*kernel)[1][1] = 8;
		(*kernel)[1][2] = -1;
		(*kernel)[2][0] = -1;
		(*kernel)[2][1] = -1;
		(*kernel)[2][2] = -1;
	}
	if (filtru == 2) {
		(*kernel)[0][0] = 0;
		(*kernel)[0][1] = -1;
		(*kernel)[0][2] = 0;
		(*kernel)[1][0] = -1;
		(*kernel)[1][1] = 5;
		(*kernel)[1][2] = -1;
		(*kernel)[2][0] = 0;
		(*kernel)[2][1] = -1;
		(*kernel)[2][2] = 0;
	}
	if (filtru == 3) {
		(*kernel)[0][0] = (double)1 / 9;
		(*kernel)[0][1] = (double)1 / 9;
		(*kernel)[0][2] = (double)1 / 9;
		(*kernel)[1][0] = (double)1 / 9;
		(*kernel)[1][1] = (double)1 / 9;
		(*kernel)[1][2] = (double)1 / 9;
		(*kernel)[2][0] = (double)1 / 9;
		(*kernel)[2][1] = (double)1 / 9;
		(*kernel)[2][2] = (double)1 / 9;
	}
	if (filtru == 4) {
		(*kernel)[0][0] = (double)1 / 16;
		(*kernel)[0][1] = (double)2 / 16;
		(*kernel)[0][2] = (double)1 / 16;
		(*kernel)[1][0] = (double)2 / 16;
		(*kernel)[1][1] = (double)4 / 16;
		(*kernel)[1][2] = (double)2 / 16;
		(*kernel)[2][0] = (double)1 / 16;
		(*kernel)[2][1] = (double)2 / 16;
		(*kernel)[2][2] = (double)1 / 16;
	}
}

void cica_modularizez(pixel ***copie, image a, int x1, int y1, int x2, int y2,
					  double kernel[3][3])
{
	double mutant = 0;
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < 3; k++) {
				//calculam noua valorea a pixelului in functie de kernel
				for (int r = 0; r < 3; r++)
					for (int l = 0; l < 3; l++)
						mutant += kernel[r][l] *
								  (a.photo[i - 1 + r][j - 1 + l].box[k]);
				//adaugam valorile in copie
				(*copie)[i][j].box[k] = clamp(mutant, a);
				mutant = 0;
			}
}

void APPLY(image *a, int x1, int y1, int x2, int y2)
{
	double kernel[3][3];
	char mofturi[17];

	//citim parametrul
	fgets(mofturi, 17, stdin);

	if (strcmp(mofturi, "\n") == 0) {
		printf("Invalid command\n");
		return;
	}
	mofturi[strlen(mofturi) - 1] = '\0';

	//facem o copie a matricei initiale ca sa putem aplica kernel-ul
	//fara sa se ameste valorile

	pixel **copie = aloc_matrice(a->rows, a->cols, 3);
	for (int i = 0; i < a->rows; i++)
		for (int j = 0; j < a->cols; j++)
			for (int k = 0; k < 3; k++)
				copie[i][j].box[k] = a->photo[i][j].box[k];

	if (strcmp(mofturi, " EDGE") == 0 && a->color == 2) {
		//verificam daca selectul contine si rama
		walking_on_thin_ice(&x1, &y1, &x2, &y2, *a);
		//modificam kernel-ul cu cel specific
		choose_your_fighter(1, &kernel);
		//aplicam filtrele
		cica_modularizez(&copie, *a, x1, y1, x2, y2, kernel);
	} else if (strcmp(mofturi, " SHARPEN") == 0 && a->color == 2) {
		walking_on_thin_ice(&x1, &y1, &x2, &y2, *a);
		choose_your_fighter(2, &kernel);
		cica_modularizez(&copie, *a, x1, y1, x2, y2, kernel);
	} else if (strcmp(mofturi, " BLUR") == 0 && a->color == 2) {
		walking_on_thin_ice(&x1, &y1, &x2, &y2, *a);
		choose_your_fighter(3, &kernel);
		cica_modularizez(&copie, *a, x1, y1, x2, y2, kernel);
	} else if (strcmp(mofturi, " GAUSSIAN_BLUR") == 0 && a->color == 2) {
		walking_on_thin_ice(&x1, &y1, &x2, &y2, *a);
		choose_your_fighter(4, &kernel);
		cica_modularizez(&copie, *a, x1, y1, x2, y2, kernel);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
	//eliberam ce era in a
	free_the_prisoner(a);

	//asociem noua matrice imaginii
	a->photo = copie;

	printf("APPLY%s done\n", mofturi);
}
