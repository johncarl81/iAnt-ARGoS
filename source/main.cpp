//
// Created by john on 9/12/15.
//

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <source/ga/BasicGA.h>
#include <source/ga/ChromosomeFactory.h>
#include "iAnt_controller.h"
#include "iAnt_loop_functions.h"
#include "iAnt_pheromone.h"
#include <limits>

/****************************************/
/****************************************/

int main(int argc, char** argv) {
    int POPULATION = 10;
    int NUM_GENERATIONS = 100;

    static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName("experiments/iAnt.xml");
    cSimulator.LoadExperiment();

    iAnt_loop_functions &cLoopFunctions = (iAnt_loop_functions&)(cSimulator.GetLoopFunctions());
    BasicGA ga;
    ChromosomeFactory chromosomeFactory;

    vector<Chromosome*> chromosomes = chromosomeFactory.buildPopulation(POPULATION);

    //initial population always scores 0
    vector<BasicGA::FitnessChromosome> preFitness;
    for (int c = 0; c < chromosomes.size(); c++) {
        BasicGA::FitnessChromosome fitChromosome;
        fitChromosome.chromosome = chromosomes.at(c);
        fitChromosome.fitness = 0;
        preFitness.push_back(fitChromosome);
    }

    chromosomes = ga.evolve(preFitness);



    for(int i = 0; i < NUM_GENERATIONS; i++) {
        argos::LOG << "Generation #";
        argos::LOG << i;
        argos::LOG << " started.\n";

            // This file is created in the directory where you run ARGoS
            // it is always created or appended to, never overwritten, i.e. ios::app
            ofstream dataOutput("iAntTagData.txt", ios::app);

            dataOutput << "Generation #";
            dataOutput << i;
            dataOutput << " started.\n";
            dataOutput.close();

        vector<BasicGA::FitnessChromosome> fitness;
        
        int genMaxFitness = 0;
        int genMinFitness = std::numeric_limits<int>::max();
        int fitsum = 0;

        for (int c = 0; c < chromosomes.size(); c++) {
            cLoopFunctions.setChromosome(chromosomes.at(c));
	        cSimulator.Reset();
            cSimulator.Execute();


            BasicGA::FitnessChromosome fitChromosome;
            fitChromosome.chromosome = chromosomes.at(c);
            fitChromosome.fitness = cLoopFunctions.getFitness();

            fitsum += fitChromosome.fitness;
            fitness.push_back(fitChromosome);

            if (fitChromosome.fitness < genMinFitness) genMinFitness = fitChromosome.fitness;
            if (fitChromosome.fitness > genMaxFitness) genMaxFitness = fitChromosome.fitness;
        }

        float genAvg = (float)fitsum/(float)POPULATION;

        argos::LOG << "Generation #";
        argos::LOG << i;
        argos::LOG << ", total: ";
        argos::LOG << fitsum;
        argos::LOG << ", Min: "<< genMinFitness;
        argos::LOG << ", Max: "<< genMaxFitness;
        argos::LOG << ", AVG: "<< genAvg;
        argos::LOG <<", done.\n";

        // This file is created in the directory where you run ARGoS
        // it is always created or appended to, never overwritten, i.e. ios::app
        ofstream dataOutput2("iAntTagData.txt", ios::app);


        dataOutput2 << "Generation #";
        dataOutput2 << i;
        dataOutput2 << ", total: ";
        dataOutput2 << fitsum;
        dataOutput2 << ", Min: "<< genMinFitness;
        dataOutput2 << ", Max: "<< genMaxFitness;
        dataOutput2 << ", AVG: "<< genAvg;
        dataOutput2 <<", done.\n";

        chromosomes = ga.evolve(fitness);
    }

    /*
     * Dispose of ARGoS stuff
     */
    cSimulator.Destroy();

    /* All is OK */
    return 0;
}

/****************************************/
/****************************************/
