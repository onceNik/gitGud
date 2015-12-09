#include <iostream>
#include <cstdlib>


using namespace std;

int main(void) {
	
	int x = 21;
	
	x = x & ((1 << 32)-1);
	
	cout << x << endl;
	
	return 0;
	
}
