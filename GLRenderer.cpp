#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")
#define _USE_MATH_DEFINES
#include <math.h>

#define TO_RAD 3.14 / 180.0f
#define TO_DEG 180.0f / 3.14

CGLRenderer::CGLRenderer(void)
{
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	tekstura = LoadTexture("ime.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	UpdateCamera();

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------

	// ================================================
	// SVETLO 0 - u beskonacnosti, uvek iza posmatraca
	// postavljamo PRE kamere da prati posmatraca
	// ================================================
	glEnable(GL_LIGHT0);

	// w=0 znaci direkciono svetlo (u beskonacnosti), w=1 bi bilo poziciono
	// x,y,z definisu pravac iz kog dolazi svetlost
	// (0,0,1) znaci da dolazi iz +z pravca, tj. iza posmatraca
	float pozicijaLight0[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, pozicijaLight0);

	// ambijentalna - osnovna osvetljenost bez izvora, veci broj = svetlije u senci
	float ambiLight0[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambiLight0);

	// difuzna - osnovna boja svetlosti, veci broj = jace svetlo
	// promeni rgb vrednosti za obojeno svetlo npr (1,0,0) = crveno
	float difLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difLight0);

	// spekularna - boja odsjaja, obicno bela za metalne povrsine
	float specLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specLight0);

	// --- ovde pozivas UpdateCamera() ili gluLookAt() ---

	// ================================================
	// SVETLO 1 - na fiksnoj poziciji u sceni
	// postavljamo POSLE kamere da bude fiksirano u svetu
	// ================================================
	glEnable(GL_LIGHT1);

	// w=1 znaci poziciono svetlo, x,y,z su koordinate u sceni
	// promeni x,y,z da premestis svetlo na drugu poziciju
	float pozicijaLight1[] = { 0.0f, 5.0f, -10.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, pozicijaLight1);

	// ambijentalna - isto kao gore
	float ambiLight1[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambiLight1);

	// difuzna - boja svetlosti
	float difLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difLight1);

	// spekularna - boja odsjaja
	float specLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_SPECULAR, specLight1);

	// atenuacija - koliko svetlo slabi sa rastojanjem (samo za poziciona svetla w=1)
	// GL_CONSTANT_ATTENUATION  - konstantno slabljenje (default 1.0, ne menjaj osim ako znas)
	// GL_LINEAR_ATTENUATION    - linearno slabljenje sa rastojanjem (default 0.0, povecaj za brzo slabljenje)
	// GL_QUADRATIC_ATTENUATION - kvadratno slabljenje (default 0.0, najrealisticnije)
	// primer: svetlo koje brzo slabi
	// glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
	// glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);


	// ================================================
// MATERIJAL
// ================================================

