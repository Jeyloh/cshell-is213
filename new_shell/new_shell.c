#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
//Simple beginnings of a shell
int main() {
	printf("Welcome to the new shell!\n");
	char c[1024];
	while(1) {
		fgets(c, 1024, stdin);
		system(c);
	}
	return 0;
}
