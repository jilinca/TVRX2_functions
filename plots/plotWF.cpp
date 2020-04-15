#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h>
#include <tuple>
// Enables plotting directly from stream
//#include <gnuplot-iostream.h>
//#include <map>
//#include <cstdio>
//#include <boost/foreach.hpp>


int main(){

    //path to files
    //std::string data_path= "/home/julu/Documents/Work/Jusrp/plots/";
    // For Navi PC
    std::string data_path = "/home/navi/usrp_cpp/plots/"; 
    
    // define file names to be opened
    std::string f_name1 = "gplot/ch1_d2.dat";
    std::string f_name2 = "gplot/ch2_d2.dat";
    //std::string f_name1 = "WBX_RX2.dat";
    //std::string f_name2 = "SBX_RX2.dat";
    
    std::string output; 
    // create file stream buffers for reading data and associate them with
    // the files
    std::ifstream i_f1(data_path + f_name1, std::ios::binary);
    if (!i_f1){
        std::cout << "Error occurred reading file " << f_name1 << std::endl; 
        std::cout << "Exiting" << std::endl;
        return 0;
    }
   
    std::ifstream i_f2(data_path + f_name2, std::ios::binary);
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
    std::vector<std::complex<float> > dv1(nsamps);
    std::vector<std::complex<float> > dv2(nsamps);

    //Yet another method to read data in from the ifstream
    //the ".data()" returns a pointer to start of the vector
    // old syntax would be &dv[0].
    i_f1.read(reinterpret_cast<char*>(dv1.data()), nsamps*sizeof(float));
    i_f2.read(reinterpret_cast<char*>(dv2.data()), nsamps*sizeof(float));
    
    i_f1.close();
    i_f2.close();    


    // Test to see if vector was read in correctly
    //for (auto i = dv2.begin(); i != dv2.end(); i++){
    //    std::cout << *i  << " ";
    //}
    
    // store real and imaginary parts in separate vectors
    std::vector<std::vector<float> > r_vec(2, std::vector<float>(nsamps));
    std::vector<std::vector<float> > i_vec(2, std::vector<float>(nsamps));

    
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
    
    // destroy previous vector with the complex data  
    dv1.clear();
    dv2.clear();
    
    // count number of actual data points
    unsigned int k = 0;
    unsigned int c = 0;
    unsigned int count = 0;
    //std::vector<std::vector<float> >* p_count;
    //float *p_count;
    
    // Process 
    for(; k != r_vec[0].size(); ++k){
        // find first 10 consecutive zero values in data. This serves to
        // indicate when relevant data has ended
        if (r_vec[0][k] == 0){
            c = k;
            for (; c <= k+9; c++){
                if (r_vec[0][c]== 0){
                    count++;
                }
            }
        } 
        if (count == 10){           
            break;
        }
        count = 0;
    }

    unsigned int s = 0;
    unsigned int z = 0;
    count = 0;
    
    for(; s != r_vec[1].size(); ++s){
        // find first 10 consecutive zero values in data. This serves to
        // indicate when relevant data has ended
        if (r_vec[1][s] == 0){
            z = s;
            for (; z <= k+9; z++){
                if (r_vec[1][z]== 0){
                    count++;
                }
            }
        } 
        if (count == 10){
            break;
        }
        count = 0;
    }

    std::vector<float>::size_type NewSize1 = k;
    std::vector<float>::size_type NewSize2 = s;
    r_vec[0].resize(NewSize1);
    i_vec[0].resize(NewSize1);
    r_vec[1].resize(NewSize2);
    i_vec[1].resize(NewSize2);

    // Check
    //std::cout <<k <<" "<<s<< std::endl;
    
    /// THIS IS WRONNG ADD POWERS OF TWO AND SQRT
    // Calculate amplitude of signal 
    std::vector<std::vector<float> > iq_amps(2, std::vector<float>(k));
    unsigned int i=0;
    for (;i != r_vec[0].size(); i++){
        iq_amps[0][i] = sqrt(r_vec[0][i]*r_vec[0][i] +
                i_vec[0][i]*i_vec[0][i]);
        iq_amps[1][i] = sqrt(r_vec[1][i]*r_vec[1][i] +
                i_vec[1][i]*i_vec[1][i]);    
        
        // Check
        //std::cout << iq_amps[0][i] << std::endl;
    
    }
    
    // create x-axis
    std::vector<float> xis;
    xis.reserve(r_vec[0].size());
    float num = 0;
    float rate = 100;
    float incr = float(r_vec[0].size()) / rate;
    for (unsigned int i = 0; i != r_vec[0].size(); ++i){
        if( i == 0 )
            xis.push_back(num);
        else{
            num = num +incr;
            xis.push_back(num);
        }
        //std::cout << xis[i] << std::endl;
    }
    

    std::vector<float> xcorr(iq_amps[0].size());
    //X-corr
    for (unsigned int i = 0, i != iq_amps[0].size(), ++i){
        for(unsigned int j = 0, j != iq_amps[1].size(), ++j){
            
            xcorr[i] = xcorr[i] + iq_amps[0][i]*iq_amps[1][j];
        } 
    }
    
    // Define iterators for going through data vectors
    typedef std::vector<float>::const_iterator itrtr;
    itrtr itx, itA1, itA2;
    
    //////////////////// GNUPLOT FILE GENERATION ////////////////////////
    // Put this in a new function later
    std::string MIMO_fName = "gplot/MIMO_IQplot.txt";
    std::ofstream MIMO_fs;
    MIMO_fs.open(data_path+MIMO_fName);
    MIMO_fs << "# X         A1         A2\n";
    
    //Loop through x-axis and amplitude data for both channels and
    //writ to file
    for (itx=xis.begin(), itA1=iq_amps[0].begin(), itA2=iq_amps[1].begin();
            (itx != xis.end()) && (itA1 != iq_amps[0].end()) 
            && (itA2 != iq_amps[1].end()); ++itx, ++itA1, ++itA2){
        MIMO_fs <<"  " << *itx <<"         "<< *itA1 <<"          " << *itA2
            <<"\n";
    }
    MIMO_fs.close();
    
    //XCORR Plot
    itrtr itXC;
    std::string xcf_name = "gplot/XCorr.txt";
    std::ofstream XCf;
    XCf.open(data_path+xcf_name);
    
    XCf << "X-corr \n"
    for (itXC= xcorr.begin(),itXC != xcorr.end(),++itXC){
        XCf << *itXC << "\n"
    }
    
    XCf.close();
    //////////////////////////////////////////////////////////////////////
    

    /*
     / Create data file to be used with gnuplot  
    std::string ch1f_name = "ch1d.txt";
    
    std::ofstream ch1;
    ch1.open(data_path + ch1f_name);
    ch1 << "# First Data block\n";
    ch1 << "# X     Y\n";

    for (itx = xis.begin(), ity = r_vec[0].begin();
           (itx != xis.end()) && (ity != r_vec[0].end()); ++itx , ++ity){
        ch1 << *itx << "    " << *ity << "\n";    
    }    
    ch1.close();
    */
    /*
    // Place vector data into tuples to be plotted. (Should work with bot pair and tuple
    // implementation but nothing is plotted)   
    // Define vector of tuples
    //typedef std::vector<std::tuple<float, float> > xy_data; 
    //xy_data dat;    
    // Define vector of pairs
    std::vector<std::pair<float, float > > dat;
    
    // Define iterators for going through data vectors
    typedef std::vector<float>::const_iterator itrtr;
    itrtr itx, ity;
    //unsigned int top = 0;
    
    
    // Move data from vectors into vector of tuples
    for (itx = xis.begin(), ity = r_vec[0].begin();
            (itx != xis.end()) && (ity != r_vec[0].end()); ++itx , ++ity){
        //dat.push_back(std::make_tuple(*itx ,*ity));
        dat.push_back(std::make_pair(*itx ,*ity));
        //std::cout << std::get<0>(dat[top]) << std::endl;
        //std::cout << std::get<1>(dat[top]) << std::endl;
        //top++;
    }
    // Clear previous vectors
    r_vec.clear();
    i_vec.clear();
    
    // Plot
    //gp << "set xrange [0:1314]\nset yrange [-3e-4:5e-4]\n";
    gp << "plot '-' with vectors title 'Waveform'\n";
    gp.send1d(dat);
    */
   
    

   return 0;
}

