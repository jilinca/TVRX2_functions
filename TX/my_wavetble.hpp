#include <cmath>
#include <complex>
#include <stdexcept>
#include <string>
#include <vector>
#include <waveform.hpp>

// wave table len is already defined in classexone.hpp
//static const size_t wave_table_len = 8192;

class wave_table_class
{
public:
    wave_table_class(const std::string& wave_type, const float ampl)
        : _wave_table(wave_table_len)
    {
        // compute real wave table with 1.0 amplitude
        std::vector<float> real_wave_table(wave_table_len);
        if (wave_type == "CONST") {
            for (size_t i = 0; i < wave_table_len; i++)
                real_wave_table[i] = 1.0;
        } else if (wave_type == "SQUARE") {
            for (size_t i = 0; i < wave_table_len; i++)
                real_wave_table[i] = (i < wave_table_len / 2) ? 0.0 : 1.0;
        } else if (wave_type == "RAMP") {
            for (size_t i = 0; i < wave_table_len; i++)
                real_wave_table[i] = 2.0 * i / (wave_table_len - 1) - 1.0;
        } else if (wave_type == "SINE") {
            static const double tau = 2 * std::acos(-1.0);
            for (size_t i = 0; i < wave_table_len; i++)
                real_wave_table[i] = std::sin((tau * i) / wave_table_len);
        } else if (wave_type == "WF"){
            // See file classexone.hpp for additional info on waveform 
            // initialization

            /*************INITIALIZE WAVEFORM PARAMETERS****************/
            // define frequency (Hz) and phase (deg) components
            float f = 100000; 
            float ph = 25;      
            // define increments (just to speed things up)
            float incr = 250;
            float incr_p = 15;
            // total number of components
            const int n_components = 4;
    
            // Create Waveform Object
            Waveform WF1;
    
            // Fill vectors with values for different frequencies and phases 
            for(int i = 0; i != n_components; ++i){
                f = f+incr;
                ph = ph + incr_p;
                WF1.add_freq(f);
                WF1.add_phase_deg(ph);
            } 
            /**********************GENERATE WAVEFORM*********************/
   
            WF1.genWF();
            WF1.cplx_to_real();
            for (size_t i = 0; i < wave_table_len; i++){
                real_wave_table[i] = WF1.wf_mag[i];
            }
        } else
            throw std::runtime_error("unknown waveform type: " + wave_type);

        // compute i and q pairs with 90% offset and scale to amplitude
        for (size_t i = 0; i < wave_table_len; i++) {
            const size_t q = (i + (3 * wave_table_len) / 4) % wave_table_len;
            _wave_table[i] =
                std::complex<float>(ampl * real_wave_table[i], ampl * real_wave_table[q]);
        }
    }

    inline std::complex<float> operator()(const size_t index) const
    {
        return _wave_table[index % wave_table_len];
    }

private:
    std::vector<std::complex<float>> _wave_table;
};
