#include <stdio.h>
#include <math.h>
#include <Windows.h>

#define PI 3.141592653/*58979323846*/

void moveCursor(int x, int y)
{
    printf("\x1b[%d;%dH",y,x);
}

//매개변수 isExploded
//0: 폭발전 폭탄
//1: 폭발함
void printBomb(int isExploded)
{
    //폭탄은 항상 7칸
    if(isExploded)
    { 
        printf("\x1b[A^^^^^^^");
        printf("\x1b[B\x1b[7D!!BAM!!"); 
        printf("\x1b[B\x1b[7D^^^^^^^");
    }
    else 
        printf("(  b  )"); 
}

int main()
{
	// 여기부터 코드를 작성하세요----------------------   
	printBomb(0);
	int a = 13;
	int posx = 7, posy = 1;
	int x, y;
	double angle = 0.0;
	int length = a;


	while (length > 0)
	{

		x = cos(angle * PI / 180.0);
		y = sin(angle * PI / 180.0);
		int count = 0;
		while (count < length)
		{
			posx += x;
			posy += y;
			moveCursor(posx, posy);
			Sleep(100);
			printf("#");
			count++;
		}
		angle += 90.0;

		x = cos(angle * PI / 180.0);
		y = sin(angle * PI / 180.0);
		length--;
		length--;
		count = 0;
		while (count < length)
		{
			posx += x;
			posy += y;
			moveCursor(posx, posy);

			Sleep(100);
			printf("#");

			count++;
		}
		angle += 90.0;

	}

	moveCursor(posx, posy);
	printf("*");
	Sleep(1000);

	angle -= 180.0;

	while (length < a)
	{
		angle -= 90.0;

		x = cos(angle * PI / 180.0);
		y = sin(angle * PI / 180.0);
		int count = 0;
		while (count < length)
		{
			Sleep(200);
			moveCursor(posx, posy);
			printf(" ");
			posx += x;
			posy += y;
			moveCursor(posx, posy);
			printf("*");
			count++;
		}
		angle -= 90.0;

		x = cos(angle * PI / 180.0);
		y = sin(angle * PI / 180.0);
		length++;
		length++;
		count = 0;
		while (count < length)
		{
			Sleep(200);
			moveCursor(posx, posy);
			printf(" ");
			posx += x;
			posy += y;
			moveCursor(posx, posy);
			if (length != a + 1) printf("*");
			count++;
		}
	}

	moveCursor(0, 0);
	printBomb(1);

    // 여기까지 코드를 작성하세요----------------------   
    moveCursor(10, 20);
    return 0;
}
