#ifndef WAVEFORM_H_
#define WAVEFORM_H_

#include <vector>
#include <complex>

//Length of waveform
static const int wave_table_len = 8192;
//static const int wave_table_len = 4096;

//The value for PI
static const float pii = std::acos(-1.0);  

//This is also defined in dualusrp.h (but with a slightly different name)
typedef std::vector<float> f_vec;
typedef std::vector<std::complex<float>> cf_vec;

class Waveform {
    public: 
        f_vec freqs;           // frequency components
        f_vec phase;           // phase components
        
        cf_vec fwf=cf_vec(wave_table_len);      // final waveform
        cf_vec wf=cf_vec(wave_table_len);       // temporary waveform
        f_vec wf_mag = f_vec(wave_table_len);   // magnitude of waveform  

        //Some inline function definitons
        void add_freq(float f){
            freqs.push_back(f);
        }
        void add_phase_deg(float ph){
            ph = (ph*2*pii)/half_r;
            phase.push_back(ph);
        }
        void add_phase_rad(float ph){
             phase.push_back(ph);
        }
        
        int get_size(std::vector<float> &freqs){
           return freqs.size(); 
        }

        // These functions are defined in a separate cpp file
        void genWF_component(float *f_vec_pos, float *p_vec_pos);
        
        void genWF(void);

        void cplx_to_real(void);
    
    private:
        const float half_r = 180.0;
        float f_var;
};

#endif /* WAVEFORM_H_ */

