// blank main template for psyq c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include "tuto2.hpp"

// create an instance of the tutorial 2 class and run it
int main(void)
{
    tuto2 t;

    t.run();

    return 0;
}
