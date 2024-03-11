/*Bulgaru Alexandra - 312CD*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image-comp.h"

int main(int argc, char const *argv[]) {
    if (argc == 5) {
        FILE *in = fopen(argv[3], "r");
        int width = 0, height = 0, maxColor = 0;
        int i = 0, j = 0;
        fscanf(in, "P6 %d %d %d", &width, &height, &maxColor);
        fseek(in, 1, SEEK_CUR);  // mut cursorul pentru citirea binara
        ImagePixel **matrix = malloc(height * sizeof(ImagePixel *));  // aloc memorie pe linii
        for (i = 0; i < height; i++) {
            matrix[i] = malloc(width * sizeof(ImagePixel));  // aloc memorie pe coloane
        }
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                fread(&(matrix[i][j].red), sizeof(unsigned char), 1, in);
                fread(&(matrix[i][j].green), sizeof(unsigned char), 1, in);
                fread(&(matrix[i][j].blue), sizeof(unsigned char), 1, in);
            }
        }
        fclose(in);
        int factor = atoi(argv[2]);
        Quadtree *root = NULL;
        divisions(matrix, &root, 0 , 0, width, factor);
        int no_levels = heightTree(root);  // nr. niveluri arbore cuaternar
        int no_divisions = no_leaves(root);
        int max = largest_square(root);
        if (strcmp(argv[1], "-c1") == 0) {
            FILE *out = fopen(argv[4], "wt");
            fprintf(out, "%d\n", no_levels);
            fprintf(out, "%d\n", no_divisions);
            fprintf(out, "%d\n", max);
            fclose(out);
        }
        if (strcmp(argv[1], "-c2") == 0) {
            FILE *out = fopen(argv[4], "wb");
            fwrite(&width, sizeof(int), 1, out);
            write_info_order(root, out);
            fclose(out);
        }
        for (i = 0; i < height; i++) {
            free(matrix[i]);
        }
        free(matrix);
        free_qtree(root);
    }
    if (argc == 4 && strcmp(argv[1], "-d") == 0) {
        FILE *in = fopen(argv[2], "rb");
        unsigned int size_img = 0;  // dimensiunea imaginii
        fread(&size_img, sizeof(unsigned int), 1, in);
        Quadtree* qtree = create_tree(in, 0, 0, size_img, 0);
        fclose(in);
        ImagePixel **remake = malloc(size_img * sizeof(ImagePixel *));
        for (int i = 0; i < size_img; i++) {
            remake[i] = malloc(size_img * sizeof(ImagePixel));
        }
        decompression(qtree, remake, 0, 0, size_img);
        FILE *out = fopen(argv[3], "w");
        fprintf(out, "P6\n");
        fprintf(out, "%d %d\n", size_img, size_img);
        fprintf(out, "255\n");
        ppm(out, remake, size_img);
        for (int i = 0; i < size_img; i++) {
            free(remake[i]);
        }
        free(remake);
        free_qtree(qtree);
        fclose(out);
    }
    return 0;
}
