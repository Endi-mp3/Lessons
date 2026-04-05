#include <stdio.h>

uint64_t mem_alloc = 0;

#define MY_MALLOC(x)		malloc(x); mem_alloc++
#define MY_FREE(x)			free(x); mem_alloc--

enum Wifi_Error_Codes
{
	wifi_ret_ememory = -3,
	wifi_ret_eabort = -2,
	wifi_ret_esignal = -1,
	wifi_ret_ok = 0
};

int ardfgsd[123] = { 1, 2, 2, 32,131,21,23,123,123,123,12, 312, 31,231,23 ,123 ,123};
int wifi_get_some_data(int dataID)
{
	return ardfgsd[dataID];
}

int wifi_some_func(int param)
{
	int* piMem = malloc(param);
	if (!piMem)	{
		return wifi_ret_ememory;
	}

	if (wifi_get_some_data(12) < 100) {
		free(piMem);
		return wifi_ret_esignal;
	}


	int* piMem2 = malloc(param);

	if (!piMem2){
		free(piMem);
		return wifi_ret_ememory;
	}
	if (wifi_get_some_data(20) > 10) {
		free(piMem);
		free(piMem2);
		return wifi_ret_esignal;
	}
}

int wifi_some_func_with_nice_error_handling(int param)
{
	int r = wifi_ret_ememory;
	int* piMem = NULL, *piMem2 = NULL;
	piMem = malloc(param);
	if (!piMem) {
		r = wifi_ret_ememory;
		goto exit_no_free;
	}

	if (wifi_get_some_data(12) < 100) {
		r = wifi_ret_esignal;
		goto exit_free_piMem;
	}

	piMem2 = malloc(param);

	if (!piMem2){
		r = wifi_ret_ememory;
		goto exit_free_piMem;
	}
	if (wifi_get_some_data(20) > 10) {
		r = wifi_ret_eabort;
		goto exit_free_piMem2;
	}

	// some more action

	r = wifi_ret_ok;

exit_free_piMem2:
	free(piMem2);
exit_free_piMem:
	free(piMem);
exit_no_free:
	return r;
}

