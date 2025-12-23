#include "mg_list.h"
#include "unittests.h"

// result 0 - success
// else - failure
int smoke_test()
{
	MGList* lst = mg_list_create(NULL, NULL, NULL);
	mg_list_destroy(lst);
	return 0;
}

int my_first_test()
{
	mg_list_destroy(NULL);
	return 0;
}

/// List of tests
MGUnitTest test_list[] = {
	{ smoke_test, "Smoke Test 1" },
	{ my_first_test, "My first test" },
	{ nullptr, "", 0 }
};









int main(int argc, char* argv[])
{
	mg_ut_run(test_list);
	mg_ut_report(test_list);
	return 0;
}
