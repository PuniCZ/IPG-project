#include "Scene.h"



Scene::~Scene(void)
{
    while(primitives.empty())
    {
        delete primitives.back();
        primitives.pop_back();
    }

}

void Scene::Init()
{
    /*primitives.push_back(new Plane(glm::vec3(0, 0, -1), 25.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(0, 1, 0, 1), 1.f));


    primitives.push_back(new Plane(glm::vec3(0, 1, 0), -5.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(1, 0, 0, 1), 1.f));

    primitives.push_back(new Sphere(glm::vec3(0, 2, 5), 1.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(0, 1, 1, 1), .5f));

    primitives.push_back(new Sphere(glm::vec3(0, -4, -10), 1.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(1, 1, 0, 1), 0.f));
    primitives.back()->SetLigth(true);*/

    //ground
    primitives.push_back(new Plane(glm::vec3(0, 1, 0), 4.4f));
    primitives.back()->SetMaterial(Material(glm::vec4(.4f, .3f, .3f, 1), 1.f));

    //big sphere
    primitives.push_back(new Sphere(glm::vec3(1, -.8f, 3), 2.5f));
    primitives.back()->SetMaterial(Material(glm::vec4(.7f, .7f, .7f, 1), 0.2f, 0.6f));
    
    //small sphere
    primitives.push_back(new Sphere(glm::vec3(-5.5f, -.5f, 7), 2.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(.7f, .7f, 1.f, 1), 0.1f, 1.f));

    //light 1
    primitives.push_back(new Sphere(glm::vec3(0, 5, 5), 0.1f));
    primitives.back()->SetMaterial(Material(glm::vec4(.4f, .4f, .4f, 1)));
    primitives.back()->SetLigth(true);

    //light 2
    primitives.push_back(new Sphere(glm::vec3(2, 5, 1), 0.1f));
    primitives.back()->SetMaterial(Material(glm::vec4(.6f, .6f, .8f, 1)));
    primitives.back()->SetLigth(true);


    /*primitives.push_back(new Sphere(glm::vec3(0, 0, -1), 0.1f));
    primitives.back()->SetMaterial(Material(glm::vec4(.9f, .9f, .9f, 1), 10));
    primitives.back()->SetLigth(true);*/

    primitives.push_back(new Particle(glm::vec3(-5.5f, 1.5f, 5), glm::vec3(0, 0, 1), 2.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(.2f, 1.f, .2f, 1), .6f));


    BuildGrid();
}

void Scene::BuildGrid()
{
    // initialize regular grid
    grid = new ObjectList*[GRIDSIZE * GRIDSIZE * GRIDSIZE];
    memset(grid, 0, GRIDSIZE * GRIDSIZE * GRIDSIZE * sizeof(std::list<Primitive*>*));
    glm::vec3 p1(-14, -5, -6), p2( 14, 8, 30 ); //TODO: World boundaries

    //glm::vec3 p1(-20, -20, -20), p2( 20, 20, 30 ); //TODO: World boundaries

    // calculate cell width, height and depth
    float dx = (p2.x - p1.x) / GRIDSIZE, dx_reci = 1.0f / dx;
    float dy = (p2.y - p1.y) / GRIDSIZE, dy_reci = 1.0f / dy;
    float dz = (p2.z - p1.z) / GRIDSIZE, dz_reci = 1.0f / dz;
    boundingBox = GridBox(p1, p2 - p1);
    //m_Light = new Primitive*[MAXLIGHTS];
    //m_Lights = 0;
    // store primitives in the grid cells
    //for ( int p = 0; p < m_Primitives; p++ )
    for (std::vector<Primitive*>::iterator it = primitives.begin(); it != primitives.end(); it++)
    {
        if ((*it)->IsLigth()) 
            lights.push_back(*it);
        GridBox bound = (*it)->GetBoundingBox();
        glm::vec3 bv1(bound.GetPos());
        glm::vec3 bv2(bound.GetPos() + bound.GetSize());
        // find out which cells could contain the primitive (based on bounding box)
        int x1 = (int)((bv1.x - p1.x) * dx_reci), x2 = (int)((bv2.x - p1.x) * dx_reci) + 1;
        x1 = (x1 < 0)?0:x1, x2 = (x2 > (GRIDSIZE - 1))?GRIDSIZE - 1:x2;
        int y1 = (int)((bv1.y - p1.y) * dy_reci), y2 = (int)((bv2.y - p1.y) * dy_reci) + 1;
        y1 = (y1 < 0)?0:y1, y2 = (y2 > (GRIDSIZE - 1))?GRIDSIZE - 1:y2;
        int z1 = (int)((bv1.z - p1.z) * dz_reci), z2 = (int)((bv2.z - p1.z) * dz_reci) + 1;
        z1 = (z1 < 0)?0:z1, z2 = (z2 > (GRIDSIZE - 1))?GRIDSIZE - 1:z2;
        // loop over candidate cells
        for ( int x = x1; x < x2; x++ ) for ( int y = y1; y < y2; y++ ) for ( int z = z1; z < z2; z++ )
        {
            // construct bounding box for current cell
            int idx = x + y * GRIDSIZE + z * GRIDSIZE * GRIDSIZE;
            glm::vec3 pos(p1.x + x * dx, p1.y + y * dy, p1.z + z * dz);
            GridBox cell(pos, glm::vec3( dx, dy, dz ));
            // do an accurate bounding box / primitive intersection test
            if ((*it)->IntersectBox(cell))
            {
                // object intersects cell; add to object list
                ObjectList* l = new ObjectList();
                l->SetPrimitive(*it);
                l->SetNext(grid[idx]);
                if (l->GetNext())
                    l->GetNext();
                grid[idx] = l;
            }
        }
    }
}