/*Bulgaru Alexandra - 312CD*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _tema_2_h_
#define _tema_2_h_

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} ImagePixel;

typedef struct node {
    int x;  // coordonatele (x, y) pentru zonele pozei
    int y;
    int size;
    int level;  // nivelul la care se afla un nod
    unsigned char node_type;
    ImagePixel color;
    struct node* child1;
    struct node* child2;
    struct node* child3;
    struct node* child4;
} Quadtree;

typedef struct q_node {
    Quadtree* data;
    struct q_node* next;
} QNode;

typedef struct queue {
    QNode* front;
    QNode* rear;
    long size;
} Queue;

QNode* create(Quadtree* data) {
    QNode* node = malloc(sizeof(struct q_node));
    node->data = data;
    node->next = NULL;
    return node;
}

Queue* initQueue() {
    Queue* q = malloc(sizeof(struct queue));
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

int isEmptyQueue(Queue* q) {
    return q == NULL || q->front == NULL || q->size == 0;
}

void enqueue(Queue *q, Quadtree* data) {
    QNode *end;
    if (isEmptyQueue(q)) {
        q->rear = q->front = create(data);
        q->size++;
        return;
    }
    end = create(data);
    q->rear->next = end;
    q->rear = end;
    q->size++;
    return;
}

Quadtree* dequeue(Queue* q) {
    QNode *tmp;
    if (isEmptyQueue(q)) {
        return NULL;
    }
    tmp = q->front;
    Quadtree* node = tmp->data;
    q->front = q->front->next;
    q->size--;
    free(tmp);
    return node;
}

void freeQueue(Queue* q) {
    while (q->front != NULL) {
        QNode *tmp = q->front;
        q->front = q->front->next;
        free(tmp);
    }
    free(q);
}

void divisions(ImagePixel** img, Quadtree** root, int x, int y, int size, int factor) {
    Quadtree *node = malloc(sizeof(struct node));  // initializez un nod de arbore
    node->x = x;
    node->y = y;
    node->size = size;
    node->child1 = NULL;
    node->child2 = NULL;
    node->child3 = NULL;
    node->child4 = NULL;
    int i = 0, j = 0;
    unsigned long long red = 0, green = 0, blue = 0, mean = 0;
    unsigned long long sum_red = 0, sum_green = 0, sum_blue = 0;
    unsigned long long r = 0, g  = 0, b = 0, total_sum = 0;  // r = red - img[i][j].red
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {  // parcurg zonele in care este impartita imaginea si calculez sumele
            sum_red += img[i][j].red;
            sum_green += img[i][j].green;
            sum_blue += img[i][j].blue;
        }
    }
    red = sum_red / (size * size);  // media aritmetica pentru rosu
    green = sum_green/ (size * size);  // media aritmetica pentru verde
    blue = sum_blue / (size * size);  // media aritmetica pentru albastru
    node->color.red = red;  // atribui valoarea obtinuta nodului corespunzator acelei zone
    node->color.green = green;
    node->color.blue = blue;
    *root = node;  // update astfel incat root sa pointeze catre noul nod
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            r = red - img[i][j].red;
            g = green - img[i][j].green;
            b = blue - img[i][j].blue;
            total_sum = total_sum + r * r + g * g + b * b;
        }
    }
    mean = total_sum / (3 * size * size);
    if (mean > factor) {
        node->node_type = 0;  // nod parinte, care se divide mai departe in 4 frunze, fiecare de dimensiune size / 2
        divisions(img, &node->child1, x, y, size / 2, factor);  // se reapeleaza functia pentru fiecare nod
        divisions(img, &node->child2, x, y + size / 2, size / 2, factor);
        divisions(img, &node->child3, x + size / 2, y  + size / 2, size / 2, factor);
        divisions(img, &node->child4, x + size / 2, y, size / 2, factor);
    } else {
        node->node_type = 1;  // nod frunza, nu se mai divide
        return;
    }
}

int heightTree(Quadtree* tree) {
    if (tree == NULL) {
        return 0;
    } else {
        int child1_height = heightTree(tree->child1);
        int child2_height = heightTree(tree->child2);
        int child3_height = heightTree(tree->child3);
        int child4_height = heightTree(tree->child4);
        int height = child1_height;  // iau o variabila careia ii atribui inaltimea unui copil si apoi compar
        if (child2_height > height) {
            height = child2_height;
        }
        if (child3_height > height) {
            height = child3_height;
        }
        if (child4_height > height) {
            height = child4_height;
        }
        return height + 1;
    }
}

int no_leaves(Quadtree* tree) {  // nr. de blocuri (frunze) pentru care scorul similaritatii este mai mic sau egal decat pragul impus
    if (tree == NULL) {
        return 0;
    } else if (tree->child1 == NULL && tree->child2 == NULL && tree->child3 == NULL && tree->child4 == NULL) {
        return 1;
    } else {
        int child1_leaves = no_leaves(tree->child1);
        int child2_leaves = no_leaves(tree->child2);
        int child3_leaves = no_leaves(tree->child3);
        int child4_leaves = no_leaves(tree->child4);
        return child1_leaves + child2_leaves + child3_leaves + child4_leaves;
    }
}

int largest_square(Quadtree* tree) {  // acelasi principiu ca la height
    if (tree == NULL) {
        return 0;
    } else {
        int child1_square = largest_square(tree->child1);
        int child2_square = largest_square(tree->child2);
        int child3_square = largest_square(tree->child3);
        int child4_square = largest_square(tree->child4);
        int max_square = child1_square;
        if (child2_square > max_square) {
            max_square = child2_square;
        }
        if (child3_square > max_square) {
            max_square = child3_square;
        }
        if (child4_square > max_square) {
            max_square = child4_square;
        }
        if (max_square == 0) {
            return tree->size;
        } else {
            return max_square;
        }
    }
}

void write_info(Quadtree* tree, FILE *out, int level) {
    if (tree == NULL) {
        return;
    }
    if (level == 0) {
        fwrite(&tree->node_type, sizeof(unsigned char), 1, out);
        if (tree->node_type == 1) {
            fwrite(&tree->color.red, sizeof(unsigned char), 1, out);
            fwrite(&tree->color.green, sizeof(unsigned char), 1, out);
            fwrite(&tree->color.blue, sizeof(unsigned char), 1, out);
        }
        return;
    }
    write_info(tree->child1, out, level - 1);  // scriu toate nodurile de pe un anumit nivel
    write_info(tree->child2, out, level - 1);
    write_info(tree->child3, out, level - 1);
    write_info(tree->child4, out, level - 1);
}

void write_info_order(Quadtree* tree, FILE *out) {  // scriu in ordine nodurile
    int i = 0, h = heightTree(tree);
    for (i = 0; i <= h; i++) {
        write_info(tree, out, i);
    }
}

Quadtree* create_tree(FILE *in, int x, int y, int size, int level) {  // citesc direct din fisierul binar creat si adaug in copac
    Quadtree* node = malloc(sizeof(struct node));  // initializez primul nod din arbore
    node->x = x;
    node->y = y;
    node->child1 = NULL;
    node->child2 = NULL;
    node->child3 = NULL;
    node->child4 = NULL;
    Queue *q = initQueue();  // ne folosim de o coada, deoarece datele din fisierul binar sunt pe nivel (breadth-first)
    enqueue(q, node);
    while (!isEmptyQueue(q)) {
        Quadtree *root = dequeue(q);  // scot primul element din coada si il atribui lui root pentru a crea copiii lui node
        fread(&root->node_type, sizeof(unsigned char), 1, in);
        if (root->node_type == 1) {
            fread(&root->color.red, sizeof(unsigned char), 1, in);
            fread(&root->color.green, sizeof(unsigned char), 1, in);
            fread(&root->color.blue, sizeof(unsigned char), 1, in);
        } else {  // incep sa creez nodurile copil cu informatiile corespunzatoare
            if (root->child1 == NULL) {
                root->child1 = malloc(sizeof(struct node));
                root->child1->x = root->x;
                root->child1->y = root->y;
                root->child1->level = root->level + 1;
                root->child1->child1 = NULL;
                root->child1->child2 = NULL;
                root->child1->child3 = NULL;
                root->child1->child4 = NULL;
                enqueue(q, root->child1);
            }
            if (root->child2 == NULL) {
                root->child2 = malloc(sizeof(struct node));
                root->child2->x = root->x;
                root->child2->y = root->y + size / 2;
                root->child2->level = root->level + 1;
                root->child2->child1 = NULL;
                root->child2->child2 = NULL;
                root->child2->child3 = NULL;
                root->child2->child4 = NULL;
                enqueue(q, root->child2);
            }
            if (root->child3 == NULL) {
                root->child3 = malloc(sizeof(struct node));
                root->child3->x = root->x + size / 2;
                root->child3->y = root->y;
                root->child3->level = root->level + 1;
                root->child3->child1 = NULL;
                root->child3->child2 = NULL;
                root->child3->child3 = NULL;
                root->child3->child4 = NULL;
                enqueue(q, root->child3);
            }
            if (root->child4 == NULL) {
                root->child4 = malloc(sizeof(struct node));
                root->child4->x = root->x + size / 2;
                root->child4->y = root->y + size / 2;
                root->child4->level = root->level + 1;
                root->child4->child1 = NULL;
                root->child4->child2 = NULL;
                root->child4->child3 = NULL;
                root->child4->child4 = NULL;
                enqueue(q, root->child4);
            }
        }
    }
    freeQueue(q);  // eliberez memoria pentru coada
    return node;
}

void decompression(Quadtree* root, ImagePixel** img, int x, int y, int size) {  // procesul invers de la divisions
    int i = 0, j = 0;
    if (root->child1 == NULL && root->child2 == NULL && root->child3 == NULL && root->child4 == NULL) {
        for (i = x; i < x + size; i++) {
            for (j = y; j < y + size; j++) {
                img[i][j].red = root->color.red;  // iau valorile din arbore si le atribui imaginii
                img[i][j].green = root->color.green;
                img[i][j].blue = root->color.blue;
            }
        }
    } else {
        decompression(root->child1, img, x, y, size / 2);
        decompression(root->child2, img, x, y + size / 2, size / 2);
        decompression(root->child3, img, x + size / 2, y + size / 2, size / 2);
        decompression(root->child4, img, x + size / 2, y, size / 2);
    }
}

void ppm(FILE *out, ImagePixel** img, int size) {  // scriu informatiile din ImagePixel** in fisierul *.ppm
    int i = 0, j = 0;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fwrite(&img[i][j].red, sizeof(unsigned char), 1, out);
            fwrite(&img[i][j].green, sizeof(unsigned char), 1, out);
            fwrite(&img[i][j].blue, sizeof(unsigned char), 1, out);
        }
    }
}

void free_qtree(Quadtree* tree) {
    if (tree == NULL) {
        return;
    }
    free_qtree(tree->child1);
    free_qtree(tree->child2);
    free_qtree(tree->child3);
    free_qtree(tree->child4);
    free(tree);
}

#endif
