#include <uhd/types/tune_request.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/math/special_functions/round.hpp>
#include <chrono>
#include <complex>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include "my_wavetble.hpp"
#include <vector>

/****************************************************************************
 * Signal handler
 ***************************************************************************/

static bool stop_signal_called = false;
void sig_int_handler(int){
    stop_signal_called = true;
}

/****************************************************************************
 * MAIN FUNCTION
 ***************************************************************************/


int UHD_SAFE_MAIN(int argc, char* argv[]){
    uhd::set_thread_priority_safe();
    
    uint64_t total_num_samps = 0;
    //Data packet rate
    double rate=1e9;
    //RF center frequency
    double freq=400e6;
    double gain=10;
    //Waveform Frequency
    // this the frequency of the wave before rf modulation
    double wave_freq = 100000; // you might want to use float because of the other functions
    double bw = 4000;
    double lo_offset = 0.0;
    float ampl= 0.5;
    std::complex<float> samps_per_buff = 1;
    
    std::string address = "addr0=192.168.10.2";
    std::string subdev = "A:0"; //N210
    //std::string subdev = "A:A"; //B210
    // Reference clock source
    std::string ref = "internal";    
    std::string antenna = "TX/RX";
    //Type of wave to be generated
    const std::string wave_type = "WF"; 
    
    //Create USRP device
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(address);
    
    //Selecting subdevice
    usrp->set_tx_subdev_spec(subdev);

    //Define channels
    std::vector<size_t> channel_nums;
    channel_nums.push_back(0);

    //Lock mboard clocks
    usrp->set_clock_source(ref);
    
    usrp->set_tx_rate(rate);
    usrp->set_tx_gain(gain);
    
    uhd::tune_request_t tune_request(freq, lo_offset);
    usrp->set_tx_freq(tune_request,channel_nums[0]);

    usrp->set_tx_bandwidth(bw);
    usrp->set_tx_antenna(antenna, channel_nums[0]);
    
    //Give error if it is not possible to generate the waveform
    if(std::abs(wave_freq)>usrp->get_tx_rate()/2){
        throw std::runtime_error("Wave frequency out of Nyquist Zone");
    }

    if(usrp->get_tx_rate()/ std::abs(wave_freq)> wave_table_len/2){
        throw std::runtime_error("wave freq too small for table");
    }
   
    // pre-compute waveform values
    // see wavetable.hpp and make ur own wave
    const wave_table_class wave_table(wave_type, ampl);
    const size_t step = boost::math::iround(wave_freq / usrp->get_tx_rate() * wave_table_len);
    size_t index = 0;
    
    // Probably need to make this buffer longer

    //create a transmit streamer
    uhd::stream_args_t stream_args("fc32", "sc16");
    stream_args.channels = channel_nums;
    uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

    // AN ALLACTION FOR AN ADDITIONAL BUFFER MIGHT BE NEEDED HERE
    // (comapare to the example file)



    //allocate stream buffer only for one channel
    std::vector<std::complex<float> > buff(sizeof(samps_per_buff));
    
    //allocate vector of pointers to buffer
    std::vector<std::complex<float>*> p_buff(channel_nums.size(), &buff.front());

    //pre-fill buffer
    for(size_t n = 0; n < buff.size(); n++){
        buff[n] = wave_table(index += step);
    
    }

    usrp->set_time_now(0.0);

    //  Check Ref and LO Lock detect
    std::vector<std::string> sensor_names;
    const size_t tx_sensor_chan = channel_nums.empty() ? 0 : channel_nums[0];
    sensor_names = usrp->get_tx_sensor_names(tx_sensor_chan);
    if(std::find(sensor_names.begin(),sensor_names.end(),"lo_locked")
            != sensor_names.end()){
        uhd::sensor_value_t lo_locked
            = usrp->get_tx_sensor("lo_locked", tx_sensor_chan);
        std::cout<<boost::format("Checking TX: %s ...>") % lo_locked.to_pp_string()
            << std::endl;
        UHD_ASSERT_THROW(lo_locked.to_bool());
        }

    std::signal(SIGINT, &sig_int_handler);
    std::cout << "Press Ctrl + C to stop streaming..." << std::endl;

    //Metadata Setup
    uhd::tx_metadata_t md;
    md.start_of_burst = true;
    md.end_of_burst = false;
    md.has_time_spec = true;
    md.time_spec = usrp->get_time_now()+uhd::time_spec_t(0.1);

    //send data untill the signal handelr gets called
    //or if the number of samples specified  are accumulated
    uint64_t num_acc_samps = 0;
    while(true){
        // stop signal called by pressing Ctrl+C
        if (stop_signal_called){
            break;
        }
        // Break when nsamps have been received
        if(total_num_samps > 0 and num_acc_samps >= total_num_samps){
            break;
        }

        //send the entire contents of buffer
        num_acc_samps += tx_stream->send(p_buff, buff.size(),md);    
        
        //refill buffer
        for (size_t n =0;n<buff.size(); n++){
            buff[n] = wave_table(index += step);
        }

        md.start_of_burst = true;   //orig: false
        md.has_time_spec = true;    //orig: false
    }

    //send a mini EOB packer
    md.end_of_burst = true;
    tx_stream->send("",0,md);

    //finished
    std::cout <<std::endl << "Done!" << std::endl << std::endl;
    return EXIT_SUCCESS;
}




