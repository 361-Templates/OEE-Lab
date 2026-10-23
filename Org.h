#ifndef ORG_H
#define ORG_H

#include "emp/math/Random.hpp"
#include "emp/tools/string_utils.hpp"

class Organism {
    private:
        double points;
        emp::Ptr<emp::Random> random;
        double cooperation_probability;

    public:
    Organism(emp::Ptr<emp::Random> _random, double coop = 0.0, double _points=0.0) :
         points(_points), random(_random), cooperation_probability(coop) {;}

    void SetPoints(double _in) {points = _in;}
    void AddPoints(double _in) {points += _in;}
    double GetCooperationProbability() {return cooperation_probability;}
    void SetCooperationProbability(double _in) {cooperation_probability = _in;}

    void Mutate() {
        cooperation_probability += random->GetNormal(0.0, 0.02);
        if (cooperation_probability > 1.0) {
            cooperation_probability = 1.0;
        } else if (cooperation_probability < 0.0) {
            cooperation_probability = 0.0;
        }
    }

    emp::Ptr<Organism> CheckReproduction() {
        if(points>1000) {
            emp::Ptr<Organism> offspring = new Organism(*this);
            offspring->SetPoints(0);
            offspring->Mutate();
            AddPoints(-1000);
            return offspring;
        }
        return nullptr;
    }

    void Process(int _points) {
        AddPoints(_points);
        std::cout << "Processing" << std::endl; // Feel free to remove this if you want.
       
    }


};
#endif