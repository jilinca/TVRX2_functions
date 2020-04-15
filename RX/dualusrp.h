#ifndef DUALUSRP_H_
#define DUALUSRP_H_
//#include <cstddef> // this appears to be included in multi_usrp.hpp
#include <uhd/usrp/multi_usrp.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>

// Defining vectors
typedef std::vector<float> fvec;
typedef std::vector<std::vector<float> > tdfvec;


// Check that USRP devices are set up properly
// This function just prints the current specs of the setup 
void pref_checks(const size_t, const size_t, std::vector<size_t>, uhd::usrp::multi_usrp::sptr);

// Function for performing cross correlation
//fvec xcorr(tdfvec sig);


///////////////// GARBAGE ////////////////////////////////////////////
/*
typedef struct {
    fvec chan1;
    fvec chan2;    
}BB_str; //<--- this is now the type of the struct
// The struct hasnt been given a name or tag yet, it's type has just been 
// declared. You could try and remove the typedef from the front since c++
// should add it automatically (if i understood correctly)


// We need to construct a vector for the timestamps for each sample.
// Since only the time of when each packet is sent is recorded we divide
// 1/sampling_frequency and multiply it 



// Function Declaratiion for Constructing Baseband signal from IQ-data. 
// Function is designed to process 2 sets of IQ data and returns a struct 
// with two vectors. 
typedef vcf = std::vector<complex<float> >
BB_strc make_BB(vcf,vcf, double, std::vector<float>);

// Functiiion is defined in an other cpp file.
*/


#endif /* DUALUSRP_H_ */ 
