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
public:                                   //�ⲿ�ӿ�
	CLinequ(int dims = 2);                   //���캯��
	virtual ~CLinequ();                            //��������
	void setLinequ(double *a, double *b);  //���Ƹ�ֵ
	void setMatrix(double *rmatr);
	int Solve();                          //ȫѡ��Ԫ��˹��ȥ����ⷽ��
	double *getSolution() const;

private:
	double *sums;                         //�����Ҷ���
	double *MatrixA;
	double *solu;                         //���̵Ľ�
	int index;
};

#endif // !defined(AFX_LINEQU_H__3673E7FC_1154_436A_9D22_B472DD858F13__INCLUDED_)