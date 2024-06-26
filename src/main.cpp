#include <iostream>
#include "Application.h"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	lve::Application app{};

	try
	{
		app.Run();
	} catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}