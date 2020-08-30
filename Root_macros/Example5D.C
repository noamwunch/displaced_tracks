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

#if defined(_MSC_VER)
# define RaveDllExport __declspec(dllexport)
#else
# define RaveDllExport
#endif


//#ifdef __CLING__
//R__LOAD_LIBRARY(libDelphes)
#ifdef __CLING__
R__LOAD_LIBRARY(/usr/local/lib/libRaveBase.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveCore.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveVertex.so)

#include "/usr/local/include/rave/Version.h"
#include "/usr/local/include/rave/VertexFactory.h"
#include "/usr/local/include/rave/Vertex.h"
#include "/usr/local/include/rave/Track.h"
#include "/usr/local/include/rave/Covariance6D.h"
#include "/usr/local/include/rave/Vector6D.h"
#include "/usr/local/include/rave/ConstantMagneticField.h"

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
double delta_phi_calculator(double phi1, double phi2) {
    return (abs(phi1 - phi2) <= PI) ? abs(phi1 - phi2) : (2 * PI - abs(phi1 - phi2));
}

rave::Vector6D TrackConvert(Track *track) {

    double eps = track->D0 * 0.1;  // also d0   //Delphes is in mm and rave is in cm
    double z0 = track->DZ * 0.1;  // z0
    double pt = track->PT;
    double phi = track->Phi;              // phi_0 (for us)
    double ctth = track->CtgTheta;
    double q = track->Charge;
    double x = eps * sin(phi);
    double y = -eps * cos(phi);
    double z = z0;
    double px = pt / q * cos(phi);
    double py = pt / q * sin(phi);
    double pz = pt / q * ctth;


    rave::Vector6D track6d(x, y, z, px, py, pz);
    return track6d;
}

vector<double> TrackInvConvert(vector<rave::Track>::const_iterator track6) {
    double x = track6->position().x();
    double y = track6->position().y();
    double z = track6->position().z();
    double px = track6->momentum().x();
    double py = track6->momentum().y();
    double pz = track6->momentum().z();
    double charge = double(track6->charge());

    double pt = sqrt(pow(px, 2) + pow(py, 2)) * abs(charge);
    double theta = atan2(pt, pz * charge);
    double eta = -log(tan(0.5 * theta));
    double phi = atan2(py * charge, px * charge);
    double d0 = sqrt(pow(x, 2) + pow(y, 2)) * abs(charge) * 10;
    double dz = z * 10;

    vector<double> track5 = {pt, eta, phi, d0, dz};
    return track5;
}

