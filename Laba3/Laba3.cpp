#include <windows.h>        // for SetConsoleCursorPosition(...)
#include <conio.h>          // for _getch()

#include <iostream>
#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////
// настройки вывода в консоль
//
								// т.к. по заданию максимальное число целое и максимум трехзначное,
								// то ограничимся выводом трех знаков и отведем один знак на отступ
const int g_CellSize = 4;       // размер ячейки для вывода
const int gSleep = 200;          // задержка (в милисекундах) при анимированном выводе матрицы

int g_xOffset = 2;              // верхний левый угол матрицы по X-оси (слева направо)
int g_yOffset = 2;              // верхний левый угол матрицы по Y-оси (сверху вниз)
//
// 
/////////////////////////////////////////////////////////////////

void  GoToXY(COORD coord);                          // переместить курсор вывода в кооринаты coord
void  GoToXY(int x, int y);                         // переместить курсор вывода в кооринаты (x,y)
COORD GetCoord(int row, int col);                   // получить координаты для ячейки матрицы A[row][col]

void  EraseLine(int x, int y);                      // стереть (забить пробелами) строку в консоли

void  PrintMatrix(int* matrix, int N);              // вывод матрицы на экран в позиции (g_xOffset,g_yOffset)

void  IToPos(int index, int N, int& row, int& col); // пересчет индекса (линейного) в строку и столбец
int   PosToI(int row, int col, int N);              // пересчет строки и столбца в индекс (линейный) 

int   GetValue(int row, int col, int N, int G);     // получение значения для ячейки матрицы/массива
int   GetValue(int index, int N, int G);            // получение значения для ячейки матрицы/массива

///////////////////////////////////////////////////////////////////////////////
// функции расчета положения в массиве в зависимости от индекса
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
	case 2:	iOffset = GetOffsetA(index, N);		break;	// заполнение матрицы по схеме 'a' - спираль
	case 3:	iOffset = GetOffsetB(index, N);		break; 	// заполнение матрицы по схеме 'b' - змейка
	case 0:
	default:
		iOffset = GetOffset0(index, N); 		break;  // последовательное заполнение (для отладки)
	}
	return iOffset;
}

// для заполнения по порядку
int   GetOffset0(int index, int N)
{
	int iOffset = 0;

	int row = index / N;
	int col = index - (row*N);

	iOffset = PosToI(row, col, N);
	return iOffset;
}

