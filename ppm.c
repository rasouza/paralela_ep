#include "ppm.h"

PPMImage *readPPM(const char *filename) {
	char buff[16];
	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;
	//open PPM file for reading
	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

	//check the image format
	if (buff[0] != 'P' || buff[1] != '3') {
		fprintf(stderr, "Invalid image format (must be 'P3')\n");
		exit(1);
	}

	//alloc memory form image
	img = (PPMImage *)malloc(sizeof(PPMImage));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//check for comments
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n');
		c = getc(fp);
	}

	ungetc(c, fp);
	//read image size information
	if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}

	//read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
		exit(1);
	}

	//check rgb component depth
	if (rgb_comp_color != RGB_COMPONENT_COLOR) {
		fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n');
	//memory allocation for pixel data
	img->data = (PPMPixel**)malloc(img->y * sizeof(PPMPixel*));
	for (int i = 0; i < img->y; i++)
		img->data[i] = (PPMPixel*)malloc(img->x * sizeof(PPMPixel));

	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//read pixel data from file
	for (int i = 0; i < img->y; i++) {
		for (int j = 0; j < img->x; j++) {
			fscanf(fp, "%lf %lf %lf ", &img->data[i][j].red, &img->data[i][j].green, &img->data[i][j].blue);

			// Normaliza as cores entre 0 e 1
			img->data[i][j].red /= 255.0;
			img->data[i][j].green /= 255.0;
			img->data[i][j].blue /= 255.0;
		}
	}


	fclose(fp);
	return img;
}

void writePPM(const char *filename, PPMImage *img) {
	FILE *fp;
	//open file for output
	fp = fopen(filename, "w");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//write the header file
	//image format
	fprintf(fp, "P3\n");


	//image size
	fprintf(fp, "%d %d\n", img->x, img->y);

	// rgb component depth
	fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

	// pixel data
	//fwrite(img->data, 3 * img->x, img->y, fp);
	for (int i = 0; i < img->y; i++) {
		for (int j = 0; j < img->x; j++) {
			fprintf(
				fp, 
				"%d %d %d ", 
				(int)(img->data[i][j].red*255), 
				(int)(img->data[i][j].green*255), 
				(int)(img->data[i][j].blue*255)
			);
		}
	}
	fclose(fp);
}

//void changeColorPPM(PPMImage *img)
//{
//  int i;
//  if (img) {
//
//      for (i = 0; i<img->x*img->y; i++) {
//          img->data[i].red = RGB_COMPONENT_COLOR - img->data[i].red;
//          img->data[i].green = RGB_COMPONENT_COLOR - img->data[i].green;
//          img->data[i].blue = RGB_COMPONENT_COLOR - img->data[i].blue;
//      }
//  }
//}
