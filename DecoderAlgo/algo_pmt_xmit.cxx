#ifndef NEVISDECODER_ALGO_PMT_XMIT_CXX
#define NEVISDECODER_ALGO_PMT_XMIT_CXX

#include "algo_pmt_xmit.h"

namespace decoder {

  //#########################################
  algo_pmt_xmit::algo_pmt_xmit() 
    : algo_fem_decoder_base(),
      _ch_data()
  {
  //#########################################

    reset();

  }

  //#########################################
  void algo_pmt_xmit::reset() {
  //#########################################

    _event_data = 0;

    _check_fifo_overflow = false;

    clear_event();

    algo_fem_decoder_base::reset();

  }


  //#########################################
  void algo_pmt_xmit::clear_event() {
  //#########################################

    algo_fem_decoder_base::clear_event();

    if(_event_data)

      _event_data->clear_data();

    _ch_data.clear_data();

    _channel_header_count = 0;
    _nword_within_ch = 0;

  }

  //#########################################
  bool algo_pmt_xmit::process_ch_word(const UInt_t word, 
				      UInt_t &last_word) 
  {
  //#########################################

    bool status=true;

    if(_verbosity[::larlite::msg::kDEBUG]){
      sprintf(_buf,"Processing ch-word: %x",word);
      print(::larlite::msg::kDEBUG,__FUNCTION__,_buf);
    }

    status = decode_ch_word(word,last_word);

    if(status){
      _nwords++;
      _checksum+=word;
    }else{
      std::string msg("Failed decoding channel data...\n");
      /*
      if(!_event_data || !(_event_data->size()))
	msg += "    Channel data not found so far in this event...\n";
      else{
      auto const& ch_data = _event_data->back();
      */
      msg +=      "    Last found channel info shown below.\n";
      msg += Form("    Module   : %d\n",(int)(_header_info.module_address));
      msg += Form("    Channel  : %d\n",_ch_data.channel_number());
      msg += Form("    Disc. ID : %d\n",(int)(_ch_data.disc_id()));
      msg += Form("    Frame    : %d\n",_ch_data.readout_frame_number());
      msg += Form("    Sample   : %d\n",_ch_data.readout_sample_number_RAW());
      msg += Form("    Stored # Words  : %zu\n",_ch_data.size());
      
      print(::larlite::msg::kERROR,__FUNCTION__,msg.c_str());

    }

    last_word = word;


    return status;
  }

  //#########################################################
  bool algo_pmt_xmit::check_event_quality(){
  //#########################################################

    bool status = true;

    // Check if _checksum and _nwords agrees with that of event header.
    _nwords-=1;
    if(_nwords!=_header_info.nwords){

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Disagreement on nwords: counted=%u, expected=%u",_nwords,_header_info.nwords));

      status = false;
    }

    // checksum is only 24 bit!
    _checksum = (_checksum & 0xffffff);
    if(_checksum!=_header_info.checksum){

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Disagreement on checksum: summed=%x, expected=%x",_checksum,_header_info.checksum));

      status = false;

    }

    return status;

  }



  //#################################################
  bool algo_pmt_xmit::process_event_header(const UInt_t word, 
					   UInt_t &last_word) 
  {
  //#################################################

    bool status = true;

    if(!_event_data)

      _event_data = (::larlite::event_fifo*)(_storage->get_data(::larlite::data::kFIFO,"pmt_xmit"));

    else if(get_word_class(last_word) != kEVENT_LAST_WORD){

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Unexpected word (%x, previous=%x) while processing event header!",word,last_word));
      
      status = false;
    }

    last_word = word;

    return status;

  }

