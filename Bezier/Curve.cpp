#include "Curve.h"
#include "OpenGLcore.h"

Curve::Curve(vector<Vertex> controlPoints, vector<Vertex> curvePoints)
: controlPoints(controlPoints), curvePoints(curvePoints), step(0.1f) 
{
	VBOControl = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * controlPoints.size(), controlPoints.data());
	VBOCurve = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * curvePoints.size(), curvePoints.data());
}


Curve::Curve(vector<Vertex> controlPoints, float step)
: controlPoints(controlPoints), step(step)
{
	createBeziers(curvePoints, controlPoints, step);
	VBOControl = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * controlPoints.size(), controlPoints.data());
	VBOCurve = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * curvePoints.size(), curvePoints.data());
}

void Curve::createBeziers(std::vector<Vertex>& tabBezierPoints, std::vector<Vertex> tabControlPoints, float step)
{
	std::vector<Vertex> barycentre;

	//teste si il y a plus d'un point de controle
	if (tabControlPoints.size() > 2)
	{
		for (float t = 0; t < 1; t += step)
		{
			//reset des barycentres
			barycentre = tabControlPoints;

			for (int j = 1; j < tabControlPoints.size(); j++)
			{
				for (int i = 0; i < tabControlPoints.size() - j; i++)
				{
					barycentre[i].x = (1 - t) * barycentre[i].x + t * barycentre[i + 1].x;
					barycentre[i].y = (1 - t) * barycentre[i].y + t * barycentre[i + 1].y;
				}
			}

			tabBezierPoints.push_back(barycentre[0]);
		}

	}

	//sinon, on calcule pas de barycentre et on retourne les deux point
	else
	{
		tabBezierPoints.push_back(tabControlPoints[0]);
		tabBezierPoints.push_back(tabControlPoints[1]);
	}
}