rave::Covariance6D CovConvert(Track *track) {
    double pt = track->PT;
    double phi = track->Phi;
    double eta = track->Eta;
    double px = pt * cos(phi);
    double py = pt * sin(phi);
    double pz = pt * sinh(eta);
    double ctth = track->CtgTheta;
    double d0 = track->D0 * 0.1; //epsilon
    double q = double(track->Charge);

    double deld0 = (track->ErrorD0) * 0.1; //in cm
    double delz0 = (track->ErrorDZ) * 0.1; //in cm
    double delpt = track->ErrorPT; //in GeV/c
    double delphi = track->ErrorPhi;
    double delctth = track->ErrorCtgTheta;
    double deltht = delctth / (1 + ctth * ctth);

    double covd0d0 = deld0 * deld0;
    double covz0z0 = delz0 * delz0;
    double covptpt = delpt * delpt;
    double covphiphi = delphi * delphi;
    double covthth = deltht * deltht;

    double dpxpx = py * py * covphiphi + covptpt * cos(phi) * cos(phi) / (q * q);
    double dpxpy = -px * py * covphiphi + covptpt * cos(phi) * sin(phi) / (q * q);
    double dpxpz = covptpt * cos(phi) * ctth / (q * q);// + 0.00000001;
    double dxpx = -d0 * py * covphiphi * cos(phi);
    double dypx = -d0 * py * covphiphi * sin(phi);
    double dpypy = px * px * covphiphi + covptpt * sin(phi) * sin(phi) / (q * q);
    double dpypz = covptpt * sin(phi) * ctth / (q * q);// + 0.0000001;
    double dxpy = d0 * px * covphiphi * cos(phi);
    double dypy = d0 * px * covphiphi * sin(phi);
    double dypz = 0;
    double dxpz = 0;
    double dzpx = 0;
    double dzpy = 0;
    double dzpz = 0;
    double dpzpz = covptpt * ctth * ctth + pt * pt * covthth * pow((1 + ctth * ctth), 2) / (q * q);
    double dxx = d0 * d0 * cos(phi) * cos(phi) * covphiphi + covd0d0 * sin(phi) * sin(phi);
    double dxy = (-covd0d0 + d0 * d0 * covphiphi) * cos(phi) * sin(phi);
    double dxz = 0;
    double dyy = covd0d0 * cos(phi) * cos(phi) + d0 * d0 * sin(phi) * sin(phi) * covphiphi;
    double dyz = 0;
    double dzz = covz0z0;


    rave::Covariance6D cov6d(dxx, dxy, dxz,
                             dyy, dyz, dzz,
                             dxpx, dxpy, dxpz,
                             dypx, dypy, dypz,
                             dzpx, dzpy, dzpz,
                             dpxpx, dpxpy, dpxpz,
                             dpypy, dpypz, dpzpz);

    return cov6d;
}