  //#########################################################
  bool algo_pmt_xmit::process_fem_last_word(const UInt_t word,
					    UInt_t &last_word)
  {
  //#########################################################

    bool status = true;

    if(_verbosity[::larlite::msg::kINFO])

      print(::larlite::msg::kINFO,__FUNCTION__,Form("End of FEM word: %x...",word));

    UInt_t last_word_class = get_word_class(last_word);
    if(last_word_class != kCHANNEL_LAST_WORD &&
       last_word_class != kFEM_FIRST_WORD &&
       !( (_event_data->size() && (_event_data->back().size() & 0x1)) && _last_word==0x8000)) {

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Unexpected word: %x (previous = %x)",word,last_word));
      
      status = false;
      
    }

    _nwords++;
    //_checksum+=word;

    if(status) status=store_event();

    last_word = word;

    return status;
  }

  //#########################################################
  bool algo_pmt_xmit::process_event_last_word(const UInt_t word,
					      UInt_t &last_word)
  {  
  //#########################################################

    bool status = true;

    if(_verbosity[::larlite::msg::kINFO])

      print(::larlite::msg::kINFO,__FUNCTION__,Form("End of event word: %x...",word));

    if(get_word_class(last_word)!=kFEM_LAST_WORD){

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Unexpected word: %x (previous = %x)",word,last_word));

      status = false;

    }

    last_word = word;

    return status;
  }

  //#########################################################
  bool algo_pmt_xmit::store_event(){
  //#########################################################

    bool status = check_event_quality();

    // Store if condition is satisfied
    if(status) {

      if(_verbosity[::larlite::msg::kINFO]){

	print(::larlite::msg::kINFO,__FUNCTION__,
		      Form("Storing event %u with %zu channel entries...",
			   _event_data->event_number(), _event_data->size()));

      }

      //_event_data->set_module_address         ( _header_info.module_address    );
      //_event_data->set_module_id              ( _header_info.module_id         );
      _event_data->set_event_number           ( _header_info.event_number          );
      _event_data->set_event_frame_number     ( _header_info.event_frame_number    );
      _event_data->set_fem_trig_frame_number  ( _header_info.fem_trig_frame_number  );
      _event_data->set_fem_trig_sample_number ( _header_info.fem_trig_sample_number );
      _event_data->set_nwords                 ( _header_info.nwords            );
      _event_data->set_checksum               ( _header_info.checksum          );

      _storage->set_id(0,0,_storage->get_index());

      if(_header_info.module_address == 6) {
	status = _storage->next_event();
	clear_event();
      }
      
    }
    else

      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Skipping to store event %d...",_header_info.event_number));

    //clear_event();

    return status;
  }

