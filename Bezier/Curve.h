#pragma once
#include <vector>
#include "Vertex.h"
#include "Matrix.h"
#include "Mesh.h"

using namespace std;

enum class CurveType {
	None,
	Bezier,
	Chaikin
};

class Curve
{

	vector<Vertex> controlPoints;
	vector<Vertex> curvePoints;
	float step;
	uint32_t VBOControl;
	uint32_t VBOCurve;
	Color color;

	float u;
	float v;
	float iteration;

	CurveType type;

public:
	Curve();
	Curve(vector<Vertex> controlPoints, vector<Vertex> curvePoints, Color col, CurveType type);
	//Chaikin curves
	Curve(vector<Vertex> controlPoints, float u, float v, float iteration, Color color);
	//Beziers curves
	Curve::Curve(vector<Vertex> controlPoints, float s, Color color);

	void createBeziers(std::vector<Vertex>& curvePoints, std::vector<Vertex> controlPoints, float step, Color choosedColor);
	void updateCurve();
	void updateBuffers();
	void link(Curve& c2);

	//Matrix
	void Transform(Matrix& mat);
	void TransformCurvePoint(Matrix& mat);

	//Curve point functions
	void clearCurvePoints() { curvePoints.clear(); }
	void setCurveColor(Color col);

	//Control points functions
	void clearControlPoints() { controlPoints.clear(); }
	void setControlPoints(vector<Vertex> v);
	void setControlPoints(int id, Vertex v);
	void setControlPointColor(int id, Color col);
	void addControlPointAtIndex(int id, Vertex v);

	void setCurvePoints(std::vector<Vertex> v);

	vector<Vertex> getControlPoints()const { return controlPoints; }
	vector<Vertex> getCurvePoints()const { return curvePoints; }
	uint32_t getVBOControl()const { return VBOControl; }
	uint32_t getVBOCurve()const { return VBOCurve; }
	Color getColor() const { return color; }

	//Extrusion
	Mesh SimpleExtrude(int h, float scale, float step);
	Mesh Revolution(float step);
	Mesh GenericExtrusion(Curve& path);
	
	//CUTTINGS
	std::vector<Vertex> cornerCuttings(std::vector<Vertex> tabPoints, float u, float v, int nbreIteration);
};