// ambijentalna - boja materijala u senci, obicno tamnija verzija difuzne
// veci broj = svetliji u senci
	float ambMat[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambMat);

	// difuzna - osnovna boja materijala, ovo je boja koju vidis kad svetlost pada na objekat
	// promeni rgb za drugaciju boju npr (1,0,0) = crveni materijal
	float difMat[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, difMat);

	// spekularna - boja odsjaja
	// (1,1,1) = beli odsjaj (metal), (r,g,b) = obojen odsjaj (plastika, staklo)
	// (0,0,0) = nema odsjaja (mat materijal)
	float specMat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specMat);

	// sjaj - ostrina odsjaja
	// 0   = rasprsen odsjaj (mat)
	// 128 = vrlo uzak i ostar odsjaj (sjajni metal)
	// tipicne vrednosti: drvo=10, plastika=32, metal=64-128
	float sjaj = 64.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, sjaj);

	// emisiona - materijal koji sam isijava svetlost (npr sijalica, ekran)
	// (0,0,0) = ne isijava nista (default)
	// (1,1,1) = isijava belu svetlost, izgleda kao da sija
	// VAZNO: emisija ne osvetljava okolne objekte, samo materijal izgleda svetliji
	float emiMat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emiMat);

	// GL_FRONT - primenjuje materijal samo na prednju stranu poligona
	// GL_BACK  - samo na zadnju stranu
	// GL_FRONT_AND_BACK - na obe strane (sporije)

	// ----------------------------------------------------------

	glFlush();
	SwapBuffers(pDC->m_hDC);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// fov se menja
	gluPerspective(50, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &tekstura);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT idTeksture;
	DImage tekstura;
	tekstura.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &idTeksture);

	glBindTexture(GL_TEXTURE_2D, idTeksture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tekstura.Width(), tekstura.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, tekstura.GetDIBBits());
	return idTeksture;
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2);

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	// boje se mozda menjaju

	glColor3f(1, 0, 0);		// x - crvena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);		// y - zelena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);

	glColor3f(0, 0, 1);		// z - plava osa
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glEnd();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	//glDisable(GL_LIGHTING);

	double h = a / 2;

	// prednji deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-h, h, -h);
	glTexCoord2f(0, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(1, 1); glVertex3f(h, -h, -h);
	glTexCoord2f(1, 0); glVertex3f(h, h, -h);
	glEnd();

	// levi deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-h, h, h);
	glTexCoord2f(0, 1); glVertex3f(-h, -h, h);
	glTexCoord2f(1, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(1, 0); glVertex3f(-h, h, -h);
	glEnd();

	// desni deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(h, h, -h);
	glTexCoord2f(0, 1); glVertex3f(h, -h, -h);
	glTexCoord2f(1, 1); glVertex3f(h, -h, h);
	glTexCoord2f(1, 0); glVertex3f(h, h, h);
	glEnd();

	// zadnji deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(h, h, h);
	glTexCoord2f(0, 1); glVertex3f(h, -h, h);
	glTexCoord2f(1, 1); glVertex3f(-h, -h, h);
	glTexCoord2f(1, 0); glVertex3f(-h, h, h);
	glEnd();

	// gornji deo (top)
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-h, h, h);
	glTexCoord2f(0, 0); glVertex3f(-h, h, -h);
	glTexCoord2f(1, 0); glVertex3f(h, h, -h);
	glTexCoord2f(1, 1); glVertex3f(h, h, h);
	glEnd();

	// donji deo (bot)
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(0, 0); glVertex3f(-h, -h, h);
	glTexCoord2f(1, 0); glVertex3f(h, -h, h);
	glTexCoord2f(1, 1); glVertex3f(h, -h, -h);
	glEnd();

	/*
	
	// --- PREDNJA strana (FRONT) --- col=1, row=0
	// U: [1/4, 2/4], V: [0, 1/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 0.0);			glVertex3f(-h, h, -h);
	glTexCoord2f(1.0 / 4, 1.0 / 3);     glVertex3f(-h, -h, -h);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glTexCoord2f(2.0 / 4, 0.0);			glVertex3f(h, h, -h);
	glEnd();

	// --- DESNA strana (RIGHT) --- col=2, row=1
	// U: [2/4, 3/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glTexCoord2f(3.0 / 4, 2.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(3.0 / 4, 1.0 / 3);     glVertex3f(h, h, -h);
	glEnd();

	// --- ZADNJA strana (BACK) --- col=1, row=3
	// U: [1/4, 2/4], V: [2/3, 3/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 2.0 / 3);     glVertex3f(-h, -h, h);
	glTexCoord2f(1.0 / 4, 3.0 / 3);     glVertex3f(-h, h, h);
	glTexCoord2f(2.0 / 4, 3.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glEnd();

	// --- LEVA strana (LEFT) --- col=0, row=1
	// U: [0, 1/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 1.0 / 3);		glVertex3f(-h, -h, -h);
	glTexCoord2f(1.0 / 4, 2.0 / 3);		glVertex3f(-h, -h, h);
	glTexCoord2f(0.0, 2.0 / 3);			glVertex3f(-h, h, h);
	glTexCoord2f(0.0, 1.0 / 3);			glVertex3f(-h, h, -h);
	glEnd();

	// --- GORNJA strana (TOP) --- col=4, row=1
	// U: [3/4, 4/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(4.0 / 4, 1.0 / 3);     glVertex3f(-h, h, -h);
	glTexCoord2f(4.0 / 4, 2.0 / 3);     glVertex3f(-h, h, h);
	glTexCoord2f(3.0 / 4, 2.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(3.0 / 4, 1.0 / 3);     glVertex3f(h, h, -h);
	glEnd();

	// --- DONJA strana (BOTTOM) --- col=1, row=1
	// U: [1/4, 2/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 1.0 / 3);     glVertex3f(-h, -h, -h);
	glTexCoord2f(1.0 / 4, 2.0 / 3);     glVertex3f(-h, -h, h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glEnd();

	*/

	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double ugaoKorak = M_PI * 2 / n;

	double normalY = r / sqrt(pow(r, 2) + pow(h, 2));
	double normalXZ = h / sqrt(pow(r, 2) + pow(h, 2));

	glBegin(GL_TRIANGLE_FAN);

	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);

	for (int i = 0; i <= n; i++)
	{
		double ugao = i * ugaoKorak;

		double x = r * cos(ugao);
		double z = r * sin(ugao);

		double nx = cos(ugao) * normalXZ;
		double nz = sin(ugao) * normalXZ;

		// ovo je kad se prosledi centar teksture i poluprecnik
		// a izracunava se krug oko toga i mapira
		// kruzno mapiranje teksture, moze da se iskoristi i za bazu

		/*double tx = texR * x / r + texU;
		double ty = texR * z / r + texV;*/

		//glTexCoord2f(tx, ty);

		glNormal3f(nx, normalY, nz);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, 0);

	for (int i = n; i >= 0; i--)
	{
		double angle = i * ugaoKorak;

		// mozda moze i ovde mapiranje teksutre

		glVertex3f(r * cos(angle), 0, r * sin(angle));
	}
	glEnd();
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	double dAng1 = M_PI / (double)nSeg;
	double dAng2 = 2 * M_PI / (double)nSeg;
	double ang1 = -M_PI / 2.0;
	double ang2;

	for (int i = 0; i < nSeg; i++)
	{
		ang2 = 0;

		glBegin(GL_QUAD_STRIP);

		for (int j = 0; j < nSeg + 1; j++)
		{

			// moze i ovako ako treba ova funkcija
			PolarToCertesian(r, ang1, ang2, x1, y1, z1);
			double x1 = r * cos(ang1) * cos(ang2);
			double y1 = r * sin(ang1);
			double z1 = r * cos(ang1) * sin(ang2);

			double x2 = r * cos(ang1 + dAng1) * cos(ang2);
			double y2 = r * sin(ang1 + dAng1);
			double z2 = r * cos(ang1 + dAng1) * sin(ang2);

			// tekstura
			double tx1 = texR * x1 / r + texU;
			double ty1 = texR * z1 / r + texV;

			double tx2 = texR * x2 / r + texU;
			double ty2 = texR * z2 / r + texV;

			glTexCoord2f(tx1, ty1);
			glVertex3f(x1, y1, z1);

			glTexCoord2f(tx2, ty2);
			glVertex3f(x2, y2, z2);

			ang2 += dAng2;
		}
		glEnd();
		ang1 += dAng1;
	}
}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tekstura);
	glColor3f(1, 1, 1);

	// ako ne treba na teksutra izbaciti sve za teksturu i ostaviti ovo
	// glDisable(GL_TEXTURE_2D);
	// glColor3f(r, g, b);

	double korak = 2 * M_PI / nSeg;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		
		// iz lampe
		float u = (float)i / (float)nSeg; // horizontalna koordinata teksture

		glTexCoord2f(u, 0.5);
		glVertex3d(r2 * x, h, r2 * z);

		glTexCoord2f(u, 0.5);
		glVertex3d(r1 * x, 0, r1 * z);
	}
	glEnd();

	// donja baza
	glBegin(GL_TRIANGLE_FAN);
	//glTexCoord2f(0.5f, 0.25f);
	glVertex3d(0, 0, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		// float u = 0.5f + 0.5f * x;		// horizontalna koordinata teksture
		// float v = 0.25f + 0.25f * z;	// vertikalna koordinata teksture
		// glTexCoord2f(u, v);
		glVertex3d(r1 * x, 0, r1 * z);
	}
	glEnd();

	// gornja baza
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.25f);
	glVertex3d(0, h, 0); // OVDE RAZLIKA U H
	for (int i = 0; i <= nSeg; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		// float u = 0.5f + 0.5f * x;		// horizontalna koordinata teksture
		// float v = 0.25f + 0.25f * z;	// vertikalna koordinata teksture
		// glTexCoord2f(u, v);
		glVertex3d(r2 * x, h, r2 * z);		// OVDE RAZLIKA U H
	}
	glEnd();
}

