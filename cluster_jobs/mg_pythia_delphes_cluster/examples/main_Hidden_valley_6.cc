/**********************************/
/*                                */
/*      Hidden valley PYTHIA      */
/*                                */
/**********************************/

/*Comments
- Reading LHE is based on main11.
- I have removed unecessary code and comments.
*/

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"
#include "Pythia8/HiddenValleyFragmentation.h"
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"
#include <iostream>

using namespace Pythia8;

int main() {

  //Pythia info
  Pythia pythia;

  //Read in commands from external file
  //Note: This needs to be the first line to be read (very important)
  pythia.readFile("main_hidden_valley_6.cmd");

  //Read other commands
  pythia.readString("Beams:frameType = 4");
  pythia.readString("Beams:LHEF = Hidden_valley_6.lhe");
  pythia.init();

  //HepMC information
  HepMC::Pythia8ToHepMC ToHepMC;
  HepMC::IO_GenEvent ascii_io("main_Hidden_valley_6.hepmc", std::ios::out);

  //Allow for possibility of a few faulty events.
  int nAbort = 10;
  int iAbort = 0;

  //Begin event loop; generate until none left in input file.
  for (int iEvent = 0; ; ++iEvent)
  {
    //Generate events, and check whether generation failed.
    if (!pythia.next())
    {
      //If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) 
        break;

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort)
        continue;

      break;
    }

    //Construct new empty HepMC event and fill it.
    //Units will be as chosen for HepMC build; but can be changed
    //by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
    ToHepMC.fill_next_event( pythia, hepmcevt );

    //Write the HepMC event to file. Done with it.
    ascii_io << hepmcevt;
    delete hepmcevt;

  //End of event loop.
  }

  //Done.
  return 0;
}