  //#########################################################
  bool algo_pmt_xmit::decode_ch_word(const UInt_t word, 
				     UInt_t &last_word)
  {
  //#########################################################

    FEMWord_t word_class      = get_word_class(word);
    FEMWord_t last_word_class = get_word_class(last_word);
    bool status=true;

    if(_verbosity[::larlite::msg::kDEBUG]){
      sprintf(_buf,"Decoding word: %x ... last word class: %d",word,last_word_class);
      print(::larlite::msg::kDEBUG,__FUNCTION__,_buf);
    }  

    switch(word_class){

    case kEVENT_HEADER:
    case kFEM_HEADER:
    case kUNDEFINED_WORD:
    case kFEM_LAST_WORD:
    case kEVENT_LAST_WORD:
      print(::larlite::msg::kERROR,__FUNCTION__,
		    Form("Unexpected word: %x (previous=%x)",word,last_word));
      status = false;
      break;
    case kFEM_FIRST_WORD:
      //
      // First channel word in this event ... nothing special to do
      if(last_word_class!=kFEM_HEADER){
	print(::larlite::msg::kERROR,__FUNCTION__,
		      Form("Found FEM first word (%x) in an unexpected place (previous=%x)!",word,last_word));
	status=false;
      }
      break;

    case kCHANNEL_HEADER:
      //
      // Channel header ... read in channel info from this word
      if(last_word_class!=kFEM_FIRST_WORD && 
	 last_word_class!=kCHANNEL_LAST_WORD &&
	 !( (_event_data->size() && (_event_data->back().size() & 0x1)) && _last_word==0x8000) &&
	 _last_word != 0x8000) {
	print(::larlite::msg::kERROR,__FUNCTION__,
		      Form("Found channel header (%x) in an unexpected place (previous=%x)!",word,last_word));
	status=false;
      }else{
	// Initialize channel info, then fill with channel number & disc. id.
	_channel_header_count=0;
	_ch_data.clear_data();
	_channel_header_count=1;
	// Channel Number
	_ch_data.set_channel_number( word & 0x3f ); 
	// Discriminator ID
	_ch_data.set_disc_id((::larlite::fem::Discriminator_t)((word & 0xfff)>>9));  
      }
      break;

    case kCHANNEL_WORD:
    case kCHANNEL_LAST_WORD:
      //
      // This is channel ADC count or possibly remaining channel header info
      // The first two sample are treated as channel header info
      if(last_word_class!=kCHANNEL_HEADER && 
	 last_word_class!=kCHANNEL_WORD &&
	 last_word_class!=kCHANNEL_LAST_WORD ) {
	// Sanity check: the last word should be either channel header or data word.
	print(::larlite::msg::kERROR,__FUNCTION__,
		      Form("Found channel word (%x) in an unexpected place (previous=%x)!",word,last_word));

	status=false;
      }else{

	// Treat a case of missing very first channel header word: happens 
	// when there is no time in between two readout data.
	if(last_word_class==kCHANNEL_LAST_WORD){
	  // Two possible cases: 
	  //    (1) previous waveform had odd # of ADC samples ... then this should be 0x8000
	  //    (2) two readout windows are combined
	  
	  //case (1)
	  if( word == 0x8000 ) {

	    if( ((*_event_data->rbegin()).size())%2 ) return true;

	    else { 
	      //print(::larlite::msg::kERROR,__FUNCTION__,
	      print(::larlite::msg::kWARNING,__FUNCTION__,
		    Form("Found a padding 0x8000 in an unexpected place (previous readout was even (%zu)!)...",
			 (*_event_data->rbegin()).size()));
	      //status = false;
	      return true;
	    }
	  }else{
	    // In this case, we should be missing the CHANNEL_HEADER because this pulse is from 
	    // the same channel & discriminator id. Do an operation that is done for the case of 
	    // CHANNEL_HEADER, but use a stored value of channel number & disc. id.
	    _channel_header_count=1;
	    _ch_data.set_channel_number( (*_event_data->rbegin()).channel_number() );
	    _ch_data.set_disc_id( (*_event_data->rbegin()).disc_id() );
	    if(_verbosity[::larlite::msg::kNORMAL])
	      print(::larlite::msg::kNORMAL,__FUNCTION__,
		    Form("Found consecutively readout data arrays @ event %d ch %d (%zu samples) (missing channel very first header)!",
			 _header_info.event_number,
			 _ch_data.channel_number(),
			 (*_event_data->rbegin()).size()
			 )
		    );
	  }
	}

	//
	// Ordinary operation for channel header / adc words
	//
	if(_channel_header_count==CHANNEL_HEADER_COUNT ) {

	  // Finished reading in header words. This should be a mere ADC count.
	  // Raise error if the last word class is not channel word
	  if(last_word_class!=kCHANNEL_WORD){

	    print(::larlite::msg::kERROR,__FUNCTION__,
			  Form("Found channel word (%x) in an unexpected place (previous=%x)!",word,last_word));

	    status = false;
	  }	  
	  else if(word_class!=kCHANNEL_LAST_WORD)

	    _ch_data.push_back(word & 0xfff);	
	  
	  ++_nword_within_ch;

	}else if(last_word_class==kCHANNEL_HEADER   ) {
	  // First of 2 channel header words. Record the values.

	  // This gives upper 8 bits of 20-bit readout_sample_number number
	  _ch_data.set_readout_sample_number( (word & 0x1f)<<12 );

	  // Correct channel frame id roll-over w.r.t. event frame id
	  //std::cout<<"Checking roll over for 3 bits..."<<std::endl;
	  if(_header_info.event_frame_number == kINVALID_WORD) {
	    print(::larlite::msg::kERROR,__FUNCTION__,Form("Invalid event frame number: %d",kINVALID_WORD));
	    return false;
	  }
	  
	  // Using 3-bit version
	  _ch_data.set_readout_frame_number(roll_over(_header_info.event_frame_number,
						      ((word & 0xff)>>5),
						      3)
	  				    );

	  // Using 4-bit version (no roll-over umbiguity so just replace 4 bits
	  //_ch_data.set_readout_frame_number( (((_header_info.event_frame_number)>>4)<<4) + ((word & 0x1ff)>>5));

	  // Check if the frame is -1 to +2 w.r.t. event frame number
	  int diff = ( ((int)(_ch_data.readout_frame_number())) - ((int)(_header_info.event_frame_number)) );
	  if(diff < 0 || diff > 3) {
	    print(::larlite::msg::kWARNING,__FUNCTION__,Form("Found event frame %d and discriminator frame %d (difference too big!)",
						 _header_info.event_frame_number,
						 _ch_data.readout_frame_number()));
	  }

	  ++_channel_header_count; 
	  ++_nword_within_ch;

	}else if(last_word_class==kCHANNEL_WORD) {
	  // Second of 2 channel header words. Record the values & inspect them.
	  // This gives lower 12 bits of 20-bit readout_sample_number number
	  _ch_data.set_readout_sample_number(_ch_data.readout_sample_number_64MHz() + (word & 0xfff));

	  _channel_header_count++;
	  ++_nword_within_ch;
	  if(_verbosity[::larlite::msg::kINFO]){
	    sprintf(_buf,"Read-in headers for Ch. %-3d!",_ch_data.channel_number());
	    print(::larlite::msg::kINFO,_buf);
	  }
	}

	// If this is channel's last word, store & clear channel info
	if(word_class==kCHANNEL_LAST_WORD){
	  if(_verbosity[::larlite::msg::kINFO]){
	    print(::larlite::msg::kINFO,Form("Encountered the last word (%x) for channel %d",word,_ch_data.channel_number()));
	    print(::larlite::msg::kINFO,Form("Event frame  : %d",_header_info.event_frame_number));
	    print(::larlite::msg::kINFO,Form("PMT frame    : %d",_ch_data.readout_frame_number()));
	    print(::larlite::msg::kINFO,Form("Disc. ID     : %d",_ch_data.disc_id()));
	    print(::larlite::msg::kINFO,Form("Start Time   : %d",_ch_data.readout_sample_number_64MHz()));
	    print(::larlite::msg::kINFO,Form("# ADC sample : %zd",_ch_data.size()));
	  }
	  store_ch_data();
	}
	// else check fifo overflow
	else if( _check_fifo_overflow && _nword_within_ch &&
		 ((_nword_within_ch % 8) == 0 && ( word & 0xf000 ) != 0x8000) ) {
	  print(::larlite::msg::kERROR,"Bit marker 0x8xxx not found at expected position (possible FIFO overflow)!");
	  return false;
	}
      }
      break;
    default:
      print(::larlite::msg::kERROR,__FUNCTION__,Form("Unexpected word type: %d",word_class));
      status = false;
    }

    return status;

  }

  void algo_pmt_xmit::store_ch_data(){

    // Save
    _ch_data.set_module_id(_header_info.module_id);
    _ch_data.set_module_address(_header_info.module_address);
    if(_ch_data.size())
      _event_data->push_back(_ch_data);

    // Clear
    _channel_header_count=0;
    _nword_within_ch=0;
    _ch_data.clear_data();

  }

}


#endif
