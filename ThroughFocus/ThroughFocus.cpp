#include <afx.h>
#include "ThroughFocus.h"
#include "mythread.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <direct.h>
#include <io.h>
#include <QIcon>
#include <fstream>
#include <qdatetime.h>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <cmath>
#include<sstream>
#include<afxtempl.h>
#include "Linequ.h"
#include "LS.h"
#include "Matrix.h"
#include <vector>

using namespace std;
using namespace cv;

vector<double> fitCurve(vector<double> arr_1, vector<double> arr_2, int n);
vector<double> getFitPoint(vector<double> coArr, vector<double> pointArr);

ofstream fout, curve_Data, debug_log;
ofstream TF_Check_Result;
ifstream fin;
bool start = true;
bool manual = false, show_Diagram = false, move_Center = false, AA_SFR = false;
bool Initial_log = true;
bool F3 = false, F5 = false, F6 = false, F7 = false, F8 = false, Curve_Fit = false, MTF = false;
int OK[11] = { 0 }, SFR_Table[65][65], F_POS[10] = { 0 };
unsigned int Item_Select = 0;
int mode = 1, TF_step = 41, Field_Cnt = 38, center_Field = 0, center_Max_Step = 0;
int startCode = 0, Skip_Data = 0, Line_pair=90, Module_No=0;
int PD_Spec[10][3] = { 0 }, PS_Spec[10][3] = { 0 }, OK_Range_Spec = 4, current_Step = 0, HVD_Spec[10] = { 0 };
int Center_H = 0, Center_V = 0, Dia_H3 = 0, Dia_V3 = 0, LR_H3 = 0, LR_V3 = 0, Dia_H5 = 0, Dia_V5 = 0;
int Dia_H6 = 0, Dia_V6 = 0, Dia_H7 = 0, Dia_V7 = 0, Dia_H8 = 0, Dia_V8 = 0, Dia_H9 = 0, Dia_V9 = 0;
float umPerCode = 0;
string skip_Words[5] = { "" };
string del_TF_Setting_Words[3] = { "temp","log","mes" };
string src = "", dst = "", src2 = "", dst2 = "", src3 = "", TF_Data_Path = "";
string upper_src = "", upper_src2 = "", upper_src3 = "";
string setting = "", setting2 = "", setting3 = "", last_Time = "";
LPCWSTR Lsetting = TEXT("");
HANDLE myHandle;

MyThread *subThread = new MyThread;

//typedef CArray<double, double> CDoubleArray;

typedef struct {
	float Peak_Code = 0;
	float Peak_CodeHV = 0;
	int Field = 0;
	float min_Code = 9999999;
	float max_Code = 0;
	float PD = 0, PD_AVG = 0;
}FD;

FD Field_Data[65];
FD Field_Data_AVG[65];

typedef struct {
	string fuseID = "0000000000000000000000000000000";
	string time = "0000000000000000000000000000000";
	int step = -1, code = 0;
	float sfr[65];
	float sfr_AVG[65];
}TD;

TD TF_Data[65];

typedef struct {
	int min[3] = { 0 };
	int min_index[3] = { 0 };
	int max[3] = { 0 };
	int max_index[3] = { 0 };
	int PD_AVG[3] = { 0 };
	int group[16] = { 0 };
	int cnt = 0;
	float offset[4] = { 0 }, offsetHV[4] = { 0 };
}PD;

PD PD_Data[10];

float min(float a, float b) {
	return a < b ? a : b;
}
float max(float a, float b) {
	return a > b ? a : b;
}

Mat polyfit(vector<Point2d>& in_point, int n)
{
	int size = in_point.size();
	//所求未知数个数
	int x_num = n + 1;
	//构造矩阵U和Y
	Mat mat_u(size, x_num, CV_64F);
	Mat mat_y(size, 1, CV_64F);

	for (int i = 0; i < mat_u.rows; ++i)
		for (int j = 0; j < mat_u.cols; ++j)
		{
			mat_u.at<double>(i, j) = pow(in_point[i].x, j);
		}

	for (int i = 0; i < mat_y.rows; ++i)
	{
		mat_y.at<double>(i, 0) = in_point[i].y;
	}

	//矩阵运算，获得系数矩阵K
	Mat mat_k(x_num, 1, CV_64F);
	mat_k = (mat_u.t()*mat_u).inv()*mat_u.t()*mat_y;
	cout << mat_k << endl;
	return mat_k;
}


int data_Parse(string line) {

	int len = line.length(), step = current_Step--;
	string s = "";
	string fuseID = "";
	int i = 0, d = 0;;

	while (line[i] != '\t') {
		s += line[i++];
	}

	TF_Data[step].time = s;
	if (step == TF_step - 1) {

		if (!last_Time.compare(s)) {
			return -2;
		}
		else {
			last_Time = s;
		}
	}
	i++;
	d = 0;
	if (TF_Data[step].time.length() > 15) {
		TF_Data[step].fuseID = s;
	}
	else {
		while (line[i] != '\t') {

			fuseID += line[i++];
		}
		i++;
		TF_Data[step].fuseID = fuseID;
	}

	TF_Data[step].step = step;
	int f = 0, k = Skip_Data;

	s = "0000000000000000000";
	while (i < len) {
		d = 0;
		while (i < len&&line[i] != '\t') {
			if (line[i] == '(')
				break;
			s[d++] = line[i++];
		}
		if (d > 9) {
			d = 0;	i++;
			while (i < len&&line[i] != '\t') {
				if (line[i] == '(')
					break;
				s[d++] = line[i++];
			}
		}
		s[d] = '\0';
		int c = stoi(s);
		if (!MTF&&c > 45) {
			TF_Data[step].code = stoi(s);
			if (i > 50)
				break;
		}
		while (i < len&&line[i] != '\t')i++;
		i++;

		if (k > 0) {
			k--;
			continue;
		}

		float data_1 = stof(s);
		if (MTF&&data_1 < 90)
			data_1 /= 100;

		if ((data_1 > 0.99|| data_1==0)&&i < 51)
			continue;

		//if (!MTF&&data_1 > 0.99)
		//	continue;
		//else if (MTF&&data_1 > 1000)
		//	continue;

		TF_Data[step].sfr[f++] = data_1;

		if (f % 2 == 0)
			TF_Data[step].sfr_AVG[f - 2] = (TF_Data[step].sfr[f - 2] + TF_Data[step].sfr[f - 1]) / 2;

		if (TF_Data[step].sfr[f - 1] > 1 || (TF_Data[step].sfr[f - 1] == 0 && f == 1)) {   // SFR = 0 case
			if (f == 1)f--;
			else {
				TF_Data[step].code = TF_Data[step].sfr[--f];
				while (i < len) i++;
				break;
			}
		}
	}

	if (step < TF_step - 1 && TF_Data[step].code >= TF_Data[step + 1].code) {
		if (TF_Data[step].code - 30 >= TF_Data[step + 1].code) {
			TF_Data[step + 1] = TF_Data[step];
			current_Step++;
		}
		else
			return -2;
	}
	if (step > 0 && f != Field_Cnt)
		return -1;


	for (int i = 1; i < 65; i++) {
		int f = Field_Data[i].Field / 10;
		int f2 = f;
		float H = 0, V = 0;
		if (f2 > 10)
			f2 = f / 10;
		if (Field_Data[i].Field % 10 == 1 && F_POS[f2] == 7) {
			H = TF_Data[step].sfr[i];
			for (int j = 1; j < 65; j++) {
				if (Field_Data[j].Field / 10 == Field_Data[i].Field / 10 && i != j) {
					if (Field_Data[j].Field % 2 == 1) {
						H = (TF_Data[step].sfr[j] + H) / 2;
					}
					else {
						if (V == 0) {
							V = TF_Data[step].sfr[j];
						}
						else {
							V = (TF_Data[step].sfr[j] + V) / 2;
						}
					}
				}
			}
			TF_Data[step].sfr_AVG[i] = (H + V) / 2;
		}
	}
	int a = 0, b = 0;
	for (int i = 0; i < 65; i++) {
		int f = Field_Data[i].Field / 10;
		int f2 = Field_Data[i].Field % 10;
	
		if (f == 1 && f2 == 1)
			a = b = i;

		if (f == 1 && f2 == 3)
			b = i;
	}
	TF_Data[step].sfr_AVG[b] = (TF_Data[step].sfr_AVG[a] + TF_Data[step].sfr_AVG[b]) / 2;
	return 0;
}


int TF_Data_Check(int mode) {

	if (!fin) {
		QMessageBox msgBox;
		msgBox.setText(QObject::tr("Load TF Data Fail, please check Data Path"));
		msgBox.exec();
		return -1;
	}

	int n = 0, ret = 0, m = 0;
	vector<string> result;
	std::string  line;

	if (mode <= 1) {
		// 先倒回文件末尾两个字符
		fin.seekg(-3, fin.cur);
	}
	else {
		fin.seekg(-3, fin.cur);
	}

	int lineCount = 65, step = 0, s = TF_step - 1;

	for (int i = 0; i < lineCount; i++)
	{
		m = 0;
		// 查看前一个字符是否为回车符
		while (fin.peek() != fin.widen('\n'))
		{
			fin.seekg(-1, fin.cur);
			m++;
			if (m > 1000)
				return -3;
		}
		// 走到这里表示跳过一行了，所以继续跳直到够100行
		fin.seekg(2, fin.cur);
		getline(fin, line);

		fin.seekg(-5 - m, fin.cur);
		if (line.length() < 100) {
			int back = -1 * line.length();
			fin.seekg(back, fin.cur);

			continue;
		}
		ret = data_Parse(line);

		if (ret != 0) {
			if (ret == -2) {
				fin.seekg(-3, fin.cur);
				m = 0;
				while (fin.peek() != fin.widen('\n'))
				{
					fin.seekg(-1, fin.cur);
					m++;
					if (m > 1000)
						return -3;
				}

			}
			return ret;
		}

		if (current_Step < 0) {
			break;
		}

	}
	if (current_Step >= 0)
		return -3;

	return 0;
}


