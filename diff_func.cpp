#include <iostream>
#include <fstream>
#include <cmath>

#include "diff_func.hpp"
using namespace std;


void fill_errors(double* error)
{
    //int n;
    double A;
    double h = 1;
    // Цикл по схемам: для каждой схемы свой слой размерности 3х4
    for(int i = 1; i <= 6; ++i) 
    {
        //n = 1 * (k == 0) + 2 * (k == 1) + 3 * (k == 2) + 6 * (k == 3);
        
        for(int l = 0; l < 3; ++l) // перебираем А
        {
            A = 1 * (l == 0) + 10 * (l == 1) + 1000 * (l == 2);
            for(int k = 0; k < 4 ; ++k) // перебираем n
            {
                h = 0.1 * (k == 0) + 0.01 * (k == 1) + 0.001 * (k == 2) + 0.000001 * (k == 3);
                error[(i - 1) * 3 * 4 + l * 4 + k] = calc_error(h, A, i);
            }
        }
        
    }
}
void print_errors(double* error, FILE* out)
{
    double A;
    double h = 1;
    // Цикл по схемам: для каждой схемы три строки (для каждого А) по 4 погрешности
    for(int i = 1; i <= 6; ++i) // Цикл по схемам
    {
        fprintf(out, "Scheme %d:\n", i);
        fprintf(out, "A:     E1:      E2:      E3:      E6:     \n");
        for(int l = 0; l < 3; ++l) // перебираем А
        {
            A = 1 * (l == 0) + 10 * (l == 1) + 1000 * (l == 2);
            char s[4] = "";
            switch(l)
            {
                case 0:
                    s[0] = s[1] = s[2] = ' ';
                case 1:
                    s[0] = s[1] = ' ';
            }
            fprintf(out, "%.1f%s %.2e %.2e %.2e %.2e\n", A, s, error[(i - 1) * 3 * 4 + l * 4 + 0], \
            error[(i - 1) * 4 * 3 + l * 4 + 1], error[(i - 1) * 3 * 4 + l * 4 + 2], error[(i - 1) * 3 * 4 + l * 4 + 3]);
        }
    }
}
double calc_error(double h, double A, int i) 
{
    double y = 1;
    double y1 = 1 - A * h; 
    double y2;
    double real;
    double max = 0;
    double segment_length = 0.00001; // 1. - для обычного случая 0.01 - для получения точности на плохих схемах
    int N = (int) (segment_length / h) + 1; // n - число из задачи, N - число точек на отрезке
    //printf("N = %d\n", N);
    switch(i)
    {
        // Для схем 1-3 y_k+1 и y_k хранятся в одной переменной
        case 1: // y_k+1 = y_k * (1 - A * h)
            max = 0;
            y = 1;
            // FILE *logs;
            // logs = fopen("logs.txt", "w");
            // if(N > 200 && A > 500) 
            // {
            //     fprintf(logs, "Scheme %d N = %d A = %lf\n", i, N, A);
            //     fprintf(logs, "k y_k:    y(x_k):   max_k:\n");
            // }
            for(int j = 1; j < N; ++j) // цикл по всем y_k
            {
                real = exp(-A * j * h); // x_j = 0 + j * h
                y = y * (1 - A * h);
                if(max < fabs(real - y))
                {
                    max = fabs(real - y);
                }
                //if(N > 200 && A > 500) fprintf(logs, "%d %.2e %.2e %.2e\n", j, y, real, max);
            }
            //fclose(logs);
            return max;
        case 2: // y_k+1 = y_k / (1 + A * h)
            y = 1;
            max = 0;
            for(int j = 1; j < N; ++j)
            {
                real = exp(-A * j * h);
                y = y / (1 + A * h);
                if(max < fabs(real - y))
                {
                    max = fabs(real - y);
                }
            }
            return max;
            
        case 3: //y_k+1 = y_k * (1- A * h / 2) / (1 + A * h / 2)
            y = 1;
            max = 0;
            for(int j = 1; j < N; ++j)
            {
                real = exp(-A * j * h);
                y = y * (1- A * h / 2) / (1 + A * h / 2);
                if(max < fabs(real - y))
                {
                    max = fabs(real - y);
                }
            }
            return max;
            
        // Для схем второго порядка, т.е. 4-6
        // y_k+1 хранится в y2
        // y_k хранится в y1
        // y_k-1 хранится в y
    
        case 4: //y_k+1 = y_k-1 - 2 * y_k * A * h
            max = 0;
            y = 1;
            y1 = 1 - A * h; 
            max = fabs(y1 - exp(-A * h));
            for(int j = 2; j < N; ++j)
            {
                real = exp(-A * j * h);
                // y1 = y_1, y = y_0
                y2 = y - 2 * h * A * y1; //y_2 -> y_3
                y = y1; // y_0 -> y_1
                y1 = y2; // y_1 -> y2
                if(max < fabs(real - y2))
                {
                    max = fabs(real - y2);
                }
            }
            return max;
            
        case 5: // y_k+1 = (2 * y_k - 0.5 * y_k-1) / (1.5 + Ah)
            max = 0;
            y = 1;
            y1 = 1 - A * h;
            max = fabs(y1 - exp(-A * h));
            for(int j = 2; j < N; ++j)
            {
                real = exp(-A * j * h);
                // y1 = y_1, y = y_0
                y2 =(2 * y1 - 0.5 * y) / (1.5 + A * h); //y_2 -> y_3
                y = y1; // y_0 -> y_1
                y1 = y2; // y_1 -> y2
                if(max < fabs(real - y2))
                {
                    max = fabs(real - y2);
                }
            }
            return max;
            
        case 6: // y_k+1 = (- 2 * y_k +(1.5 - A * h) * y_k-1) / (-0.5)
            max = 0;
            y = 1;
            y1 = 1 - A * h;
            max = fabs(y1 - exp(-A * h));
            for(int j = 2; j < N; ++j)
            {
                real = exp(-A * j * h);
                // y1 = y_1, y = y_0
                y2 = (-2 * y1 + (1.5 - A * h) * y) / (-0.5); //y_2 -> y_3
                y = y1; // y_0 -> y_1
                y1 = y2; // y_1 -> y2
                if(max < fabs(real - y2))
                {
                    max = fabs(real - y2);
                }
            }
            return max;
            
            

    }
    return max;

}