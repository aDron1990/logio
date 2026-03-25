#include "game.hpp"

#include <exception>
#include <print>

int main(int argc, char* argv[])
{
    try
    {
        auto game = Game{};
        game.run();
    }
    catch (std::exception& ex)
    {
        std::println("{}", ex.what());
        return 1;
    }
    return 0;
}