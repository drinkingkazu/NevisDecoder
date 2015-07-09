/**
 * \file DecoderConstants.h
 *
 * \ingroup Base
 * 
 * \brief defines Decoder related constants
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup Base

    @{*/

#ifndef NEVISDECODER_DECODERCONSTANTS_H
#define NEVISDECODER_DECODERCONSTANTS_H
#include <string>
#include <limits>
#include <climits>
/// Namespace of everything in this framework
namespace decoder{

  const UInt_t   kINVALID_WORD=std::numeric_limits<unsigned int>::max();
  const UInt_t   kEMPTY_WORD=0x0;                  ///< Empty word

  /// Defines PMT word-type
  enum FEMWord_t{
    kUNDEFINED_WORD = 0, ///< Word type ... undefined
    kEVENT_HEADER,       ///< A signal word (no data) to flag the start of headers
    kFEM_HEADER,         ///< Event header word
    kFEM_FIRST_WORD,     ///< First word in the event (after the event header, before channel data )
    kCHANNEL_HEADER,     ///< Channel header word
    kCHANNEL_WORD,       ///< Channel word such as ADC sample
    kCHANNEL_LAST_WORD,  ///< Last word per channel-wise readout
    kFEM_LAST_WORD,      ///< Last word per fem
    kEVENT_LAST_WORD,    ///< Last word in the event
    kCHANNEL_TIME,       ///< Channel time word
    kCHANNEL_PACKET_LAST_WORD  ///< Channel: last word of the packet 
  };
  
  /// Defines input file format for decoder (i.e. output of electronics)
  enum InputFile_t {
    /// Bin format for decoder (input) and coder (output)   
    kBINARY=0,
    /// ASCII format for (de)coder
    kASCII,
    /// Undefined...
    kUNDEFINED_FILE_FORMAT
  };
}
#endif
/** @} */ // end of doxygen group
