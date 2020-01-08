#include "gbm2gc.h"

int main(int argc, const char* argv[])
{
   try 
   {
      return gbm2gc::run(argc, argv);
   }
   catch (const std::exception& e)
   {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
   }
}
