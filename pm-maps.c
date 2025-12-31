#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pm-maps.h"

struct cell {
    int r, c;
    int wall;
    int visited;
    int occupied;
};

struct map {
    int xSize;
    int ySize;
    int ghostCount;
    Cell* cells;
};

static FILE* file = NULL;

int openMapsFile(const char* filename)
{
    if (file) {
        fclose(file);
        file = NULL;
    }

    file = fopen(filename, "r");
    return file != NULL;
}

void closeMapsFile(void)
{
    if (file) {
        fclose(file);
        file = NULL;
    }
}

Map mapCreate(void)
{
    Map m = (Map)malloc(sizeof(struct map));
    if (!m) return NULL;

    m->xSize = 0;
    m->ySize = 0;
    m->ghostCount = 0;
    m->cells = NULL;

    return m;
}

int getBoardGhostCount(Map m) {
    return m ? m->ghostCount : 0;
}

void deleteMap(Map m)
{
    if (!m) return;

    if (m->cells) {
        int total = m->xSize * m->ySize;
        for (int i = 0; i < total; i++) {
            free(m->cells[i]);
        }
        free(m->cells);
        m->cells = NULL;
    }

    m->xSize = 0;
    m->ySize = 0;
    m->ghostCount = 0;
}

int getvisitedCell(Cell c)
{
    return c ? c->visited : 0;
}


Cell getCellByCR(Map m, int c, int r)
{
    if (!m || !m->cells) return NULL;

    int maxCells = m->xSize * m->ySize;   

    for (int i = 0; i < maxCells; i++) {
        if (!m->cells[i]) break;   

        if (m->cells[i]->c == c && m->cells[i]->r == r) {
            return m->cells[i];
        }
    }

    return NULL;
}

int setCellOcuppied(Map m, int c, int r)
{
	if (!m) return 0;
	Cell cell = getCellByCR(m, c, r);
	if (!cell) return 0;
	cell->occupied = 1;
    return 1;
}

int setCellNotOcuppied(Map m, int c, int r)
{
    if (!m) return 0;
    Cell cell = getCellByCR(m, c, r);
    if (!cell) return 0;
    cell->occupied = 0;
    return 1;
}

void mapDestroy(Map m)
{
    if (!m) return;
    deleteMap(m);
    free(m);
}

int mapXSize(Map m)
{
    return m ? m->xSize : 0;
}

int mapYSize(Map m)
{
    return m ? m->ySize : 0;
}

Cell cellAt(Map m, int r, int c)
{
    if (!m) return NULL;
    if (r < 0 || c < 0 || r >= m->ySize || c >= m->xSize) return NULL;
    return m->cells[r * m->xSize + c];
}

int cellIsWall(Cell c)
{
    return c ? c->wall : 0;
}

int getCellOccupied(Cell c)
{
    return c ? c->occupied : 0;
}

int nextMap(Map m)
{
    int ghosts, smartGhosts;
    int xSize, ySize;
    char line[4096];

    if (!m || !file) return 0;

    deleteMap(m);

    if (fscanf(file, "%d %d", &ghosts, &smartGhosts) != 2) return 0;
    if (fscanf(file, "%d %d", &xSize, &ySize) != 2) return 0;

    if (xSize <= 0 || ySize <= 0) return 0;

    m->ghostCount = ghosts + smartGhosts;
    m->xSize = xSize;
    m->ySize = ySize;

    int total = xSize * ySize;

    m->cells = (Cell*)malloc((size_t)total * sizeof(Cell));
    if (!m->cells) {
        deleteMap(m);
        return 0;
    }

    for (int i = 0; i < total; i++) {
        m->cells[i] = (Cell)malloc(sizeof(struct cell));
        if (!m->cells[i]) {
            deleteMap(m);
            return 0;
        }
        m->cells[i]->r = 0;
        m->cells[i]->c = 0;
        m->cells[i]->wall = 0;
        m->cells[i]->visited = 0;
        m->cells[i]->occupied = 0;
    }

    for (int r = 0; r < ySize; r++) {
        if (fscanf(file, "%s", line) != 1) {
            deleteMap(m);
            return 0;
        }

        if ((int)strlen(line) < xSize) {
            deleteMap(m);
            return 0;
        }

        for (int c = 0; c < xSize; c++) {
            int idx = r * xSize + c;

            m->cells[idx]->r = r;
            m->cells[idx]->c = c;
            m->cells[idx]->wall = (line[c] == '1');
            m->cells[idx]->visited = 0;
            m->cells[idx]->occupied = 0;
        }
    }

    return 1;
}

int readAllMaps(Map** maps, const char* filename)
{
    if (!maps || !filename) return 0;

    *maps = NULL;
    int count = 0;
    int cap = 0;

    if (!openMapsFile(filename)) return 0;

    int numMaps = 0;
    if (fscanf(file, "%d", &numMaps) != 1 || numMaps <= 0) {
        closeMapsFile();
        return 0;
    }

    for (int i = 0; i < numMaps; i++) {
        Map m = mapCreate();
        if (!m) break;

        if (!nextMap(m)) {
            mapDestroy(m);
            break;
        }

        if (count >= cap) {
            int newCap = (cap == 0) ? 4 : cap * 2;
            Map* tmp = (Map*)realloc(*maps, (size_t)newCap * sizeof(Map));
            if (!tmp) {
                mapDestroy(m);
                break;
            }
            *maps = tmp;
            cap = newCap;
        }

        (*maps)[count++] = m;
    }

    closeMapsFile();
    return count;
}
