// LS.h: interface for the CLS class.
//
//

#if !defined(AFX_LS_H__208D279A_F391_4DA8_BBE3_3895A9800FFE__INCLUDED_)
#define AFX_LS_H__208D279A_F391_4DA8_BBE3_3895A9800FFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLS
{
private:
	static CLS *_instance;

	double pVertex[2000][2];//最多可以拟合2000个点
	int m;	//点的个数
	int n;	//多项式次数
	double *pSolution;	//多项式系数

public:
	CLS();
	virtual ~CLS();
	static CLS *getInstance();



	void setN(int t);	//n次多项式拟合
	void addPoint(double x, double y);	//添加一个点
	void restart();

	bool Solve();
	double *getSolution() const;	//获得多项式系数
	double calcY(double x);	//根据x坐标计算y
};

#endif // !defined(AFX_LS_H__208D279A_F391_4DA8_BBE3_3895A9800FFE__INCLUDED_)