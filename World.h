#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/data/DataFile.hpp"

#include "Org.h"

class OrgWorld : public emp::World<Organism> {

    emp::Random &random;
    emp::Ptr<emp::Random> random_ptr;
    emp::Ptr<emp::DataMonitor<int>> data_node_orgcount;

  public:
    OrgWorld(emp::Random &_random) : emp::World<Organism>(_random), random(_random) {
        random_ptr.New(_random);
    }

    ~OrgWorld() {
      if(data_node_orgcount) data_node_orgcount.Delete();
    }

    emp::DataMonitor<int>& GetOrgCountDataNode() {
      if(!data_node_orgcount) {
          data_node_orgcount.New();
          OnUpdate([this](size_t){
          data_node_orgcount -> Reset();
          for (size_t i = 0; i < pop.size(); i++)
              if(IsOccupied(i))
              data_node_orgcount->AddDatum(1);
          });
      }
      return *data_node_orgcount;
      }

    emp::DataFile & SetupOrgFile(const std::string & filename) {
      auto & file = SetupFile(filename);
      auto & node1 = GetOrgCountDataNode();
      file.AddVar(update, "update", "Update");
      file.AddTotal(node1, "count", "Total number of organisms");

      file.PrintHeaderKeys();

      return file;
    }

    void Update() {
      emp::World<Organism>::Update();

      emp::vector<size_t> schedule = emp::GetPermutation(random, GetSize());
      for(int i : schedule) {
        if(!IsOccupied(i)) {continue;}
        pop[i]->Process(100);
      }

      schedule = emp::GetPermutation(random, GetSize());
      for(int i : schedule) {
        if(!IsOccupied(i)) {continue;}
        emp::Ptr<Organism> offspring = pop[i]->CheckReproduction();
        if(offspring) {
            DoBirth(*offspring, i);
        }
      }
      
    }

};
#endif