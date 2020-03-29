#pragma once
#include <iostream>
#include "Color.h"

struct Vertex
{
	double x, y;
	float r, g, b;
	Vertex();
	Vertex(double x, double y, float r, float g, float b);

	friend std::ostream& operator <<(std::ostream&, const Vertex& obj);
	bool operator!=(const Vertex v) { return x != v.x || y != v.y || r != v.r || g != v.g || b != v.b; }
	bool operator==(const Vertex v) { return !(*this != v); }
	Vertex& operator=(Vertex v) { x = v.x; y = v.y; r = v.r; g = v.g; b = v.b; return *this; }

	void setColor(Color color);
	void setPositionUsingMouse(double x, double y);
};