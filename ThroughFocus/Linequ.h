// Linequ.h: interface for the CLinequ class.
//
//

#if !defined(AFX_LINEQU_H__3673E7FC_1154_436A_9D22_B472DD858F13__INCLUDED_)
#define AFX_LINEQU_H__3673E7FC_1154_436A_9D22_B472DD858F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLinequ
{
public:                                   //外部接口
	CLinequ(int dims = 2);                   //构造函数
	virtual ~CLinequ();                            //析构函数
	void setLinequ(double *a, double *b);  //方称赋值
	void setMatrix(double *rmatr);
	int Solve();                          //全选主元高斯消去法求解方程
	double *getSolution() const;

private:
	double *sums;                         //方程右端项
	double *MatrixA;
	double *solu;                         //方程的解
	int index;
};

#endif // !defined(AFX_LINEQU_H__3673E7FC_1154_436A_9D22_B472DD858F13__INCLUDED_)