//some of the following code was used for the implementation of min heap
//https://www.geeksforgeeks.org/dijkstras-algorithm-for-adjacency-list-representation-greedy-algo-8/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct MinHeapNode {
    int posX, posY;
    int time;
} MinHeapNode;

typedef struct MinHeap {
    int size, capacity;
    int width, height;
    int **pos;
    struct MinHeapNode **array;
} MinHeap;

typedef struct PathTile {
    char pathType;
    int posX, posY;
    int time;
    int accessible, visited;
    struct PathTile *prevPathTile;
} PathTile;

typedef struct PathFinder {
    int width, height;
    struct PathTile ***array;
} PathFinder;

void swapMinHeapNodes(MinHeap *minHeap, int posA, int posB) {
    MinHeapNode **aNode = &minHeap->array[posA];
    MinHeapNode **bNode = &minHeap->array[posB];
    MinHeapNode *tempNode;

    minHeap->pos[(*aNode)->posY][(*aNode)->posX] = posB;
    minHeap->pos[(*bNode)->posY][(*bNode)->posX] = posA;

    tempNode = *aNode;
    *aNode = *bNode;
    *bNode = tempNode;
}

void minHeapify(MinHeap *minHeap, int parentPos) {
    int leftChildPos = 2 * parentPos + 1;
    int rightChildPos = 2 * parentPos + 2;
    int smallestPos;

    MinHeapNode *parent = minHeap->array[parentPos];
    MinHeapNode *leftChild = NULL;
    if (leftChildPos < minHeap->size - 1) {
        leftChild = minHeap->array[leftChildPos];
    }
    MinHeapNode *rightChild = NULL;
    if (rightChildPos < minHeap->size - 1) {
        rightChild = minHeap->array[rightChildPos];
    }

    MinHeapNode *smallestChild = NULL;
    if (leftChild != NULL && rightChild != NULL) {
        if (leftChild->time < rightChild->time) {
            smallestChild = leftChild;
            smallestPos = leftChildPos;
        } else {
            smallestChild = rightChild;
            smallestPos = rightChildPos;
        }
    } else if (leftChild != NULL) {
        smallestChild = leftChild;
        smallestPos = leftChildPos;
    } else if (rightChild != NULL) {
        smallestChild = rightChild;
        smallestPos = rightChildPos;
    }

    if (smallestChild != NULL && smallestChild->time < parent->time) {
        swapMinHeapNodes(minHeap, parentPos, smallestPos);
        minHeapify(minHeap, smallestPos);
    }
}

MinHeapNode *extractMin(MinHeap *minHeap) {
    int firstNodePos = 0;
    int lastNodePos = minHeap->size - 1;
    MinHeapNode *firstNode = minHeap->array[firstNodePos];

    swapMinHeapNodes(minHeap, firstNodePos, lastNodePos);

    minHeap->size--;
    minHeapify(minHeap, 0);

    return firstNode;
}

void fillMinHeap(MinHeap *minHeap) {
    int posX, posY;

    for (posY = 0; posY < minHeap->height; posY++) {
        for (posX = 0; posX < minHeap->width; posX++) {
            int pos = posY * (minHeap->width) + posX;

            minHeap->pos[posY][posX] = pos;
            MinHeapNode *minHeapNode = minHeap->array[pos];
            minHeapNode->posX = posX;
            minHeapNode->posY = posY;
            minHeapNode->time = INT_MAX;
            minHeap->size++;
        }
    }
}

MinHeap* createMinHeap(int width, int height) {
    int i, capacity = width * height;

    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->capacity = capacity;
    minHeap->size = 0;
    minHeap->width = width;
    minHeap->height = height;
    minHeap->pos = (int **)malloc(height * sizeof(int *));
    for (i = 0; i < width; i++) {
        minHeap->pos[i] = (int *)malloc(width * sizeof(int));
    }
    minHeap->array = (MinHeapNode **)malloc(capacity * sizeof(MinHeapNode *));
    for (i = 0; i < capacity; i++) {
        minHeap->array[i] = (MinHeapNode *)malloc(sizeof(MinHeapNode));
    }

    fillMinHeap(minHeap);

    return minHeap;
}

