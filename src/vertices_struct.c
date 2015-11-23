#include <stdlib.h>
#include <stdio.h>

#include "vertices_struct.h"

/* Hash function for array of integers
 * It is clearly not optimized. */
int hashArray(int tab[], int size, int mod)
{
  int i,j = 0;
  for(i = 0; i < size; i ++)
    j += tab[i];

  return j%mod;
}

/* Compare arrays of integers
 * return 1 is identique, else return 0 */
int intscmp(int tab1[], int tab2[], int size)
{
  int i;
  for (i = 0; i < size; i++)
    if (tab1[i] != tab2[i])
      return 0;

  return 1;
}

void sortVrtx(int vrt[])
{
  int i, tmp, o;
  do
  {
    o = 1;
    for (i = 1; i < 3; i++)
    {
      if(vrt[i-1] > vrt[i])
      {
        tmp = vrt[i];
        vrt[i] = vrt[i-1];
        vrt[i-1] = tmp;
        o = 0;
      }
    }
  } while(!o);
}

Vertex *createVertex(int coor[], int prevIndice, int newIndice)
{
  Vertex *vrtx;
  int i;
  if (!(vrtx = (Vertex*) malloc(sizeof(Vertex))))
  {
    printf("Problem when allocating vertex.\n");
    exit(1);
  }

  for (i = 0; i < NB_DIMENSIONS; i++)
    vrtx->coor[i] = coor[i];

  vrtx->prevIndice = prevIndice;
  vrtx->newIndice = newIndice;
  vrtx->p_next = NULL;
  return vrtx;
}

Vertex *processVertex(HashTableVertex* table, int coordinates[], int index, int *nbDup)
{
  int hash;
  Vertex *vrtx;

  hash = hashArray(coordinates, NB_DIMENSIONS, table->size);

  for (vrtx = table->list[hash]; vrtx; vrtx = vrtx->p_next)
  {
    if (intscmp(coordinates, vrtx->coor, NB_DIMENSIONS))
    {
      (*nbDup)++; /* If the vertex is already in the table */
      return vrtx;/* we return the address of the existing vertex */
    }
  }
  vrtx = createVertex(coordinates, index, index-(*nbDup));
  vrtx->p_next = table->list[hash];
  table->list[hash] = vrtx;
  return vrtx;
}

HashTableVertex *createHashTableVrtx(int size)
{
  HashTableVertex *hashTable;
  int i;

  if (!(hashTable = malloc(sizeof(HashTableVertex))))
  {
    printf("Problem when allocating HashTableVertex.\n");
    exit(1);
  }

  if (!(hashTable->list = (Vertex**) malloc(sizeof(Vertex*) * size)))
  {
    printf("Problem when allocating HashTableVertex.\n");
    exit(1);
  }

  /* Initialization of the list of the table */
  for (i = 0; i < size; i++)
    hashTable->list[i] = NULL;

  hashTable->size = size;
  return hashTable;
}

HashTableFaces *createHashTableFc(int size)
{
  HashTableFaces *hashTable;
  int i;

  if (!(hashTable = malloc(sizeof(HashTableFaces))))
  {
    printf("Problem when allocating HashTableFaces.\n");
    exit(1);
  }

  if (!(hashTable->list = (Face**) malloc(sizeof(Face*) * size)))
  {
    printf("Problem when allocating HashTableFaces.\n");
    exit(1);
  }

  /* Initialization of the list of the table */
  for (i = 0; i < size; i++)
    hashTable->list[i] = NULL;

  hashTable->size = size;
  return hashTable;
}


void processFace(HashTableFaces* table, int vertex[])
{
  int hash;
  int t;
  Face *fc;

  hash = hashArray(vertex, 3, table->size);

  for (fc = table->list[hash]; fc; fc = fc->p_next)
  {
    if (intscmp(vertex, fc->vrtx, 3))
      return; /* The Face already exist */
  }
  fc = createFace(vertex);
  fc->p_next = table->list[hash];
  table->list[hash] = fc;
}

Face *createFace(int vertex[])
{
  Face *face;
  int i;
  if (!(face = (Face*) malloc(sizeof(Face))))
  {
    printf("Problem when allocating new Faces.\n");
    exit(1);
  }
  for (i = 0; i < 3; i++)
    face->vrtx[i] = vertex[i];

  face->p_next = NULL;
  return face;
}

int isValidFace(int face[])
{
  if (face[0] == face[1] || face[0] == face[2] || face[2] == face[1])
    return 0;
  return 1;
}

void deleteHashTableVrtx(HashTableVertex* table)
{
  int i;
  Vertex *vrtx, *vtemp;

  for(i = 0; i < table->size; i++)
  {
    vrtx = table->list[i];
    while (vtemp = vrtx)
    {
      free(vrtx);
      vrtx = vtemp->p_next;
    }
  }
  free(table);
}

void deleteHashTableFc(HashTableFaces* table)
{
  int i;
  Face *fc, *ftemp;

  for(i = 0; i < table->size; i++)
  {
    fc = table->list[i];
    while (ftemp = fc)
    {
      free(fc);
      fc = ftemp->p_next;
    }
  }
  free(table);
}
