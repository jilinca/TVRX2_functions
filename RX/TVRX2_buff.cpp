#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/types/sensors.hpp>
#include <uhd/device.hpp>
#include <uhd/stream.hpp>
#include <uhd/usrp/fe_connection.hpp>
#include <dualusrp.h>
#include <fstream>
#include <vector>
#include <uhd/usrp/fe_connection.hpp>
#include <constants.h>

int UHD_SAFE_MAIN(int argc, char *argv[]){
    
    uhd::set_thread_priority_safe();
    
    // Variables for devices
    std::string ant("RX1, RX2");
    std::string ref0("internal");
    //std::string ref1("mimo");
    std::string time("internal");


    // sub_dev parameters for channel mapping
    std::string subdev0("A:RX1 A:RX2"); // two channels on one mother board (TVRX2)
    	
    // Common variables
    double freq(108e6);
    double gain(5);
    double bw(1.7e6);
    double rate2(1e7); // for data transmission rate from USRP to host
    //double lo_offset(105e6);
    	
    // Motherboard indices for setting sources
    //const size_t mb1 = 1;
    const size_t mb0 = 0;
    
    
    // Addressing and defining the usrp devices
    uhd::device_addr_t addresses("addr0=192.168.10.2"); // Usign just one TVRX2 device
    uhd::usrp::multi_usrp::sptr usrp =uhd::usrp::multi_usrp::make(addresses);
    
    // Setup Subdevice specification TVRX2 one MB
    usrp->set_rx_subdev_spec(subdev0,mb0);

    // Detect channel configuratons
    std::vector<size_t> channel_nums;
    size_t n_chan = usrp->get_rx_num_channels();
    if (n_chan !=2){
        throw std::runtime_error("Invalid channels specified");
        return ~0;
    }
    else{
        std::cout << "Number of channels specified: " << n_chan << std::endl;
        channel_nums.push_back(0);
        channel_nums.push_back(1);
    }

    // Set rx rate (rate at which the data is 
    //transferred between the host and the device)
    usrp->set_rx_rate(rate2);
	
    // Sleep to lock local oscillators
    sleep(1);
	
    // Throw error if exactly two mother boards aren't connected
    UHD_ASSERT_THROW(usrp->get_num_mboards()==1);
	
    // Set clock source for motherboards separately
    usrp->set_clock_source(ref0, mb0);

    // Set time on master mboard  \should have master mboard number
    usrp->set_time_now(uhd::time_spec_t(0,0),mb0); 
	
    // Sleep a while so time can be properly locked
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	
    // Set Gain
    usrp->set_rx_gain(gain,channel_nums[0]);
    usrp->set_rx_gain(gain,channel_nums[1]);
    
    for (size_t ch =0; ch < channel_nums.size(); ch++){
		
    	// Set center frequncy
    	uhd::tune_request_t tune_request(freq);
    	usrp->set_rx_freq(tune_request,channel_nums[ch]);
	// Set Bandwidth
	usrp->set_rx_bandwidth(bw,channel_nums[ch]);
    }
    // Set antenna to recieve only
    usrp->set_rx_antenna(ant, channel_nums[0]);
    usrp->set_rx_antenna(ant, channel_nums[1]);
    
    size_t check = usrp->get_rx_num_channels();

    std::cout<< "Check channel nums again: " << check << std::endl;
    

    //------------------- SETUP STREAMING PARAMETERS-------------------
	
    //Set initial time stamp to zero
    std::cout << boost::format("Setting device timestamp to zero... ") << std::endl;
    usrp->set_time_now(uhd::time_spec_t(0.0));
    // Set streaming format
    uhd::stream_args_t stream_args("fc32","sc16"); 
    // Linearly map channels
    stream_args.channels = channel_nums;
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
	
    double seconds_in_future = 1.5;
    const size_t total_num_samps = 10000;
    // the first call to recieve will later on block this many seconds before recieving
    double timeout = seconds_in_future +0.1;
    
    //number of accumulated samples
    size_t num_acc_samps = 0; 
    
    // SET UP STREAMING
    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
    stream_cmd.num_samps = total_num_samps;
    stream_cmd.stream_now = false;
    stream_cmd.time_spec = uhd::time_spec_t(seconds_in_future);
    rx_stream->issue_stream_cmd(stream_cmd);
	
    // metadata will be filled by recv() automatically when data is received
    uhd::rx_metadata_t md; 
	
    // Allocate buffers (One buffer per channel)
    const size_t samps_per_buff = 10;

    // a paramter of interest
    std::cout << "Samples per one buffer \n";
    std::cout << boost::format("%d") %samps_per_buff << std::endl;

    // 2D complex buffer vector 
    std::vector<std::vector<std::complex<float> > > buffs(
	usrp->get_rx_num_channels(), std::vector<std::complex<float> > (samps_per_buff));

    // Vector of pointers to point to each of the channel buffers
    std::vector<std::complex<float>*> buff_ptrs;
    for (size_t i=0; i<buffs.size();i++) buff_ptrs.push_back(&buffs[i].front());
    
    std::vector<std::vector<std::complex<float> > > rx_buff(
	usrp->get_rx_num_channels(), std::vector<std::complex<float> >(total_num_samps));

    //std::cout << "Got here 2" << std::endl;

    // VECTORS FOR TIMESTAMPS
    // Time values in this vector will indicate at what time the last sample was received
    int n_packets = (int) ceil(total_num_samps/(samps_per_buff));
    //ticks
    std::vector<long long> t_stamps(n_packets);
    //full seconds
    std::vector<int> full(n_packets);
    //fractional seconds    
    std::vector<double> frac(n_packets);
    
    ///////////////////// CREATION OF FILES TO STREAM IN ///////////////////////////
    // The file names
    std::string file1 = "/home/jnavi/usrp_cpp/plots/TestData/ch1.dat";
    std::string file2 = "/home/jnavi/usrp_cpp/plots/TestData/ch2.dat";
    std::string file4 = "/home/jnavi/usrp_cpp/plots/TestData/full.dat";
    std::string file5 = "/home/jnavi/usrp_cpp/plots/TestData/frac.dat";
    
    // Open files
    std::ofstream outfile1;
    outfile1.open(file1.c_str(), std::ofstream::binary);
	
    std::ofstream outfile2;
    outfile2.open(file2.c_str(), std::ofstream::binary);
    
    std::ofstream outfile4;
    outfile4.open(file4.c_str(), std::ofstream::binary);

    std::ofstream outfile5;
    outfile5.open(file5.c_str(), std::ofstream::binary);

    ////////////////////////////////////////////////////////////////////////////////

    // THE ACTUAL STREAMING AND WRITING IS DONE HERE
    uint64_t rx_idx = 0;
    int ts_n = 0;
    double tick_rate = 1e8;
    long long t_stamp;
    int full_s;
    double frac_s;

    while(num_acc_samps < total_num_samps){
        std::cout << "Got here 4 " << std::endl;
    	size_t num_rx_samps = rx_stream->recv(buff_ptrs, samps_per_buff, md, timeout);
        std::cout << num_rx_samps << std::endl;
    	for(size_t n=0; n< num_rx_samps; n++){
            rx_buff[0][rx_idx] = buffs[0][n];
    	    rx_buff[1][rx_idx] = buffs[1][n];
            rx_idx++;
            std::cout << buffs[0][n] << " ";
            //std::cout << buffs[1][n] << " ";
	}
        std::cout << std::endl;
        std::cout << "num_rx_samps: " << num_rx_samps << std::endl;
        std::cout << "rx_idx:  " << rx_idx << std::endl;
        
        t_stamp = md.time_spec.get_tick_count(tick_rate);
        full_s = md.time_spec.get_full_secs();
        frac_s = md.time_spec.get_frac_secs();
        
        t_stamps[ts_n] = t_stamp;
	full[ts_n] = full_s;
        frac[ts_n] = frac_s;
        ts_n++;	
        
        // use small timeout for subsequent packets
        timeout=0.1;
	
	// ERROR HANDLING
	if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){ 
	    std::cout << boost::format("Timeout while streaming") << std::endl;
	    break;
	}
		
	if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE){
	    throw std::runtime_error(str(boost::format("Reciever Error %s")%md.strerror()));
	}
	
        // SOME ADDITIONAL INFO YOU MIGHT BE INTERESTED TO SEE ON THE COMMAND LINE
        
        std::cout << frac_s << std::endl;

        /* 
        std::cout <<boost::format("Received packet: %u samples, %u full secs, %d frac secs") 
	    %num_rx_samps % md.time_spec.get_full_secs() 
	    %md.time_spec.get_frac_secs() << std::endl;
        
        std::cout <<boost::format("N ticks: %lld")  
            %md.time_spec.get_tick_count(tick_rate) << std::endl;
	
       	*/	
	num_acc_samps += num_rx_samps;
        //std::cout << "Samps accumulated so far:" << std::endl; 
        //std::cout << num_acc_samps << std::endl;
        
    }

    if (num_acc_samps<total_num_samps) std::cerr << "Recieve timeout before"
       	    << "all samples received..." << std::endl;
    
    if (outfile1.is_open()){
        outfile1.write((const char*) &rx_buff[0].front(),
                total_num_samps*sizeof(float));
        outfile1.close();
    }	
    if (outfile2.is_open()){
    	outfile2.write((const char*) &rx_buff[1].front(),
                total_num_samps*sizeof(float));
        outfile2.close();
    }

    // Write time stamps into binary file
    if (outfile4.is_open()){
        outfile4.write((const char*) &full.front(),n_packets*sizeof(int));
        outfile4.close();
    }
    
    if (outfile5.is_open()){
        outfile5.write((const char*) &frac.front(), n_packets*sizeof(double));
        outfile5.close();
    }
    
    //finished
    std::cout<< "Done!" <<std::endl;
   
    return EXIT_SUCCESS;
}