void CGLRenderer::PolarToCertesian(double R, double phi, double theta, double& x, double& y, double& z)
{
	x = R * cos(phi) * cos(theta);
	y = R * sin(phi);
	z = R * cos(phi) * sin(theta);
}

void CGLRenderer::PolarToCertesian2D(double R, double phi, double& x, double& y)
{
	x = R * cos(phi);
	y = R * sin(phi);
}

void CGLRenderer::SetMaterial()
{
	// ================================================
	// MATERIJAL
	// ================================================
	
	// ambijentalna - boja materijala u senci, obicno tamnija verzija difuzne
	// veci broj = svetliji u senci
	float ambMat[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambMat);

	// difuzna - osnovna boja materijala, ovo je boja koju vidis kad svetlost pada na objekat
	// promeni rgb za drugaciju boju npr (1,0,0) = crveni materijal
	float difMat[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, difMat);

	// spekularna - boja odsjaja
	// (1,1,1) = beli odsjaj (metal), (r,g,b) = obojen odsjaj (plastika, staklo)
	// (0,0,0) = nema odsjaja (mat materijal)
	float specMat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specMat);

	// sjaj - ostrina odsjaja
	// 0   = rasprsen odsjaj (mat)
	// 128 = vrlo uzak i ostar odsjaj (sjajni metal)
	// tipicne vrednosti: drvo=10, plastika=32, metal=64-128
	float sjaj = 64.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, sjaj);

	// emisiona - materijal koji sam isijava svetlost (npr sijalica, ekran)
	// (0,0,0) = ne isijava nista (default)
	// (1,1,1) = isijava belu svetlost, izgleda kao da sija
	// VAZNO: emisija ne osvetljava okolne objekte, samo materijal izgleda svetliji
	float emiMat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emiMat);

	// GL_FRONT - primenjuje materijal samo na prednju stranu poligona
	// GL_BACK  - samo na zadnju stranu
	// GL_FRONT_AND_BACK - na obe strane (sporije)
}


