#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/device.hpp>
#include <uhd/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <fstream>
#include <vector>

static bool stop_signal_called =false;
void sig_int_handler(int){stop_signal_called = true;}

int UHD_SAFE_MAIN(int argc, char *argv[]){ 

	uhd::set_thread_priority_safe();
	// -------------------------- PARAMETERS ------------------------------
	// Variables for devices
	std::string ant("TX/RX");
	//std::string ant("RX2");
	std::string ref("internal");
	std::string time("internal");	
	std::string subdev("A:0");
	

	// Variables
	double rate(1e6); // for ACD master clock (this setting is fixed for the N210)
	double freq(102.8e6);
	double gain(10);
	double bw(1e6);
	double rate2(1e7); // for data transmission rate from USRP to host
	double time_requested = 1;
	unsigned long long num_requested_samples = 200000;
	unsigned long long num_total_samps =0;
	const double timeout = 0.5;
	size_t samps_per_buff = 10000;	
	
	// Addressing and defining the usrp  the Devices
	uhd::device_addr_t addresses("addr=192.168.10.2");
	uhd::usrp::multi_usrp::sptr usrp =uhd::usrp::multi_usrp::make(addresses);
	usrp->set_rx_subdev_spec(subdev); 
	usrp->set_rx_rate(rate2); 
		
	usrp->set_clock_source(ref);	
	
	// Set center frequncy
	uhd::tune_request_t tune_request(freq);
	usrp->set_rx_freq(tune_request);

	// Set Gain
	usrp->set_rx_gain(gain);

	// Set Bandwidth
	usrp->set_rx_bandwidth(bw);
	
	// Set antenna to recieve only
	usrp->set_rx_antenna(ant);
	

	//-------------- GET AND WRITE DATA STREAM TO FILE ----------------
	
	// SETUP STREAMING PARAMETERS
	uhd::stream_args_t stream_args("fc32","sc16");	
	uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
	
	// 1D buffer vector	
	uhd::rx_metadata_t md; 
	std::vector<std::complex<float> > buffs(samps_per_buff);
	
	// Open and create file
	std::string file = "onesig.dat";
	std::ofstream outfile;
	outfile.open(file.c_str(), std::ofstream::binary);
	bool overflow_message =true;

	// Begin streaming
	uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
	stream_cmd.num_samps = num_requested_samples;
	stream_cmd.stream_now = true;
	stream_cmd.time_spec = uhd::time_spec_t();
	rx_stream->issue_stream_cmd(stream_cmd);
	
	boost::system_time start = boost::get_system_time();
       	unsigned long long ticks_requested = (long)(time_requested *
			(double)boost::posix_time::time_duration::ticks_per_second());	
	boost::posix_time::time_duration ticks_diff;
	boost::system_time last_update = start;
	unsigned long long last_update_samps = 0;
	
	
	while(not stop_signal_called and 
			(num_requested_samples != num_total_samps or num_requested_samples==0)){

		boost::system_time now = boost::get_system_time();
		
		size_t num_rx_samps = rx_stream->recv(&buffs.front(), buffs.size(), md, timeout);

		num_total_samps += num_rx_samps;

		
		// ERROR HANDLING
		if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){ 
			std::cout << boost::format("Timeout while streaming") << std::endl;
			break;
		}

		if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE){
			throw std::runtime_error(str(boost::format("Reciever Error %s")
				%md.strerror()));
		}
		
		std::cout <<boost::format(
		    	"Received packet: %u samples, %u full secs, %f frac secs") 
			%num_rx_samps % md.time_spec.get_full_secs() 
			%md.time_spec.get_frac_secs() << std::endl;
		
		
		// Write to file
		if (outfile.is_open()){
			outfile.write((const char*)&buffs.front(),
				       num_rx_samps*sizeof(std::complex<float>));
		}

		ticks_diff = now - start;
		if (ticks_requested > 0){
			if ((unsigned long long)ticks_diff.ticks() > ticks_requested)
				break;
		}
	}

	stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
	rx_stream->issue_stream_cmd(stream_cmd);
	

	if (num_total_samps<num_requested_samples) std::cerr << "Recieve timeout before"
         	<< "all samples received..." << std::endl;
	
	// close files
	if(outfile.is_open())
		outfile.close();

	//finished
	std::cout<<std::endl << "Done!" <<std::endl<<std::endl;

	return EXIT_SUCCESS;
}