void resetMinHeap(MinHeap *minHeap) {
    int i, capacity = minHeap->capacity;

    for (i = 0; i < minHeap->size; i++) {
        free(minHeap->array[i]);
    }
    free(minHeap->array);

    minHeap->size = 0;

    minHeap->array = (MinHeapNode **)malloc(capacity * sizeof(MinHeapNode *));
    for (i = 0; i < capacity; i++) {
        minHeap->array[i] = (MinHeapNode *)malloc(sizeof(MinHeapNode));
    }

    fillMinHeap(minHeap);
}

PathFinder *copyPathFinder(PathFinder *pathFinder) {
    int posX, posY;

    PathFinder *pathFinderCopy = (PathFinder *)malloc(sizeof(PathFinder));
    pathFinderCopy->width = pathFinder->width;
    pathFinderCopy->height = pathFinder->height;

    pathFinderCopy->array = (PathTile ***)malloc(pathFinder->height * sizeof(PathTile **));
    for (posY = 0; posY < pathFinder->height; posY++) {
        pathFinderCopy->array[posY] = (PathTile **)malloc(pathFinder->width * sizeof(PathTile *));

        for (posX = 0; posX < pathFinder->width; posX++) {
            pathFinderCopy->array[posY][posX] = (PathTile *)malloc(sizeof(PathTile));
        }
    }

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinderCopy->array[posY][posX];

            pathTile->posX = posX;
            pathTile->posY = posY;
            pathTile->time = INT_MAX;
            pathTile->visited = 0;
            pathTile->accessible = 0;
            pathTile->prevPathTile = (PathTile *)malloc(sizeof(PathTile));
            pathTile->prevPathTile = NULL;
            pathTile->pathType = pathFinder->array[posY][posX]->pathType;
        }
    }

    return pathFinderCopy;
}

PathFinder *createPathFinder(int width, int height) {
    int posX, posY;

    PathFinder *pathFinder = (PathFinder *)malloc(sizeof(PathFinder));
    pathFinder->width = width;
    pathFinder->height = height;

    pathFinder->array = (PathTile ***)malloc(height * sizeof(PathTile **));
    for (posY = 0; posY < height; posY++) {
        pathFinder->array[posY] = (PathTile **)malloc(width * sizeof(PathTile *));

        for (posX = 0; posX < width; posX++) {
            pathFinder->array[posY][posX] = (PathTile *)malloc(sizeof(PathTile));
        }
    }

    for (posY = 0; posY < height; posY++) {
        for (posX = 0; posX < width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];

            pathTile->posX = posX;
            pathTile->posY = posY;
            pathTile->time = INT_MAX;
            pathTile->visited = 0;
            pathTile->accessible = 0;
            pathTile->prevPathTile = (PathTile *)malloc(sizeof(PathTile));
            pathTile->prevPathTile = NULL;
        }
    }

    return pathFinder;
}

void setPathFinderStart(PathFinder *pathFinder, int posX, int posY) {
    PathTile *pathTile = pathFinder->array[posY][posX];

    char tileType = pathTile->pathType;

    int timeInc = 0;
    switch (tileType) {
    case 'C': case 'D': case 'P':
        timeInc = 1;
        break;
    case 'H':
        timeInc = 2;
        break;
    }

    pathTile->time = timeInc;
    pathTile->accessible = 1;
    pathTile->prevPathTile = NULL;
}

void setPathFinderTileTypes(PathFinder *pathFinder, char **map) {
    int posX, posY;

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];
            pathTile->pathType = map[posY][posX];
        }
    }
}

void resetPathFinder(PathFinder *pathFinder) {
    int posX, posY;

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];

            pathTile->time = INT_MAX;
            pathTile->accessible = 0;
            pathTile->visited = 0;
            pathTile->prevPathTile = NULL;
        }
    }
}

void printPathFinder(PathFinder *pathFinder) {
    int posX, posY;

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];

            printf("pathFinder->array[%d][%d]: ", posY, posX);
            printf("time - %d, ", pathTile->time);
            if (pathTile->prevPathTile != NULL) {
                printf("prevPathTile[Y][X] - %d/%d, ", pathTile->prevPathTile->posY, pathTile->prevPathTile->posX);
            } else {
                printf("prevPathTile[Y][X] - NULL, ");
            }
            printf("pathType - %c, ", pathTile->pathType);
            printf("accessible - %d, ", pathTile->accessible);
            printf("visited - %d", pathTile->visited);
            printf("\n");
        }
    }

    printf("\n");
}

