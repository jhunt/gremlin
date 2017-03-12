#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *ptr;
	int i;

	for (i = 0; i < 10; i++) {
		ptr = malloc(42 + i);
		write(1, ptr ? "." : "x", 1);
	}
	write(1, "\n", 1);

	return 0;
}
