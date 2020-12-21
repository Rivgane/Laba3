#include <windows.h>        // for SetConsoleCursorPosition(...)
#include <conio.h>          // for _getch()

#include <iostream>
#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////
// ��������� ������ � �������
//
								// �.�. �� ������� ������������ ����� ����� � �������� �����������,
								// �� ����������� ������� ���� ������ � ������� ���� ���� �� ������
const int g_CellSize = 4;       // ������ ������ ��� ������
const int gSleep = 200;          // �������� (� ������������) ��� ������������� ������ �������

int g_xOffset = 2;              // ������� ����� ���� ������� �� X-��� (����� �������)
int g_yOffset = 2;              // ������� ����� ���� ������� �� Y-��� (������ ����)
//
// 
/////////////////////////////////////////////////////////////////

void  GoToXY(COORD coord);                          // ����������� ������ ������ � ��������� coord
void  GoToXY(int x, int y);                         // ����������� ������ ������ � ��������� (x,y)
COORD GetCoord(int row, int col);                   // �������� ���������� ��� ������ ������� A[row][col]

void  EraseLine(int x, int y);                      // ������� (������ ���������) ������ � �������

void  PrintMatrix(int* matrix, int N);              // ����� ������� �� ����� � ������� (g_xOffset,g_yOffset)

void  IToPos(int index, int N, int& row, int& col); // �������� ������� (���������) � ������ � �������
int   PosToI(int row, int col, int N);              // �������� ������ � ������� � ������ (��������) 

int   GetValue(int row, int col, int N, int G);     // ��������� �������� ��� ������ �������/�������
int   GetValue(int index, int N, int G);            // ��������� �������� ��� ������ �������/�������

///////////////////////////////////////////////////////////////////////////////
// ������� ������� ��������� � ������� � ����������� �� �������
//
int   GetOffset0(int index, int N);
int   GetOffsetA(int index, int N);
int   GetOffsetB(int index, int N);
int   GetOffset(int index, int N, int type);

int   GetOffset(int index, int N, int type)
{
	int iOffset = 0;

	switch (type)
	{
	case 2:	iOffset = GetOffsetA(index, N);		break;	// ���������� ������� �� ����� 'a' - �������
	case 3:	iOffset = GetOffsetB(index, N);		break; 	// ���������� ������� �� ����� 'b' - ������
	case 0:
	default:
		iOffset = GetOffset0(index, N); 		break;  // ���������������� ���������� (��� �������)
	}
	return iOffset;
}

// ��� ���������� �� �������
int   GetOffset0(int index, int N)
{
	int iOffset = 0;

	int row = index / N;
	int col = index - (row*N);

	iOffset = PosToI(row, col, N);
	return iOffset;
}

