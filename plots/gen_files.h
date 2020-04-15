#ifndef GENFILES_H_INCLUDED
#define GENFILES_H_INCLUDED

#include <constants.h>
//typedef std::vector<float> fvec;
//typedef std::vector<double> dvec;
//typedef std::vector<std::vector<float>> fvvec;

int gen_files(dvec& xis, f2vec& iq_amps, fvec& CXAmps, fvec& ACAmps, fvec& xcorr){
    
    // iterators
    itrtr itA1, itA2, iXCc, iAc, itXC;
    ditr itx;
    
    // Amplitude Plot
    // Put this in a new function later
    std::ofstream MIMO_fs;
    MIMO_fs.open(DATA_PATH2+MIMO_fName);
    MIMO_fs << "# X         A1         A2\n";
    //Loop through x-axis and amplitude data for both channels and
    //writ to file
    for (itx=xis.begin(), itA1=iq_amps[0].begin(), itA2=iq_amps[1].begin();
            (itx != xis.end()) && (itA1 != iq_amps[0].end()) 
            && (itA2 != iq_amps[0].end()); ++itx, ++itA1, ++itA2){
        MIMO_fs << std::setprecision(10) << "  " << *itx <<"         "<< *itA1 <<"          " << *itA2
            <<"\n";
    }
    MIMO_fs.close();
    
    //Complex X-Corr plot
    std::ofstream cxc_fs;
    cxc_fs.open(DATA_PATH2 + cxc_fname);
    cxc_fs << "#x       CXcorr\n";
    //itrtr iXCc;
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
    //itrtr iAc;
    for (itx = xis.begin(), iAc= ACAmps.begin(); 
            (itx != xis.end()) && (iAc != ACAmps.end()); 
            ++itx, ++iAc){
        ac_fs << std::setprecision(10) << *itx <<"     " << *iAc << "\n";
    }
    ac_fs.close();

    //XCORR Plot
    //itrtr itXC;
    std::ofstream XCf;
    XCf.open(DATA_PATH2+xc_fname);
    XCf << "#x         X-corr \n";
    for (itx = xis.begin(), itXC= xcorr.begin(); 
            (itx != xis.end()) && (itXC != xcorr.end()); 
            ++itx, ++itXC){
        XCf << std::setprecision(10) << *itx <<"     " << *itXC << "\n";
    }
    
    XCf.close();
    
   return 0;
}

#endif // GENFILES_H_INCLUDED
