#include <iostream>

#include "Org.h"
#include "emp/base/vector.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"

#include "World.h"

// This is the main function for the NATIVE version of this project.


int main(int argc, char* argv[])
{

  emp::Random random(5);
  OrgWorld world(random);
  world.SetupOrgFile("data.csv").SetTimingRepeat(1);




  world.Resize(100,100);
  world.SetPopStruct_Grid(100, 100);

  Organism* new_org = new Organism(&random, random.GetDouble(0,1.0));
  world.Inject(*new_org);




  for(int update=0; update<4000; update++){
    world.Update();
  }

}
