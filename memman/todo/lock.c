#include <stdio.h>
#include <stdlib.h>
#include <fnctl.h>

typedef enum
{
	LOCK_SUCCESS,
	LOCK_FAIL
} LOCK_STATUS;

void SetupLockMem(struct flock *fl, size_t offset, size_t len)
{
	fl->l_whence = SEEK_SET;
	fl->l_start = offset;
	fl->l_len = len;
}

LOCK_STATUS LockMemReadBlock(int fd, size_t offset, size_t len)
{
	/* https://gavv.github.io/blog/file-locks/ */
	struct flock fl = {0};

	fl.l_type = F_RDLCK;
	SetupLockMem(&fl, offset, len);

	/* todo : block */

	return (LOCK_FAIL);
}

LOCK_STATUS LockMemWriteBlock(int fd, size_t offset, size_t len)
{
	/* https://gavv.github.io/blog/file-locks/ */
	struct flock fl = {0};

	fl.l_type = F_WRLCK;
	SetupLockMem(&fl, offset, len);

	/* todo : block */

	return (LOCK_FAIL);
}