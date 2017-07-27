#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int testFiles = 1;//number of files in the folder created
int sizeFiles = 1000;//number of random packets in each file

int main(){
	int n = 1, i = 1;
	for(n = 1; n <= testFiles; n++){

		char name[12];

		sprintf(name, "test1%d.in", n+1);

		FILE * fileN;
		fileN = fopen(name, "w");

		fprintf(fileN, "38080\n\n");
		for(i = 1; i<=sizeFiles ; i++){
		fprintf(fileN, "0.0 0.5 tests/%dkb.txt\n", rand()%128+1);
		}
	}

return 0;
}
