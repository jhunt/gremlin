#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char **ll;
	int i, j, l, n;

	n = argc - 1;
	ll = calloc(n, sizeof(char *));
	for (i = 0; i < n; i++) {
		l = strlen(argv[i+1]);

		ll[i] = malloc(l+1);
		ll[i][l] = '\0';
		for (j = 0; j < l; j++) {
			ll[i][j] = argv[i+1][l-j-1];
		}
	}

	for (i = 0; i < n; i++) {
		printf("[%d] '%s'\n", i, ll[i]);
		free(ll[i]);
	}
	free(ll);
	return 0;
}
