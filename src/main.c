
/*
 * To compile and use :
 * gcc main.c vertices_struct.c -o "file name"
 * ./"file name" arg1 arg2
 *
 * To Change the resolution, change RESOLUTION
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "vertices_struct.h"

#define NB_MAX_VRT 20000
#define NB_MAX_FACES 7000
#define RESOLUTION 80

int main(int argc, char const *argv[])
{
  FILE *p_file;
  int nbV = 0, nbF = 0;
  int i, j, n; /* for loops */
  int tab[3];
  int *nbDup;
  int matrixFaces[NB_MAX_FACES][3];
  char str[50]; /* string read from file. */
  float theFloat;
  float invSizeMesh;
  float min[NB_DIMENSIONS];  /* Hold values of minimum */
  float max[NB_DIMENSIONS];  /* and maximum coordinates on each axe */
  float distMax = 0;
  float matrixVrt[NB_MAX_VRT][NB_DIMENSIONS];

  HashTableVertex *vrtTable;
  HashTableFaces *faceTable;
  Vertex **vrtList;
  Vertex *vertex;
  Face *face;

  /* Check if good number of arguments */
  if(argc != 3)
  {
    printf("Problems with arguments\n");
    exit(1);
  }

  /**** TASK 1 : READING AN OBJ FILE ****/
  /* Open file. If we cannot open it, we close the application */
  if (!(p_file = fopen(argv[1], "r")))
  {
    printf("Can't open file. \n");
    exit(1);
  }

  /* Start scan of file .obj */
  while ((fscanf(p_file, "%s", str)) != EOF)
  {
    if (!strcmp(str, "v")) /* Reading of vertices coordinates */
    {
      for (i = 0; i < NB_DIMENSIONS; i++)
      {
        fscanf(p_file, "%f", &theFloat);
        matrixVrt[nbV][i] = theFloat;

        /* Initialization of max and min values */
        if (!nbV)
          min[i] = max[i] = theFloat;

        /* During scanning the file we hold the max and min values for each axe */
        if (theFloat < min[i])
          min[i] = theFloat;
        else if (theFloat > max[i])
          max[i] = theFloat;
      }
      nbV++;
    }
    else if (!strcmp(str, "f")) /* Reading of faces compositions */
    {
      for (i = 0; i < 3; i++)
      {
        fscanf(p_file, "%s", str);
        /* Convert from string to int. strtol takes care of the "/" */
        n = strtol(str, NULL, 10);
        if (n == 0 || errno == ERANGE || errno == EINVAL)
        {
          printf("Problem with conversion face\n");
          exit(1);
        }
        else
          matrixFaces[nbF][i] = n-1; /* Substract by 1 because vertex index start at 1 */
      }
      nbF++;
    }
  }
  fclose(p_file); /* End reading OBJ file */

  /**** TASK 2 : SIMPLIFY THE GEOMETRY ****/
  /*** PART 1 :  ATTRIBUTE NEW SIMPLIFIED COORDONATE TO EACH VERTEX ***/
  /* Calculate distance on each axe and hold maximum value */
  for (i = 0; i < NB_DIMENSIONS; i++)
    if (distMax < max[i]-min[i])
      distMax = max[i]-min[i];

  invSizeMesh = (RESOLUTION - 1)/distMax; /* Use the inverse */

  /* Allocation array of pointers of Vertex.
   * Used to keep the original order of the vertices */
  if (!(vrtList = (Vertex**) (Vertex**) malloc(sizeof(Vertex*) * nbV)))
  {
    printf("Memory problem\n");
    exit(1);
  }

  if (!(vrtTable = (HashTableVertex*) createHashTableVrtx(nbV)))
  {
    printf("Memory problem\n");
    exit(1);
  }

  /* nbDup will count the number of duplicate vertices */
  if (!(nbDup = (int*) malloc(sizeof(int))))
  {
    printf("Memory problem\n");
    exit(1);
  }
  *nbDup = 0;

  /** Attribute new vertex for each former point **/
  for (j = 0; j < nbV; j++)
  {
    for (i = 0; i < NB_DIMENSIONS; i++)
    {
      theFloat = (matrixVrt[j][i] - min[i])* invSizeMesh;
      /* Round function */
      n = (int) theFloat;
      tab[i] = theFloat < n + 0.5 ? n : n+1;
    }

    /* We try to add the new vertex to the HashTable. If those coordinates are not in the
     * the table, it will create a new Vertex structure and return its address otherwise
     * it will return the address of the existing vertex */
    vrtList[j] = processVertex(vrtTable, tab, j, nbDup);
  }

  /*** PART 2 : ELIMINATE DUPLICATE AND INVALID FACES ***/
  /* HashTable for faces */
  faceTable = createHashTableFc(nbF);

  for (j = 0; j < nbF; j++)
  {
    for (i = 0; i < 3; i++)
      tab[i] = vrtList[matrixFaces[j][i]]->newIndice;

    if (isValidFace(tab))
    {
      sortVrtx(tab); /* We sort them to make the comparison easier */
      processFace(faceTable, tab);
    }
  }

  /**** TASK 3 : OUTPUT A NEW OBJ FILE ****/
  /* Write new vertices */
  if (!(p_file = fopen(argv[2], "w")))
  {
    printf("Can't open file. \n");
    exit(1);
  }
  /* Write new vertices coordonates */
  for (i = 0; i < nbV; i++)
  {
    if(i == vrtList[i]->prevIndice)
    {
      fprintf(p_file, "v %d %d %d\n",
       vrtList[i]->coor[0], vrtList[i]->coor[1], vrtList[i]->coor[2]);
    }
  }
  fprintf(p_file, "\n");

  /* Write new faces */
  for (i = 0; i < faceTable->size; i++)
  {
    for(face = faceTable->list[i]; face; face = face->p_next)
      fprintf(p_file, "f %d %d %d\n",
       (face->vrtx[0]) + 1, (face->vrtx[1]) + 1, (face->vrtx[2]) + 1);
  }
  fclose(p_file);

  /* We release the memory dynamically allocated */
  deleteHashTableFc(faceTable);
  deleteHashTableVrtx(vrtTable);
  free(vrtList);

  return 0;
}
