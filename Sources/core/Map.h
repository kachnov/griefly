#pragma once

#include <list>
#include <vector>

#include "Constheader.h"
#include "objects/Tile.h"
#include "Atmos.h"

class LOSfinder
{
public:
    std::list<point>* CalculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0);
private:
    void Clear();
    std::list<point> worklist;

};

class MapMaster
{
public:
    MapMaster();
    typedef id_ptr_on<CubeTile> SqType;
    std::vector<std::vector<std::vector<SqType> > > squares;

    Atmosphere atmosphere;

    void SaveToMapGen(const std::string& name);
    void LoadFromMapGen(const std::string& name);




    int GetMapW() const
    {
        return squares.size();
    }
    int GetMapH() const
    {
        return squares[0].size();
    }
    int GetMapD() const
    {
        return squares[0][0].size();
    }

    void MoveToDir(Dir dir, int* x, int* y, int* z = nullptr)
    {
        if (x)
        {
            *x += DirToVDir[dir].x;
            if (*x >= GetMapW() ||
                *x <= -1)
            {
                *x -= DirToVDir[dir].x;
            }
        }
        if (y)
        {
            *y += DirToVDir[dir].y;
            if (*y >= GetMapH() ||
                *y <= -1)
            {
                *y -= DirToVDir[dir].y;
            }
        }
        if (z)
        {
            *z += DirToVDir[dir].z;
            if (*z >= GetMapD() ||
                *z <= -1)
            {
                *z -= DirToVDir[dir].z;
            }
        }
    }

    void ResizeMap(int x, int y, int z);

    void Represent();
    void GenerateFrame();

    bool IsTransparent(int posx, int posy, int posz = 0);

    
    void FillAtmosphere();

    void MakeTiles(int size_x, int size_y, int size_z);

    PassableLevel GetPassable(int posx, int posy, int posz = 0, Dir direct = D_ALL);

    static void switchDir(int& posx, int& posy, Dir direct, int num = 1, bool back = false);
    
    id_ptr_on<IOnMapObject> Click(int x, int y);
    
    LOSfinder losf;
};

bool IsMapValid();
MapMaster& GetMap();
void SetMapMaster(MapMaster* map_master);
