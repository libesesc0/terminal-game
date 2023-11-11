#include "utilites.h"
#include <cstdlib> // Pour utiliser system() et rand()
#include <vector>

int main()
{
	system("stty -icanon -echo");

	printf("\e[2J");
	printf("\e[?25l");

	Engine engine;
	engine.setup();

	engine.run();

	printf("\e[?25h");
	printf("\e[%d;1H",yScreenSize);
	printf("\e[37m");

	system("stty icanon echo");

	return 0;
}
