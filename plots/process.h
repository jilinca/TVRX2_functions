#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED

#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "constants.h"

// Get data from files
void read_data(cf2vec &ch_data){
        
    //std::string output;
    std::ifstream i_f1(DATA_PATH + CH1_NAME, std::ios::binary);
    if (!i_f1){
        std::cout << "Error occurred reading file " << f_name1 << std::endl; 
        std::cout << "Exiting" << std::endl;
        return 0;
    }

    std::ifstream i_f2(DATA_PATH + CH2_NAME, std::ios::binary);
    if (!i_f2){
        std::cout << "Error occurred reading file " << f_name2 << std::endl;
        std::cout << "Exiting" << std::endl;
        return 0;
    }
    
    if (NUM_CHANNELS == 4){
        std::ifstream i_f2(DATA_PATH + CH3_NAME, std::ios::binary);
        if (!i_f3){
            std::cout << "Error occurred reading file " << f_name2 << std::endl;
            std::cout << "Exiting" << std::endl;
            return 0;
        }
    
        std::ifstream i_f2(DATA_PATH + CH4_NAME, std::ios::binary);
        if (!i_f4){
            std::cout << "Error occurred reading file " << f_name2 << std::endl;
            std::cout << "Exiting" << std::endl;
            return 0;
        }
    }

    // Read the data into the given vector
    i_f1.read(reinterpret_cast<char*>(ch_data[0].data()), nsamps*sizeof(float));
    i_f2.read(reinterpret_cast<char*>(ch_data[1].data()), nsamps*sizeof(float));
    if (NUM_CHANNELS == 4){
        i_f3.read(reinterpret_cast<char*>(ch_data[2].data()), nsamps*sizeof(float));
        i_f4.read(reinterpret_cast<char*>(ch_data[3].data()), nsamps*sizeof(float));
    } 

    i_f1.close();
    i_f2.close();
    if(NUM_CHANNELS = 4){
        i_f3.close();
        i_f4.close();
   }

}



// Function to find the number of channel combinations to perform correlation between all channels 
int get_num_combnations(const int num_channels){
    int dummy1 = 1;
    int comb_size = 2;
    int dummy2 = 1;
    for (int i = num_channels; i > 1; i--){
        dummy1 = dummy1*i;
    }
    for (int j = num_channels-comb_size; j>1; j--){
        dummy2 = dummy2*j;
    }
    
    // Number of combinations
    int n_comb = dummy1/(comb_size*dummy2);
    std::cout << "n_comb " << n_comb << std::endl;
    return n_comb;
}



// FIND THE MAXIMUM VALUE FOR THE CORRELATION
int process(dvec& xis, fvec& CXAmps, fvec& ACAmps, fvec& xcorr, dvec& ch_delays){
    
    // Array to correlation peak time values (i.e delays) 
    //NOTE: THE SIZE OF THIS ARRAY HAS TO BE CHANGED DEPENDING ON THE NUMBER OF ANTENNAS THAT ARE USED
    
    //double delay_v [3] = {};
    // Temp variable to hold the vector index of the peak value of the corr peak
    int p_index [3] = {};
    
    p_index[0] = std::distance(CXAmps.begin(), 
            max_element(CXAmps.begin(), CXAmps.end()));
    
    p_index[1] = std::distance(ACAmps.begin(), 
            max_element(ACAmps.begin(), ACAmps.end()));
    
    p_index[2] = std::distance(xcorr.begin(), 
            max_element(xcorr.begin(), xcorr.end()));
    
    //delay_v
    
    ch_delays[i] = xis[p_index[0]];
    // JUST CHECKING
    //int i = 0;
    //for (; i <3; i++) 
    //    std::cout << "Peak index" << p_index[i] << std::endl;
    
    return 0;
}


#endif // PROCESS_H_INCLUDED
