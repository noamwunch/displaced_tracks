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

// Convert 5D delphes track to 6D rave track
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

// Convert 6D rave to 5D delphes track
vector<double> TrackInvConvert(vector<rave::Track>::const_iterator track6) {
    // Read 6D track
    double x = track6->position().x();
    double y = track6->position().y();
    double z = track6->position().z();
    double px = track6->momentum().x();
    double py = track6->momentum().y();
    double pz = track6->momentum().z();
    double charge = double(track6->charge());
    // Compute 5D track
    double pt = sqrt(pow(px, 2) + pow(py, 2)) * abs(charge);
    double theta = atan2(pt, pz * charge);
    double eta = -log(tan(0.5 * theta));
    double phi = atan2(py * charge, px * charge);
    double d0 = sqrt(pow(x, 2) + pow(y, 2)) * abs(charge) * 10;
    double dz = z * 10;
    vector<double> track5 = {pt, eta, phi, d0, dz};
    return track5;
}

// Convert 5D delphes covariance to 6D rave covariance
rave::Covariance6D CovConvert(Track *track) {
    // Read 5D track
    double pt = track->PT;
    double phi = track->Phi;
    double eta = track->Eta;
    double px = pt * cos(phi);
    double py = pt * sin(phi);
    double pz = pt * sinh(eta);
    double ctth = track->CtgTheta;
    double d0 = track->D0 * 0.1; //epsilon
    double q = double(track->Charge);
    // Read 5D errors
    double deld0 = (track->ErrorD0) * 0.1; //in cm
    double delz0 = (track->ErrorDZ) * 0.1; //in cm
    double delpt = track->ErrorPT; //in GeV/c
    double delphi = track->ErrorPhi;
    double delctth = track->ErrorCtgTheta;
    double deltht = delctth / (1 + ctth * ctth);
    // Compute 5D covariance
    double covd0d0 = deld0 * deld0;
    double covz0z0 = delz0 * delz0;
    double covptpt = delpt * delpt;
    double covphiphi = delphi * delphi;
    double covthth = deltht * deltht;
    // Compute 5D covariance
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
    rave::Covariance6D cov6d(dxx, dxy, dxz, dyy, dyz, dzz, dxpx, dxpy, dxpz, dypx, dypy, dypz, dzpx, dzpy, dzpz, dpxpx,
                             dpxpy, dpxpz, dpypy, dpypz, dpzpz);
    return cov6d;
}

//Main code
void RaveGetVertsByJetv2(const char *inputFile, double dRjetsMax, int label, int max_ev, const char *result) {
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    //Prepare to write
    ofstream myfile;
    myfile.open(result);
    //Load Delphes and Rave libraries
    gSystem->Load("libDelphes");
    gSystem->Load("/usr/local/lib/libRaveBase");
    gSystem->Load("/usr/local/lib/libRaveCore");
    gSystem->Load("/usr/local/lib/libRaveVertex");
    gSystem->Load("/usr/local/lib/libRaveVertexKinematics");
    // Prepare to read root information
    TChain *chain = new TChain("Delphes");
    chain->Add(inputFile);
    ExRootTreeReader *treeReader = new ExRootTreeReader(chain);
    TClonesArray *branchParticle = treeReader->UseBranch("Particle");
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
    // Define vertex variables
    double xp;
    double yp;
    double zp;
    double chisq;
    int n_jet;
    int n_vert;
    // Create vertex factory
    float Bz = 2.0;   // Magnetic field
    rave::ConstantMagneticField mfield(0., 0., Bz);
    rave::VertexFactory factory(mfield);
    factory.setDefaultMethod("avr");
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
        if (label == 1) //If the event is a signal
        {
            // Initialize variables
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
            if (p1Ass && p2Ass) {
                //Write information of the partons
                myfile << "    Parton 1    pT: " << PTP1 << " eta: " << EtaP1 << " phi: " << PhiP1 << endl;
                myfile << "    Parton 2    pT: " << PTP2 << " eta: " << EtaP2 << " phi: " << PhiP2 << endl;
                //Find closest jets
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
                        EtaJ[0] = jet->Eta;
                        PhiJ[0] = jet->Phi;
                        PTJ[0] = jet->PT;
                        JetJ[0] = true;
                    }
                    if (deltaR2t < deltaR2) {
                        deltaR2 = deltaR2t;
                        EtaJ[1] = jet->Eta;
                        PhiJ[1] = jet->Phi;
                        PTJ[1] = jet->PT;
                        JetJ[1] = true;
                    }
                }
            }
        } else { // If the event is background
            myfile << "    Not a signal" << endl;
            //Get information from the two leading jets
            j = 0;
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
        // Vertexing
        vector <rave::Track> event_tracks;
        //Loop over tracks
        for (i = 0; i < branchTrack->GetEntriesFast(); ++i) {
            //Get track
            track = (Track *) branchTrack->At(i);
            // Convert to 6D rave track and push into tracks vector
            rave::Vector6D track6d = TrackConvert(track);
            rave::Covariance6D cov6d = CovConvert(track);
            event_tracks.push_back(rave::Track(track6d, cov6d, track->Charge, 0.0, 0.0));
        }
        n_vert = 1; // Vertex number (1=primary)
        vector <rave::Vertex> event_vertices = factory.create(event_tracks); // Reconstruct vertices
        // Loop over vertices
        for (vector<rave::Vertex>::const_iterator r = event_vertices.begin(); r != event_vertices.end(); ++r) {
            xp = (*r).position().x() * 10; //Converting to mm (RAVE produces output in cm)
            yp = (*r).position().y() * 10;
            zp = (*r).position().z() * 10;
            chisq = (*r).chiSquared();
            vector <rave::Track> tracks = (*r).tracks();
            // Loop over vertex constituents
            double vert_Px = 0;
            double vert_Py = 0;
            double vert_mult =0;
            for (vector<rave::Track>::const_iterator t = tracks.begin(); t != tracks.end(); ++t) {
                    vert_mult ++;
                    double track_px = t->momentum().x();
                    double track_py = t->momentum().y();
                    vert_Px += track_px;
                    vert_Py += track_py;
                  }
            double vert_PT = pow(pow(vert_Px, 2) + pow(vert_Py, 2), 0.5);
            double vert_D0 = pow(pow(xp, 2) + pow(yp, 2), 0.5);
            double vert_Phi = atan2(yp, xp);
            double vert_Eta = atan2(vert_D0, zp);
            //Check for distance from both jets
            deltaR1 = pow(pow(vert_Eta - EtaJ[0], 2) + pow(delta_phi_calculator(vert_Phi, PhiJ[0]), 2), 0.5);
            deltaR2 = pow(pow(vert_Eta - EtaJ[1], 2) + pow(delta_phi_calculator(vert_Phi, PhiJ[1]), 2), 0.5);
            if (deltaR1 < dRjetsMax)
                myfile << n_vert << " " << 1 << " " << vert_D0 << " " << vert_mult << " " << vert_PT << endl;
            if (deltaR2 < dRjetsMax)
                myfile << n_vert << " " << 2 << " " << vert_D0 << " " << vert_mult << " " << vert_PT << endl;
            n_vert = n_vert + 1;
        }
    }
    myfile << "Done" << endl;
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    std::cout << "Elapsed time = " << chrono::duration_cast<chrono::seconds>(end - begin).count() << " s" << endl;
    delete treeReader;
    delete chain;
}




