#include<iostream>
using namespace std;
int main()
{
	long long int i =0;
	int cnt =0;
	while(true)
	{
		if(i%(1000000)==0)
		{
			printf("");cnt++;
		}
		i++;
		if(cnt>6000)
		{
			return 0;
		}
	}
}
