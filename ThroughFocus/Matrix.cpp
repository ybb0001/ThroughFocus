// Matrix.cpp: implementation of the CMatrix class.
//
//

//#include "stdafx.h"
#include "Matrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

//
// Construction/Destruction
//

void CMatrix::setMatrix(double *rmatr)                   //设置矩阵
{
	for (int i = 0; i < m * n; i++)
	{
		*(pMatrix + i) = rmatr[i];                          //矩阵成员赋初值
	}
}

CMatrix::CMatrix(int a, int b)                               //矩阵Matrix类的构造函数
{
	m = a; n = b;                                         //保护数据赋值
	pMatrix = new double[m * n];                    //动态分配内存
}

CMatrix::~CMatrix()                                       //矩阵Matrix类的析构函数
{
	delete[] pMatrix;                                   //内存释放
}

CMatrix *CMatrix::getRev()
{
	CMatrix *pR = new CMatrix(n, m);
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < m; i++)
			*(pR->pMatrix + i + m * j) = *(pMatrix + i * n + j);
	}
	return pR;
}

CMatrix *CMatrix::getMul(CMatrix *b)
{
	if (b->m != n)
		return 0;
	CMatrix *pR = new CMatrix(m, b->n);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < b->n; j++)
		{
			double temp = 0.0;
			for (int k = 0; k < n; k++)
				temp += *(pMatrix + i * n + k) * *(b->pMatrix + k * b->n + j);
			*(pR->pMatrix + i * b->n + j) = temp;
		}
	}

	return pR;
}

int CMatrix::getM() const
{
	return m;
}

int CMatrix::getN() const
{
	return n;
}

double *CMatrix::getMatrix() const
{
	return pMatrix;
}

double CMatrix::getData(int i, int j) const
{
	if (i < m && j < n && i >= 0 && j >= 0)
		return *(pMatrix + i * n + j);
	else
		return 0.0;
}

void CMatrix::setData(int i, int j, double t)
{
	if (i < m && j < n && i >= 0 && j >= 0)
	{
		*(pMatrix + i * n + j) = t;
	}
}