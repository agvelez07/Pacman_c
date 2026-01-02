#ifndef PM_MAPS_H
#define PM_MAPS_H

typedef struct map* Map;
typedef struct cell* Cell;

int openMapsFile(const char* filename);
void closeMapsFile(void);

int isCellVisited(Cell c);
int hasWall(Cell c);

Map mapCreate(void);
void mapDestroy(Map m);
void deleteMap(Map m);

int setCellNotOcuppied(Map m, int c, int r);
int setCellOcuppied(Map m, int c, int r);
int setCellVisited(Cell c);
int toggleWall(Cell c);

int nextMap(Map m);
int readAllMaps(Map** maps, const char* filename);

Cell cellAt(Map m, int r, int c);

int mapXSize(Map m);
int mapYSize(Map m);

int cellIsHouse(Cell c);
int getCellOccupied(Cell c);

void resetMapVisited(Map m);
void resetWalls(Map m);

#endif