// ���������� �� ����� 'a' (�������, �� ������� �������)
int   GetOffsetA(int index, int N)
{
	int iOffset = 0;

	int orN = N;

	int ix = 0;

	int r = 0;
	int c = 0;

	int row = r;
	int col = c;


	for (int i = 0; i < orN; i++)
	{
		N--;
		//
		// ���� �����
		{
			row = r; r++;
			for (col = c; col <= N; col++)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// ���� ����
		{
			col = N;
			for (row = r; row <= N; row++)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// ���� ������
		{
			row = N;
			for (col = N - 1; col >= c; col--)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// ���� �����
		{
			col = c; c++;
			for (row = N - 1; row >= r; row--)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}
	}

	iOffset = PosToI(row, col, orN);
	return iOffset;
}

// ���������� �� ����� 'b' (������������ ������)
int   GetOffsetB(int index, int N)
{
	int iOffset = 0;

	int col = index / N;
	int row = index - (col*N);

	if (col % 2)            // ������� �� ������
	{                       // ���� � �������� �������
		row = N - 1 - row;
	}

	iOffset = PosToI(row, col, N);
	return iOffset;
}

///////////////////////////////////////////////////////////////////////////////
// ������-����
//
void    ShakerSort(int* A, int iSize)
{
	int iRight = iSize - 1;
	int iLeft = 0;
	int iSwap = iRight;
	int i;

	if (A == NULL)
		return;

	for (int g = 0; g < iSize; g++)
	{
		for (i = iLeft; i < iRight; i++)
		{
			int *p0 = A + i;
			int *p1 = p0 + 1;

			if ((*p0) > (*p1))
			{
				std::swap((*p0), (*p1));
				iSwap = i;
			}
		}
		iRight = iSwap;

		for (i = iRight; i > iLeft; i--)
		{
			int *p0 = A + i;
			int *p1 = p0 - 1;

			if ((*p0) < (*p1))
			{
				std::swap((*p0), (*p1));
				iSwap = i;
			}
		}
		iLeft = iSwap;

		if (iLeft >= iRight) break;
	}
}
///////////////////////////////////////////////////////////////////////////////
// �����. ��������
// return =0  - on OK
// return <0 - on Error
//
int MatrixAdd(int* A, int iSize, int op)
{
	if (A == NULL)
		return -1;

	for (int i = 0; i < iSize; i++)
	{
		int v = *A;
		v += op;
		*A = v;
		A++;
	}

	return 0;
}
int MatrixSub(int* A, int iSize, int op)
{
	if (A == NULL)
		return -1;

	for (int i = 0; i < iSize; i++)
	{
		int v = *A;
		v -= op;
		*A = v;
		A++;
	}

	return 0;
}
int MatrixMul(int* A, int iSize, int op)
{
	if (A == NULL)
		return -1;

	for (int i = 0; i < iSize; i++)
	{
		int v = *A;
		v *= op;
		*A = v;
		A++;
	}

	return 0;
}

int MatrixDel(int* A, int iSize, int op)
{
	if (A == NULL)
		return -1;

	if (op == 0)
	{
		GoToXY(0, 1);
		cout << "Error: devide by zero!";
		return -2;
	}

	for (int i = 0; i < iSize; i++)
	{
		int v = *A;
		v /= op;
		*A = v;
		A++;
	}

	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// ������������

int ShuffleA(int* A, int N)
{
	if (A == NULL)
		return -1;

	int n2 = N / 2;

	int row, row0 = n2;
	int col, col0 = n2;

	// ���������� 1� � 3� ���������
	for (row = 0; row < row0; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row + n2, col, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}

	// ���������� 3� � 4� ���������
	for (row = row0; row < N; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row, col + n2, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}


	// ���������� 4� � 2� ���������
	for (row = 0; row < row0; row++)
	{
		for (col = col0; col < N; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row + n2, col, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}


	return 0;
}

int ShuffleB(int* A, int N)
{
	if (A == NULL)
		return -1;

	int n2 = N / 2;

	int row, row0 = n2;
	int col, col0 = n2;

	// ���������� 1� � 4� ���������
	for (row = 0; row < row0; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row + n2, col + n2, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}

	// ���������� 2� � 3� ���������
	for (row = n2; row < N; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row - n2, col + n2, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}

	return 0;
}

int ShuffleC(int* A, int N)
{
	if (A == NULL)
		return -1;

	int n2 = N / 2;

	int row, row0 = n2;
	int col, col0 = N;

	for (row = 0; row < row0; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row + n2, col, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}

	return 0;
}

int ShuffleD(int* A, int N)
{
	if (A == NULL)
		return -1;

	int n2 = N / 2;

	int row, row0 = N;
	int col, col0 = n2;

	for (row = 0; row < row0; row++)
	{
		for (col = 0; col < col0; col++)
		{
			int i0 = PosToI(row, col, N);
			int i1 = PosToI(row, col + n2, N);

			int *p0 = A + i0;
			int *p1 = A + i1;

			std::swap((*p0), (*p1));
		}
	}

	return 0;
}

int ShuffleMatrix(int* A, int N, int S)
{
	int e = -1;

	if (A == NULL)
		return -1;

	switch (S)
	{
	case 1:  e = ShuffleA(A, N); break;
	case 2:  e = ShuffleB(A, N); break;
	case 3:  e = ShuffleC(A, N); break;
	case 4:  e = ShuffleD(A, N); break;
	default: e = -1;
	}

	return e;
};


///////////////////////////////////////////////////////////////////////////////
//

void EraseLine(int x, int y)
{
	char line[121];

	memset(line, int(' '), 120);
	line[120] = '\0';

	GoToXY(x, y);
	printf(line);
}

// ������� ����������� ������
int GetMatrixSize()
{
	int N;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "������� ������ ������� NxN (����� ���� 6,8, ��� 10): ";
		cin >> N;

		switch (N)
		{
		case 6:
		case 8:
		case 10:
			return N;
			break;

		default:
		{
			char ch;
			cout << "������������ ����... �����? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// ������� ��� ���������� �������
int GetFillType()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "�������� ����� ���������� ������� ( [a]-\"�������\", [b]-\"������\", [s]-��������������� ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 2; break;
		case 'b':  return 3; break;
		case 's':  return 0; break;

		default:
		{
			cout << "������������ ����... �����? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// ������� ������ ��������� ����� (���������������� ��� ���������)
int GetGenType()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "�������� ��� ���������� ������� ( [r]-��������� �����, [s]-���������������� ����� ): ";
		cin >> ch;

		switch (ch)
		{
		case 'r':  return 0; break;
		case 's':  return 1; break;

		default:
		{
			cout << "������������ ����... �����? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// ������� ��� �������������� �������� � �������� ��������
int GetMathOp()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "�������� �������������� �������� ( [a]-���������, [s]-�������, [m]-��������, [d]-��������� ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 1; break;
		case 's':  return 2; break;
		case 'm':  return 3; break;
		case 'd':  return 4; break;

		default:
		{
			cout << "������������ ����... �����? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
};


int GetNumber()
{
	int n;

	EraseLine(0, 0);
	EraseLine(0, 1);

	GoToXY(0, 0);
	cout << "������� ����� �����: ";
	cin >> n;

	return n;
};

int GetShuffleType()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "�������� ��� ������������ ( [a]-�������, [b]-�����, [c]-�������������� �������, [d]-������������ ������� ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 1; break;
		case 'b':  return 2; break;
		case 'c':  return 3; break;
		case 'd':  return 4; break;

		default:
		{
			cout << "������������ ����... �����? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}



///////////////////////////////////////////////////////////////////////////////
int main()
{
	setlocale(LC_ALL, "Russian");

	int N;                      // ����������� �������
	int T;                      // ������ ���������� �������
	int G;                      // ������ ��������� �����

	int index;

	int matrix[100] = { 0 };    // ������ ��� �������
	int iSize;                  // ������ �������
	int *ptr;


	N = GetMatrixSize();
	if (N < 0)
	{
		_getch();
		return 0;
	}

	T = GetFillType();
	if (T < 0)
	{
		_getch();
		return 0;
	}

	G = GetGenType();
	if (G < 0)
	{
		_getch();
		return 0;
	}

	/////////////////////////////////////////////////////////////////
	// 1. ��������� ������� �������
	// ������������ ������� (�� ������ ����)
	// �������� ��������

	g_xOffset = 2;              // matrix output offset on X-axis (from left to right)
	g_yOffset = 3;              // matrix output offset on Y-axis (from top to bottom)
	PrintMatrix(matrix, N);

	GoToXY(g_xOffset, g_yOffset - 1);
	cout << "1. ���������� �������:";

	iSize = N * N;
	for (index = 0; index < iSize; index++)
	{
		ptr = matrix;
		ptr = ptr + GetOffset(index, N, T);

		*ptr = GetValue(index, N, G);

		Sleep(gSleep);
		PrintMatrix(matrix, N);
	}

	GoToXY(g_xOffset, g_yOffset - 1);
	cout << "1. ������� ���������: ";
	/////////////////////////////////////////////////////////////////
	// 2. ������������ ������
	//
	int iOffs;

	int S = GetShuffleType();
	if (S < 1)
	{
		_getch();
		return 0;
	}

	int e = ShuffleMatrix(matrix, N, S);
	if (e == 0)                        // ��� ������, ������� �������
	{
		iOffs = N + 2;
		g_yOffset = 3 + iOffs;      // matrix output offset on Y-axis (from top to bottom)

		GoToXY(g_xOffset, g_yOffset - 1);
		cout << "2. ������������:";

		PrintMatrix(matrix, N);
	}
	/////////////////////////////////////////////////////////////////
	// 3. �����������
	//
	ShakerSort(matrix, iSize);

	iOffs = g_CellSize * N + 4;

	g_xOffset = 2 + iOffs;      // matrix output offset on X-axis (from left to right)
	g_yOffset = 3;              // matrix output offset on Y-axis (from top to bottom)

	GoToXY(g_xOffset, g_yOffset - 1);
	cout << "3. ������������� (� ���������� �������):";

	PrintMatrix(matrix, N);
	/////////////////////////////////////////////////////////////////
	// 4. ���������� �����.�������� ��� ��������
	//
	int op = GetMathOp();
	int v = GetNumber();
	e = -1;

	if (op < 1)
	{
		_getch();
		return 0;
	}

	// [1]-����, [2]-�����, [3]-��������, [4]-��������� )
	switch (op)
	{
	case 1: e = MatrixAdd(matrix, iSize, v); break;
	case 2: e = MatrixSub(matrix, iSize, v); break;
	case 3: e = MatrixMul(matrix, iSize, v); break;
	case 4: e = MatrixDel(matrix, iSize, v); break;
	}

	if (e == 0)                        // ��� ������, ������� �������
	{
		iOffs = N + 2;
		g_yOffset = 3 + iOffs;      // matrix output offset on Y-axis (from top to bottom)

		GoToXY(g_xOffset, g_yOffset - 1);
		cout << "4. ��������� ��������:";

		PrintMatrix(matrix, N);
	}


	/////////////////////////////////////////////////////////////////
	EraseLine(0, 0);
	GoToXY(0, 0);
	cout << "��� ������ ������� ����� �������...";
	_getch();
	return 0;
}

/////////////////////////////////////////////////////////////////////
// ��������� ������ � �������
//

void GoToXY(COORD coord)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void GoToXY(int x, int y)
{
	COORD coord;

	coord.X = x;
	coord.Y = y;

	GoToXY(coord);
}


COORD GetCoord(int row, int col)
{
	COORD coord;

	coord.X = g_xOffset + (g_CellSize * col);
	coord.Y = g_yOffset + row;

	return coord;
}

void  PrintMatrix(int* matrix, int N)
{
	int *ptr = matrix;

	if (ptr == NULL)
	{
		cout << "������: ������� ������� ���������!" << endl;
		return;
	}

	int row, col;

	// �.�. �� ������� ������������ ����� ����� � �������� �����������,
	// �� ����������� ������� ���� ������
	for (row = 0; row < N; row++)
	{
		for (col = 0; col < N; col++)
		{
			COORD coord = GetCoord(row, col);
			GoToXY(coord);
			printf("%3d", *ptr);
			ptr++;
		}
	}
};

/////////////////////////////////////////////////////////////////
// ��������� ������ � ��������
//
void IToPos(int index, int N, int& row, int& col)
{
	row = index / N;
	col = index - row * N;
}

int PosToI(int row, int col, int N)
{
	int index = (row* N) + col;
	return index;
}

// row - ����� ������
// col - ����� �������
// N - ����������� �������
// G - ������ ���������
//  G = 0 - ��� ��������� ���������� �����
//  G = 1 - ��� ��������� ����������������� ��������
int GetValue(int row, int col, int N, int G)
{
	int N2 = N * N;
	int value = int(rand() % N2 + 1);

	if (G)
		value = 1 + (row*N) + col;

	return value;
}

// index - ������ (��������) � �������
// N - ����������� �������
// G - ������ ���������
//  G = 0 - ��� ��������� ���������� �����
//  G = 1 - ��� ��������� ����������������� ��������
int GetValue(int index, int N, int G)
{
	int row, col;
	IToPos(index, N, row, col);

	return GetValue(row, col, N, G);
}
