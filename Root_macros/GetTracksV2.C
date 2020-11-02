/*************************************************************/
/*                                                           */
/*                Information classifier                     */
/*                                                           */
/*************************************************************/

/* Explanation
This macro extracts the relevant information for machine learning and puts in a .txt file.

Comments:
- Based on Example3.c
*/


#ifdef __CLING__

R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"

#include <iostream>
#include <fstream>
#include <chrono>
#define PI 3.14159265359

#else

class ExRootTreeReader;

#endif

using namespace std;

// Calculate difference between two phi angles
double delta_phi_calculator(double phi1, double phi2) {
    return (abs(phi1 - phi2) <= PI) ? abs(phi1 - phi2) : (2 * PI - abs(phi1 - phi2));
}

//Main code
void GetTracksV2(const char *inputFile, double dRjetsMax, int label, int max_ev, const char *result) {
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    //Prepare to write
    ofstream myfile;
    myfile.open(result);
    //Load Delphes libraries
    gSystem->Load("libDelphes");
    // Prepare to read root information
    TChain *chain = new TChain("Delphes");
    chain->Add(inputFile);
    ExRootTreeReader *treeReader = new ExRootTreeReader(chain);
    //TClonesArray *branchParticle = treeReader->UseBranch("Particle");
    TClonesArray *branchJet = treeReader->UseBranch("Jet");
    TClonesArray *branchTrack = treeReader->UseBranch("Track");
    //TClonesArray *branchTower    = treeReader->UseBranch("Tower");
    TClonesArray *branchMissingET = treeReader->UseBranch("MissingET");
    //File info
    Long64_t allEntries = treeReader->GetEntries();
    myfile << "** Chain contains " << allEntries << " events" << endl;
    //Define variables
    GenParticle *particle;
    Track *track;
    Tower *tower;
    Jet *jet;
    TObject *object;
    MissingET *met;
    // Define parton variables
    double EtaP1, EtaP2;
    double PhiP1, PhiP2;
    double PTP1, PTP2;
    bool p1Ass, p2Ass;
    // Define closest jets temp variables
    double deltaEta1, deltaEta2;
    double deltaPhi1, deltaPhi2;
    double deltaR1t, deltaR2t;
    double deltaR1;
    double deltaR2;
    // Define closest jets variables
    int j;
    // Define track variables
    double EtaT;
    double PhiT;

    // Loop over all events (except first one)
    Long64_t entry;
    Int_t i, pdgCode;
    for (entry = 1; (entry < allEntries)&&(entry < max_ev); ++entry) {
        // Load Event
        treeReader->ReadEntry(entry);
        // Event info
        myfile << "--  Event " << entry << "  --" << endl;
        met = (MissingET *) branchMissingET->At(0);
        myfile << "    MET: " << met->MET << endl; // Event mission energy
        // Get leading jets
        double EtaJ[2] = {-1000, -1000};
        double PhiJ[2] = {0, 0};
        double PTJ[2] = {0, 0};
        bool JetJ[2] = {false, false};
        j = 0;
        deltaR1 = 10000;
        deltaR2 = 10000;
        while (j < 2 && j < branchJet->GetEntriesFast()) {
            //Get jet
            jet = (Jet *) branchJet->At(j);
            //Save information
            EtaJ[j] = jet->Eta;
            PhiJ[j] = jet->Phi;
            PTJ[j] = jet->PT;
            JetJ[j] = true;
            //Increment
            j++;
        }
        //Write information about leading jets
        if (JetJ[0])
            myfile << "    Jet 1    pT: " << PTJ[0] << " eta: " << EtaJ[0] << " phi: " << PhiJ[0]
                   << endl;
        if (JetJ[1])
            myfile << "    Jet 2    pT: " << PTJ[1] << " eta: " << EtaJ[1] << " phi: " << PhiJ[1]
                   << endl;
        if (JetJ[0])
            myfile << "entry Jet PT Eta Phi D0 DZ" << endl;
        //Loop over tracks
        for (i = 0; i < branchTrack->GetEntriesFast(); ++i) {
            //Get track
            track = (Track *) branchTrack->At(i);
            // Compute deltaR
            EtaT = track->Eta;
            PhiT = track->Phi;
            //Check for distance from both jets
            deltaR1 = pow(pow(EtaT - EtaJ[0], 2) + pow(delta_phi_calculator(PhiT, PhiJ[0]), 2), 0.5);
            deltaR2 = pow(pow(EtaT - EtaJ[1], 2) + pow(delta_phi_calculator(PhiT, PhiJ[1]), 2), 0.5);
            //Write information accordingly
            if (deltaR1 < dRjetsMax) {
                myfile << entry << " " << 1 << " " << track->PT << " " << track->Eta << " " << track->Phi << " " << track->D0 << " " << track->DZ << endl;
            }
            if (deltaR2 < dRjetsMax) {
                myfile << entry << " " << 2 << " " << track->PT << " " << track->Eta << " " << track->Phi << " " << track->D0 << " " << track->DZ << endl;
            }
        }
    }
    myfile << "Done" << endl;
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    std::cout << "Elapsed time = " << chrono::duration_cast<chrono::seconds>(end - begin).count() << " s" << endl;
    delete treeReader;
    delete chain;
}




