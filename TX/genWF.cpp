#include <waveform.hpp>
#include <cmath>
#include <iterator>
#include <iostream>
#include <fstream>


/*------------------------------------------
 * Generates complex vector: exp(j2pi f t)
 *----------------------------------------*/
 


// Generate complex waveform component
void Waveform::genWF_component (float *f_vec_pos, float *p_vec_pos){
    float p_var = 2*pii*(*p_vec_pos);
    
    // if denotes the imaginary float
    for (unsigned int c =0; c != wave_table_len; ++c){  
        f_var = 2*pii*(*f_vec_pos)*c/wave_table_len;
        wf[c] = std::complex<float>(
                    std::cos(f_var + p_var), std::sin(f_var + p_var)); 
    }
}

void Waveform::genWF (void){
    
    // Iterator for going through frequencies 
    typedef std::vector<float>::iterator itr;
    itr it_f, it_ph;

    
    int check = 0;
    for(it_f = freqs.begin(), it_ph = phase.begin(); 
            (it_f != freqs.end()) && (it_ph != phase.end());
            ++it_f, ++it_ph){
        
        // Derefernce iterator (this is allowed) then get the address
        // Note! iterator cannot be passed as a pointer
        genWF_component(&(*it_f), &(*it_ph));
       
        //std::cout << wf[check] << std::endl;

        for(unsigned int c =0; c != wf.size(); c++){
            if(check == 0){
                fwf[c] = std::complex<float>(
                        wf[c].real(), wf[c].imag());
                
                //std::cout << fwf[c] << std::endl;
            }
            else{
                fwf[c] = std::complex<float>(
                        wf[c].real() * fwf[c].real(), 
                        wf[c].imag() * fwf[c].imag());   
            }
        }check++;
    }
}

void Waveform::cplx_to_real(void){
    for (unsigned int c = 0; c != wf_mag.size(); c++){
        wf_mag[c] = std::norm(fwf[c]);
    } 
}