float find_Spec(int i) {

	int f = Field_Data[i].Field / 10;
	if (f > 10) f = f / 10;
	int hv = Field_Data[i].Field % 2;
	float spec = 0.9;
	switch (f) {
	case 1:
		if (hv == 1)
			spec = Center_H / 100.0;
		else spec = Center_V / 100.0;
		break;

	case 2:
		if (hv == 1)
			spec = LR_H3 / 100.0;
		else spec = LR_V3 / 100.0;
		break;

	case 3:
		if (hv == 1)
			spec = Dia_H3 / 100.0;
		else spec = Dia_V3 / 100.0;
		break;

	case 5:
		if (hv == 1)
			spec = Dia_H5 / 100.0;
		else spec = Dia_V5 / 100.0;
		break;

	case 6:
		if (hv == 1)
			spec = Dia_H6 / 100.0;
		else spec = Dia_V6 / 100.0;
		break;

	case 7:
		if (hv == 1)
			spec = Dia_H7 / 100.0;
		else spec = Dia_V7 / 100.0;
		break;

	case 8:
		if (hv == 1)
			spec = Dia_H8 / 100.0;
		else spec = Dia_V8 / 100.0;
		break;

	case 9:
		if (hv == 1)
			spec = Dia_H9 / 100.0;
		else spec = Dia_V9 / 100.0;
		break;

	default:
		break;
	}

	return spec;
}


void ThroughFocus::Display_Result() {
	ui->log->clear();
	ui->log->setFontPointSize(16);
	if (OK[0] >= -1) {

		if (OK[0] == 1) {
			ui->log->setTextColor(QColor(0, 200, 0, 255));
			ui->log->insertPlainText("TF:PASS");
		}
		if (OK[0] == 0) {
			ui->log->setTextColor(QColor(255, 0, 0, 255));
			ui->log->insertPlainText("TF: NG");
		}
		if (OK[0] == -1) {
			ui->log->setTextColor(QColor(0, 0, 255, 255));
			ui->log->insertPlainText("No Data");
		}

		ui->log->insertPlainText("\n");
		if (F5) {
			if (OK[5] > 0) {
				ui->log->setTextColor(QColor(0, 200, 0, 255));
				ui->log->insertPlainText("0.5AA:PASS\n");
			}
			else {
				ui->log->setTextColor(QColor(255, 0, 0, 255));
				ui->log->insertPlainText("0.5AA:NG\n");
			}
		}

		if (F6) {
			if (OK[6] > 0) {
				ui->log->setTextColor(QColor(0, 200, 0, 255));
				ui->log->insertPlainText("0.6AA:PASS\n");
			}
			else {
				ui->log->setTextColor(QColor(255, 0, 0, 255));
				ui->log->insertPlainText("0.6AA:NG\n");
			}

		}

		if (F7) {
			if (OK[7] > 0) {
				ui->log->setTextColor(QColor(0, 200, 0, 255));
				ui->log->insertPlainText("0.7AA:PASS\n");
			}
			else {
				ui->log->setTextColor(QColor(255, 0, 0, 255));
				ui->log->insertPlainText("0.7AA:NG\n");
			}

		}

		if (F8) {
			if (OK[8] > 0) {
				ui->log->setTextColor(QColor(0, 200, 0, 255));
				ui->log->insertPlainText("0.8AA:PASS\n");
			}
			else {
				ui->log->setTextColor(QColor(255, 0, 0, 255));
				ui->log->insertPlainText("0.8AA:NG\n");
			}

		}
		ui->result->clear();

		ui->result->setFontPointSize(9);
		ifstream in(".\\logData.txt");
		ostringstream outStr;
		outStr << in.rdbuf();
		string outContent = outStr.str();
		ui->result->insertPlainText(outContent.c_str());
		in.close();
	}
}


void OK_Range() {

	int mid = TF_step / 2 ;
	int mid_Code = TF_Data[mid].code;
	int max_OK_Code = 0, min_OK_Code = 1023000;

	for (int i = 0; i < Field_Cnt; i++) {
		if(Field_Data[i].min_Code<16384&&Field_Data[i].min_Code > max_OK_Code)
			max_OK_Code = Field_Data[i].min_Code;

		if (Field_Data[i].max_Code>0&&Field_Data[i].max_Code <min_OK_Code)
			min_OK_Code = Field_Data[i].max_Code;
	}

	TF_Check_Result << TF_Data[TF_step - 1].time << "	";

	int range = min_OK_Code - max_OK_Code;

	if (range < OK_Range_Spec) {
		OK[0] = 0;
	}
	bool pass = 1;

	if(Field_Data[0].Peak_CodeHV>= max_OK_Code&&Field_Data[0].Peak_CodeHV<= min_OK_Code){
		fout << "Center Code OK " << endl;
		TF_Check_Result << "PASS" << "	";
	}
	else {
		fout << "Center Code NG " << endl;
		TF_Check_Result << "FAIL" << "	";
	}

	fout << "OK Code Range = ";
	if (range > 0) {
		fout << max_OK_Code;
		fout << " ~ ";
		fout << min_OK_Code << " = ";
	}
	fout << range << endl;
	TF_Check_Result << range << "	";

	for (int j = 3; j < 10; j++)
		if (PD_Data[j].cnt>1) {
			int max = 0, field = 0, field2 = 0;
			if (F_POS[j] != 7) {
				for (int i = 0; i < PD_Data[j].cnt; i += 2) {
					int HV = abs(Field_Data[PD_Data[j].group[i]].Peak_Code - Field_Data[PD_Data[j].group[i + 1]].Peak_Code);
					if (HV > max) {
						max = HV;
						field = Field_Data[PD_Data[j].group[i]].Field;
					}
				}
			}
			else {

				for (int i = 0; i < PD_Data[j].cnt; i ++){
					if(Field_Data[PD_Data[j].group[i]].Field%2==1)
						for (int k = 0; k < PD_Data[j].cnt; k++)
							if (Field_Data[PD_Data[j].group[k]].Field % 2==0)
							if ((Field_Data[PD_Data[j].group[k]].Field % 100)/10 == (Field_Data[PD_Data[j].group[i]].Field % 100) / 10) {

								int HV = abs(Field_Data[PD_Data[j].group[i]].Peak_Code - Field_Data[PD_Data[j].group[i + 1]].Peak_Code);
								if (HV > max) {
									max = HV;
									field = Field_Data[PD_Data[j].group[i]].Field;
									field2 = Field_Data[PD_Data[j].group[k]].Field;  
								}
							}
				}
			}

			if (max > HVD_Spec[8]) {
				OK[0] = 0;
			}
			if(TF_Data[center_Max_Step].sfr[0]<1)
				fout << "H/V Seperate Max ";
			else
				fout << "S/T Seperate Max ";
			fout << field << " =";
			fout << max << endl;
			TF_Check_Result << max << "	";
		}

}


