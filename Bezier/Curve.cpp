#include<algorithm>
#include "Curve.h"
#include "OpenGLcore.h"
#include "Color.h"

#define M_PI 3.14159265

Curve::Curve()
{
	controlPoints = {};
	curvePoints = {};
	step = 0;
	VBOControl = 0;
	VBOCurve = 0;
	color = Color(1.0f, 0.0f, 0.0f);
}

Curve::Curve(vector<Vertex> controlPoints, vector<Vertex> curvePoints, Color color)
: controlPoints(controlPoints), curvePoints(curvePoints), step(0.1f), color(color)
{
	VBOControl = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * controlPoints.size(), controlPoints.data());
	VBOCurve = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * curvePoints.size(), curvePoints.data());
}

Curve::Curve(vector<Vertex> controlPoints, float step, Color color)
: controlPoints(controlPoints), step(step), color(color)
{
	createBeziers(curvePoints, controlPoints, step, color);
	VBOControl = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * controlPoints.size(), controlPoints.data());
	VBOCurve = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * curvePoints.size(), curvePoints.data());
}

void Curve::createBeziers(std::vector<Vertex>& tabBezierPoints, std::vector<Vertex> tabControlPoints, float step, Color choosedColor)
{
	Vertex temp = tabControlPoints[0];
	temp.setColor(choosedColor);

	//teste si il y a plus d'un point de controle
	if (tabControlPoints.size() > 2)
	{
		std::vector<Vertex> barycentre;

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
					barycentre[i].z = 0.f;
				}
			}

			barycentre[0].setColor(choosedColor);
			tabBezierPoints.push_back(barycentre[0]);
		}
	}

	temp = tabControlPoints.back();
	temp.setColor(choosedColor);
	tabBezierPoints.push_back(temp);
}

void Curve::updateCurve()
{
	Curve newCurve(this->controlPoints, this->step, this->color);
	*this = newCurve;
}

void Curve::updateBuffers()
{
	VBOControl = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * controlPoints.size(), controlPoints.data());
	VBOCurve = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * curvePoints.size(), curvePoints.data());
}

void Curve::link(Curve& c2)
{
	std::vector<Vertex>* cp1 = &this->controlPoints;

	std::vector<Vertex> newControlPoints;

	std::copy_n(cp1->begin(), cp1->size() - 1, std::back_inserter(newControlPoints));

	Vertex meanPoint(Color(1.0f, 1.0f, 1.0f));
	meanPoint.x = (cp1->back().x + c2.controlPoints[0].x) / 2;
	meanPoint.y = (cp1->back().y + c2.controlPoints[0].y) / 2;
	newControlPoints.push_back(meanPoint);

	std::copy_n(c2.controlPoints.begin() + 1, c2.controlPoints.size() - 1, std::back_inserter(newControlPoints));

	c2.clearControlPoints();
	c2.clearCurvePoints();

	this->controlPoints = newControlPoints;
	updateCurve();
}

void Curve::Transform(Matrix& mat)
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		controlPoints[i] = mat * controlPoints[i];
	}

	updateCurve();
}

void Curve::setCurveColor(Color col)
{
	for (int i = 0; i < curvePoints.size(); ++i)
	{
		curvePoints[i].setColor(col);
	}

	color = col;
	updateBuffers();
}

void Curve::setControlPoints(vector<Vertex> v)
{
	controlPoints.resize(v.size());
	for (int i = 0; i < controlPoints.size(); i++)
	{
		controlPoints[i] = v[i];
	}
}

void Curve::setControlPoints(int id, Vertex v)
{
	controlPoints[id] = v;
}

void Curve::setControlPointColor(int id, Color col)
{
	Vertex v = getControlPoints()[id];
	v.setColor(col);
	setControlPoints(id, v);
	updateBuffers();
}

void Curve::addControlPointAtIndex(int id, Vertex v)
{
	controlPoints.insert(controlPoints.begin() + id, v);
	updateCurve();
}

void Curve::setCurvePoints(std::vector<Vertex> v)
{
	curvePoints.resize(v.size());
	for (int i = 0; i < curvePoints.size(); i++)
	{
		curvePoints[i] = v[i];
	}
}

