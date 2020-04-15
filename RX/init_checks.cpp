#include <dualusrp.h>


// Performs checks for USRP device parameters
void pref_checks(const size_t mb0, const size_t mb1,std::vector<size_t> channel_nums, uhd::usrp::multi_usrp::sptr usrp){
	
	// Summary of configuration
	std::string summary = usrp->get_pp_string();
	std::cout << boost::format("Using Devices: %s") %summary << std::endl;	
	
	//Check master clock rate from 
	double mcr0 = usrp->get_master_clock_rate(1);
      	double mcr1 = usrp->get_master_clock_rate(0);	
	
	std::cout<< std::endl;
	std::cout<<boost::format("Master device connected. The master clock rate is %d")
	       	%mcr0 << std::endl;
	std::cout<<boost::format("Slave device connected. The master clock rate is %d")
	       	%mcr1 << std::endl << std::endl;

	// Check clock source
	std::string mcs = usrp->get_clock_source(mb0);
	std::string scs = usrp->get_clock_source(mb1);
	std::cout<<boost::format("The Master clock source is: %s") %mcs << std::endl;
	std::cout<<boost::format("The Slave clock source is: %s") %scs <<std::endl;
	std::cout<< std::endl;

	for (size_t ch =0; ch < channel_nums.size(); ch++){
	// Check gain
		std::cout<<boost::format("The gain on channel %d  has been set to: %d")
		       	%ch %usrp->get_rx_gain(ch) <<std::endl;
	
	
	// Check center frequency
		std::cout<<boost::format("The center freq on channel %d has been set to: %d")
		       	%ch %usrp->get_rx_freq(ch) <<std::endl;
	
	// Check Bandwidth
		std::cout<<boost::format("The bandwidth for channel %d is: %d")
	       		%ch %usrp->get_rx_bandwidth(ch) <<std::endl;
	
	// Check the correct antenna port is being used	
		std::cout<<boost::format("The antenna on channel %d is: %s")
	       		%ch %usrp->get_rx_antenna(ch) << std::endl;

	// Check what the difference is to the master clock rate
		std::cout<<boost::format("The RX communication rate (ch %d): %d")
	       		%ch %usrp->get_rx_rate(ch) <<std::endl;
	
	// Get the master clock rate devining the DA speed
		std::cout<<boost::format("master Clock Rate (ch %d): %d") 
	       	%ch %usrp->get_master_clock_rate(ch) <<std::endl << std::endl;
	}
	// Get the current clock source
	std::string c_source = usrp->get_clock_source(mb0);
	std::cout << boost::format("The current clock source is: %s")  %c_source << std::endl;
	// Get the current clock source for mb1
	std::string c_source1 = usrp->get_clock_source(mb1);
	std::cout << boost::format("The current clock source is: %s")  %c_source1  << std::endl;

	
	//DONE
	std::cout << "Checking complete" << std::endl << std::endl;
}