void decreaseKey(struct MinHeap* minHeap, int posX, int posY, int time) {
    int pos = minHeap->pos[posY][posX];
    minHeap->array[pos]->time = time;

    int parentPos = (pos - 1) / 2;
    while (pos && minHeap->array[pos]->time < minHeap->array[parentPos]->time) {
        swapMinHeapNodes(minHeap, pos, parentPos);

        pos = (pos - 1) / 2;
        parentPos = (pos - 1) / 2;
    }
}

void setMinHeapStart(MinHeap *minHeap, PathFinder *pathFinder, int posX, int posY) {
    char tileType = pathFinder->array[posY][posX]->pathType;

    int timeInc = 0;
    switch (tileType) {
    case 'C': case 'D': case 'P':
        timeInc = 1;
    case 'H':
        timeInc = 2;
        break;
    }

    decreaseKey(minHeap, posX, posY, timeInc);
}


int existsInPathFinder(PathFinder *pathFinder, int posX, int posY) {
    int width = pathFinder->width;
    int height = pathFinder->height;

    return (posX >= 0 && posX < width && posY >= 0 && posY < height);
}

int wasVisited(PathTile *pathTile) {
    return pathTile->visited;
}

int isBlocker(PathTile *pathTile) {
    return (pathTile->pathType == 'N');
}

void check(PathFinder *pathFinder, MinHeap *minHeap, PathTile *prevPathTile, int posX, int posY) {
    if (!existsInPathFinder(pathFinder, posX, posY)) {
        return;
    }

    PathTile *curPathTile = pathFinder->array[posY][posX];

    if (wasVisited(curPathTile)) {
        return;
    }

    if (isBlocker(curPathTile)) {
        return;
    }

    char tileType = curPathTile->pathType;
    curPathTile->accessible = 1;

    int timeInc = 0;
    switch (tileType) {
    case 'C': case 'D': case 'P':
        timeInc = 1;
        break;
    case 'H':
        timeInc = 2;
        break;
    }

    int newTime = prevPathTile->time + timeInc;

    if (newTime < curPathTile->time) {
        decreaseKey(minHeap, posX, posY, newTime);
        curPathTile->time = newTime;
        curPathTile->prevPathTile = prevPathTile;
    }
}

int visit(PathFinder *pathFinder, MinHeap *minHeap, int posX, int posY) {
    PathTile *pathTile = pathFinder->array[posY][posX];

    if (!pathTile->accessible) {
        return 0;
    }

    pathTile->visited = 1;

    check(pathFinder, minHeap, pathTile, posX - 1, posY);
    check(pathFinder, minHeap, pathTile, posX, posY - 1);
    check(pathFinder, minHeap, pathTile, posX + 1, posY);
    check(pathFinder, minHeap, pathTile, posX, posY + 1);

    return 1;
}

void Dijkstra(MinHeap *minHeap, PathFinder *pathFinder, int startX, int startY) {
    resetPathFinder(pathFinder);
    setPathFinderStart(pathFinder, startX, startY);

    resetMinHeap(minHeap);
    setMinHeapStart(minHeap, pathFinder, startX, startY);

    while(minHeap->size != 0) {
        MinHeapNode *minHeapNode = extractMin(minHeap);

        int posX = minHeapNode->posX;
        int posY = minHeapNode->posY;

        if (!visit(pathFinder, minHeap, posX, posY)) {
            break;
        }
    }
}

PathTile *getDragonTile(PathFinder *pathFinder) {
    int posX, posY;

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];

            if (pathTile->pathType == 'D' && pathTile->accessible == 1) {
                pathTile->pathType = 'C';
                return pathTile;
            }
        }
    }

    return NULL;
}

int hasPrincessPath(PathFinder *pathFinder) {
    int posX, posY;
    int foundPrincess = 0;

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinder->array[posY][posX];

            if (pathTile->pathType == 'P') {
                foundPrincess = 1;

                if (pathTile->accessible == 0) {
                    return 0;
                }
            }
        }
    }

    return foundPrincess;
}