// заполнение по схеме 'a' (спираль, по часовой стрелке)
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
		// идем влево
		{
			row = r; r++;
			for (col = c; col <= N; col++)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// идем вниз
		{
			col = N;
			for (row = r; row <= N; row++)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// идем вправо
		{
			row = N;
			for (col = N - 1; col >= c; col--)
			{
				if (ix == index) { return PosToI(row, col, orN); }
				ix++;
			}
		}

		// идем вверх
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

// заполнение по схеме 'b' (вертикальная змейка)
int   GetOffsetB(int index, int N)
{
	int iOffset = 0;

	int col = index / N;
	int row = index - (col*N);

	if (col % 2)            // колонка не четная
	{                       // идем в обратную сторону
		row = N - 1 - row;
	}

	iOffset = PosToI(row, col, N);
	return iOffset;
}

///////////////////////////////////////////////////////////////////////////////
// шейкер-сорт
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
// арифм. операции
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
// перестановка

int ShuffleA(int* A, int N)
{
	if (A == NULL)
		return -1;

	int n2 = N / 2;

	int row, row0 = n2;
	int col, col0 = n2;

	// переставим 1й и 3й квадранты
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

	// переставим 3й и 4й квадранты
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


	// переставим 4й и 2й квадранты
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

	// переставим 1й и 4й квадранты
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

	// переставим 2й и 3й квадранты
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

// получим размерность марицы
int GetMatrixSize()
{
	int N;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "Введите размер матрицы NxN (может быть 6,8, или 10): ";
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
			cout << "Некорректный ввод... Выйти? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// получим тип заполнения матрицы
int GetFillType()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "Выберите схему заполнения матрицы ( [a]-\"спираль\", [b]-\"змейка\", [s]-последовательно ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 2; break;
		case 'b':  return 3; break;
		case 's':  return 0; break;

		default:
		{
			cout << "Некорректный ввод... Выйти? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// получим способ генерации числа (последовательные или случайные)
int GetGenType()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "Выберите тип заполнения матрицы ( [r]-случайные числа, [s]-последовательные числа ): ";
		cin >> ch;

		switch (ch)
		{
		case 'r':  return 0; break;
		case 's':  return 1; break;

		default:
		{
			cout << "Некорректный ввод... Выйти? [y/n] ";
			cin >> ch;

			if (toupper(ch) == 'Y')
				return -1;
		}break;
		}
	}

	return -1;
}

// получим тип математической операции и значение операнда
int GetMathOp()
{
	char ch;

	for (;;)
	{
		EraseLine(0, 0);
		EraseLine(0, 1);

		GoToXY(0, 0);
		cout << "Выберите математическую операцию ( [a]-прибавить, [s]-вычесть, [m]-умножить, [d]-разделить ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 1; break;
		case 's':  return 2; break;
		case 'm':  return 3; break;
		case 'd':  return 4; break;

		default:
		{
			cout << "Некорректный ввод... Выйти? [y/n] ";
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
	cout << "Введите целое число: ";
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
		cout << "Выберите тип перестановки ( [a]-поворот, [b]-крест, [c]-горизонтальный перенос, [d]-вертикальный перенос ): ";
		cin >> ch;

		switch (ch)
		{
		case 'a':  return 1; break;
		case 'b':  return 2; break;
		case 'c':  return 3; break;
		case 'd':  return 4; break;

		default:
		{
			cout << "Некорректный ввод... Выйти? [y/n] ";
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

	int N;                      // размерность матрицы
	int T;                      // способ заполнения матрицы
	int G;                      // способ генерации числа

	int index;

	int matrix[100] = { 0 };    // память под матрицу
	int iSize;                  // размер матрицы
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
	// 1. заполняем данными матрицу
	// одновременно выводим (на каждом шаге)
	// создавая анимацию

	g_xOffset = 2;              // matrix output offset on X-axis (from left to right)
	g_yOffset = 3;              // matrix output offset on Y-axis (from top to bottom)
	PrintMatrix(matrix, N);

	GoToXY(g_xOffset, g_yOffset - 1);
	cout << "1. Заполнение матрицы:";

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
	cout << "1. Матрица заполнена: ";
	/////////////////////////////////////////////////////////////////
	// 2. перестановка блоков
	//
	int iOffs;

	int S = GetShuffleType();
	if (S < 1)
	{
		_getch();
		return 0;
	}

	int e = ShuffleMatrix(matrix, N, S);
	if (e == 0)                        // нет ошибки, выведем матрицу
	{
		iOffs = N + 2;
		g_yOffset = 3 + iOffs;      // matrix output offset on Y-axis (from top to bottom)

		GoToXY(g_xOffset, g_yOffset - 1);
		cout << "2. Перестановка:";

		PrintMatrix(matrix, N);
	}
	/////////////////////////////////////////////////////////////////
	// 3. отсортируем
	//
	ShakerSort(matrix, iSize);

	iOffs = g_CellSize * N + 4;

	g_xOffset = 2 + iOffs;      // matrix output offset on X-axis (from left to right)
	g_yOffset = 3;              // matrix output offset on Y-axis (from top to bottom)

	GoToXY(g_xOffset, g_yOffset - 1);
	cout << "3. Отсортировано (в нормальном порядке):";

	PrintMatrix(matrix, N);
	/////////////////////////////////////////////////////////////////
	// 4. произведем арифм.операцию над матрицей
	//
	int op = GetMathOp();
	int v = GetNumber();
	e = -1;

	if (op < 1)
	{
		_getch();
		return 0;
	}

	// [1]-плюс, [2]-минус, [3]-умножить, [4]-разделить )
	switch (op)
	{
	case 1: e = MatrixAdd(matrix, iSize, v); break;
	case 2: e = MatrixSub(matrix, iSize, v); break;
	case 3: e = MatrixMul(matrix, iSize, v); break;
	case 4: e = MatrixDel(matrix, iSize, v); break;
	}

	if (e == 0)                        // нет ошибки, выведем матрицу
	{
		iOffs = N + 2;
		g_yOffset = 3 + iOffs;      // matrix output offset on Y-axis (from top to bottom)

		GoToXY(g_xOffset, g_yOffset - 1);
		cout << "4. Результат операции:";

		PrintMatrix(matrix, N);
	}


	/////////////////////////////////////////////////////////////////
	EraseLine(0, 0);
	GoToXY(0, 0);
	cout << "Для выхода нажмите любую клавишу...";
	_getch();
	return 0;
}

/////////////////////////////////////////////////////////////////////
// помощники вывода в консоль
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
		cout << "Ошибка: Передан нулевой указатель!" << endl;
		return;
	}

	int row, col;

	// т.к. по заданию максимальное число целое и максимум трехзначное,
	// то ограничимся выводом трех знаков
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
// помощники работы с матрицей
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

// row - номер строки
// col - номер столбца
// N - размерность матрицы
// G - способ генерации
//  G = 0 - для получения случайного числа
//  G = 1 - для получения последовательного значения
int GetValue(int row, int col, int N, int G)
{
	int N2 = N * N;
	int value = int(rand() % N2 + 1);

	if (G)
		value = 1 + (row*N) + col;

	return value;
}

// index - индекс (линейный) в массиве
// N - размерность матрицы
// G - способ генерации
//  G = 0 - для получения случайного числа
//  G = 1 - для получения последовательного значения
int GetValue(int index, int N, int G)
{
	int row, col;
	IToPos(index, N, row, col);

	return GetValue(row, col, N, G);
}
