#include <berror.h>
#include <stdio.h>

int main(void) {
	BERROR_SET("some message");
	berror_info_t err = berror_get();
	printf("%s\n", err.file);
	printf("%s\n", err.func);
	printf("%s\n", err.msg);
	printf("%d\n", err.line);
	return 0;
}
