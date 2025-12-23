#pragma once

typedef int (*MGUnitTestFun)();

typedef struct
{
	MGUnitTestFun fun;
	char name[128];
	int result;
} MGUnitTest;

void mg_ut_run(MGUnitTest* testList);
void mg_ut_report(MGUnitTest* testList);