int *getPath(PathTile *targetPathTile, int *pathLength) {
    int i;
    int *reversePath = (int *)malloc(2 * sizeof(int));

    reversePath[0] = targetPathTile->posX;
    reversePath[1] = targetPathTile->posY;
    *pathLength = 1;

    PathTile *curPathTile = targetPathTile->prevPathTile;
    while (curPathTile != NULL) {
        (*pathLength)++;
        reversePath = realloc(reversePath, 2 * (*pathLength) * sizeof(int));
        reversePath[(((*pathLength) - 1) * 2)] = curPathTile->posX;
        reversePath[(((*pathLength) - 1) * 2) + 1] = curPathTile->posY;
        curPathTile = curPathTile->prevPathTile;
    }

    int *path = (int *)malloc(2 * (*pathLength) * sizeof(int));

    for (i = 0; i < *pathLength; i++) {
        path[i * 2] = reversePath[2 * (*pathLength) - 2 * i - 2];
        path[i * 2 + 1] = reversePath[2 * (*pathLength) - 2 * i - 1];
    }

    free(reversePath);

    return path;
}

void printPath(int *path, int pathLength) {
    int i;

    printf("Path [length %d]:\n", pathLength);
    for (i = 0; i < pathLength; i++) {
        printf("%d %d\n", path[i * 2], path[i * 2 + 1]);
    }
    printf("\n");
}

int *addPath(int *path, int* addedPath, int *pathLength, int addedPathLength) {
    int i;
    int oldPathLength = *pathLength;

    *pathLength += addedPathLength - 1;

    path = (int *)realloc(path, 2 * (*pathLength) * sizeof(int));

    for (i = 1; i < addedPathLength; i++) {
        path[2 * oldPathLength + 2 * (i - 1)] = addedPath[2 * i];
        path[2 * oldPathLength + 2 * (i - 1) + 1] = addedPath[2 * i + 1];
    }

    return path;
}

int *getPrincessPath(MinHeap *minHeap, PathFinder *pathFinder, int startX, int startY, int *princessPathLength, int *time) {
    int posX, posY;
    int *princessPath = NULL;

    PathFinder *pathFinderCopy = copyPathFinder(pathFinder);
    Dijkstra(minHeap, pathFinderCopy, startX, startY);

    for (posY = 0; posY < pathFinder->height; posY++) {
        for (posX = 0; posX < pathFinder->width; posX++) {
            PathTile *pathTile = pathFinderCopy->array[posY][posX];

            if (pathTile->pathType == 'P' && pathTile->accessible == 1) {
                pathFinder->array[pathTile->posY][pathTile->posX]->pathType = 'C';

                int newPathLength = 0;
                int newTime = pathTile->time;
                int *newPath = getPath(pathTile, &newPathLength);

                if (hasPrincessPath(pathFinder)) {
                    int addedPathLength = 0;
                    int addedTime = 0;
                    int *addedPath = getPrincessPath(minHeap, pathFinder, pathTile->posX, pathTile->posY, &addedPathLength, &addedTime);

                    newPath = addPath(newPath, addedPath, &newPathLength, addedPathLength);
                    newTime += addedTime;
                }


                if (princessPath == NULL || newTime < *time) {
                    princessPath = newPath;
                    *princessPathLength = newPathLength;
                    *time = newTime;
                }

                pathFinder->array[pathTile->posY][pathTile->posX]->pathType = 'P';
            }
        }
    }

    return princessPath;
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
    int width = m;
    int height = n;
    char **map = mapa;
    int time = t;
    int *pathLength = dlzka_cesty;

    int i, *path;

    MinHeap *minHeap = createMinHeap(width, height);
    PathFinder *pathFinder = createPathFinder(width, height);
    setPathFinderTileTypes(pathFinder, map);

    Dijkstra(minHeap, pathFinder, 0, 0);

    PathTile *dragonTile = getDragonTile(pathFinder);
    if (dragonTile != NULL) {
        path = getPath(dragonTile, pathLength);
    } else {
        *pathLength = 0;
        return NULL;
    }

    if (dragonTile->time > time) {
        *pathLength = 0;
        return NULL;
    }

    if (!hasPrincessPath(pathFinder)) {
        *pathLength = 0;
        return NULL;
    }

    PathTile *prevTile = dragonTile;

    int princessPathLength = 0;
    int startTime = 0;
    int *princessPath = getPrincessPath(minHeap, pathFinder, prevTile->posX, prevTile->posY, &princessPathLength, &startTime);

    path = addPath(path, princessPath, pathLength, princessPathLength);

    return path;
}

