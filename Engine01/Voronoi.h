#ifndef ENGINE01_VORONOI_H
#define ENGINE01_VORONOI_H

#include <algorithm>
#include "glm\glm.hpp"
#include <vector>

#include "Biome.h"
#include "DebuggingTools.h"

class GraphEdge;
class Corner;

class Site
{
public:
	//these two are used in voronoi generation
	glm::vec2 coord;
	int sitenbr;
	//pointers to associated voronoi objects
	std::vector<GraphEdge*> edges;
	std::vector<Site*> adjacentSites;
	std::vector<Corner*> corners;
	//world data (set after voronoi graph is generated)
	float elevation;
	float moisture;
	float temperature;
	bool water;
	bool ocean;
	bool pointsOutsideTriangles = false;
	Biome* biome;
};

class GraphEdge
{
public:
	float x1, y1, x2, y2;
	Site *site1, *site2;		//the vector site 1 -> site 2 marks a delaunay edge
	Corner *c1, *c2;			//c1 to c2 are the corners that this edge runs between (voronoi edge)
};

class Corner
{
public:
	glm::vec2 pos;
	std::vector<Site*> touches;
	float elevation;
};

class Edge
{
public:
	float a = 0.0f, b = 0.0f, c = 0.0f;
	std::vector<Site*> ep;
	std::vector<Site*> reg;
	int edgenbr;

};

class HalfEdge
{
public:
	HalfEdge* ELleft;
	HalfEdge* ELright;
	Edge* ELedge;
	bool deleted;
	int ELpm;
	Site* vertex;
	float ystar;
	HalfEdge* PQnext = nullptr;
};

//I suspect this class may have a few memory leaks where I construct an object and replace an existing one in an array
//without realising that the array entry is the only reference to the previous one...
class Voronoi
{
private:
	//fields
	int siteIdx;
	float xmin, xmax, ymin, ymax, deltax, deltay;
	int nvertices;
	int nedges;
	int nsites;
	Site* bottomsite;
	int sqrt_nsites;
	float minDistanceBetweenSites;
	int PQcount;
	int PQmin;
	int PQhashsize;
	std::vector<HalfEdge*> PQHash;
	const int LE = 0;
	const int RE = 1;
	int ELhashsize;
	std::vector<HalfEdge*> ELhash;
	HalfEdge* ELleftend;
	HalfEdge* ELrightEnd;
	
	//methods
	void sort(std::vector<glm::vec2> points);
	//void qsort();
	void sortnode(std::vector<glm::vec2> points);
	Site* nextOne();
	Edge* bisect(Site* s1, Site* s2);
	void makevertex(Site* v);
	bool PQinitialise();
	int PQbucket(HalfEdge* he);
	void PQinsert(HalfEdge* he, Site* v, float offset);
	void PQDelete(HalfEdge* he);
	bool PQEmpty();
	glm::vec2 PQ_min();
	HalfEdge* PQextractmin();
	HalfEdge* HEcreate(Edge* e, int pm);
	bool ELinitialise();
	HalfEdge* ELright(HalfEdge* he);
	HalfEdge* ELleft(HalfEdge* he);
	Site* leftreg(HalfEdge* he);
	void ELinsert(HalfEdge* lb, HalfEdge* newHe);
	void ELdelete(HalfEdge* he);
	HalfEdge* ELgethash(int b);
	HalfEdge* ELleftbnd(glm::vec2 p);
	void pushGraphEdge(Site* leftSite, Site* rightSite, float x1, float y1, float x2, float y2);
	void clip_line(Edge* e);
	void endpoint(Edge* e, int lr, Site* s);
	bool right_of(HalfEdge* el, glm::vec2 p);
	Site* rightreg(HalfEdge* he);
	float dist(Site* s, Site* t);
	Site* intersect(HalfEdge* el1, HalfEdge* el2);
	bool voronoi_bd();

	Corner* addCorner(Site* s1, Site* s2, float x, float y);
public:
	Voronoi(float minDistanceBetweenSites);
	~Voronoi();
	std::vector<GraphEdge*> generateVoronoi(std::vector<glm::vec2> points, float minX, float maxX, float minY, float maxY);
	float borderMinX, borderMaxX, borderMinY, borderMaxY;
	std::vector<GraphEdge*> allEdges;
	std::vector<Site*> sites;
	std::vector<Corner*> corners;
};


#endif