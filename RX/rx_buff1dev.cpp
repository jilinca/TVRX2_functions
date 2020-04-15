#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/device.hpp>
#include <uhd/stream.hpp>
#include <dualusrp.h>
#include <fstream>
#include <vector>
#include <uhd/usrp/fe_connection.hpp>



// The includes below are only needed for the stuf below the current main
// used for retriveing time stamps. (They should not be needed anymore
// since another method is being used now)
//#include <gnuradio/tags.h>
//#include <gnuradio/block_detail.h>
//#include <gnuradio/buffer.h>
// Used for getting and reading metadata
//#include <gnuradio/blocks/file_meta_sink.h>
//#include <gnuradio/blocks/file_meta_source.h>
//#include <file_meta_source_impl.h>
// Used for manipulating metadat files
//#include <uhd/types/omponent_file.hpp>	
//#include <pmt/pmt.h> 			
//#include <pmt/api.h>			


// Test getting timestamps
int UHD_SAFE_MAIN(int argc, char *argv[]){
    
    uhd::set_thread_priority_safe();
    
    // Variables for devices
    std::string ant_ch0("RX2");
    std::string ant_ch1("RX2");
    std::string ref0("internal");
    std::string ref1("mimo");
    std::string time("mimo");


    // sub_dev parameters for channel mapping
    std::string subdev0("A:0"); // might need to change to A:one
    std::string subdev1("A:0"); // use subdev(A:RX2) if using TVRX2 daughter board
	
    // Common variables
    double freq(450e6);
    double gain(10);
    double gainsbx(10);
    //double gaintvrx(5);
    double bw(2e6);
    double rate2(1e7); // for data transmission rate from USRP to host
    //double lo_offset(105e6);
    	
    // Motherboard indices for setting sources
    const size_t mb1 = 1;
    const size_t mb0 = 0;
	
    // Addressing and defining the usrp devices
    uhd::device_addr_t addresses("addr0=192.168.10.2, addr1=192.168.10.3");
    uhd::usrp::multi_usrp::sptr usrp =uhd::usrp::multi_usrp::make(addresses);
	
    // Detect channel configuratons
    std::vector<size_t> channel_nums;
    size_t n_chan = usrp->get_rx_num_channels();
    if (n_chan != 2){
    	throw std::runtime_error("Invalid channels specified, the USRP device must have two channels");
	return ~0;
    }else{
    	channel_nums.push_back(0);
    	channel_nums.push_back(1);
    }
    std::cout << "N channels: " << n_chan << std::endl;

    // Setup Subdevice specification:
    // The subdev spec maps a physical part of the daughter board to a channel number
    // Here we create two channels on two separate mboards on two separate usrp devices
    usrp->set_rx_subdev_spec(subdev0,channel_nums[0]); //sets across all channels on mboard 0
    usrp->set_rx_subdev_spec(subdev1,channel_nums[1]); //sets across all channels on mboard 1

		
    // Set rx rate (rate at which the data is 
    //transferred between the host and the device)
    usrp->set_rx_rate(rate2,channel_nums[0]); // sets across all channels
    usrp->set_rx_rate(rate2,channel_nums[1]);
	
    // Sleep to lock local oscillators
    sleep(1);
	
    // master clock rate/desired_sample_rate =even integer number
    // carrier frequency = LO frequency - DSP frequency
	

    // Throw error if exactly two mother boards aren't connected
    UHD_ASSERT_THROW(usrp->get_num_mboards() == 2);
	
    // Set clock source for motherboards separately
    usrp->set_clock_source(ref0, mb0);
    usrp->set_clock_source(ref1, mb1);

    // Set time on master mboard  \should have master mboard number
    usrp->set_time_now(uhd::time_spec_t(0,0),mb0); 
	
    // configuring the slave over mimo cable	
    usrp->set_time_source(ref1, mb1);
	
    // Sleep a while so time can be properly locked
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	
    // Set Gain
    usrp->set_rx_gain(gain,channel_nums[0]);
    usrp->set_rx_gain(gainsbx,channel_nums[1]);
    //usrp->set_rx_gain(gaintvrx,channel_nums[1]);

	
    for (size_t ch =0; ch < channel_nums.size(); ch++){
		
    	// Set center frequncy
    	//uhd::tune_request_t tune_request(freq,lo_offset);
    	uhd::tune_request_t tune_request(freq);
    	usrp->set_rx_freq(tune_request,channel_nums[ch]);
    					
	// Set Bandwidth
	usrp->set_rx_bandwidth(bw,channel_nums[ch]);
		
	// Set antenna to recieve only
	usrp->set_rx_antenna(ant_ch1,channel_nums[ch]);
    }
	

    // ------------------------- PERFORMING CHECKS -------------------------
    pref_checks (mb0, mb1,channel_nums, usrp); 	
    ////////////////////////////////////////////////////////////////////////
    

    /*
    // -------------------------- NEGATE Q CHANNEL--------------------------
    
    // Set sampling type to REAL and third parameter to 1. See other options
    // from uhd manual for fe_connection_t
    double if_freq = 0.0;
    uhd::usrp::fe_connection_t fe_con(uhd::usrp::fe_connection_t::sampling_t::REAL,0,0,1,if_freq);
    
    // Check if the sampling mode is set correctly
    uhd::usrp::fe_connection_t::sampling_t smp_m = fe_con.get_sampling_mode();

    switch(smp_m){
        case '0':
            std::cout << "The samplying type has been set to QUADRATURE." << std::endl;
            break;
        case '1':
            std::cout << "The samplying type has been set to HETERODYNE." << std::endl;
            break;
        case '2':
            std::cout << "The samplying type has been set to REAL." << std::endl;
            break;
    }

    */
	
    //------------------- SETUP STREAMING PARAMETERS-------------------
	
    //Set initial time stamp to zero
    std::cout << boost::format("Setting device timestamp to zero... ") << std::endl;
    usrp->set_time_now(uhd::time_spec_t(0.0));


    uhd::stream_args_t stream_args("fc32","sc16"); // probably needs to be changed to sc8 if
    // alternate sampling is implemented with all 4 channels
    // Linearly map channels
    stream_args.channels = channel_nums;
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
	
    double seconds_in_future = 1.5;
    double total_num_samps = 100000;
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
    const size_t samps_per_buff = 1;// rx_stream->get_max_num_samps();	
    
    // this was added here 9.5.2019
    // a paramter of interest
    std::cout << "Samples per one buffer \n";
    std::cout << boost::format("%d") %samps_per_buff << std::endl;

    // 2D buffer vector
    std::vector<std::vector<std::complex<float> > > buffs(
    	usrp->get_rx_num_channels(), std::vector<std::complex<float> > (samps_per_buff));

    // Vector of pointers to point to each of the channel buffers
    std::vector<std::complex<float>*> buff_ptrs;
    for (size_t i=0; i<buffs.size();i++) buff_ptrs.push_back(&buffs[i].front());
    
    //vector for storing in all the buffer data as it comes in (i wonder if i could use
    //the buffer only without having this indermediate step. Currently getting something 
    //wierd if I try it.
    std::vector<std::vector<std::complex<float> > > rx_buff(
    	usrp->get_rx_num_channels(), std::vector<std::complex<float> >(total_num_samps));
    
    // VECTORS FOR TIMESTAMPS
    // Time values in this vector will indicate at what time the last sample was received
    int n_packets = (int) ceil(total_num_samps/samps_per_buff);
    //ticks
    std::vector<long long> t_stamps(n_packets);
    //full seconds
    std::vector<int> full(total_num_samps);
    //fractional seconds    
    std::vector<double> frac(total_num_samps);
    
    

    uint64_t rx_idx = 0;

   

    ///////////////////// CREATION OF FILES TO STREAM IN ///////////////////////////
    // The file names
    std::string file1 = "/home/navi/usrp_cpp/plots/TestData/ch1_d2.dat";
    std::string file2 = "/home/navi/usrp_cpp/plots/TestData/ch2_d2.dat";
    std::string file3 = "/home/navi/usrp_cpp/plots/TestData/meta_d.txt";
    std::string file4 = "/home/navi/usrp_cpp/plots/TestData/full.dat";
    std::string file5 = "/home/navi/usrp_cpp/plots/TestData/frac.dat";
    
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
    int ts_n = 0;
    double tick_rate = 1e8;
    while(num_acc_samps < total_num_samps){
    	size_t num_rx_samps = rx_stream->recv(buff_ptrs, samps_per_buff, md, timeout);
       
        
    	for(size_t n=0; n< num_rx_samps; n++){
            rx_buff[0][rx_idx] = buffs[0][n];
    	    rx_buff[1][rx_idx] = buffs[1][n];
            rx_idx +=1;
	}
        
        long long t_stamp = md.time_spec.get_tick_count(tick_rate);
        int full_s = md.time_spec.get_full_secs();
        double frac_s = md.time_spec.get_frac_secs();

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
	 
        std::cout <<boost::format("Received packet: %u samples, %u full secs, %d frac secs") 
	    %num_rx_samps % md.time_spec.get_full_secs() 
	    %md.time_spec.get_frac_secs() << std::endl;
        	
	
        
        //std::cout <<boost::format("N ticks: %lld")  
        //    %md.time_spec.get_tick_count(tick_rate) << std::endl;
	

        /*
	if (outfile1.is_open())
	    outfile1.write((const char*) &rx_buff[0].front(),
                    total_num_samps*sizeof(std::complex<float>));
		
	if (outfile2.is_open())
	    outfile2.write((const char*) &rx_buff[1].front(),
                    total_num_samps*sizeof(std::complex<float>));
        */
		num_acc_samps += num_rx_samps;       
    }

    if (num_acc_samps<total_num_samps) std::cerr << "Recieve timeout before"
       	    << "all samples received..." << std::endl;
    
    // Write time stamps into file .txt file (Not working quite as expected, however code
    // compiles and executes without errors)
    //if (outfile3.is_open()){
        // When intializing the file buffer
        // the ofstream class creates a ostream class object which in turn creates a char file buffer
    //    outfile3.write((const char*) &t_stamps.front(), n_packets*sizeof(long long));
    //    outfile3.close();
    //}
    
    // Write time stamps into binary file
    //if (outfile4.is_open()){
    //    outfile4.write((const char*) &t_stamps.front(), n_packets*sizeof(long long));
    //    outfile4.close();
    //}
    
    if (outfile1.is_open())
        outfile1.write((const char*) &rx_buff[0].front(),
                total_num_samps*sizeof(std::complex<float>));
		
    if (outfile2.is_open())
	outfile2.write((const char*) &rx_buff[1].front(),
                total_num_samps*sizeof(std::complex<float>));
   
    rx_buff[0].clear();
    rx_buff[1].clear();
    rx_buff.clear();
    // Write time stamps into binary file
    if (outfile4.is_open()){
        outfile4.write((const char*) &full.front(), total_num_samps*sizeof(int));
        outfile4.close();
    }
    
    if (outfile5.is_open()){
        outfile5.write((const char*) &frac.front(), total_num_samps*sizeof(double));
        outfile5.close();
    }

    // close files
    if(outfile1.is_open())
        outfile1.close();

    if(outfile2.is_open())
        outfile2.close();

    //finished
    std::cout<< "Done!" <<std::endl;

    return EXIT_SUCCESS;
}	
