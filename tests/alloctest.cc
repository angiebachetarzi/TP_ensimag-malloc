#include <gtest/gtest.h>

#include "../src/mem.h"

int
main(int argc, char **argv)
{
  ::testing::InitGoogleTest(& argc, argv);
  return RUN_ALL_TESTS();
}

