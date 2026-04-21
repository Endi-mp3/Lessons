#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>


bool flag_is_recursive = false, flag_is_dir = false;
int arg_dir = -1;

int Grep_File (const char *search, const char *filename);
int Search_In_DirectoryName (const char* search, const char *dirname);

int Search_In_DirectoryName (const char* search, const char *dirname)
{
	DIR *lptrDir = opendir(dirname);
	if (lptrDir == NULL) {
		printf ("open error");
		return -1;
	}
	struct dirent *lptrDirEntr;
	while ( (lptrDirEntr = readdir(lptrDir)) != NULL) {
		char lsTempPath[4096];
		snprintf(lsTempPath, 4096, "%s/%s", dirname, lptrDirEntr->d_name);
		if (lptrDirEntr->d_namlen == 1 && lptrDirEntr->d_name[0] == '.')
			continue;

		if (lptrDirEntr->d_namlen == 2 && lptrDirEntr->d_name[0] == '.' && lptrDirEntr->d_name[1] == '.')
			continue;
			
		if (lptrDirEntr->d_type == DT_REG) {
			Grep_File(search, lsTempPath);
		} else if (lptrDirEntr->d_type == DT_DIR && flag_is_recursive) {
			Search_In_DirectoryName(search, lsTempPath);
		}
	}
	return 0;
}

int Grep_File (const char *search, const char *filename)
{
	FILE *file = fopen(filename, "r");
	char line[1024];
	unsigned line_num = 0;
	if(file == NULL) {
		printf("file dont open");
		return -1;	
	}

	while (fgets(line, sizeof(line), file) != NULL) {
		line_num++;
		if(strstr(line, search) != NULL) {
			printf("%s %d: %s", filename, line_num, line);
		}
	}
	fclose(file);
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc < 4) {
		printf("\n");
		return -1;
	}
	char *searh = argv[1];
	char *filename = argv[2];
	struct stat sb;


	for (int i = 0; i < argc; ++i){
		if (strcmp(argv[i], "-r") == 0){
			flag_is_recursive = true;
		} else if (strcmp(argv[i], "-d") == 0){
			flag_is_dir = true;
			arg_dir = i++ + 1;
		}
	}


	if (stat(filename, &sb)) {
		printf("stat error\n");
		return -2;
	}

	if (!flag_is_dir) {
		if ((sb.st_mode & S_IFMT) == S_IFREG) {
			return Grep_File(searh, filename);
		}
		else {
			return -1;
		}
	} else {
		if ((sb.st_mode & S_IFMT) == S_IFDIR) {
			return Search_In_DirectoryName(searh, filename);
		}
		else {
			return -1;
		}
	}
	return 0;
}
