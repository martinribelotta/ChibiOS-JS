#include <stdio.h>

static void do_cat(const char *file) {
	FILE *f = fopen(file, "rt");
	if (f) {
		do {
			char buf[32];
			int r = fread(buf, sizeof(buf), sizeof(char), f);
			if (r > 0)
				fwrite(buf, r, sizeof(char), stdout);
			else if (r<0)
				perror("cat(read)");
			else
				break;
		} while(!feof(f));
		fclose(f);
	} else
		perror("cat(open)");
}

int cmd_cat(int argc, char *argv[]) {
	int i;
	for(i=1; i<argc; i++)
		do_cat(argv[i]);
	return 0;
}
