#include <stdio.h>
#include "unittests.h"

void mg_ut_run(MGUnitTest* lst)
{
	int i = 0;
	while(lst[i].fun != nullptr) {
		int r = 0;
		try {
			r = lst[i].fun();
		} catch (...) {
			r = -0xF;
		}
		lst[i].result = r;
		printf("Result of execution %s: %s\n", lst[i].name, (lst[i].result == 0) ? "Passed." : "FAILED!");
		i++;
	}
}

void mg_ut_report(MGUnitTest* lst)
{
	int i = 0;
	printf("------- Summary -------\n");
	while(lst[i].fun != nullptr) {
		printf("Result of execution %s: %s\n", lst[i].name, (lst[i].result == 0) ? "Passed." : "FAILED!");
		i++;
	}
}
