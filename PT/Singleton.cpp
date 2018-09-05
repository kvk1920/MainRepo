#include <bits/stdc++.h>

using namespace std;

/**
* It's horrible Singletone Implementation.
* The task was to make the most horrible singletone.
*/
class Singletone
{
public:
	friend Singletone &make_Singletone();

private:
	Singletone()
	{
		ofstream fout("__singletone__file__");
		fout.write(reinterpret_cast<char *>(&int(this)), sizeof (int));
		fout.close();
	}

	Singletone(const Singletone &) {}
	Singletone &operator=(const Singletone &) {}
};

inline Singletone &make_Singletone()
{
	ifstream fin("__singletone__file__");
	if (!fin.is_open())
	{
		ofstream fout("__singletone__file__");
		fout.write(reinterpret_cast <char *>(&int(new Singletone)), sizeof (int));
		fout.close();
		fin.open("__singletone__file__");
	}
	Singletone *ptr;
	int input;
	fin.read(reinterpret_cast <char *>(&input), sizeof (int));
	ptr = input;
	fin.close();
	return *ptr;
}