Mesh Curve::SimpleExtrude(int h, float scale, float step)
{
	Mesh m;
	vector<Vertex> curvePoints = this->getCurvePoints();

	bool close = false;
	if(close)
		curvePoints.push_back(curvePoints[0]);

	for (float t = 0; t < 1; t += step)
	{
		for (int s = 0; s < curvePoints.size(); s++)
		{
			float x = curvePoints[s].x * (1 + t * (scale - 1));
			float y = curvePoints[s].y * (1 + t * (scale - 1));
			float z = h * t;

			m.getVertices().push_back(Vertex(x, y, z));
		}
	}
	
	m.CalculateIndices(curvePoints.size(), 1 / step - 1);
	m.CalculateNormals();
	m.updateBuffers();

	return m;
}

Mesh Curve::Revolution(float step)
{
	Mesh m;

	//Recherche du point le plus pres de l'origine
	double xMin = abs(this->getCurvePoints()[0].x);
	int negative = 1;

	for (int i = 1; i < this->getCurvePoints().size(); ++i)
	{
		if (abs(this->getCurvePoints()[i].x) < xMin)
		{
			xMin = abs(this->getCurvePoints()[i].x);
			
			if (this->getCurvePoints()[i].x < 0)
				negative = -1;
			else
				negative = 1;
		}
	}

	//D�calage � l'origine
	this->Transform(Matrix::Translate(-xMin * negative, 0.f));

	//R�volution
	for (float t = 0; t < 1; t += step)
	{
		for (int s = 0; s < this->getCurvePoints().size(); s++)
		{
			float rad = 360 * t * M_PI / 180.0f;

			float x = this->getCurvePoints()[s].x * cos(rad) ;
			float y = this->getCurvePoints()[s].y;
			float z = this->getCurvePoints()[s].x * sin(rad);

			m.getVertices().push_back(Vertex(x, y, z));
		}
	}

	//Cr�ation du mesh
	m.CalculateIndices(this->getCurvePoints().size(), 1 / step - 1);
	m.CalculateNormals();

	//Retour � la position
	m.Transform(Matrix::Translate(xMin * negative, 0.f));
	
	m.updateBuffers();

	return m;
}

Mesh Curve::GenericExtrusion(Curve& path, float step)
{
	Mesh m;

	//Path positionner � l'origine
	path.Transform(Matrix::Translate(-path.getCurvePoints()[0].x, -path.getCurvePoints()[0].y, 0.f));

	//place le barycentre de la courbe sous le chemin
	float factor[2] = { 0.0f, 0.0f };
	for (int j = 0; j < this->getControlPoints().size(); j++)
	{
		factor[0] = factor[0] + this->getControlPoints()[j].x;
		factor[1] = factor[1] + this->getControlPoints()[j].y;
	}

	factor[0] = factor[0] / this->getControlPoints().size();
	factor[1] = factor[1] / this->getControlPoints().size();

	Matrix z = Matrix::Translate(-factor[0], -factor[1]);
	this->Transform(z);
	
	//Tangeante au point 0
	Vec3 T(path.curvePoints[1].x - path.curvePoints[0].x, path.curvePoints[1].y - path.curvePoints[0].y, 0);
	T = T.normalise();

	//Normale de la courbe
	Vec3 v = Vec3(curvePoints.back().x - curvePoints[0].x, curvePoints.back().y - curvePoints[0].y, 0);
	v = v.normalise();

	//angle entre la courbe et la tangente du chemin
	float angle = acos(v.dot(T) / (v.magnitude() * T.magnitude()))  * 180 / M_PI;

	//rotate en fonction de la tangente
	std::vector<Vertex> tempToRotate;
	tempToRotate.clear();
	for (int i = 0; i < curvePoints.size(); i++)
		tempToRotate.push_back(Matrix::RotateZ(angle - 90) * curvePoints[i]); 
	this->setCurvePoints(tempToRotate);
	updateBuffers();

	//rotate � 90 perpendiculaire au chemin
	tempToRotate.clear();
	for (int i = 0; i < curvePoints.size(); i++)
		tempToRotate.push_back(Matrix::RotateX(90) * curvePoints[i]);
	this->setCurvePoints(tempToRotate);
	updateBuffers();

	//Extrusion
	/*for (float t = 0; t < 1; t += step)
	{
		m.getVertices().push_back(Vertex(x, y, z));
	}
	*/


	//Cr�ation du mesh
	//m.CalculateIndices(this->getCurvePoints().size(), 1 / step - 1);
	//m.CalculateNormals();

	//m.updateBuffers();

	return m;
}


