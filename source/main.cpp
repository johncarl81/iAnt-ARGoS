//
// Created by john on 9/12/15.
//

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <QtCore/qtextstream.h>
#include "iAntBase/iAntBaseController.h"
#include "CPFA/CPFA_loop_functions.h"

/****************************************/
/****************************************/
using namespace std;
using namespace argos;

int main(int argc, char** argv) {

    string filename = "experiments/gant";

    if(argc > 1) {
        filename += argv[1];
    }

    filename += ".xml";

    ofstream outfile (filename.c_str());

    //read stdin to the iant file
    for (std::string line; std::getline(std::cin, line);) {
        outfile << line << std::endl;
    }

    static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName(filename.c_str());
    cSimulator.LoadExperiment();

    CPFA_loop_functions &cLoopFunctions = (CPFA_loop_functions&)(cSimulator.GetLoopFunctions());

    cSimulator.Reset();
    cSimulator.Execute();

    //todo:cout << "Fitness:" << cLoopFunctions.getFitness() << endl;
    cout << "Fitness:0" << endl;

    /*
     * Dispose of ARGoS stuff
     */
    cSimulator.Destroy();

    /* All is OK */
    return 0;
}

/****************************************/
/****************************************/
