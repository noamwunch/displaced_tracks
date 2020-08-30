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


#if defined(_MSC_VER)
# define RaveDllExport __declspec(dllexport)
#else
# define RaveDllExport
#endif


//R__LOAD_LIBRARY(libRaveBase)
R__LOAD_LIBRARY(/usr/local/lib/libCLHEP-2.0.3.0.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveCore.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveVertex.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveFlavorTag.so)
R__LOAD_LIBRARY(/usr/local/lib/libRaveVertexKinematics.so)


#include "/usr/local/include/rave/Version.h"
#include "/usr/local/include/rave/VertexFactory.h"
//#include "/usr/local/include/rave/Vertex.h"
//#include "/usr/local/include/rave/Track.h"
//#include "/usr/local/include/rave/Covariance6D.h"
//#include "/usr/local/include/rave/Vector6D.h"
//i:include "/usr/local/include/rave/ConstantMagneticField.h"

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

//Main code
void Example6D()
{
  //Prepare to write
  ofstream myfile;
  myfile.open ("Results.txt");

  //Load Delphes information
  gSystem->Load("/usr/local/lib/libCLHEP-2.0.3.0");
  gSystem->Load("/usr/local/lib/libRaveCore");
  gSystem->Load("/usr/local/lib/libRaveVertex");
  gSystem->Load("/usr/local/lib/libRaveFlavorTag");
  gSystem->Load("/usr/local/lib/libRaveVertexKinematics");



  float Bz = 2.0;   // Magnetic field
  rave : rave :: ConstantMagneticField mfield(0.,0.,Bz);
  myfile << "Done" << endl;

}




