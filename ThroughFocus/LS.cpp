// LS.cpp: implementation of the CLS class.
//
//

//#include "stdafx.h"
#include "LS.h"
#include "Matrix.h"
#include "Linequ.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

//
// Construction/Destruction
//

CLS* CLS::_instance = 0;

CLS::CLS()
{
	pSolution = 0;
	m = 0;
	n = 0;
	_instance = this;
}

CLS::~CLS()
{
	if (pSolution)
		delete[] pSolution;
}

CLS *CLS::getInstance()
{
	if (!_instance)
		new CLS();
	return _instance;
}

void CLS::setN(int t)
{
	n = t + 1;
	if (pSolution)
		delete[] pSolution;
	pSolution = new double[n];
}

void CLS::addPoint(double x, double y)
{
	pVertex[m][0] = x;
	pVertex[m][1] = y;
	m++;
}

bool CLS::Solve()
{
	if (m <= 0 || n <= 0)
		return false;
	CMatrix *A = new CMatrix(m, n);
	int i, j;
	for (j = 0; j < m; j++)
		A->setData(j, 0, 1.0);
	for (i = 1; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			A->setData(j, i, A->getData(j, i - 1) * pVertex[j][0]);
		}
	}
	CMatrix *B = A->getRev();
	CMatrix *b = new CMatrix(m, 1);
	for (i = 0; i < m; i++)
		b->setData(i, 0, pVertex[i][1]);
	CMatrix *C = B->getMul(A);
	CMatrix *d = B->getMul(b);

	CLinequ *pL = new CLinequ(n);
	pL->setLinequ(C->getMatrix(), d->getMatrix());
	pL->Solve();
	double *t = pL->getSolution();
	for (i = 0; i < n; i++)
		pSolution[i] = t[i];

	return true;
}

double *CLS::getSolution() const
{
	return pSolution;
}

double CLS::calcY(double x)
{
	double y = 0.0, temp = 1.0;
	for (int i = 0; i < n; i++)
	{
		y += pSolution[i] * temp;
		temp *= x;
	}
	return y;
}

void CLS::restart()
{
	m = 0;
	if (pSolution)
		delete[] pSolution;
	pSolution = 0;
	n = 0;
}
