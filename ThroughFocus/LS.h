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

	double pVertex[2000][2];//���������2000����
	int m;	//��ĸ���
	int n;	//����ʽ����
	double *pSolution;	//����ʽϵ��

public:
	CLS();
	virtual ~CLS();
	static CLS *getInstance();



	void setN(int t);	//n�ζ���ʽ���
	void addPoint(double x, double y);	//���һ����
	void restart();

	bool Solve();
	double *getSolution() const;	//��ö���ʽϵ��
	double calcY(double x);	//����x�������y
};

#endif // !defined(AFX_LS_H__208D279A_F391_4DA8_BBE3_3895A9800FFE__INCLUDED_)