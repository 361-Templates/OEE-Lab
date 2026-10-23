#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/data/DataFile.hpp"
#include "emp/Evolve/OEE.hpp"

#include "Org.h"

class OrgWorld : public emp::World<Organism> {

    emp::Random &random;
    emp::Ptr<emp::Random> random_ptr;
    emp::Ptr<emp::DataMonitor<int>> data_node_orgcount;
    int MODES_RESOLUTION = 1;
    int FILTER_LENGTH = 1000;

    emp::DataFile oee_file;

  public:
    emp::Ptr<emp::OEETracker<emp::Systematics<Organism, Organism>, Organism, emp::vector<double>>> oee; //no default constructor?

    OrgWorld(emp::Random &_random) : emp::World<Organism>(_random), random(_random), oee_file("oee.csv") {
        random_ptr.New(_random);

        emp::Ptr<emp::Systematics<Organism, Organism> > sys;
        sys.New([](const Organism & o){return o;});
        oee.New(sys, [](Organism & org){return org;}, [](const Organism & org){
            return 1;
        }); 
        oee->SetResolution(MODES_RESOLUTION);
        oee->SetGenerationInterval(FILTER_LENGTH);
        AddSystematics(sys);
        OnUpdate([this](int ud){oee->Update(ud); oee_file.Update(ud);});


        oee_file.AddVar(update, "generation", "Generation");
        oee_file.AddCurrent(*oee->GetDataNode("change"), "change", "change potential");
        oee_file.AddCurrent(*oee->GetDataNode("novelty"), "novelty", "novelty potential");
        oee_file.AddCurrent(*oee->GetDataNode("diversity"), "ecology", "ecology potential");
        oee_file.AddCurrent(*oee->GetDataNode("complexity"), "complexity", "complexity potential");
        oee_file.PrintHeaderKeys();
        oee_file.SetTimingRepeat(MODES_RESOLUTION);
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
      std::cout << "Updating!" << std::endl;
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