#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h>
#include <tuple>
#include <complex>
#include "constants.h"
#include "get_time.h"
#include "gen_files.h"
#include "process.h"

int main(){

    // For Navi PC
    //std::string data_path = "/home/navi/usrp_cpp/plots/";
    // For Manjaro
    //std::string data_path = "/home/jnavi/usrp_cpp/plots/";
    // define file names to be opened
    std::string f_name1 = "ch1.dat";
    std::string f_name2 = "ch2.dat";

    std::string output;
    // create file stream buffers for reading data and associate them with
    // the files
    std::ifstream i_f1(DATA_PATH + f_name1, std::ios::binary);
    if (!i_f1){
        std::cout << "Error occurred reading file " << f_name1 << std::endl; 
        std::cout << "Exiting" << std::endl;
        return 0;
    }

    std::ifstream i_f2(DATA_PATH + f_name2, std::ios::binary);
    if (!i_f2){
        std::cout << "Error occurred reading file " << f_name2 << std::endl;
        std::cout << "Exiting" << std::endl;
        return 0;
    }

    //Number of samples
    int nsamps = 10000;

    //https://stackoverflow.com/questions/40392644/how-to-read-a-binary-file-of-complex-numbers-in-c
    // We are reading the complex floats by reading consecutive real and 
    // imaginary floats. The complex template stores the both the imaginary     
    // and real parts as separate floats.
    std::vector<std::complex<float> > dv1(nsamps/2);
    std::vector<std::complex<float> > dv2(nsamps/2);

    //read data in from ifstream
    i_f1.read(reinterpret_cast<char*>(dv1.data()), nsamps*sizeof(float));
    i_f2.read(reinterpret_cast<char*>(dv2.data()), nsamps*sizeof(float));

    i_f1.close();
    i_f2.close();

    // Test to see if vector was read in correctly
    for (auto i = dv1.begin(); i != dv1.end(); i++){
        std::cout << *i  << " ";
    }
    
    std::cout << "Data vector 1"  << std::endl;
    std::cout << "size " <<  dv1.size() << std::endl;
    std::cout << "Data vector 2"  << std::endl;
    std::cout << "Size " << dv2.size() << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // Remove Unneneccesary padded zeros from end of the vectors dv1 and dv2
    // count number of actual data points
    unsigned int k = 0;
    unsigned int c = 0;
    unsigned int count = 0;
    for(; k != dv1.size(); ++k){
        // find first 10 consecutive zero values in data. This serves to
        // indicate when relevant data has ended
        //std::cout << "k before if: " << k << std::endl;
        if (dv1[k].real() == 0){
            c = k;
            for (; c <= k+9; c++){
                if (dv1[c].real() == 0) count++;
            }
        } 
        if (count == 10) break;
        count = 0;
    }
    
    unsigned int s = 0;
    unsigned int z = 0;
    count = 0;

    for(; s != dv2.size(); ++s){
        // find first 10 consecutive zero values in data. This serves to
        // indicate when relevant data has ended
        if (dv2[s].real() == 0){
            z = s;
            for (; z <= k+9; z++){
                if (dv2[z].real()== 0){
                    count++;
                }
            }
        } 
        if (count == 10){
            break;
        }
        count = 0;
    }
    
    std::vector<std::complex<float>>::size_type NewSize1 = k;
    //std::vector<float>::size_type NewSize2 = s;
    dv1.resize(NewSize1);
    dv2.resize(NewSize1);
    
    std::cout << "Data vector 1 "  << std::endl;
    std::cout << "size "  <<  dv1.size() << std::endl;
    std::cout << "Data vector 2 "  << std::endl;
    std::cout << "size " << dv2.size() << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // store real and imaginary parts in separate vectors
    std::vector<std::vector<float> > r_vec(2, std::vector<float>(dv1.size()));
    std::vector<std::vector<float> > i_vec(2, std::vector<float>(dv1.size()));

    // Separate the real and the imaginary parts of the two channels
    // into two 2D vectors.
    int n =0;
    for (auto i=dv1.begin(); i!=dv1.end(); i++){
        r_vec[0][n] = (*i).real();
        i_vec[0][n] = (*i).imag();
        n++; 
    }

    n = 0;
    for (auto j=dv2.begin(); j!=dv2.end(); j++){
        r_vec[1][n] = (*j).real();
        i_vec[1][n] = (*j).imag();
        n++;
    }

    //Calculate cross correlation using the complex values
    typedef std::vector<std::complex<float>>::const_iterator cplxI; 
    cplxI ic1, ic2;
    std::vector<std::complex<float> > CXcorr(dv1.size());
    unsigned int xcc = 0;
    // Variables for multiplication of complex numbers
    float  var1=0; float var2=0; float var3=0; float var4=0;
    float r_part=0; float i_part=0;
    
    std::cout << "XCcorr" << std::endl;
    for (ic1 = dv1.begin(); ic1 != dv1.end(); ++ic1){
        for ( ic2 = dv2.begin(); ic2 != dv2.end(); ++ic2){
            var1 = (*ic1).real()*(*ic2).real();
            var2 = (*ic1).real()*(*ic2).imag();
            var3 = (*ic1).imag()*(*ic2).real();
            var4 = (*ic1).imag()*(*ic2).imag();
            r_part = r_part + (var1-var4);
            i_part = i_part + (var2+var3);
        }
        CXcorr[xcc] = {r_part,i_part}; 
        var1 = 0; var2 =0; var3 = 0; var4 =0;
        r_part =0; i_part = 0;
        xcc++;
    }
    
    // Calculate the auto-correlation    
    std::vector<std::complex<float> > AC(dv1.size());
    xcc = 0;
    // Variables for multiplication of complex numbers
    var1=0; var2=0; var3=0; var4=0; r_part=0; i_part=0;
    // Computation
    std::cout << "Computing Auto-Correlation of Ch1 signal" << std::endl;
    for (ic1 = dv1.begin(); ic1 != dv1.end(); ++ic1){
        for ( ic2 = dv1.begin(); ic2 != dv1.end(); ++ic2){
            var1 = (*ic1).real()*(*ic2).real();
            var2 = (*ic1).real()*(*ic2).imag();
            var3 = (*ic1).imag()*(*ic2).real();
            var4 = (*ic1).imag()*(*ic2).imag();
            r_part = r_part + (var1-var4);
            i_part = i_part + (var2+var3);
        }
        AC[xcc] = {r_part,i_part}; 
        var1 = 0; var2 =0; var3 = 0; var4 =0;
        r_part =0; i_part = 0;
        xcc++;
    }

    // Get amplitude off cross correlated signal
    cplxI cA;
    std::vector<float> CXAmps;
    for (cA = CXcorr.begin(); cA != CXcorr.end(); ++cA){
        CXAmps.push_back(abs((*cA)));
    }

    // Get amplitude off Auto-Correlated signal
    cplxI aA;
    std::vector<float> ACAmps;
    for (aA = AC.begin(); aA != AC.end(); ++aA){
        ACAmps.push_back(abs((*aA)));
    }
    // destroy previous vector with the complex data  
    dv1.clear();
    dv2.clear();

    // count number of actual data points
    k = 0; c = 0; count = 0;
    std::cout << "rvec0 " << r_vec[0].size() << std::endl;
    std::cout << "ivec0 " << i_vec[0].size() << std::endl;
    std::cout << "rvec1 " << r_vec[1].size() << std::endl;
    std::cout << "ivec1 " << i_vec[1].size() << std::endl;
    

    //////////////////// SOME SIGNAL PROCCESSING ////////////////////////
    std::cout << "Starinig Signal processing" << std::endl;
    // Calculate amplitude of signal from IQ data
    std::vector<std::vector<float> > iq_amps(2, std::vector<float>(r_vec[0].size()));
    unsigned int i=0;
    for (;i != r_vec[0].size(); i++){
        iq_amps[0][i] = sqrt(r_vec[0][i]*r_vec[0][i] +
                i_vec[0][i]*i_vec[0][i]);
        iq_amps[1][i] = sqrt(r_vec[1][i]*r_vec[1][i] +
                i_vec[1][i]*i_vec[1][i]);    
    }
 
    // Calculate time stamp values
    // Divided by two since there are 5 samples per buffer channel and there are two channels
    std::vector<double> xis(TOTAL_NUM_SAMPS/2);
    //xis.reserve(r_vec[0].size());
    int n_packets = (int) ceil(TOTAL_NUM_SAMPS/SAMPS_PER_BUFF);
    if (get_time(xis, n_packets) == 1) return 0;
    
    // Calculate cross correllation from amplitude values
    // here you need to implement a correlation function between all the
    // diffent channels. 1-2 1-3 1-4 2-3 2-4 3-4
    std::vector<float> xcorr(r_vec[0].size());
    //X-corr
    for (unsigned int i = 0; i != iq_amps[0].size(); ++i){
        for(unsigned int j = 0; j != iq_amps[1].size(); ++j){
            xcorr[i] = xcorr[i] +iq_amps [0][i]*iq_amps[1][j];
        }
    }
    
    //check that lengths of the vectors are correct
    std::cout << "CXAmps " << CXAmps.size() << std::endl;
    std::cout << "ACAmps  " << ACAmps.size() << std::endl;
    std::cout << "r_vec[0] " << r_vec[0].size() << std::endl;
    std::cout << "xcorr " << xcorr.size() << std::endl;
    std::cout << "xis " << xis.size()<< std::endl;

    // Define iterators for going through data vectors
    //typedef std::vector<float>::const_iterator itrtr;
    //typedef std::vector<double>::const_iterator ditr;
    //itrtr  itA1, itA2;
    //ditr itx;
    
    process(CXAmps, ACAmps, xcorr);

    //////////////////// GNUPLOT FILE GENERATION ////////////////////////
    if (gen_files(xis, iq_amps, CXAmps, ACAmps, xcorr) != 0) {
        std::cout << "EXITING" << std::endl;
        return 0; 
    }
    /*
    // Amplitude Plot
    // Put this in a new function later
    std::ofstream MIMO_fs;
    MIMO_fs.open(DATA_PATH2+MIMO_fName);
    MIMO_fs << "# X         A1         A2\n";
    //Loop through x-axis and amplitude data for both channels and
    //writ to file
    for (itx=xis.begin(), itA1=iq_amps[0].begin(), itA2=iq_amps[1].begin();
            (itx != xis.end()) && (itA1 != r_vec[0].end()) 
            && (itA2 != i_vec[0].end()); ++itx, ++itA1, ++itA2){
        MIMO_fs << std::setprecision(10) << "  " << *itx <<"         "<< *itA1 <<"          " << *itA2
            <<"\n";
    }
    MIMO_fs.close();
    
    //Complex X-Corr plot
    std::ofstream cxc_fs;
    cxc_fs.open(DATA_PATH2 + cxc_fname);
    cxc_fs << "#x       CXcorr\n";
    itrtr iXCc;
    for (itx = xis.begin(), iXCc= CXAmps.begin(); 
            (itx != xis.end()) && (iXCc != CXAmps.end()); 
            ++itx, ++iXCc){
        cxc_fs << std::setprecision(10) <<*itx <<"     " << *iXCc << "\n";
    }
    cxc_fs.close();

    // Complex Autocorrelation
    std::ofstream ac_fs;
    ac_fs.open(DATA_PATH2 + ac_fname);
    ac_fs << "#x       aXcorr\n";
    itrtr iAc;
    for (itx = xis.begin(), iAc= ACAmps.begin(); 
            (itx != xis.end()) && (iAc != ACAmps.end()); 
            ++itx, ++iAc){
        ac_fs << std::setprecision(10) << *itx <<"     " << *iAc << "\n";
    }
    ac_fs.close();

    //XCORR Plot
    itrtr itXC;
    std::ofstream XCf;
    XCf.open(DATA_PATH2+xc_fname);
    
    XCf << "#x         X-corr \n";
    for (itx = xis.begin(), itXC= xcorr.begin(); 
            (itx != xis.end()) && (itXC != xcorr.end()); 
            ++itx, ++itXC){
        XCf << std::setprecision(10) << *itx <<"     " << *itXC << "\n";
    }
    
    XCf.close();
   */ 
   return 0;
}

