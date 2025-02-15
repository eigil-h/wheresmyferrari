#include <proto/exec.h>
#include "utils.h"

int os_version(void)
{
	return SysBase->LibNode.lib_Version;
}
