#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include <iterator>
#include <cmath>


// Function to make a vector of time stamps for each sample taken. The time stamps will be stored in the 
// vector given as the first argument.

// Return: 0 on sucess
//       : 1 on failure 

// Input arguments
//get_time([refence to the begging of  vector to store the time stamp values into.], [size of vector with the time stamps for each time a data packet of samples was sent from the USRP to the host])

int get_time(std::vector<double> &t_fin, int n_packets){
    
    /////////////////////// Read the packet timestamps //////////////////////
    
    // Empty data vector time values
    std::vector<int> t_full(n_packets); 
    std::vector<double> t_frac(n_packets);
    //*    
    // Read time data from txt file
    std::ifstream full_s(TIME_SEC, std::ios::binary);
    if(!full_s){
        std::cout << "Could not open file "<< TIME_SEC << std::endl;
        std::cout << "Exiting" << std::endl;
        return 1;
    }
    full_s.read(reinterpret_cast<char*>(t_full.data()), n_packets*sizeof(int));
    full_s.close();
    //*/

    std::ifstream frac_s(TIME_FRAC, std::ios::binary);
    if(!frac_s){
        std::cout << "Could not open file "<< TIME_FRAC  << std::endl;
        std::cout << "Exiting" << std::endl;
        return 1;
    }
    frac_s.read(reinterpret_cast<char*>(t_frac.data()), n_packets*sizeof(double));
    frac_s.close();
        
    //////////////////////// Create the time vector ///////////////////////////
    /* 
    std::cout << "t_full" << std::endl;
    for (auto i=t_full.begin(); i != t_full.end(); i++){
        std::cout << *i << " ";
    }
    std::cout << std::endl;

    std::cout << "t_frac" << std::endl;
    for (auto j=t_frac.begin(); j != t_frac.end(); j++){
        std::cout << *j << " ";
    }
    std::cout << std::endl;
    */ 
    // Add the full and fractional time-stamp values and place them into a vector
    int c = 0;
    double diff = 0;
    double step;
    std::vector<double> t_vec(n_packets);
    
    std::vector<int>::const_iterator i;
    std::vector<double>::const_iterator j;
    for (i=t_full.begin(), j=t_frac.begin(); 
            (i != t_full.end()) && (j != t_frac.end()); ++i, ++j){
        t_vec[c] = (*i)+(*j);
        //*
        // This is just to check that the time difference between the packets 
        // truely stays the same.
        if (c > 0){
            diff = t_vec[c]-t_vec[c-1];
        }
        //*/
        c++;
    }
    std::cout << "c: " << c << std::endl;
    
    // Create the indermediate time stamp values 
    diff = t_vec[1]-t_vec[0]; 
    step = diff/5;
    c = 0;

    for (auto i = t_vec.begin(); i != t_vec.end(); ++i){
        for (int j = 0; j < 5; j++){
            if (j == 0){
                t_fin[c] = *i;
                //std::cout <<std::setprecision(10) << *i << std::endl;
                //std::cout << std::setprecision(12) << t_fin[c] << std::endl;
                c++;
            }    
            else{    
                t_fin[c] = *i + j*step; 
                //std::cout << std::setprecision(12) << t_fin[c] << std::endl;
                //std::cout << j*step << std::endl;
                //std::cout << std::setprecision(12) << t_fin[c] - t_fin[c-1]<<std::endl;
                c++;
             }
        }
    }

    std::cout << c << std::endl; 
    std::cout << "Writing finished successfully." << std::endl;
    /* 
    //Check
    for (auto i = t_fin.begin(); i != t_fin.end(); ++i){
        if (i!=t_fin.begin()){ 
            std::cout << *i-*(i-1) << std::endl;
        }    
    }
    //Check 2
    for (auto i = t_fin.begin(); i != t_fin.end(); ++i){
        if (i!=t_fin.begin()){ 
            std::cout << std::setprecision(12) << *i << std::endl;
        }    
    }
    */
    return 0;
}
