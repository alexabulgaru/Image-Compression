#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _image_comp_h_
#define _image_comp_h_

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} ImagePixel;

typedef struct node {
    int x;  // (x, y) coordinates for image areas
    int y;
    int size;
    int level;  // the level at which a node is located
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
    Quadtree *node = malloc(sizeof(struct node));  // initialize a tree node
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
        for (j = y; j < y + size; j++) {
            sum_red += img[i][j].red;
            sum_green += img[i][j].green;
            sum_blue += img[i][j].blue;
        }
    }
    red = sum_red / (size * size);  // arithmetic mean for red
    green = sum_green/ (size * size);  // arithmetic mean for green
    blue = sum_blue / (size * size);  // arithmetic mean for blue
        node->color.red = red;  // assign the obtained value to the node corresponding to that area
        node->color.green = green;
        node->color.blue = blue;
        *root = node;  // update so that root points to the new node
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
            node->node_type = 0;  // parent node, which is further divided into 4 leaves, each of size size / 2
            divisions(img, &node->child1, x, y, size / 2, factor);  // the function is called again for each node
            divisions(img, &node->child2, x, y + size / 2, size / 2, factor);
            divisions(img, &node->child3, x + size / 2, y  + size / 2, size / 2, factor);
            divisions(img, &node->child4, x + size / 2, y, size / 2, factor);
        } else {
            node->node_type = 1;  // leaf node, does not divide further
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
        int height = child1_height;  // take a variable to which I assign the height of a child and then compare
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

int no_leaves(Quadtree* tree) {  // number of blocks (leaves) for which the similarity score is less than or equal to the imposed threshold
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

int largest_square(Quadtree* tree) {
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
    write_info(tree->child1, out, level - 1);  // recursive call for each child
    write_info(tree->child2, out, level - 1);
    write_info(tree->child3, out, level - 1);
    write_info(tree->child4, out, level - 1);
}

void write_info_order(Quadtree* tree, FILE *out) {
    int i = 0, h = heightTree(tree);
    for (i = 0; i <= h; i++) {
        write_info(tree, out, i);
    }
}

Quadtree* create_tree(FILE *in, int x, int y, int size, int level) {  // create the tree from the information in the binary file
    Quadtree* node = malloc(sizeof(struct node));
    node->x = x;
    node->y = y;
    node->child1 = NULL;
    node->child2 = NULL;
    node->child3 = NULL;
    node->child4 = NULL;
    Queue *q = initQueue();  // we use a queue because the data from the binary file is on level (breadth-first)
    enqueue(q, node);
    while (!isEmptyQueue(q)) {
        Quadtree *root = dequeue(q);  // remove the first element from the queue and assign it to root to create node's children
        fread(&root->node_type, sizeof(unsigned char), 1, in);
        if (root->node_type == 1) {
            fread(&root->color.red, sizeof(unsigned char), 1, in);
            fread(&root->color.green, sizeof(unsigned char), 1, in);
            fread(&root->color.blue, sizeof(unsigned char), 1, in);
        } else {  // start creating child nodes with corresponding information
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
    freeQueue(q);  // free memory for the queue
    return node;
}

void decompression(Quadtree* root, ImagePixel** img, int x, int y, int size) {  // the reverse process from divisions
    int i = 0, j = 0;
    if (root->child1 == NULL && root->child2 == NULL && root->child3 == NULL && root->child4 == NULL) {
        for (i = x; i < x + size; i++) {
            for (j = y; j < y + size; j++) {
                img[i][j].red = root->color.red;  // take the values from the tree and assign them to the image
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

void ppm(FILE *out, ImagePixel** img, int size) {  // write the information from ImagePixel** into the *.ppm file
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
