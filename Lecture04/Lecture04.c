#include <stdio.h>
#include <stdlib.h>

#define NUM 100

int main()
{
	int i, j, data = 0;

	int a[NUM];
	int b[NUM];

	for (i = 0;i < NUM;i++){
		a[i] = (rand() % NUM);
		for (j = 0;j < i;j++){
			if (a[i] == a[j]){
				i--;
			}
		}
	}

	/*
	for (i = 0; i < NUM - 1; i++){
		for (j = i + 1; j < NUM; j++){
			if (b[i] > b[j]) {
				data = b[i];
				b[i] = b[j];
				b[j] = data;
			}
		}
	}
	*/

	for (int i = 0; i < NUM; i++){
		data = -1;
		for (int j = 0; j < NUM; j++){
			if (a[j] == i) {
				data = j;
				break;
			}
		}
		b[i] = data;
	}

	printf("i   \ta\tb");
	for (i = 0; i < NUM; i++){
		//Sleep(5);
		printf("\n%d   \t%d\t%d", i, a[i], b[i]);
	}

	return 0;
}