//void CGLRenderer::NacrtajZvezdu(double r1, double r2, int n, double dubina)
//{
//
//	double korak = 2 * M_PI / (double)n;        // ugao izmedju dva zupca
//	double polaKoraka = korak / 2;
//
//	glBegin(GL_TRIANGLE_FAN);
//	glNormal3f(0, 0, 1);
//	glVertex3f(0, 0, dubina / 2);
//
//	double ugaoPret = 0;
//
//	for (int i = 0; i <= n; i++)
//	{
//		double ugao = i * korak;
//		glVertex3f(r1 * cos(ugao - polaKoraka), r1 * sin(ugao - polaKoraka), 0);
//		glVertex3f(r2 * cos(ugao), r2 * sin(ugao), 0);
//		ugaoPret = ugao;
//	}
//	glEnd();
//
//	glBegin(GL_TRIANGLE_FAN);
//	glNormal3f(0, 0, -1);
//	glVertex3f(0, 0, -dubina / 2);
//
//	for (int i = 0; i <= n; i++)
//	{
//		double ugao = i * korak;
//		glVertex3f(r1 * cos(ugao - polaKoraka), r1 * sin(ugao - polaKoraka), 0);
//		glVertex3f(r2 * cos(ugao), r2 * sin(ugao), 0);
//		ugaoPret = ugao;
//	}
//	glEnd();
//}