void Max_OK_Range_Adjust() {

	int mid = TF_step / 2;
	int mid_Code = TF_Data[mid].code;
	int max_OK_Code = mid_Code - 1, min_OK_Code = mid_Code + 1;
	bool ok = true;
	float Corner[4], max_X_tilt = -16384, max_Y_tilt = -16384, min_X_tilt = 16384, min_Y_tilt = 16384;

	FD Field_Data_Adjust[50], Field_Data_Best[50];
	for (int i = 0; i < 50; i++)
		Field_Data_Adjust[i] = Field_Data[i];

	for (int f = 3; f < 10; f++)
		if (PD_Data[f].cnt>1) {

			int n = PD_Data[f].cnt / 4;

			Corner[0] = Field_Data[PD_Data[f].group[n - 2]].PD_AVG;
			Corner[1] = Field_Data[PD_Data[f].group[2 * n - 2]].PD_AVG;
			Corner[2] = Field_Data[PD_Data[f].group[3 * n - 2]].PD_AVG;
			Corner[3] = Field_Data[PD_Data[f].group[4 * n - 2]].PD_AVG;

			if (PD_Data[f].cnt == 16) {
				for (int i = 0; i < PD_Data[f].cnt; i++) {
					int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field;
					if (x > 99)
						sub_Field = x % 100;
					else sub_Field = x % 10;
					if ((sub_Field - 3) % 4 == 0)
						Corner[(sub_Field - 3) / 4] = Field_Data[PD_Data[f].group[i]].PD_AVG;
				}
			}
			else if (F_POS[f] != 7) {
				for (int i = 0; i < PD_Data[f].cnt; i++) {
					int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field;
					if (x > 99)
						sub_Field = x % 100;
					else sub_Field = x % 10;
					if (sub_Field < 9 && sub_Field % 2 == 1)
						Corner[sub_Field / 2] = Field_Data[PD_Data[f].group[i]].PD_AVG;
				}
			}

			else {
				for (int i = 0; i < PD_Data[f].cnt; i++) {
					int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field1, sub_Field2;
					sub_Field2 = x % 10;
					sub_Field1 = (x % 100) / 10;
					if (sub_Field2 == 1) {
						Corner[sub_Field1 - 1] = Field_Data[PD_Data[f].group[i]].PD_AVG;
					}
				}
			}

			float X_Tilt = (Corner[0] - Corner[1] + Corner[3] - Corner[2]) / 4.0 / f;  //Left - right
			float Y_Tilt = (Corner[0] - Corner[3] + Corner[1] - Corner[2]) / 4.0 / f;   // up - down


			if (F_POS[f] == 7) {
				X_Tilt = (Corner[0] - Corner[2])/2.0;
				Y_Tilt = (Corner[1] - Corner[3])/2.0;
			}

			if (X_Tilt > max_X_tilt)
				max_X_tilt = X_Tilt;
			if (Y_Tilt > max_Y_tilt)
				max_Y_tilt = Y_Tilt;

			if (X_Tilt < min_X_tilt)
				min_X_tilt = X_Tilt;
			if (Y_Tilt < min_Y_tilt)
				min_Y_tilt = Y_Tilt;

		}

	float  x_max = ((int)(max_X_tilt * 10) / 10.0) + 0.2, y_max = ((int)(max_Y_tilt * 10)) / 10.0 + 0.2;
	float  x_min = ((int)(min_X_tilt * 10) / 10.0) - 0.2, y_min = ((int)(min_Y_tilt * 10)) / 10.0 - 0.2;

	if (x_max < 0)x_max = 0.1;
	if (y_max < 0)y_max = 0.1;
	if (x_min > 0)x_min = -0.1;
	if (y_min > 0)y_min = -0.1;

	float offsetHV[4], offset[4], Max_range = -16384, best_x, best_y;
	float min_of_max_Code = 16384;
	float max_of_min_Code = 0;

	for (float a = x_min; a < x_max; a += 0.05)
		for (float b = y_min; b < y_max; b += 0.05) {
			///////////////////
			if (a < 0.01 && b < 0.01&&a>-0.01&&b>-0.01) {
				int xxx = 0;
			}
			///////////////
			for (int i = 0; i < 50; i++)
				Field_Data_Adjust[i] = Field_Data[i];

			offsetHV[0] = a;
			offsetHV[1] = b;
			offsetHV[2] = -a;
			offsetHV[3] = -b;

			offset[0] = a + b;
			offset[1] = -a + b;
			offset[2] = -a - b;
			offset[3] = a - b;

			for (int j = 2; j < 9; j++)
				if (PD_Data[j].cnt>1) {
					for (int i = 0; i < PD_Data[j].cnt; i++) {
						int x = Field_Data[PD_Data[j].group[i]].Field, sub_Field;

						if (x > 99)
							sub_Field = x % 100;
						else
							sub_Field = x % 10;

						int k = (sub_Field - 1) / 2, m = 0, p = 0;

						if (PD_Data[j].cnt == 16) {
							k = (sub_Field - 1) / 4;
							if (p > 1) {
								Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offset[k] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offset[k] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offset[k] * j;
								if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offset[k] * j;
							}
							else {
								Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[k] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[k] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[k] * j;
								if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[k] * j;
							}
						}
						else if (PD_Data[j].cnt == 8 && F_POS[j] == 0) {
							Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offset[k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offset[k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offset[k] * j;
							if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offset[k] * j;
						}
						else if (PD_Data[j].cnt == 8 && F_POS[j] == 1) {
							Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[k] * j;
							if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[k] * j;
						}
						else if (PD_Data[j].cnt == 4 && F_POS[j] == 3) {
							Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[2 * k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[2 * k] * j;
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[2 * k] * j;
							if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[2 * k] * j;
						}
						else if (PD_Data[j].cnt == 12) {

							if (F_POS[j] != 7) {
								if (sub_Field < 9) {
									k = (sub_Field - 1) / 2;
									Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offset[k] * j;
									Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offset[k] * j;
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offset[k] * j;
									if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
										Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offset[k] * j;
								}
								else {
									if (F_POS[j] < 7) {
										k = (sub_Field - 9) / 2;
										if (F_POS[j] == 5) {
											Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[2 * k] * j;
											Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[2 * k] * j;
											Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[2 * k] * j;
											if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
												Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[2 * k] * j;
										}
										if (F_POS[j] == 6) {
											Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[2 * k + 1] * j;
											Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[2 * k + 1] * j;
											Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[2 * k + 1] * j;
											if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
												Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[2 * k + 1] * j;
										}
									}
									else if (F_POS[j] == 7) {
										Field_Data_Adjust[PD_Data[j].group[i]].max_Code += offsetHV[sub_Field - 9] * j;
										Field_Data_Adjust[PD_Data[j].group[i]].min_Code += offsetHV[sub_Field - 9] * j;
										Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code += offsetHV[sub_Field - 9] * j;
										if(Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV!=0)
											Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV += offsetHV[sub_Field - 9] * j;
									}
								}

							}
							else {
								int pos = (x % 100) / 10;
								Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= offsetHV[pos - 1] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= offsetHV[pos - 1] * j;
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= offsetHV[pos - 1] * j;
								if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= offsetHV[pos - 1] * j;
							}
						}
					}
				}

			float up_Code = 16384;
			float down_Code = 0;
			for (int j = 0; j < Field_Cnt; j++) {

				if (Field_Data_Adjust[j].max_Code < up_Code)
					up_Code = Field_Data_Adjust[j].max_Code;

				if (Field_Data_Adjust[j].min_Code > down_Code)
					down_Code = Field_Data_Adjust[j].min_Code;
			}


			bool update = false;
			if (up_Code - down_Code > Max_range)
				update = true;
			//else if(up_Code - down_Code > Max_range - 0.01) {
			//	float temp1 = min(up_Code-mid_Code, mid_Code- down_Code);
			//	float temp2 = min(min_of_max_Code - mid_Code, mid_Code - max_of_min_Code);
			//	if(temp1>temp2)
			//		update = true;
			//}

			if (update) {
				max_of_min_Code = down_Code;
				min_of_max_Code = up_Code;
				Max_range = up_Code - down_Code;

				best_x = a;
				best_y = b;

				for (int i = 0; i < 50; i++)
					Field_Data_Best[i] = Field_Data_Adjust[i];
			}

		}

	TF_Check_Result << TF_Data[TF_step - 1].time << "	";

	if (min_of_max_Code < max_of_min_Code) {

		fout << "Field Adjust OK Range = 0\n";
		TF_Check_Result << 0 << "	";
		return;
	}

	float range = min_of_max_Code - max_of_min_Code;

	if (range < OK_Range_Spec) {
		OK[10] = 0;
	}

	fout << "Adjust Max OK Range = ";
	if (range > 0) {
		fout << max_of_min_Code;
		fout << " ~ ";
		fout << min_of_max_Code << " = ";
	}
	fout << range << endl;

	TF_Check_Result << range << "	";

	for (int j = 2; j < 10; j++)
		if (PD_Data[j].cnt>1) {

			int max = 0, min = 16384;
			for (int i = 0; i < PD_Data[j].cnt; i++) {
				int x = Field_Data[PD_Data[j].group[i]].Field, sub_Field;
				if (x > 99)
					sub_Field = x % 100;
				else
					sub_Field = x % 10;

				int peak_code = Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV;

				if (peak_code != 0 && peak_code > max)
					max = peak_code;
				if (peak_code != 0 && peak_code < min)
					min = peak_code;
			}

			fout << "0.";
			fout << j;
			fout << "F PS = ";
			fout << max;
			fout << " - ";
			fout << min;
			fout << " = ";
			fout << max - min << endl;

			TF_Check_Result << max - min << "	";
		}
	fout << endl;

	if (show_Diagram) {

		Mat M(TF_step * 10 + 30, Field_Cnt * 20 + 10, CV_8UC3, Scalar(235, 235, 235));

		for (int j = 0; j < TF_step; j++) {
			Rect rect = Rect(5, 10 * j + 25, Field_Cnt * 20, 10);//起点；长宽
			Scalar color = Scalar(150, 150, 150);
			rectangle(M, rect, color, 1);
		}

		int ff = 0;
		for (int i = 0; i < Field_Cnt; i++) {
			for (int j = 0; j < TF_step; j++) {
				int field = Field_Data[i].Field / 10;
				if (field > 10)
					field = field / 10;
				Scalar color = Scalar(field % 3 * 50, field % 4 * 50, field % 5 * 50);

				int current_code = TF_Data[j].code;
				if (current_code<Field_Data_Best[i].min_Code || current_code>Field_Data_Best[i].max_Code) {
					Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
					rectangle(M, rect, color, 2);
				}
				if (abs(current_code - Field_Data_Best[i].Peak_Code) <= (TF_Data[1].code - TF_Data[0].code) / 2.0) {
					Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
					rectangle(M, rect, Scalar(0, 150, 150), 1);
				}

				if (field != ff) {
					ff = field;
					string text = "0";
					if (ff > 1) {
						text += ".";
						text += to_string(ff);
					}
					text += "F";
					putText(M, text, Point(i * 20 + 10, 20), 5, 1, color, 2);
				}
			}
		}

		Rect rect = Rect(5, 10 * (TF_step / 2) + 25, Field_Cnt * 20, 10);//起点；长宽
		Scalar color = Scalar(0, 0, 255);
		rectangle(M, rect, color, 1);

		string s = "Adjust Max OK Range AA Simulation";
		imshow(s.c_str(), M);   //窗口中显示图像
	}
}


void OK_Range_Adjust(int f) {

	int mid = TF_step / 2 + 1;
	int mid_Code = TF_Data[mid].code;
	int max_OK_Code = mid_Code - 1, min_OK_Code = mid_Code + 1;
	int n = PD_Data[f].cnt / 4;
	if (n == 3)n--;
	bool ok = true;

	FD Field_Data_Adjust[50];
	for (int i = 0; i < 50; i++)
		Field_Data_Adjust[i] = Field_Data[i];

	int Corner[4] = { 0 };

	if (PD_Data[f].cnt == 16) {
		for (int i = 0; i < PD_Data[f].cnt; i++) {
			int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field;
			if (x > 99)
				sub_Field = x % 100;
			else sub_Field = x % 10;
			if ((sub_Field - 3) % 4 == 0)
				Corner[(sub_Field - 3) / 4] = Field_Data[PD_Data[f].group[i]].PD_AVG;
		}
	}
	else if (F_POS[f] != 7) {
		for (int i = 0; i < PD_Data[f].cnt; i++) {
			int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field;;
			if (x > 99)
				sub_Field = x % 100;
			else sub_Field = x % 10;
			if (sub_Field < 9 && sub_Field % 2 == 1)
				Corner[sub_Field / 2] = Field_Data[PD_Data[f].group[i]].PD_AVG;
		}
	}
	else {
		for (int i = 0; i < PD_Data[f].cnt; i++) {
			int x = Field_Data[PD_Data[f].group[i]].Field, sub_Field1, sub_Field2;
			sub_Field2 = x % 10;
			sub_Field1 = (x % 100) / 10;
			if (sub_Field2 == 1) {
				Corner[sub_Field1 - 1] = Field_Data[PD_Data[f].group[i]].PD_AVG;
			}
		}
	}

	float X_Tilt = (Corner[0] - Corner[1] + Corner[3] - Corner[2]) / 4.0;
	float Y_Tilt = (Corner[0] - Corner[3] + Corner[1] - Corner[2]) / 4.0;

	if (F_POS[f] == 7) {
		X_Tilt = (Corner[0] - Corner[2]) / 2.0;
		Y_Tilt = (Corner[1] - Corner[3]) / 2.0;
	}

	PD_Data[f].offsetHV[0] = X_Tilt;
	PD_Data[f].offsetHV[1] = Y_Tilt;
	PD_Data[f].offsetHV[2] = -X_Tilt;
	PD_Data[f].offsetHV[3] = -Y_Tilt;

	PD_Data[f].offset[0] = X_Tilt + Y_Tilt;
	PD_Data[f].offset[1] = -X_Tilt + Y_Tilt;
	PD_Data[f].offset[2] = -X_Tilt - Y_Tilt;
	PD_Data[f].offset[3] = X_Tilt - Y_Tilt;

	for (int j = 2; j < 9; j++)
		if (PD_Data[j].cnt>1) {
			for (int i = 0; i < PD_Data[j].cnt; i++) {
				int x = Field_Data[PD_Data[j].group[i]].Field, sub_Field;
				if (x > 99)
					sub_Field = x % 100;
				else
					sub_Field = x % 10;

				int k = (sub_Field - 1) / 2, m = 0, p = 0;

				if (PD_Data[j].cnt == 16) {
					k = (sub_Field - 1) / 4;
					if (p > 1) {
						Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offset[k] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offset[k] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
						if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
					}
					else {
						Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[k] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[k] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
						if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
					}
				}
				else if (PD_Data[j].cnt == 8 && F_POS[j] == 0) {
					Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offset[k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offset[k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
					if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
				}
				else if (PD_Data[j].cnt == 8 && F_POS[j] == 1) {
					Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
					if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
				}
				else if (PD_Data[j].cnt == 4 && F_POS[j] == 3) {
					Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[2 * k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[2 * k] * j / f;
					Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
					if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
				}
				else if (PD_Data[j].cnt == 12) {
					if (F_POS[j] != 7) {
						if (sub_Field < 9) {
							k = (sub_Field - 1) / 2;
							Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offset[k] * j / f;
							Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offset[k] * j / f;
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[k] * j / f;
							if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
								Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[k] * j / f;
						}
						else {
							if (F_POS[j] < 7) {
								k = (sub_Field - 9) / 2;
								if (F_POS[j] == 5) {
									Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[2 * k] * j / f;
									Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[2 * k] * j / f;
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[2 * k] * j / f;
									if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
										Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[2 * k] * j / f;
								}
								if (F_POS[j] == 6) {
									Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[2 * k + 1] * j / f;
									Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[2 * k + 1] * j / f;
									Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[2 * k + 1] * j / f;
									if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
										Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[2 * k + 1] * j / f;
								}
							}
						}
					}
					else {
						int pos = (x % 100) / 10;
						Field_Data_Adjust[PD_Data[j].group[i]].max_Code -= PD_Data[f].offsetHV[pos - 1] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].min_Code -= PD_Data[f].offsetHV[pos - 1] * j / f;
						Field_Data_Adjust[PD_Data[j].group[i]].Peak_Code -= PD_Data[f].offset[pos - 1] * j / f;
						if (Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV != 0)
							Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV -= PD_Data[f].offset[pos - 1] * j / f;
					}
				}
			}
		}

	float min_of_max_Code = 16384;
	float max_of_min_Code = 0;
	for (int j = 0; j < Field_Cnt; j++) {

		if (Field_Data_Adjust[j].max_Code < min_of_max_Code)
			min_of_max_Code = Field_Data_Adjust[j].max_Code;

		if (Field_Data_Adjust[j].min_Code > max_of_min_Code)
			max_of_min_Code = Field_Data_Adjust[j].min_Code;
	}

	TF_Check_Result << TF_Data[TF_step - 1].time << "	";

	fout << f;
	if (min_of_max_Code < max_of_min_Code) {
		OK[f] = 0;
		fout << "Field Adjust OK Range = 0\n";
		TF_Check_Result << 0 << "	";
		//return;
	}else{

//		float range = min_of_max_Code - max_of_min_Code + TF_Data[1].code - TF_Data[0].code;
		float range = min_of_max_Code - max_of_min_Code;

		if (range < OK_Range_Spec) {
			OK[f] = 0;
		}

		fout << "Field Adjust OK Range = ";
		if (range > 0) {
			fout << max_of_min_Code;
			fout << " ~ ";
			fout << min_of_max_Code << " = ";
		}
		fout << range << endl;

		TF_Check_Result << range << "	";
	}


	for (int j = 2; j < 10; j++)
		if (PD_Data[j].cnt>1) {

			int max = 0, min = 16384;
			for (int i = 0; i < PD_Data[j].cnt; i++) {
				int x = Field_Data[PD_Data[j].group[i]].Field, sub_Field;
				if (x > 99)
					sub_Field = x % 100;
				else
					sub_Field = x % 10;

				int peak_code = Field_Data_Adjust[PD_Data[j].group[i]].Peak_CodeHV;

				if (peak_code != 0 && peak_code > max)
					max = peak_code;
				if (peak_code != 0 && peak_code < min)
					min = peak_code;
			}

			fout << "0.";
			fout << j;
			fout << "F PS = ";
			fout << max;
			fout << " - ";
			fout << min;
			fout << " = ";
			fout << max - min << endl;

			TF_Check_Result << max - min << "	";
		}
	fout << endl;

	if (show_Diagram) {

		Mat M(TF_step * 10 + 30, Field_Cnt * 20 + 10, CV_8UC3, Scalar(235, 235, 235));

		for (int j = 0; j < TF_step; j++) {
			Rect rect = Rect(5, 10 * j + 25, Field_Cnt * 20, 10);//起点；长宽
			Scalar color = Scalar(150, 150, 150);
			rectangle(M, rect, color, 1);
		}

		int ff = 0;
		for (int i = 0; i < Field_Cnt; i++) {
			for (int j = 0; j < TF_step; j++) {
				int field = Field_Data[i].Field / 10;
				if (field > 10)
					field = field / 10;
				Scalar color = Scalar(field % 3 * 50, field % 4 * 50, field % 5 * 50);

				int current_code = TF_Data[j].code;
				if (current_code<Field_Data_Adjust[i].min_Code || current_code>Field_Data_Adjust[i].max_Code) {
					Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
					rectangle(M, rect, color, 2);
				}

				if (abs(current_code - Field_Data_Adjust[i].Peak_Code)<= (TF_Data[1].code - TF_Data[0].code)/2.0) {
					Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
					rectangle(M, rect, Scalar(0, 150, 150), 1);
				}

				if (field != ff) {
					ff = field;
					string text = "0";
					if (ff > 1) {
						text += ".";
						if (ff == 2)
							text += to_string(ff + 1);
						else
							text += to_string(ff);

					}
					text += "F";
					putText(M, text, Point(i * 20 + 10, 20), 5, 1, color, 2);
				}
			}
		}

		Rect rect = Rect(5, 10 * (TF_step / 2) + 25, Field_Cnt * 20, 10);//起点；长宽
		Scalar color = Scalar(0, 0, 255);
		rectangle(M, rect, color, 1);

		string s = to_string(f) + "F Adjust AA Simulation";
		imshow(s.c_str(), M);   //窗口中显示图像
	}

}


void PS_PD_check(int f) {

	PD_Data[f].min[0] = PD_Data[f].max[0] = Field_Data[PD_Data[f].group[0]].PD_AVG;
	PD_Data[f].min[1] = PD_Data[f].max[1] = Field_Data[PD_Data[f].group[0]].PD;
	PD_Data[f].min[2] = PD_Data[f].max[2] = Field_Data[PD_Data[f].group[1]].PD;

	for (int i = 0; i < PD_Data[f].cnt; i++) {

		if (i % 2 == 0) {

			if (Field_Data[PD_Data[f].group[i]].PD > PD_Data[f].max[1]) {
				PD_Data[f].max[1] = Field_Data[PD_Data[f].group[i]].PD;
			}
			if (Field_Data[PD_Data[f].group[i]].PD < PD_Data[f].min[1]) {
				PD_Data[f].min[1] = Field_Data[PD_Data[f].group[i]].PD;
			}
			PD_Data[f].PD_AVG[1] += Field_Data[PD_Data[f].group[i]].PD;
		}
		else {
			if (Field_Data[PD_Data[f].group[i]].PD > PD_Data[f].max[2]) {
				PD_Data[f].max[2] = Field_Data[PD_Data[f].group[i]].PD;
			}
			if (Field_Data[PD_Data[f].group[i]].PD < PD_Data[f].min[2]) {
				PD_Data[f].min[2] = Field_Data[PD_Data[f].group[i]].PD;
			}
			PD_Data[f].PD_AVG[2] += Field_Data[PD_Data[f].group[i]].PD;

		}

		if (Field_Data[PD_Data[f].group[i]].PD_AVG > PD_Data[f].max[0]) {
			PD_Data[f].max[0] = Field_Data[PD_Data[f].group[i]].PD_AVG;
			PD_Data[f].max_index[0] = i+1;
		}
		if (Field_Data[PD_Data[f].group[i]].PD_AVG < PD_Data[f].min[0]) {
			PD_Data[f].min[0] = Field_Data[PD_Data[f].group[i]].PD_AVG;
			PD_Data[f].min_index[0] = i+1;
		}
		PD_Data[f].PD_AVG[0] += Field_Data[PD_Data[f].group[i]].PD_AVG;
	}

	PD_Data[f].PD_AVG[0] = PD_Data[f].PD_AVG[0] * 2 / PD_Data[f].cnt;
	PD_Data[f].PD_AVG[1] = PD_Data[f].PD_AVG[1] * 2 / PD_Data[f].cnt;
	PD_Data[f].PD_AVG[2] = PD_Data[f].PD_AVG[2] * 2 / PD_Data[f].cnt;

	//if (F_POS[f] == 7) {
	//	PD_Data[f].PD_AVG[0] = PD_Data[f].PD_AVG[0] / 4;
	//	PD_Data[f].PD_AVG[1] = PD_Data[f].PD_AVG[1] / 6;
	//	PD_Data[f].PD_AVG[2] = PD_Data[f].PD_AVG[2] / 6;
	//}

	int ps = PD_Data[f].max[0] - PD_Data[f].min[0];
	int ps_H = PD_Data[f].max[1] - PD_Data[f].min[1];
	int ps_V = PD_Data[f].max[2] - PD_Data[f].min[2];

	if (ps >= PS_Spec[f][0] || ps_H >= PS_Spec[f][1] || ps_V >= PS_Spec[f][2])
		OK[0] = 0;
	if (abs(PD_Data[f].PD_AVG[0]) >= PD_Spec[f][0] || abs(PD_Data[f].PD_AVG[1]) >= PD_Spec[f][1] || abs(PD_Data[f].PD_AVG[2]) >= PD_Spec[f][2])
		OK[0] = 0;


	int h = TF_step * 8;
	startCode = TF_Data[0].code;
	Mat BackGround(350, h, CV_8UC3, Scalar::all(255));
	Mat out[4];
	for (int i = 0; i < 4; i++)
		out[i] = BackGround.clone();

	int k = 0;

	if (Curve_Fit)
		for (int i = 0; i < PD_Data[f].cnt; i++)
			if (i % 2 == 0) {

				vector<Point2d> in_point;
				for (int j = 0; j < TF_step; j++) {
					in_point.push_back(Point2d((TF_Data[j].code - startCode) * 2, TF_Data[j].sfr_AVG[i] * 500));
				}
				//n:多项式阶次
				int n = 4;
				Mat mat_k = polyfit(in_point, n);

				//画出拟合曲线
				int cnt = in_point.size();
				string ff = to_string(f) + "_" + to_string(i);
				curve_Data << ff << endl;
				for (int i = in_point[0].x; i < in_point[cnt - 1].x; ++i)
					//	for (int i = in[0].x; i < in[size(in) - 1].x; ++i)
				{
					Point2d ipt;
					ipt.x = i;
					ipt.y = 0;
					for (int j = 0; j < n + 1; ++j)
					{
						ipt.y += mat_k.at<double>(j, 0)*pow(i, j);
					}
					curve_Data << ipt.x + startCode << "	" << ipt.y / 500 << endl;
					ipt.y = h + 100 - ipt.y;
					circle(out[k], ipt, 1, Scalar(255, 0, 0), CV_FILLED, CV_AA);

				}
				curve_Data << endl;
				//画出原始散点
				for (int i = 0; i < cnt; ++i)
				{
					Point ipt(in_point[i].x, h - in_point[i].y + 100);
					//	Point ipt = in_point[i];
					circle(out[k], ipt, 3, Scalar(0, 0, 255), CV_FILLED, CV_AA);
				}

				imshow(ff.c_str(), out[k]);
				k++;
			}

	fout << "0.";
	fout << f;
	fout << "F ";

	fout << PD_Data[f].max_index[0];
	fout << "vs";
	fout << PD_Data[f].min_index[0];
	fout << " PS = ";

	fout << PD_Data[f].max[0];
	fout << " - ";
	fout << PD_Data[f].min[0];
	fout << " = ";
	fout << ps << endl;

	TF_Check_Result << PD_Data[f].max_index[0] << "vs" << PD_Data[f].min_index[0] << "	";
	TF_Check_Result << ps << "	";

	fout << "0.";
	fout << f;
	fout << "F PS_H = ";
	fout << PD_Data[f].max[1];
	fout << " - ";
	fout << PD_Data[f].min[1];
	fout << " = ";
	fout << ps_H << endl;

	TF_Check_Result << ps_H << "	";

	fout << "0.";
	fout << f;
	fout << "F PS_V = ";
	fout << PD_Data[f].max[2];
	fout << " - ";
	fout << PD_Data[f].min[2];
	fout << " = ";
	fout << ps_V << endl;

	TF_Check_Result << ps_V << "	";
	
	fout << "0.";
	fout << f;
	fout << "F PD = ";
	fout << PD_Data[f].PD_AVG[0] << endl;

	TF_Check_Result << PD_Data[f].PD_AVG[0] << "	";

	fout << "0.";
	fout << f;
	fout << "F PD_H = ";
	fout << PD_Data[f].PD_AVG[1] << endl;

	TF_Check_Result << PD_Data[f].PD_AVG[1] << "	";

	fout << "0.";
	fout << f;
	fout << "F PD_V = ";
	fout << PD_Data[f].PD_AVG[2] << endl;

	TF_Check_Result << PD_Data[f].PD_AVG[2] << "	";
}


void check_SFR_Max() {


	debug_log.open(".\\debug_log.txt");
	for (int i = 0; i < 10; i++)
		OK[i] = 1;

	memset(SFR_Table, 0, sizeof(SFR_Table));
	memset(PD_Data, 0, sizeof(PD_Data));

	for (int i = 0; i < Field_Cnt; i++) {

		float max_SFR = 0, max_SFR_AVG = 0; int max_step = 0, max_AVG_step = 0;
		float spec = find_Spec(i);

		int f = Field_Data[i].Field / 10;
		if (f > 10) f = f / 10;

		PD_Data[f].group[PD_Data[f].cnt++] = i;

		if (i % 2 == 0 && f == 1) {
			center_Field = i;
		}
		if (TF_Data[0].code > TF_Data[TF_step - 1].code) {
			Field_Data[i].max_Code = TF_Data[TF_step - 1].code;
			Field_Data[i].min_Code = TF_Data[0].code;
		}
		else {
			Field_Data[i].max_Code = TF_Data[0].code;
			Field_Data[i].min_Code = TF_Data[TF_step - 1].code;
		}

		for (int j = 0; j < TF_step; j++) {
			if (TF_Data[j].sfr[i]>max_SFR) {
				max_SFR = TF_Data[j].sfr[i];
				max_step = j;
				Field_Data[i].Peak_Code = TF_Data[j].code;
			}
			if (i % 2 == 0 || Field_Data[i].Field % 10 == 1)
				if (TF_Data[j].sfr_AVG[i] > max_SFR_AVG) {
					max_SFR_AVG = TF_Data[j].sfr_AVG[i];
					max_AVG_step = j;
					Field_Data[i].Peak_CodeHV = TF_Data[j].code;
				}
			//debug_log << TF_Data[j].code << "	" << TF_Data[j].sfr[i] << endl;
		}

		if (i == 0)
			center_Max_Step = max_step;

		vector<Point2d> in_point;
		vector<double> Data_x, Data_y;

		int start = max_step - 2, end = max_step + 2;
		if (start < 0) {
			start = 0;
			end -= start;
		}
		if (end > TF_step - 1) {
			end = TF_step - 1;
			start -= end - TF_step + 1;
		}
		for (int j = start; j <= end; j++) {
			in_point.push_back(Point2d(TF_Data[j].code, TF_Data[j].sfr[i]));
			//step1: 读取txt文件中的数据
			Data_x.push_back(TF_Data[j].code);
			Data_y.push_back(TF_Data[j].sfr[i]);
		}

		//step2:调用拟合函数
		// CoArr 表示多项式拟合的系数
		// myRes 表示拟合的系数与拟合前的横坐标，计算得到新的纵坐标
		vector<double> CoArr = fitCurve(Data_x, Data_y, 4); //调用函数，4表示阶数，（可以随意取，1—线性拟合，2—平方拟合，3—立方拟合,>=4,高阶拟合）

		float st = 1;
		if (TF_Data[0].code > TF_Data[TF_step - 1].code)
			st *= -1;

		double P[5];
		for (double x = TF_Data[start].code; x > TF_Data[end].code|| x < TF_Data[end].code; x += st) {

			double sfr = 0;
			for (int j = 0; j < CoArr.size(); j++) {
				sfr += pow(x, j)*CoArr[j];
			}

			debug_log <<x<<"	"<< sfr << endl;

			if (sfr>max_SFR) {
				max_SFR = sfr;
				Field_Data[i].Peak_Code = x;
			}

			if (sfr > spec) {
				if (Field_Data[i].max_Code < x)
					Field_Data[i].max_Code = x;

				if (Field_Data[i].min_Code > x)
					Field_Data[i].min_Code = x;
			}
		}
		debug_log << endl;
		// for AVG SFR Peak code search
		if (i % 2 == 0 || Field_Data[i].Field % 10 == 1) {

			start = max_step - 2, end = max_step + 2;
			if (start < 0) {
				start = 0;
				end -= start;
			}
			if (end > TF_step - 1) {
				end = TF_step - 1;
				start -= end - TF_step + 1;
			}
			vector<double> Data_x1, Data_y1;
			for (int j = start; j <= end; j++) {
				Data_x1.push_back(TF_Data[j].code);
				Data_y1.push_back(TF_Data[j].sfr_AVG[i]);
			}
			vector<double> CoArr1 = fitCurve(Data_x1, Data_y1, 4); //调用函数，4表示阶数，（可以随意取，1—线性拟合，2—平方拟合，3—立方拟合,>=4,高阶拟合）
			max_SFR_AVG = 0;
			for (double x = TF_Data[start].code;  x > TF_Data[end].code || x < TF_Data[end].code; x += st) {

				double sfr = 0;
				for (int j = 0; j < CoArr1.size(); j++) {
					sfr += pow(x, j)*CoArr1[j];
				}
				if (sfr>max_SFR_AVG) {
					max_SFR_AVG = sfr;
					Field_Data[i].Peak_CodeHV = x;
				}
			}
		}
		// for diagram drawing flag
		for (int j = 0; j < TF_step; j++) {

			if (TF_Data[j].sfr[i] > spec) {
				SFR_Table[j][i] = 0;
			}
			else {
				SFR_Table[j][i] = 1;
			}
		}
	}

}


int upload(int mode) {

	fout.open(".\\logData.txt");
	TF_Check_Result.open("TF_Check_Result.txt", ios::app);
	curve_Data.open(".\\Curve_Fit.txt");

	memset(TF_Data, 0, sizeof(TF_Data));
	last_Time = "";

	current_Step = TF_step - 1;
	int ret = 0;
	ret = TF_Data_Check(mode);

	if (ret != 0) {
		OK[0] = ret;
	}
	else if (ret == 0) {

		for (int i = 0; i < 10; i++)
			OK[i] = 1;

		memset(SFR_Table, 0, sizeof(SFR_Table));
		memset(PD_Data, 0, sizeof(PD_Data));

		if (AA_SFR) {
			check_SFR_Max();
		}
		else {

			for (int i = 0; i < Field_Cnt; i++) {

				float max_SFR = 0, max_SFR_AVG = 0;
				float spec = find_Spec(i);

				int f = Field_Data[i].Field / 10;
				if (f > 10) f = f / 10;

				PD_Data[f].group[PD_Data[f].cnt++] = i;

				if (i % 2 == 0 && f == 1) {
					center_Field = i;
				}

				Field_Data[i].max_Code = 0;
				Field_Data[i].min_Code = 16384;

				for (int j = 0; j < TF_step; j++) {

					if (TF_Data[j].sfr[i]>max_SFR) {
						max_SFR = TF_Data[j].sfr[i];
						Field_Data[i].Peak_Code = TF_Data[j].code;
					}

					if (i % 2 == 0 || Field_Data[i].Field % 10 == 1)
						if (TF_Data[j].sfr_AVG[i] > max_SFR_AVG) {
							max_SFR_AVG = TF_Data[j].sfr_AVG[i];
							Field_Data[i].Peak_CodeHV = TF_Data[j].code;
						}

					if (TF_Data[j].sfr[i] > spec) {

						if (Field_Data[i].max_Code < TF_Data[j].code)
							Field_Data[i].max_Code = TF_Data[j].code;

						if (Field_Data[i].min_Code > TF_Data[j].code)
							Field_Data[i].min_Code = TF_Data[j].code;

						SFR_Table[j][i] = 0;
					}
					else {
						SFR_Table[j][i] = 1;
					}
				}
			}
		}

		for (int i = 0; i < Field_Cnt; i++) {
			if (Field_Data[i].Field / 10 != 1) {
				Field_Data[i].PD = Field_Data[i].Peak_Code - Field_Data[center_Field].Peak_CodeHV;
				if (i % 2 == 0)
					Field_Data[i].PD_AVG = Field_Data[i].Peak_CodeHV - Field_Data[center_Field].Peak_CodeHV;
			}
		}

		if (Initial_log) {
			Initial_log = false;
			TF_Check_Result << "Source_Time	" << "Result	" << "OK_Range	";

			if (PD_Data[3].cnt > 0)
				TF_Check_Result << "0.3F_HVD	";

			if (PD_Data[5].cnt > 0)
				TF_Check_Result << "0.5F_HVD	";

			if (PD_Data[6].cnt > 0)
				TF_Check_Result << "0.6F_HVD	";

			if (PD_Data[7].cnt > 0)
				TF_Check_Result << "0.7F_HVD	";

			if (PD_Data[8].cnt > 0)
				TF_Check_Result << "0.8F_HVD	";

			if (F3&&PD_Data[3].cnt > 0)
				TF_Check_Result << "0.3F_Tilt	" << "0.3F_PS	" << "0.3F_PS_H	" << "0.3F_PS_V	" << "0.3F_PD	" << "0.3F_PD_H	" << "0.3F_PD_V	";

			if (F5&&PD_Data[5].cnt > 0)
				TF_Check_Result << "0.5F_Tilt	" << "0.5F_PS	" << "0.5F_PS_H	" << "0.5F_PS_V	" << "0.5F_PD	" << "0.5F_PD_H	" << "0.5F_PD_V	";

			if (F6&&PD_Data[6].cnt > 0)
				TF_Check_Result << "0.6F_Tilt	" << "0.6F_PS	" << "0.6F_PS_H	" << "0.6F_PS_V	" << "0.6F_PD	" << "0.6F_PD_H	" << "0.6F_PD_V	";

			if (F7&&PD_Data[7].cnt > 0)
				TF_Check_Result << "0.7F_Tilt	" << "0.7F_PS	" << "0.7F_PS_H	" << "0.7F_PS_V	" << "0.7F_PD	" << "0.7F_PD_H	" << "0.7F_PD_V	";

			if (F8&&PD_Data[8].cnt > 0)
				TF_Check_Result << "0.8F_Tilt	" << "0.8F_PS	" << "0.8F_PS_H	" << "0.8F_PS_V	" << "0.8F_PD	" << "0.8F_PD_H	" << "0.8F_PD_V	";

			if (F3) {
				TF_Check_Result << "0.3F_Adjust	OK_Range	";
				for (int k = 3; k < 9; k++) {
					if (PD_Data[k].cnt > 0) {
						TF_Check_Result <<"0."<<k << "F_PS	";
					}
				}
			}
			if (F5) {
				TF_Check_Result << "0.5F_Adjust	OK_Range	";
				for (int k = 3; k < 9; k++) {
					if (PD_Data[k].cnt > 0) {
						TF_Check_Result << "0." << k << "F_PS	";
					}
				}
			}
			if (F6) {
				TF_Check_Result << "0.6F_Adjust	OK_Range	";
				for (int k = 3; k < 9; k++) {
					if (PD_Data[k].cnt > 0) {
						TF_Check_Result << "0." << k << "F_PS	";
					}
				}
			}
			if (F7) {
				TF_Check_Result << "0.7F_Adjust	OK_Range	";
				for (int k = 3; k < 9; k++) {
					if (PD_Data[k].cnt > 0) {
						TF_Check_Result << "0." << k << "F_PS	";
					}
				}
			}
			if (F8) {
				TF_Check_Result << "0.8F_Adjust	OK_Range	";
				for (int k = 3; k < 9; k++) {
					if (PD_Data[k].cnt > 0) {
						TF_Check_Result << "0." << k << "F_PS	";
					}
				}
			}

			TF_Check_Result << "Max_Adjust	OK_Range	";
			for (int k = 3; k < 9; k++) {
				if (PD_Data[k].cnt > 0) {
					TF_Check_Result << "0." << k << "F_PS	";
				}
			}
			TF_Check_Result << endl;
		}

		OK_Range();

		if (F3&&PD_Data[3].cnt > 0)
			PS_PD_check(3);
		if (F5&&PD_Data[5].cnt > 0)
			PS_PD_check(5);
		if (F6&&PD_Data[6].cnt > 0)
			PS_PD_check(6);
		if (F7&&PD_Data[7].cnt > 0)
			PS_PD_check(7);
		if (F8&&PD_Data[8].cnt > 0)
			PS_PD_check(8);

		if (F3&&PD_Data[3].cnt > 0)
			OK_Range_Adjust(3);
		if (F5&&PD_Data[5].cnt > 0)
			OK_Range_Adjust(5);
		if (F6&&PD_Data[6].cnt > 0)
			OK_Range_Adjust(6);
		if (F7&&PD_Data[7].cnt > 0)
			OK_Range_Adjust(7);
		if (F8&&PD_Data[8].cnt > 0)
			OK_Range_Adjust(8);

		Max_OK_Range_Adjust();
	}

	if (OK[0] >= 0)
		TF_Check_Result << endl;

	fout.close();
	TF_Check_Result.close();

	return ret;
}


DWORD WINAPI myThread(LPVOID argv) {

	char buff[10];
	ZeroMemory(buff, sizeof(buff));
	memcpy(buff, argv, 10);
	bool work = true;
	int s = 0;
	while (1) {
		if (start) {

	//		upload(1);
			Sleep(500);
			subThread->inputSet(QString::number(s++));
		}
		Sleep(1000);
	}

	return NULL;
}

int getLastTF_Setting(string s) {

	int len = s.length() - 1;
	while (len > 0 && s[len] != '\\') {
		len--;
	}
	return len + 1;
}

ThroughFocus::ThroughFocus(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::mQWidget)
{
	int lastTF_Setting;
	ui->setupUi(this);

	TCHAR lpTexts[200];
	src = CT2A(lpTexts);
	lastTF_Setting = getLastTF_Setting(src);
	upper_src = src.substr(0, lastTF_Setting);
	lastTF_Setting = getLastTF_Setting(src3);
	upper_src3 = src3.substr(0, lastTF_Setting);

	GetPrivateProfileString(TEXT("TF_Setting"), TEXT("TF_Data_Path"), TEXT(""), lpTexts, 200, TEXT(".\\Setting\\Setting.ini"));
	TF_Data_Path = CT2A(lpTexts);

	GetPrivateProfileString(TEXT("TF_Setting"), TEXT("umPerCode"), TEXT(""), lpTexts, 8, TEXT(".\\Setting\\Setting.ini"));
	umPerCode = atof(CT2A(lpTexts));

	Center_H = GetPrivateProfileInt(_T("TF_Setting"), TEXT("Center_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Center_V = GetPrivateProfileInt(_T("TF_Setting"), TEXT("Center_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H3 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("3F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V3 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("3F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	LR_H3 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("3F_LR_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	LR_V3 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("3F_LR_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H5 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("5F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V5 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("5F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H6 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("6F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V6 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("6F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H7 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("7F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V7 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("7F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H8 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("8F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V8 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("8F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_H9 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("9F_Dia_H"), 0, TEXT(".\\Setting\\Setting.ini"));
	Dia_V9 = GetPrivateProfileInt(_T("TF_Setting"), TEXT("9F_Dia_V"), 0, TEXT(".\\Setting\\Setting.ini"));
	Line_pair= GetPrivateProfileInt(_T("TF_Setting"), TEXT("Line_pair"), 0, TEXT(".\\Setting\\Setting.ini"));

	TF_step = GetPrivateProfileInt(_T("TF_Setting"), TEXT("TF_step"), 0, TEXT(".\\Setting\\Setting.ini"));

	F_POS[2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("2F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[3] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("3F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[4] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("4F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[5] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("5F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[6] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("6F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[7] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("7F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[8] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("8F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));
	F_POS[9] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("9F_Pos"), 0, TEXT(".\\Setting\\Setting.ini"));

	PD_Spec[3][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec3"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[3][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_H3"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[3][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_V3"), 0, TEXT(".\\Setting\\Setting.ini"));

	PS_Spec[3][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec3"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[3][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_H3"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[3][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_V3"), 0, TEXT(".\\Setting\\Setting.ini"));

	PD_Spec[5][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec5"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[5][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_H5"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[5][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_V5"), 0, TEXT(".\\Setting\\Setting.ini"));

	PS_Spec[5][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec5"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[5][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_H5"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[5][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_V5"), 0, TEXT(".\\Setting\\Setting.ini"));

	PD_Spec[6][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec6"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[6][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_H6"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[6][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_V6"), 0, TEXT(".\\Setting\\Setting.ini"));

	PS_Spec[6][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec6"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[6][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_H6"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[6][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_V6"), 0, TEXT(".\\Setting\\Setting.ini"));

	PD_Spec[7][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec7"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[7][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_H7"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[7][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_V7"), 0, TEXT(".\\Setting\\Setting.ini"));

	PS_Spec[7][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec7"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[7][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_H7"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[7][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_V7"), 0, TEXT(".\\Setting\\Setting.ini"));

	PD_Spec[8][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec8"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[8][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_H8"), 0, TEXT(".\\Setting\\Setting.ini"));
	PD_Spec[8][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PD_Spec_V8"), 0, TEXT(".\\Setting\\Setting.ini"));

	PS_Spec[8][0] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec8"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[8][1] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_H8"), 0, TEXT(".\\Setting\\Setting.ini"));
	PS_Spec[8][2] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("PS_Spec_V8"), 0, TEXT(".\\Setting\\Setting.ini"));

	OK_Range_Spec = GetPrivateProfileInt(_T("TF_Setting"), TEXT("OK_Range_Spec"), 0, TEXT(".\\Setting\\Setting.ini"));
	Item_Select = GetPrivateProfileInt(_T("TF_Setting"), TEXT("Item_Select"), 0, TEXT(".\\Setting\\Setting.ini"));
	HVD_Spec[8] = GetPrivateProfileInt(_T("TF_Setting"), TEXT("HVD_Spec"), 0, TEXT(".\\Setting\\Setting.ini"));
	Skip_Data = GetPrivateProfileInt(_T("TF_Setting"), TEXT("Skip_Data"), 0, TEXT(".\\Setting\\Setting.ini"));

	for (int i = 0; i < 65; i++) {
		string f = "SFR_Data_";
		f += to_string(i + 1);
		Field_Data[i].Field = GetPrivateProfileInt(_T("TF_Setting"), CA2CT(f.c_str()), 0, TEXT(".\\Setting\\Setting.ini"));
		if (Field_Data[i].Field == 0) {
			Field_Cnt = i;
			break;
		}
	}

	ui->data_path->setText(TF_Data_Path.c_str());

	if (Item_Select & 1 > 0) {
		ui->F3->setChecked(true);
		F3 = true;
	}
	else {
		ui->F3->setChecked(false);
		F3 = false;
	}

	int x = Item_Select & 2;

	if ((Item_Select & 2) > 0) {
		ui->F5->setChecked(true);
		F5 = true;
	}
	else {
		ui->F5->setChecked(false);
		F5 = false;
	}

	if ((Item_Select & 4) > 0) {
		ui->F6->setChecked(true);
		F6 = true;
	}
	else {
		ui->F6->setChecked(false);
		F6 = false;
	}

	if ((Item_Select & 8) > 0) {
		ui->F7->setChecked(true);
		F7 = true;
	}
	else {
		ui->F7->setChecked(false);
		F7 = false;
	}

	if ((Item_Select & 16) > 0) {
		ui->F8->setChecked(true);
		F8 = true;
	}
	else {
		ui->F8->setChecked(false);
		F8 = false;
	}

	if ((Item_Select & 32) > 0) {
		ui->curve_fit->setChecked(true);
		Curve_Fit = true;
	}
	else {
		ui->curve_fit->setChecked(false);
		Curve_Fit = false;
	}

	if ((Item_Select & 64) > 0) {
		ui->MTF->setChecked(true);
		MTF = true;
	}
	else {
		ui->MTF->setChecked(false);
		MTF = false;
	}

	if ((Item_Select & 128) > 0) {
		ui->move_Center->setChecked(true);
		move_Center = true;
	}
	else {
		ui->move_Center->setChecked(false);
		move_Center = false;
	}

	if ((Item_Select & 256) > 0) {
		ui->AA_SFR->setChecked(true);
		AA_SFR = true;
	}
	else {
		ui->AA_SFR->setChecked(false);
		AA_SFR = false;
	}

}

ThroughFocus:: ~ThroughFocus()
{
	TerminateProcess(myHandle, 4);
	delete ui;
}


void ThroughFocus::displayText(QString s) {

	Display_Result();
}


void ThroughFocus::on_pushButton_Start_clicked() {

	string s = "1";

	if (start) {
		start = false;
		ui->pushButton_Start->setText("Start");

	}
	else {
		connect(subThread, SIGNAL(changeText(QString)), this, SLOT(displayText(QString)));
		start = true;
		ui->pushButton_Start->setText("Stop");
		string s = "";
		HANDLE myHandle = CreateThread(NULL, 0, myThread, (LPVOID)s.c_str(), 0, NULL);
		subThread->start();
	}
}


void ThroughFocus::get_Selection() {

	if (ui->F3->isChecked()) {
		Item_Select |= 1;
		F3 = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 1;
		F3 = false;
	}
	if (ui->F5->isChecked()) {
		Item_Select |= 2;
		F5 = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 2;
		F5 = false;
	}
	if (ui->F6->isChecked()) {
		Item_Select |= 4;
		F6 = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 4;
		F6 = false;
	}
	if (ui->F7->isChecked()) {
		Item_Select |= 8;
		F7 = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 8;
		F7 = false;
	}
	if (ui->F8->isChecked()) {
		Item_Select |= 16;
		F8 = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 16;
		F8 = false;
	}
	if (ui->curve_fit->isChecked()) {
		Item_Select |= 32;
		Curve_Fit = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 32;
		Curve_Fit = false;
	}
	if (ui->MTF->isChecked()) {
		Item_Select |= 64;
		MTF = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 64;
		MTF = false;
	}
	if (ui->move_Center->isChecked()) {
		Item_Select |= 128;
		move_Center = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 128;
		move_Center = false;
	}
	if (ui->AA_SFR->isChecked()) {
		Item_Select |= 256;
		move_Center = true;
	}
	else {
		Item_Select &= 0xFFFFFFFF - 256;
		move_Center = false;
	}


	WritePrivateProfileString(TEXT("TF_Setting"), TEXT("Item_Select"), CA2CT(to_string(Item_Select).c_str()), TEXT(".\\Setting\\Setting.ini"));

}


void ThroughFocus::on_pushButton_Manual_clicked() {

	fin.open(TF_Data_Path, ios::ate);
	get_Selection();
	show_Diagram = true;
	int ret = upload(1);
	if (ret > -2) {
		Display_Result();
		on_pushButton_Diagram_clicked();
	}
	else {
		ui->log->setText("Data Format Error!");

	}
	fin.clear();
	fin.close();
}


void ThroughFocus::on_pushButton_Test_All_clicked()
{
	fin.open(TF_Data_Path, ios::ate);
	get_Selection();
	show_Diagram = false;
	int ret = 0, n = 1;
	while (ret != -3) {
		ret = upload(n);
		memset(TF_Data, 0, sizeof(TF_Data));
		n++;
	}
	if (ret == -3) {
		ui->log->setText("Some Data Format Error!");
	}
	else {
		n++;
		string str = to_string(n) + "EA Data Dump done!";
		ui->log->setText(str.c_str());
	}
	fin.clear();
	fin.close();
}


void ThroughFocus::on_pushButton_Save_clicked() {

	get_Selection();

	TF_Data_Path = ui->data_path->document()->toPlainText().toLocal8Bit().toStdString();
	WritePrivateProfileString(TEXT("TF_Setting"), TEXT("TF_Data_Path"), CA2CT(TF_Data_Path.c_str()), TEXT(".\\Setting\\Setting.ini"));

}


void ThroughFocus::on_pushButton_open_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open TXT"), "", tr("TXT File(*.txt)"));
	QTextCodec *code = QTextCodec::codecForName("gb18030");
	TF_Data_Path = code->fromUnicode(filename).data();

	WritePrivateProfileString(TEXT("TF_Setting"), TEXT("TF_Data_Path"), CA2CT(TF_Data_Path.c_str()), TEXT(".\\Setting\\Setting.ini"));
	ui->data_path->setText(TF_Data_Path.c_str());
}


void ThroughFocus::on_pushButton_Diagram_clicked()
{
	Mat M(TF_step * 10 + 30, Field_Cnt * 20 + 10, CV_8UC3, Scalar(235, 235, 235));

	for (int j = 0; j < TF_step; j++) {
		Rect rect = Rect(5, 10 * j + 25, Field_Cnt * 20, 10);//起点；长宽
		Scalar color = Scalar(150, 150, 150);
		rectangle(M, rect, color, 1);
	}

	int f = 0, c = 0;
	for (int i = 0; i < Field_Cnt; i++) {
		for (int j = 0; j < TF_step; j++) {
			int field = Field_Data[i].Field / 10;
			if (field > 10)
				field = field / 10;
			Scalar color = Scalar(field % 3 * 50, field % 4 * 50, field % 5 * 50);
			if (SFR_Table[j][i] == 1) {
				Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
				rectangle(M, rect, color, 2);
			}
 
			if (TF_Data[j].code == Field_Data[i].Peak_Code) {
				Rect rect = Rect(20 * i + 6, 10 * j + 26, 18, 8);//起点；长宽			
				rectangle(M, rect, Scalar(0, 150, 150), 1);
			}

			if (field != f) {
				f = field;
				string text = "0";
				if (f > 1) {
					text += ".";
					if (f == 2)
						text += to_string(f + 1);
					else
						text += to_string(f);
				}
				text += "F";
				putText(M, text, Point(i * 20 + 10, 20), 5, 1, color, 2);
			}
		}
	}

	Rect rect = Rect(5, 10 * (TF_step / 2) + 25, Field_Cnt * 20, 10);//起点；长宽
	Scalar color = Scalar(0, 0, 255);
	rectangle(M, rect, color, 1);

	imshow("SFR_Table", M);   //窗口中显示图像
	imwrite("SFR_Table.jpg", M);
	getchar();

}



//getFitPoint函数用于获取拟合后的数据点
vector<double> getFitPoint(vector<double> coArr, vector<double> pointArr)
{
	vector<double> finalPoint;
	if (pointArr.size() == 0 || coArr.size() == 0)
	{
		cout << "数据点有误！" << endl;
	}
	if (pointArr.size() > 0 && coArr.size() > 0)
	{
		for (int i = 0; i < pointArr.size(); i++)
		{
			double temp = 0;
			for (int j = 0; j < coArr.size(); j++)
			{
				temp += pow(pointArr[i], j)*coArr[j];
			}
			finalPoint.push_back(temp);
		}
	}
	return finalPoint;
}

//fitCurve函数用于曲线拟合
vector<double> fitCurve(vector<double> arr_1, vector<double> arr_2, int n)
{
	CLS m_cls;
	vector<double> coefficientsSet;
	if (arr_1.size() != arr_2.size())
	{
		cout << " 输入数据有误！" << endl;
	}
	if (arr_1.size() == arr_2.size())
	{
		for (int i = 0; i < arr_1.size(); i++)
		{
			m_cls.addPoint(arr_1[i], arr_2[i]);
		}
		m_cls.setN(n);
		m_cls.Solve();
		double *t_paracof = m_cls.getSolution();
		for (int i = 0; i < n + 1; i++)
		{
			coefficientsSet.push_back(t_paracof[i]); //多项式的系数项，第一项为常数项，最后一项为x^n项
		}
	}
	return coefficientsSet;
}


void check_MTF_Max() {

	for (int i = 0; i < 10; i++)
		OK[i] = 1;

	memset(SFR_Table, 0, sizeof(SFR_Table));
	memset(PD_Data, 0, sizeof(PD_Data));

	for (int i = 0; i < Field_Cnt; i++) {

		float max_SFR = 0, max_SFR_AVG = 0; int max_step = 0, max_AVG_step = 0;
		float spec = find_Spec(i) * 100;

		int f = Field_Data[i].Field / 10;
		if (f > 10) f = f / 10;

		PD_Data[f].group[PD_Data[f].cnt++] = i;

		if (i % 2 == 0 && f == 1) {
			center_Field = i;
		}
		if (TF_Data[0].code > TF_Data[TF_step - 1].code){
			Field_Data[i].max_Code = TF_Data[TF_step - 1].code;
			Field_Data[i].min_Code = TF_Data[0].code;
		}
		else {
			Field_Data[i].max_Code = TF_Data[0].code;
			Field_Data[i].min_Code = TF_Data[TF_step - 1].code;
		}

		for (int j = 0; j < TF_step; j++) {
			if (TF_Data[j].sfr[i]>max_SFR) {
				max_SFR = TF_Data[j].sfr[i];
				max_step = j;
				Field_Data[i].Peak_Code = TF_Data[j].code;
			}
			if (i % 2 == 0 || Field_Data[i].Field % 10 == 1)
				if (TF_Data[j].sfr_AVG[i] > max_SFR_AVG) {
					max_SFR_AVG = TF_Data[j].sfr_AVG[i];
					max_AVG_step = j;
					Field_Data[i].Peak_CodeHV = TF_Data[j].code;
				}
			//debug_log << TF_Data[j].code << "	" << TF_Data[j].sfr[i] << endl;
		}

		if(i==0)
			center_Max_Step = max_step;

		vector<Point2d> in_point;
		vector<double> Data_x, Data_y;

		int start = max_step - 5, end = max_step + 5;
		if (start < 0) {
			start = 0;
			end -= start;
		}
		if (end > TF_step - 1){
			end = TF_step - 1;
			start -= end - TF_step + 1;
		}
		for (int j = start; j <= end; j++) {
			in_point.push_back(Point2d(TF_Data[j].code, TF_Data[j].sfr[i]));
			//step1: 读取txt文件中的数据
			Data_x.push_back(TF_Data[j].code);
			Data_y.push_back(TF_Data[j].sfr[i]);
		}

		//step2:调用拟合函数
		// CoArr 表示多项式拟合的系数
		// myRes 表示拟合的系数与拟合前的横坐标，计算得到新的纵坐标
		vector<double> CoArr = fitCurve(Data_x, Data_y, 7); //调用函数，4表示阶数，（可以随意取，1—线性拟合，2—平方拟合，3—立方拟合,>=4,高阶拟合）

		float st = 1;
		if (TF_Data[0].code > TF_Data[TF_step - 1].code)
			st *= -1;

		double P[5];
		for (double x = TF_Data[start].code; x > TF_Data[end].code; x += st) {

			double sfr = 0;
			for (int j = 0; j < CoArr.size(); j++){
				sfr += pow(x, j)*CoArr[j];
			}

	//		debug_log <<x<<"	"<< sfr << endl;

			if (sfr>max_SFR) {
				max_SFR = sfr;
				Field_Data[i].Peak_Code = x;
			}

			if (sfr > spec) {
				if (Field_Data[i].max_Code < x)
					Field_Data[i].max_Code = x;

				if (Field_Data[i].min_Code > x)
					Field_Data[i].min_Code = x;
			}
		}
		// for AVG SFR Peak code search
		if (i % 2 == 0 || Field_Data[i].Field % 10 == 1) {

			start = max_step - 5, end = max_step + 5;
			if (start < 0) {
				start = 0;
				end -= start;
			}
			if (end > TF_step - 1) {
				end = TF_step - 1;
				start -= end - TF_step + 1;
			}
			vector<double> Data_x1, Data_y1;
			for (int j = start; j <= end; j++) {
				Data_x1.push_back(TF_Data[j].code);
				Data_y1.push_back(TF_Data[j].sfr_AVG[i]);
			}
			vector<double> CoArr1 = fitCurve(Data_x1, Data_y1, 7); //调用函数，4表示阶数，（可以随意取，1—线性拟合，2—平方拟合，3—立方拟合,>=4,高阶拟合）

			for (double x = TF_Data[start].code; x > TF_Data[end].code; x += st) {

				double sfr = 0;
				for (int j = 0; j < CoArr1.size(); j++) {
					sfr += pow(x, j)*CoArr1[j];
				}
				if (sfr>max_SFR_AVG) {
					max_SFR_AVG = sfr;
					Field_Data[i].Peak_CodeHV = x;
				}
			}
		}
		// for diagram drawing flag
		for (int j = 0; j < TF_step; j++) {

			if (TF_Data[j].sfr[i] > spec) {
				SFR_Table[j][i] = 0;
			}
			else {
				SFR_Table[j][i] = 1;
			} 
		}
	}

	for (int i = 0; i < Field_Cnt; i++) {
		if (Field_Data[i].Field / 10 != 1) {
			Field_Data[i].PD = Field_Data[i].Peak_Code - Field_Data[center_Field].Peak_Code;
			if (i % 2 == 0)
				Field_Data[i].PD_AVG = Field_Data[i].Peak_CodeHV - Field_Data[center_Field].Peak_CodeHV;
		}
	}

	if (Initial_log) {
		Initial_log = false;
		TF_Check_Result << "Module_No.	" <<"Result	"<< "OK_Range	";

		if (PD_Data[3].cnt > 0)
			TF_Check_Result << "0.3F_HVD	";

		if (PD_Data[5].cnt > 0)
			TF_Check_Result << "0.5F_HVD	";

		if (PD_Data[6].cnt > 0)
			TF_Check_Result << "0.6F_HVD	";

		if (PD_Data[7].cnt > 0)
			TF_Check_Result << "0.7F_HVD	";

		if (PD_Data[8].cnt > 0)
			TF_Check_Result << "0.8F_HVD	";

		if (PD_Data[9].cnt > 0)
			TF_Check_Result << "0.9F_HVD	";

		if (F3&&PD_Data[3].cnt > 0)
			TF_Check_Result << "0.3F_PS	" << "0.3F_PS_H	" << "0.3F_PS_V	" << "0.3F_PD	" << "0.3F_PD_H	" << "0.3F_PD_V	";

		if (F5&&PD_Data[5].cnt > 0)
			TF_Check_Result << "0.5F_PS	" << "0.5F_PS_H	" << "0.5F_PS_V	" << "0.5F_PD	" << "0.5F_PD_H	" << "0.5F_PD_V	";

		if (F6&&PD_Data[6].cnt > 0)
			TF_Check_Result << "0.6F_PS	" << "0.6F_PS_H	" << "0.6F_PS_V	" << "0.6F_PD	" << "0.6F_PD_H	" << "0.6F_PD_V	";

		if (F7&&PD_Data[7].cnt > 0)
			TF_Check_Result << "0.7F_PS	" << "0.7F_PS_H	" << "0.7F_PS_V	" << "0.7F_PD	" << "0.7F_PD_H	" << "0.7F_PD_V	";

		if (F8&&PD_Data[8].cnt > 0)
			TF_Check_Result << "0.8F_PS	" << "0.8F_PS_H	" << "0.8F_PS_V	" << "0.8F_PD	" << "0.8F_PD_H	" << "0.8F_PD_V	";


		if (PD_Data[5].cnt > 0) {
			TF_Check_Result << "0.5F_Adjust	OK_Range	0.3F_PS	0.5F_PS	0.8F_PS	";
		}

		if (PD_Data[8].cnt > 0) {
			TF_Check_Result << "0.8F_Adjust	OK_Range	0.3F_PS	0.5F_PS	0.8F_PS	";
		}

		TF_Check_Result << "Max_Adjust	OK_Range	0.3F_PS	0.5F_PS	0.8F_PS	";

		TF_Check_Result << endl;
	}

	TF_Check_Result << "Module_" << Module_No << "#";

	OK_Range();

	if (F3&&PD_Data[3].cnt > 0)
		PS_PD_check(3);
	if (F5&&PD_Data[5].cnt > 0)
		PS_PD_check(5);
	if (F6&&PD_Data[6].cnt > 0)
		PS_PD_check(6);
	if (F7&&PD_Data[7].cnt > 0)
		PS_PD_check(7);
	if (F8&&PD_Data[8].cnt > 0)
		PS_PD_check(8);

	TF_Check_Result << endl;

}


void ThroughFocus::on_pushButton_MTF_csv_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open csv"), "", tr("Image File(*.csv)"));
	QTextCodec *code = QTextCodec::codecForName("gb18030");
	std::string name = code->fromUnicode(filename).data();

	get_Selection();
	show_Diagram = true;
	
	ifstream in(name);

	if (!in.is_open())
	{
		QString strDisplay = "File open Failed";
		strDisplay += '\n';
		ui->log->insertPlainText(strDisplay);
	}

	fout.open(".\\MTF_logData.txt");
	TF_Check_Result.open("MTF_TF_Check_Result.txt", ios::app);

	memset(TF_Data, 0, sizeof(TF_Data));
	
	int f = 0,current_step = -1; Module_No = 0;
	while (getline(in, src))
	{
		int n = 5, len = src.length();

		if (src[0] == 'S'&&src[1] == 't'&&src[2] == 'e'&&src[3] == 'p') {

			int x = 0, LP = Line_pair / 5 + 3;
			while (src[n] != ',') {
				x = x * 10 + src[n] - 48;
				n++;
			}
			if (current_step != x-1) {
				current_step = x-1;
				f = 0;
			}
			else {
				f++;
			}
			char s[6] = { 0 }; x = 0;
			n++;
			while (src[n] != ',') {
				s[x++] = src[n++];
			}
			TF_Data[current_step].code = atof(s)*1000;

			while (LP > 0) {
				if (src[n] == ',') {
					LP--;
				}
				n++;
			}
			char s1[6] = { 0 }; x = 0;
			while (src[n] != ',') {
				s1[x++] = src[n++];
			}

			TF_Data[current_step].sfr[f] = atof(s1);

			if (f % 2 == 1) {
				TF_Data[current_step].sfr_AVG[f - 1] = (TF_Data[current_step].sfr[f] + TF_Data[current_step].sfr[f - 1]) / 2;
			}

		}

		if (src[0] == '['){		
			if (f > 10) {
				Module_No++;
				Field_Cnt = f + 1;
				TF_step = current_step + 1;
				check_MTF_Max();
		
				if (Module_No == 1){
					on_pushButton_Diagram_clicked();
					ui->result->clear();

					ui->result->setFontPointSize(9);
					ifstream in(".\\MTF_logData.txt");
					ostringstream outStr;
					outStr << in.rdbuf();
					string outContent = outStr.str();
					ui->result->insertPlainText(outContent.c_str());
					in.close();
					
				}
			}
			current_step = -1;
			memset(TF_Data, 0, sizeof(TF_Data));
		}

	}

	if (f > 10) {
		Module_No++;
		Field_Cnt = f + 1;
		TF_step = current_step + 1;
		check_MTF_Max();

		if (Module_No == 1) {
			on_pushButton_Diagram_clicked();
			ui->result->clear();

			ui->result->setFontPointSize(9);
			ifstream in(".\\MTF_logData.txt");
			ostringstream outStr;
			outStr << in.rdbuf();
			string outContent = outStr.str();
			ui->result->insertPlainText(outContent.c_str());
			in.close();
		}
	}

}
