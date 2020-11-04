#include "headers.h"
#include "permissions.h"
void permissions(struct stat fileStat)
{

        // user
        if((fileStat.st_mode & S_IRUSR))
        {
		printf("r");
        }
        else
	{
		printf("-");
        }


        if((fileStat.st_mode & S_IWUSR))
        {
		printf("w");

        }
        else
        {
		printf("-");
        }

        if((fileStat.st_mode & S_IXUSR))
        {
		printf("x");
	}
        else
        {
		printf("-");
        }
        // group

        if((fileStat.st_mode & S_IRGRP))
        {
		printf("r");
        }
        else
        {
		printf("-");
        }

        if((fileStat.st_mode & S_IWGRP))
        {
		printf("w");
        }
        else
        {
		printf("-");
        }

        if((fileStat.st_mode & S_IXGRP))
        {
		printf("x");
        }
        else
        {
		printf("-");
        }

        // other

        if((fileStat.st_mode & S_IROTH))
        {
		printf("r");
        }
        else
        {
		printf("-");
        }

        if((fileStat.st_mode & S_IWOTH))
        {
		printf("w");

        }
        else
        {
		printf("-");
        }

        if((fileStat.st_mode & S_IXOTH))
        {
		printf("x");

        }
        else
        {
		printf("-");
	}              


}


