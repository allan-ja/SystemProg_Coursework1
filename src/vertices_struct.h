#ifndef H_VRTX
#define H_VRTX

#include <stdlib.h>
#include <stdio.h>

#define NB_DIMENSIONS 3

typedef struct _vertex_s
{
  int coor[NB_DIMENSIONS];
  int newIndice;
  int prevIndice;
  /* Use in case of collision in the Hash Table */
  struct _vertex_s *p_next;
} Vertex;


typedef struct _hash_table_vertex_s
{
  Vertex **list;
  int size;
} HashTableVertex;

typedef struct _face_s
{
  int vrtx[3];
  /* Use in case of collision in the Hash Table */
  struct _face_s *p_next;
} Face;

typedef struct _hash_table_face_s
{
  Face **list;
  int size;
} HashTableFaces;


int hashArray(int[], int, int);
int intscmp(int[], int[], int);
void sortVrtx(int[]);
int isValidFace(int[]);
HashTableVertex *createHashTableVrtx(int);
Vertex *createVertex(int[], int, int);
Vertex *processVertex(HashTableVertex*, int[], int, int*);
HashTableFaces *createHashTableFc(int);
Face *createFace(int[]);
void processFace(HashTableFaces* , int[]);

void deleteHashTableVrtx(HashTableVertex*);
void deleteHashTableFc(HashTableFaces*);
#endif
