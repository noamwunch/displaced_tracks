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
#define PI 3.14159265359
#else
class ExRootTreeReader;
#endif

using namespace std;

//Calculate difference between two phi angles
double delta_phi_calculator(double phi1, double phi2)
{
  return (abs(phi1 - phi2) <= PI) ? abs(phi1 - phi2) : (2*PI - abs(phi1 - phi2));
}

//Main code
void Example3D(const char *inputFile, double dRjetsMax)
{
  //Prepare to write
  ofstream myfile;
  myfile.open ("Results.txt");

  //Load Delphes information
  gSystem->Load("libDelphes");

  TChain *chain = new TChain("Delphes");
  chain->Add(inputFile);

  ExRootTreeReader *treeReader = new ExRootTreeReader(chain);

  TClonesArray *branchParticle = treeReader->UseBranch("Particle");
  TClonesArray *branchJet      = treeReader->UseBranch("Jet");
  TClonesArray *branchTrack    = treeReader->UseBranch("Track");
  TClonesArray *branchTower    = treeReader->UseBranch("Tower");

  //Starting information
  Long64_t allEntries = treeReader->GetEntries();

  myfile << "** Chain contains " << allEntries << " events" << endl;

  //Define variables
  GenParticle *particle;
  Track       *track;
  Tower       *tower;
  Jet         *jet;
  TObject     *object;

  double EtaP1, EtaP2;
  double PhiP1, PhiP2;
  double PTP1, PTP2;
  double EtaJ1, EtaJ2;
  double PhiJ1, PhiJ2;
  double PTJ1, PTJ2;
  bool   p1Ass, p2Ass;
  double deltaEta1, deltaEta2;
  double deltaPhi1, deltaPhi2;
  double deltaR1, deltaR2;
  double deltaR1t, deltaR2t;

  Long64_t entry;

  Int_t i, pdgCode;

  // Loop over all events (except first one)
  for(entry = 1; entry < allEntries; ++entry)
  {
    //Load selected branches with data from specified event
    treeReader->ReadEntry(entry);

    myfile << "--  Event " << entry << "  --"<< endl;

    //Initialize variables
    p1Ass = false;
    p2Ass = false;

    // Loop over particles to find initial partons
    for(i = 0; i < branchParticle->GetEntriesFast(); ++i)
    {
      //Read information particle
      object = (TObject*) branchParticle->At(i);

      //Check information
      particle = (GenParticle*) object;

      if(particle->PID == 4900101 && !p1Ass)
      {
        EtaP1 = particle->Eta;
        PhiP1 = particle->Phi;
        PTP1  = particle->PT;
        p1Ass = true;
      }

      if(particle->PID == -4900101 && !p2Ass)
      {
        EtaP2 = particle->Eta;
        PhiP2 = particle->Phi;
        PTP2  = particle->PT;
        p2Ass = true;
      }
    }

    //If the event is a signal
    if(p1Ass && p2Ass)
    {
      //Write information of the partons
      myfile << "    Parton 1    pT: " << PTP1 << " eta: " << EtaP1 << " phi: " << PhiP1 << endl;
      myfile << "    Parton 2    pT: " << PTP2 << " eta: " << EtaP2 << " phi: " << PhiP2 << endl;

      //Find closest jets
      deltaR1 = 10000;
      deltaR2 = 10000;

      for(i = 0; i < branchJet->GetEntriesFast(); ++i)
      {
        //Get jet
        jet = (Jet*) branchJet->At(i);

        //Look for closest jet to parton
        deltaEta1 = abs(jet->Eta - EtaP1);
        deltaEta2 = abs(jet->Eta - EtaP2);      
        deltaPhi1 = delta_phi_calculator(jet->Phi, PhiP1);
        deltaPhi2 = delta_phi_calculator(jet->Phi, PhiP2);

        deltaR1t = pow(pow(deltaEta1, 2) + pow(deltaPhi1, 2), 0.5);
        deltaR2t = pow(pow(deltaEta2, 2) + pow(deltaPhi2, 2), 0.5);

        if(deltaR1t < deltaR1)
        {
          deltaR1 = deltaR1t;
          EtaJ1   = jet->Eta;
          PhiJ1   = jet->Phi;
          PTJ1    = jet->PT;
        }

        if(deltaR2t < deltaR2)
        {
          deltaR2 = deltaR2t;
          EtaJ2   = jet->Eta;
          PhiJ2   = jet->Phi;
          PTJ2    = jet->PT;
        }

      }

      //Write information closest jet
      myfile << "    Jet 1       pT: " << PTJ1 << " eta: " << EtaJ1 << " phi: " << PhiJ1 << endl;
      myfile << "    Jet 2       pT: " << PTJ2 << " eta: " << EtaJ2 << " phi: " << PhiJ2 << endl;
      myfile << "    Jet  T/T  PT      Eta      Phi      DeltaR   PID    D0    DZ" << endl;

      //Loop over tracks
      for(i = 0; i < branchTrack->GetEntriesFast(); ++i)
      {
        //Get track
        track = (Track*) branchTrack->At(i);

        //Read position track
        double EtaT = track->EtaOuter;
        double PhiT = track->PhiOuter;

        //Check for distance from both jets
        double deltaR1 = pow(pow(EtaT - EtaJ1, 2) + pow(delta_phi_calculator(PhiT, PhiJ1), 2), 0.5);
        double deltaR2 = pow(pow(EtaT - EtaJ2, 2) + pow(delta_phi_calculator(PhiT, PhiJ2), 2), 0.5);

        //Write information accordingly
        if(deltaR1 < dRjetsMax)
          myfile << "    1    1    " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter << " " << deltaR1 << " " << track->PID << " " << track->D0 << " " << track->DZ << endl;

        if(deltaR2 < dRjetsMax)
          myfile << "    2    1    " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter << " " << deltaR2 << " " << track->PID << " " << track->D0 << " " << track->DZ << endl;
      }

      //Loop over tower
      for(i = 0; i < branchTower->GetEntriesFast(); ++i)
      {
        //Get track
        tower = (Tower*) branchTower->At(i);

        //Read position track
        double EtaT = tower->Eta;
        double PhiT = tower->Phi;

        //Check for distance from both jets
        double deltaR1 = pow(pow(EtaT - EtaJ1, 2) + pow(delta_phi_calculator(PhiT, PhiJ1), 2), 0.5);
        double deltaR2 = pow(pow(EtaT - EtaJ2, 2) + pow(delta_phi_calculator(PhiT, PhiJ2), 2), 0.5);

        //Write information accordingly
        if(deltaR1 < dRjetsMax)
          myfile << "    1    2    " << tower->ET << " " << tower->Eta << " " << tower->Phi << " " << deltaR1 << " " << 0 << endl;

        if(deltaR2 < dRjetsMax)
          myfile << "    2    2    " << tower->ET << " " << tower->Eta << " " << tower->Phi << " " << deltaR2 << " " << 0 << endl;
      }
    }

    //If the event is not a signal
    else
    {
      //Print information event
      myfile << "    Not a signal" << endl;

      //Initialize information jets
      double EtaBJ [2] = {-1000, -1000};
      double PhiBJ [2] = {0, 0};
      double PTBJ  [2] = {0, 0};
      bool   JetBJ [2] = {false, false};
      int    j = 0;

      //Get information from the two leading jets
      while(j < 2 && j < branchJet->GetEntriesFast())
      {
        //Get jet
        jet = (Jet*) branchJet->At(j);

        //Save information
        EtaBJ[j] = jet->Eta;
        PhiBJ[j] = jet->Phi;
        PTBJ[j]  = jet->PT;
        JetBJ[j] = true;

        //Increment
        j++;
      }

      //Write information about leading jets
      if(JetBJ[0])
        myfile << "    Jet 1    pT: " << PTBJ[0] << " eta: " << EtaBJ[0] << " phi: " << PhiBJ[0] << endl;

      if(JetBJ[1])
        myfile << "    Jet 2    pT: " << PTBJ[1] << " eta: " << EtaBJ[1] << " phi: " << PhiBJ[1] << endl;

      if(JetBJ[0])
        myfile << "    Jet  T/T  PT      Eta      Phi      DeltaR   PID    D0    DZ" << endl;

      //Loop over tracks
      for(i = 0; i < branchTrack->GetEntriesFast(); ++i)
      {
        //Get track
        track = (Track*) branchTrack->At(i);

        //Read position track
        double EtaT = track->EtaOuter;
        double PhiT = track->PhiOuter;

        //Check for distance from both jets
        double deltaR1 = pow(pow(EtaT - EtaBJ[0], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[0]), 2), 0.5);
        double deltaR2 = pow(pow(EtaT - EtaBJ[1], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[1]), 2), 0.5);

        //Write information accordingly
        if(deltaR1 < dRjetsMax)
          myfile << "    1    1    " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter << " " << deltaR1 << " " << track->PID << " " << track->D0 << " " << track->DZ << endl;

        if(deltaR2 < dRjetsMax)
          myfile << "    2    1    " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter << " " << deltaR2 << " " << track->PID << " " << track->D0 << " " << track->DZ << endl;
      }

      //Loop over tower
      for(i = 0; i < branchTower->GetEntriesFast(); ++i)
      {
        //Get track
        tower = (Tower*) branchTower->At(i);

        //Read position track
        double EtaT = tower->Eta;
        double PhiT = tower->Phi;

        //Check for distance from both jets
        double deltaR1 = pow(pow(EtaT - EtaBJ[0], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[0]), 2), 0.5);
        double deltaR2 = pow(pow(EtaT - EtaBJ[1], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[1]), 2), 0.5);

        //Write information accordingly
        if(deltaR1 < dRjetsMax)
          myfile << "    1    2    " << tower->ET << " " << tower->Eta << " " << tower->Phi << " " << deltaR1 << " " << 0 << endl;

        if(deltaR2 < dRjetsMax)
          myfile << "    2    2    " << tower->ET << " " << tower->Eta << " " << tower->Phi << " " << deltaR2 << " " << 0 << endl;
      }
    }
  }

  myfile << "Done" << endl;

  delete treeReader;
  delete chain;
}




