#pragma once
#include "DImage.h"

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

public:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	// moje promenljive i funkcije ----------------------------------

	void UpdateCamera();

	double cameraDist = 0;
	double cameraBeta = 0;
	double cameraAlpha = 0;

	UINT tekstura;
	UINT teksturaOkolina[6];

	UINT LoadTexture(char* fileName);
	void DrawAxes();
	void DrawEnvCube(double a);

	void DrawCone(double r, double h, int n);
	void DrawSphere(double r, int nSeg, double texU, double texV, double texR);
	void DrawCylinder(double r1, double r2, double h, int nSeg);
	void PolarToCertesian(double R, double phi, double theta, double& x, double& y, double& z);
	void PolarToCertesian2D(double R, double phi, double& x, double& y);

	void SetMaterial();
};
