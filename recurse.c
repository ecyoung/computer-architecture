#include<stdio.h>
#include<stdlib.h>

int recurse (int n){
    if (n == 0) return 0;
    else if (n == 1) return 1;
    else {
        return recurse(n-1) + 2*recurse(n-2) + 3; //f(N) = f(N-1)+2*f(N-2)+3
    }
}

int main(int argc, char *argv[]){
    printf("%d\n", recurse(atoi(argv[1])));
return 0;
}



