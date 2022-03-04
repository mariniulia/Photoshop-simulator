//Copyright Marin Iulia 2022
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"
#define MAXEDIT 15
#define MAXNAME 100

int main(void)
{
	char filename[MAXNAME], comanda[MAXEDIT];
	int loaded = 0;
	image a;
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	scanf("%s", comanda);
	while (strcmp(comanda, "EXIT") != 0) {
		if (strcmp(comanda, "LOAD") == 0) {
			scanf("%s", filename);
			if (loaded == 1) {
				free_the_prisoner(&a);
				LOAD(filename, &a, &loaded);
				x1 = 0;
				y1 = 0;
				y2 = a.rows;
				x2 = a.cols;
			} else {
				LOAD(filename, &a, &loaded);
				x1 = 0;
				y1 = 0;
				y2 = a.rows;
				x2 = a.cols;
			}
		} else if (strcmp(comanda, "SELECT") == 0) {
			if (loaded == 0) {
				clean_up();
				fprintf(stdout, "No image loaded\n");
			} else {
				SELECT(a, &x1, &y1, &x2, &y2, loaded);
			}
		} else if (strcmp(comanda, "ROTATE") == 0) {
			if (loaded == 0) {
				clean_up();
				fprintf(stdout, "No image loaded\n");
			} else {
				ROTATE(&a, &x1, &y1, &x2, &y2);
			}
		} else if (strcmp(comanda, "SAVE") == 0) {
			if (loaded == 0) {
				clean_up();
				fprintf(stdout, "No image loaded\n");
			} else {
				scanf("%s", filename);
				SAVE(&a, filename);
			}
		} else if (strcmp(comanda, "CROP") == 0) {
			if (loaded == 0) {
				clean_up();
				fprintf(stdout, "No image loaded\n");
			} else {
				CROP(&a, x1, y1, x2, y2);
				x1 = 0;
				y1 = 0;
				y2 = a.rows;
				x2 = a.cols;
				fprintf(stdout, "Image cropped\n");
			}
		} else if (strcmp(comanda, "APPLY") == 0) {
			if (loaded == 0) {
				clean_up();
				fprintf(stdout, "No image loaded\n");
			} else {
				APPLY(&a, x1, y1, x2, y2);
			}
		} else {
			clean_up();
			fprintf(stdout, "Invalid command\n");
		}

		scanf("%s", comanda);
	}
	EXIT(loaded, &a);
	return 0;
}
