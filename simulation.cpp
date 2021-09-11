#include "probA.h"
#include "probD.h"
#include <string.h>
#include <iostream>
using std::cerr;

int main(int argc, char* argv[])
{

    if (!strcmp( argv[1], "probA"))
    {
        probA();
    }
    else if (!strcmp( argv[1], "probD"))
    {
        probD();
    }
    else
    {
        std::cerr << "You didn't specify valid program!";
    }
    return 0;
}