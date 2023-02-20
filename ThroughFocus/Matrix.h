#if !defined(AFX_MATRIX_H__AEE89FA3_05E2_44AC_AA96_5FBCB3608C13__INCLUDED_)
#define AFX_MATRIX_H__AEE89FA3_05E2_44AC_AA96_5FBCB3608C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMatrix
{
public:
	CMatrix(int a = 2, int b = 2);
	virtual ~CMatrix();
	void setMatrix(double *rmatr);

	CMatrix *getRev();
	CMatrix *getMul(CMatrix *b);

	int getM() const;	//获得行数
	int getN() const;	//获得列数
	double getData(int i, int j) const;
	void setData(int i, int j, double t);
	double *getMatrix() const;	//获得矩阵

protected:
	int m, n;
	double *pMatrix;
};

#endif