//P�vodn� testovanie

/*
int main() {
    char **mapa;
    int i, dlzka_cesty, cas, *cesta;
    int n=0, m=0, t=0;

    n = 10;
    m = 10;
    t = 12;

    mapa = (char**)malloc(n*sizeof(char*));
    mapa[0]="CCHCNHCCHN";
    mapa[1]="NNCCCHHCCC";
    mapa[2]="DNCCNNHHHC";
    mapa[3]="CHHHCCCCCC";
    mapa[4]="CCCCCNHHHH";
    mapa[5]="PCHCCCNNNN";
    mapa[6]="NNNNNHCCCC";
    mapa[7]="CCCCCPCCCC";
    mapa[8]="CCCNNHHHHH";
    mapa[9]="HHHPCCCCCC";

    cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);

    printPath(cesta, dlzka_cesty);

    return 0;
}
*/

//Nov� testovanie
//Testovacie vstupy s� v s�boroch test1.txt, test2.txt a test3.txt


int main() {
    char filename[100];
    char **mapa;
    int i, test, dlzka_cesty, cas, *cesta;
    int n=0, m=0, t=0;
    FILE* f;

    while(1) {
        printf("Zadajte cislo testu (0 ukonci program / 1 nacitaj subor):\n");
        scanf("%d",&test);
        dlzka_cesty = 0;
        n=m=t=0;
        switch(test) {
        case 0: //ukonci program
            return 0;
        case 1: //nacitanie mapy zo suboru
            printf("Zadaj meno suboru: ");
            scanf("%s", filename);
            if ((f = fopen(filename,"r")) == NULL) {
                printf("dany subor neexistuje\n");
                break;
            }
            if(f)
                fscanf(f, "%d %d %d", &n, &m, &t);
            else
                continue;
            mapa = (char**)malloc(n*sizeof(char*));
            for(i = 0; i < n; i++){
                mapa[i] = (char*)malloc(m*sizeof(char));
                    for (int j=0; j<m; j++){
                        char policko = fgetc(f);
                        if(policko == '\n') policko = fgetc(f);
                        mapa[i][j] = policko;
                    }
            }
            fclose(f);
            cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
            break;
        case 2: //nacitanie preddefinovanej mapy
            n = 10;
            m = 10;
            t = 12;
            mapa = (char**)malloc(n*sizeof(char*));
            mapa[0]="CCHCNHCCHN";
            mapa[1]="NNCCCHHCCC";
            mapa[2]="DNCCNNHHHC";
            mapa[3]="CHHHCCCCCC";
            mapa[4]="CCCCCNHHHH";
            mapa[5]="PCHCCCNNNN";
            mapa[6]="NNNNNHCCCC";
            mapa[7]="CCCCCPCCCC";
            mapa[8]="CCCNNHHHHH";
            mapa[9]="HHHPCCCCCC";
            cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
            break;
        case 3: //pridajte vlastne testovacie vzorky
            default:
            continue;
        }
        cas = 0;
        for(i = 0; i < dlzka_cesty; i++){
            printf("%d %d\n", cesta[i*2], cesta[i*2+1]);
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'H')
                cas += 2;
            else
                cas += 1;
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'D' && cas > t)
                printf("Nestihol si zabit draka!\n");
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'N')
                printf("Prechod cez nepriechodnu prekazku!\n");
            if(i>0 && abs(cesta[i*2+1]-cesta[(i-1)*2+1])+abs(cesta[i*2]-cesta[(i-1)*2])>1)
                printf("Neplatny posun Popolvara!\n");
        }
        printf("%d\n",cas);
        free(cesta);
        for(i=0; i<n; i++){
            free(mapa[i]);
        }
        free(mapa);
    }

    return 0;
}
