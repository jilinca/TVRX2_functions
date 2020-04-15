

General notes
    
    Code for operating USRP N210 Software defined radios for simultaneous signal retreival for
    angle of arrival calculations with a minimal hardware setup.

    All the code for both the data processing and the data retrieval has been written in C++
    keeping real time operaton in mind. The code needed to find the source angle is divided 
    into two cxx files and one header file. TVRX2_buff.cpp (for 2 channel retreival) or 
    TVRX4_buff.cpp (for 4 channel retreival) and plotTVRX for calculating the source direction
    and producing .txt files of the results which can then easily be plotted with for example
    gnuplot or Matlab. 

    If encoutering problems the C API documentation and the USRP mailing list are your best
    friends.
    
    Dependencies: UHD-library
    Installation recomendations: For Ubuntu systems use PyBOMBS and for Arch systems follow the
    source installation instructons on the UHD manual page. 
    
    Organization of files:
    Cxx files for receiver implementation: usrp_cpp/RX/
    Cxx files for transmitter implementation: usrp_cpp/TX/
    Documentation and other supporting files: usrp_cpp/doc/
    Cxx files for dataprocessing and algorithms: usrp_cpp/plots/
    Data and time stamps are saved in usrp_cpp/TestData/
    Text files with relevant data that can be plotted /usrp_cpp/gplot/

/////////////////////////////////////////// HARDWARE ///////////////////////////////////////////////


About the devices:
    Usrp N210: The ADC conversion rate is fixed to 100MHZ this is the frequency used before the 
    down sampling

    TVRX2 daughter-board: This is the daughterboard connected to the USRP and it can be changed 
    by removing the USRP casing. Two daughterboards are required for simultaneous 4 
    channel data aquisition.
    
    Receiving with 2 channels: One USRP N210 device and one TVRX2 DB. 

    Receiving with 4 channels: 2 USRP N210 devices equipped with a TVRX2 DB are required. The N210s
    need to be connected to each other via mimo cable.

    USRP B210: Used as a transmitter


////////////////////////////////////////////// CODE /////////////////////////////////////////////////

constants.h
    Constants and variables that can be changed to modify the operation of the USRP device.
    More information can be found in the header file itself.


TVRX2_buff.cpp
    
    This function acts as the main function and other functions are called from it.
    It handels the RX-buffers and creation of relevant data files and transmission of the 
    measuremnts to the host.
    
    The parameters are hard coded but can be changed from the source any time.e
    IMPORTANT! If the center frequency is changed re-calibriation is required as random phase shift 
    To change the settings for the center frequency, sample_rate etc modify the file constants.

TVRX4_buff.cpp

    The same as TVRX2_buff.cpp except it is meant to be used when measuring with 4 channels instead of 2.


get_time.cpp
    
    This function creates a vector of time values spanning one RX measuremnt. This vector becomes availeble
    to the calling function for further processing. 
    
    The  USRP delivers a time-stamp only each time the contents of the buffer is sent to the host, and 
    does not time-stamp each individual sample. The 'rate' parameter defines at what rate the filled 
    buffer packets are sent to the host. The time stamps of consecutive sample packets are used to calculate
    the time for the individual samples as the number of samples per packet is known and can be defined
    in constants.h

onesig.cpp
    call to generate and send one self generated signal


wavetable.cpp
    code needed to generate a userdefined waveform. Change the parameters in this file to create your
    own complex waveforms. Similar to the orginal uhd file, but modified to support arbitrary waveforms

genWF.cpp
    fuinctions called by onesig.cpp to generate waveform



   


/////////////////////////////////////// CALIBRATION AND TESTING ////////////////////////////////////////
    
    Several cxx files have been written for testing. They can be found in the in the folder 
    .../usrp_cpp/TX/

    These files contain an extension to the UHD library for generating and transmitting a user defined
    waveforms. This code has been tested on the B210 device



///////////////////////////////////////////// ALGORITHMS ////////////////////////////////////////////////

    Further information on the spectral estimation on random processes that might need to be implemented
    to improve the accuracy and functionality of the algorithm can be found in the works of Nutall and 
    Carter.

        A Generalzed framework for power spectral estimation, IEEE Trans Acoust, Speech, Signal Processing,
        vol ASSP-23 no. 3 pp 334-335, June 1980

        Spectral estimatation using combined time and lag



