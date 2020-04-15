#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED
// This header file is meant for you to manipulate if you want to 
// change the operation or other parameters of the USRP
#include <vector>
#include <complex>

// TYPE DEFINTIONS FOR VECTORS USED
typedef std::vector<float> fvec;                            // float vector
typedef std::vector<double> dvec;                           // double vector
typedef std::vector<std::vector<float>> f2vec;              // 2D float vector
typedef std::vector<std::vector<complex<float>>>  cf2vec;   // 2D complex float vector

// TYPE DEFINITIONS FOR ITERATORS
typedef std::vector<float>::const_iterator itrtr;
typedef std::vector<double>::const_iterator ditr;


// NAMES AND PATHS FOR DATA FILES
//std::string DATA_PATH("/home/jnavi/usrp_cpp/plots/TestData/");
std::string DATA_PATH("/home/julu/usrp_cpp/plots/TestData/");
//std::string DATA_PATH2("/home/jnavi/usrp_cpp/plots/gplot/");
std::string DATA_PATH2("/home/jnavi/usrp_cpp/plots/gplot/");
//std::string TIME_SEC("/home/jnavi/usrp_cpp/plots/TestData/full.dat");
std::string TIME_SEC("/home/julu/usrp_cpp/plots/TestData/full.dat");
//std::string TIME_FRAC("/home/jnavi/usrp_cpp/plots/TestData/frac.dat");
std::string TIME_FRAC("/home/julu/usrp_cpp/plots/TestData/frac.dat");
std::string CH1_NAME = "ch1.dat";
std::string CH2_NAME = "ch2.dat";
std::string CH3_NAME = "ch3.dat";
std::string CH4_NAME = "ch4.dat";

// DATA FILE NAMES (You can change these for your own convenience)
std::string MIMO_fName = "MIMO_IQplot.txt";                 // Amplitude of RAW data calculated from IQ
std::string cxc_fname = "CXcorr.txt";                       // Complex cross correlation
std::string ac_fname = "AXcorr.txt";                        // Channel 1 autocorrelation
std::string xc_fname = "XCorr.txt";                          // Cross correlation

// RX VARIABLES
int TOTAL_NUM_SAMPS = 10000;
int SAMPS_PER_BUFF = 10;
const int NUM_CHANNELS = 2;
const int NUM_CHANNELS4 = 4;  
double CLOCK_FREQ = 100e6;

// Data transmission rate
//double rate;
// Center frequency
//double freq;

// SUBDEVICE SPECIFICATION TVRX2
//std::string subdev("A::RX1 A:RX2");         // TVRX2
//std::string ant("RX1, RX2");

#endif //CONSTANTS_H_INCLUDED