//Main code
void Example5D() {
    double dRjetsMax = 0.7;
    //Prepare to write
    ofstream myfile;
    myfile.open("Results2.txt");



    //Load Delphes information
    gSystem->Load("libDelphes");
    gSystem->Load("/usr/local/lib/libRaveBase");
    gSystem->Load("/usr/local/lib/libRaveCore");
    gSystem->Load("/usr/local/lib/libRaveVertex");
    gSystem->Load("/usr/local/lib/libRaveVertexKinematics");

    TChain *chain = new TChain("Delphes");
    chain->Add("results.root");

    ExRootTreeReader *treeReader = new ExRootTreeReader(chain);

    TClonesArray *branchParticle = treeReader->UseBranch("Particle");
    TClonesArray *branchJet = treeReader->UseBranch("Jet");
    TClonesArray *branchTrack = treeReader->UseBranch("Track");
    TClonesArray *branchTower = treeReader->UseBranch("Tower");
    TClonesArray *branchMissingET = treeReader->UseBranch("MissingET");


    //Starting information
    Long64_t allEntries = treeReader->GetEntries();

    myfile << "** Chain contains " << allEntries << " events" << endl;

    //Define variables
    GenParticle *particle;
    Track *track;
    Tower *tower;
    Jet *jet;
    TObject *object;
    MissingET *met;

    double EtaP1, EtaP2;
    double PhiP1, PhiP2;
    double PTP1, PTP2;
    double EtaJ1, EtaJ2;
    double PhiJ1, PhiJ2;
    double PTJ1, PTJ2;
    bool p1Ass, p2Ass;
    double deltaEta1, deltaEta2;
    double deltaPhi1, deltaPhi2;
    double deltaR1, deltaR2;
    double deltaR1t, deltaR2t;

    Long64_t entry;

    Int_t i, pdgCode;

    // Loop over all events (except first one)
    for (entry = 1; entry < allEntries; ++entry) {
        //Load selected branches with data from specified event
        treeReader->ReadEntry(entry);

        myfile << "--  Event " << entry << "  --" << endl;

        //Initialize variables
        p1Ass = false;
        p2Ass = false;

        // Loop over particles to find initial partons
        for (i = 0; i < branchParticle->GetEntriesFast(); ++i) {
            //Read information particle
            object = (TObject *) branchParticle->At(i);

            //Check information
            particle = (GenParticle *) object;

            if (particle->PID == 4900101 && !p1Ass) {
                EtaP1 = particle->Eta;
                PhiP1 = particle->Phi;
                PTP1 = particle->PT;
                p1Ass = true;
            }

            if (particle->PID == -4900101 && !p2Ass) {
                EtaP2 = particle->Eta;
                PhiP2 = particle->Phi;
                PTP2 = particle->PT;
                p2Ass = true;
            }
        }

        //MET
        met = (MissingET *) branchMissingET->At(0);
        myfile << "    MET: " << met->MET << endl;


        //If the event is a signal
        if (p1Ass && p2Ass) {
            //Write information of the partons
            myfile << "    Parton 1    pT: " << PTP1 << " eta: " << EtaP1 << " phi: " << PhiP1 << endl;
            myfile << "    Parton 2    pT: " << PTP2 << " eta: " << EtaP2 << " phi: " << PhiP2 << endl;

            //Find closest jets
            deltaR1 = 10000;
            deltaR2 = 10000;

            for (i = 0; i < branchJet->GetEntriesFast(); ++i) {
                //Get jet
                jet = (Jet *) branchJet->At(i);

                //Look for closest jet to parton
                deltaEta1 = abs(jet->Eta - EtaP1);
                deltaEta2 = abs(jet->Eta - EtaP2);
                deltaPhi1 = delta_phi_calculator(jet->Phi, PhiP1);
                deltaPhi2 = delta_phi_calculator(jet->Phi, PhiP2);

                deltaR1t = pow(pow(deltaEta1, 2) + pow(deltaPhi1, 2), 0.5);
                deltaR2t = pow(pow(deltaEta2, 2) + pow(deltaPhi2, 2), 0.5);

                if (deltaR1t < deltaR1) {
                    deltaR1 = deltaR1t;
                    EtaJ1 = jet->Eta;
                    PhiJ1 = jet->Phi;
                    PTJ1 = jet->PT;
                }

                if (deltaR2t < deltaR2) {
                    deltaR2 = deltaR2t;
                    EtaJ2 = jet->Eta;
                    PhiJ2 = jet->Phi;
                    PTJ2 = jet->PT;
                }

            }

            //Write information closest jet
            myfile << "    Jet 1       pT: " << PTJ1 << " eta: " << EtaJ1 << " phi: " << PhiJ1 << endl;
            myfile << "    Jet 2       pT: " << PTJ2 << " eta: " << EtaJ2 << " phi: " << PhiJ2 << endl;
            myfile << "entry Jet T/T PT Eta Phi DeltaR PID D0/Ehad DZ/Eem errD0 errDZ" << endl;

            //Loop over tracks
            for (i = 0; i < branchTrack->GetEntriesFast(); ++i) {
                //Get track
                track = (Track *) branchTrack->At(i);

                //Read position track
                double EtaT = track->Eta;
                double PhiT = track->Phi;

                //Check for distance from both jets
                double deltaR1 = pow(pow(EtaT - EtaJ1, 2) + pow(delta_phi_calculator(PhiT, PhiJ1), 2), 0.5);
                double deltaR2 = pow(pow(EtaT - EtaJ2, 2) + pow(delta_phi_calculator(PhiT, PhiJ2), 2), 0.5);

                //Write information accordingly
                if (deltaR1 < dRjetsMax)
                    myfile << entry << " " << "1 1 " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter
                           << " " << deltaR1 << " " << track->PID << " " << track->D0 << " " << track->DZ << " "
                           << track->ErrorD0 << " " << track->ErrorDZ << " " << track->Xd << " " << track->Yd << endl;

                if (deltaR2 < dRjetsMax)
                    myfile << entry << " " << "2 1 " << track->PT << " " << track->EtaOuter << " " << track->PhiOuter
                           << " " << deltaR2 << " " << track->PID << " " << track->D0 << " " << track->DZ << " "
                           << track->ErrorD0 << " " << track->ErrorDZ << " " << track->Xd << " " << track->Yd << endl;
            }

            //Loop over tower
            for (i = 0; i < branchTower->GetEntriesFast(); ++i) {
                //Get track
                tower = (Tower *) branchTower->At(i);

                //Read position track
                double EtaT = tower->Eta;
                double PhiT = tower->Phi;

                //Check for distance from both jets
                double deltaR1 = pow(pow(EtaT - EtaJ1, 2) + pow(delta_phi_calculator(PhiT, PhiJ1), 2), 0.5);
                double deltaR2 = pow(pow(EtaT - EtaJ2, 2) + pow(delta_phi_calculator(PhiT, PhiJ2), 2), 0.5);

                //Write information accordingly
                if (deltaR1 < dRjetsMax)
                    myfile << entry << " " << "1 2 " << tower->ET << " " << tower->Eta << " " << tower->Phi << " "
                           << deltaR1 << " " << 0 << " " << tower->Ehad << " " << tower->Eem << endl;

                if (deltaR2 < dRjetsMax)
                    myfile << entry << " " << "2 2 " << tower->ET << " " << tower->Eta << " " << tower->Phi << " "
                           << deltaR2 << " " << 0 << " " << tower->Ehad << " " << tower->Eem << endl;
            }
        }

            //If the event is not a signal
        else {
            //Print information event
            myfile << "    Not a signal" << endl;

            //Initialize information jets
            double EtaBJ[2] = {-1000, -1000};
            double PhiBJ[2] = {0, 0};
            double PTBJ[2] = {0, 0};
            bool JetBJ[2] = {false, false};
            int j = 0;

            //Get information from the two leading jets
            while (j < 2 && j < branchJet->GetEntriesFast()) {
                //Get jet
                jet = (Jet *) branchJet->At(j);

                //Save information
                EtaBJ[j] = jet->Eta;
                PhiBJ[j] = jet->Phi;
                PTBJ[j] = jet->PT;
                JetBJ[j] = true;

                //Increment
                j++;
            }

            //Write information about leading jets
            if (JetBJ[0])
                myfile << "    Jet 1    pT: " << PTBJ[0] << " eta: " << EtaBJ[0] << " phi: " << PhiBJ[0] << endl;

            if (JetBJ[1])
                myfile << "    Jet 2    pT: " << PTBJ[1] << " eta: " << EtaBJ[1] << " phi: " << PhiBJ[1] << endl;

            if (JetBJ[0])
                myfile << "entry Jet T/T x_pos y_pos z_pos x_mom y_mom z_mom x_ver y_ver z_ver " << endl;

            vector <rave::Track> jet1_tracks;
            vector <rave::Track> jet2_tracks;

            //Loop over tracks
            for (i = 0; i < branchTrack->GetEntriesFast(); ++i) {
                //Get track
                track = (Track *) branchTrack->At(i);

                //Read position track
                double EtaT = track->Eta;
                double PhiT = track->Phi;

                //Check for distance from both jets
                double deltaR1 = pow(pow(EtaT - EtaBJ[0], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[0]), 2), 0.5);
                double deltaR2 = pow(pow(EtaT - EtaBJ[1], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[1]), 2), 0.5);

                //Write information accordingly
                if (deltaR1 < dRjetsMax) {
                    rave::Vector6D track6d = TrackConvert(track);
                    rave::Covariance6D cov6d = CovConvert(track);
                    jet1_tracks.push_back(rave::Track(track6d, cov6d, track->Charge, 0.0, 0.0));
                    if (entry == 24)
                        cout << "wha?";

                }

                if (deltaR2 < dRjetsMax) {
                    rave::Vector6D track6d = TrackConvert(track);
                    rave::Covariance6D cov6d = CovConvert(track);
                    jet2_tracks.push_back(rave::Track(track6d, cov6d, track->Charge, 0.0, 0.0));

                }
            }

            float Bz = 2.0;   // Magnetic field
            rave::ConstantMagneticField mfield(0., 0., Bz);
            rave::VertexFactory factory(mfield);
            factory.setDefaultMethod("avr");
            //cout << factory.method();
            double xp = 0;
            double yp = 0;
            double zp = 0;
            double chisq = 0;
            vector <rave::Vertex> jet1_vertices = factory.create(jet1_tracks);
            if (entry) {
                // cout << " " << jet1_vertices.size();
                //cout << " " << jet1_tracks.size();
                // for(int k=0; k < jet1_vertices.size(); k++)
                // cout << " " << jet1_vertices.at(k).position().x() << ' ';
            }
            for (vector<rave::Vertex>::const_iterator r = jet1_vertices.begin(); r != jet1_vertices.end(); ++r) {
                //RAVE produces output in cm
                xp = (*r).position().x() * 10; //converting to mm
                yp = (*r).position().y() * 10;
                zp = (*r).position().z() * 10;
                chisq = (*r).chiSquared();
                vector <rave::Track> tracks = (*r).tracks();
                for (vector<rave::Track>::const_iterator t = tracks.begin(); t != tracks.end(); ++t) {
                    vector<double> track5 = TrackInvConvert(t);
                    cout << track5[1];
                    myfile << entry << " " << "1 1 " << track5[0] << " " << track5[1] << " " << track5[2] << " " <<
                           0 << " " << 0 << " " << track5[3] << " " << track5[4] << " " << 0 << " " << 0 << " " << 0
                           << " " << 0 << " " << 0 << endl;
                }
            }

            xp = 0;
            yp = 0;
            zp = 0;
            chisq = 0;
            vector <rave::Vertex> jet2_vertices = factory.create(jet2_tracks);
            for (vector<rave::Vertex>::const_iterator r = jet2_vertices.begin(); r != jet2_vertices.end(); ++r) {
                //RAVE produces output in cm
                xp = (*r).position().x() * 10; //converting to mm
                yp = (*r).position().y() * 10;
                zp = (*r).position().z() * 10;
                chisq = (*r).chiSquared();
                vector <rave::Track> tracks = (*r).tracks();
                for (vector<rave::Track>::const_iterator t = tracks.begin(); t != tracks.end(); ++t) {
                    myfile << entry << " " << "2 1 " << (*t).position().x() << " " << (*t).position().y() << " "
                           << (*t).position().z() << " " <<
                           (*t).momentum().x() << " " << (*t).momentum().y() << " " << (*t).momentum().z() << " " <<
                           xp << " " << yp << " " << zp << " " << (*t).charge() << endl;
                }
            }

            //Loop over tower
            for (i = 0; i < branchTower->GetEntriesFast(); ++i) {
                //Get track
                tower = (Tower *) branchTower->At(i);

                //Read position track
                double EtaT = tower->Eta;
                double PhiT = tower->Phi;

                //Check for distance from both jets
                double deltaR1 = pow(pow(EtaT - EtaBJ[0], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[0]), 2), 0.5);
                double deltaR2 = pow(pow(EtaT - EtaBJ[1], 2) + pow(delta_phi_calculator(PhiT, PhiBJ[1]), 2), 0.5);

                //Write information accordingly
                if (deltaR1 < dRjetsMax)
                    myfile << entry << " " << "1 2 " << tower->ET << " " << tower->Eta << " " << tower->Phi << " "
                           << deltaR1 << " " << 0 << " " << tower->Ehad << " " << tower->Eem << endl;

                if (deltaR2 < dRjetsMax)
                    myfile << entry << " " << "2 2 " << tower->ET << " " << tower->Eta << " " << tower->Phi << " "
                           << deltaR1 << " " << 0 << " " << tower->Ehad << " " << tower->Eem << endl;
            }
        }
    }

    myfile << "Done" << endl;

    delete treeReader;
    delete chain;

}




