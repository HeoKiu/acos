#include <stdio.h>
#include <math.h>

extern double my_sin(double x);

double sin(double x){
    double res = x;
    double curr = x;
    int a = 1;
    for(int i = 0; i < 100; ++i){
        a++;
        curr/=a;
        a++;
        curr/=a;
        curr*=-1;
        curr*=x;
        curr*=x;
        res += curr;
    }
    return res;
}

int main() {
    int n = 10;
    for (double x = 0; x < n; ++x) {
        printf("x = %lf; sin(x) = %lf; my_sin(x) = %lf\n", x, sin(x), my_sin(x));
    }
    return 0;
}
