// Linequ.cpp: implementation of the CLinequ class.
//
//

//#include "stdafx.h"
#include "math.h"
#include "Linequ.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

//
// Construction/Destruction
//

CLinequ::CLinequ(int dims)
{
	index = dims;
	sums = new double[dims];
	MatrixA = new double[dims * dims];
	solu = new double[dims];
}

CLinequ::~CLinequ()
{
	delete[] sums;
	delete[] MatrixA;
	delete[] solu;
}

void CLinequ::setMatrix(double *rmatr)                   //���þ���
{
	for (int i = 0; i<index*index; i++) {
		*(MatrixA + i) = rmatr[i];                          //�����Ա����ֵ
	}
}

void CLinequ::setLinequ(double *a, double *b)             //�������Է�����
{
	setMatrix(a);                                       //���û��ຯ��
	for (int i = 0; i<index; i++)
		sums[i] = b[i];
}

int CLinequ::Solve()                                     //ȫѡ��Ԫ��˹��ȥ����ⷽ��
{
	int *js, l, k, i, j, is, p, q;
	double d, t;
	js = new int[index];
	l = 1;
	for (k = 0; k <= index - 2; k++) {                             //��ȥ����
		d = 0.0;
		for (i = k; i <= index - 1; i++)
			for (j = k; j <= index - 1; j++) {
				t = fabs(MatrixA[i*index + j]);
				if (t>d)
				{
					d = t; js[k] = j; is = i;
				}
			}
		if (d + 1.0 == 1.0) l = 0;
		else
		{
			if (js[k] != k)
				for (i = 0; i <= index - 1; i++) {
					p = i*index + k; q = i*index + js[k];
					t = MatrixA[p]; MatrixA[p] = MatrixA[q]; MatrixA[q] = t;
				}
			if (is != k)
			{
				for (j = k; j <= index - 1; j++) {
					p = k*index + j; q = is*index + j;
					t = MatrixA[p]; MatrixA[p] = MatrixA[q]; MatrixA[q] = t;
				}
				t = sums[k]; sums[k] = sums[is]; sums[is] = t;
			}
		}
		if (l == 0)
		{
			delete[] js;
			//fail to solve
			return(0);
		}
		d = MatrixA[k*index + k];
		for (j = k + 1; j <= index - 1; j++) {
			p = k*index + j; MatrixA[p] = MatrixA[p] / d;
		}
		sums[k] = sums[k] / d;
		for (i = k + 1; i <= index - 1; i++) {
			for (j = k + 1; j <= index - 1; j++) {
				p = i*index + j;
				MatrixA[p] = MatrixA[p] - MatrixA[i*index + k] * MatrixA[k*index + j];
			}
			sums[i] = sums[i] - MatrixA[i*index + k] * sums[k];
		}
	}
	d = MatrixA[(index - 1)*index + index - 1];
	if (fabs(d) + 1.0 == 1.0)
	{
		delete[] js;
		//fail to solve
		return(0);
	}
	solu[index - 1] = sums[index - 1] / d;                               //�ش�����
	for (i = index - 2; i >= 0; i--) {
		t = 0.0;
		for (j = i + 1; j <= index - 1; j++)
			t = t + MatrixA[i*index + j] * solu[j];
		solu[i] = sums[i] - t;
	}
	js[index - 1] = index - 1;
	for (k = index - 1; k >= 0; k--)
		if (js[k] != k)
		{
			t = solu[k]; solu[k] = solu[js[k]]; solu[js[k]] = t;
		}
	delete[] js;
	return(1);
}

double *CLinequ::getSolution() const
{
	return solu;
}