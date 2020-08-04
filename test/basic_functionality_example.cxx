#include <logging/logging.hpp>

int main(int argc, char *argv[])
{
	std::vector<std::string> arguments(argv + 1, argv + argc);
	Logger().setup(arguments);
	LOG_DEBUG(0) << "hello - debug level 0";
	LOG_DEBUG(1) << "hello - debug level 1";
	return (0);
}   // main
