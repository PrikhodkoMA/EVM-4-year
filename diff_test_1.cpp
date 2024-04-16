#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "diff_func.hpp"

int main()
{
    int n;
    double A;
    double h = 1;

    double *error; // 6 * 3 * 4 матрица ошибок (для шести схем три значения А и четыре мелкости разбиения,)
    FILE *out;
    error = new double [6 * 3 * 4];

    fill_errors(error);

    //Создаем файл со значениями ошибок
    out = fopen("output.txt", "w");

    print_errors(error, out);
    fclose(out);

    delete [] error;
    system("less ./'output.txt'"); // открыть файл в консоли с возможностью прокрутки
    
    return 0;
}
