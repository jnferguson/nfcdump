#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdexcept>

#include "async.hpp"
#include "hexdump.hpp"
#include "cmd.hpp"
#include "pm3.hpp"
#include "decoder.hpp"
#include "otfta.hpp"

#include <unistd.h>

#define PROGRAM_NAME "nfcdump"
#define VERSION_STR "v0.0.1"

const std::string options(
		"Usage: " PROGRAM_NAME " " VERSION_STR " [-v][-V][-h|-?]\r\n\r\n"
		"[-v]        	Print program version information and exit\r\n"
		"[-V]        	Print Proxmark3 version information and exit\r\n"
		"[-t]        	Tune Proxmark3 and exit (FIXME)\r\n"
		"[-s] < file >	Sniff NFC traffic and dump to file named < file >\r\n"
		"[-r] < file >	Read and parse NFC traffic from file named < file >\r\n"
		"[-p]			Behave as a NFC PCD (reader)\r\n"
		"[-h|-?]     	Print this message and exit\r\n"
		"\r\n"
);


signed int
main(signed int ac, char** av)
{
	hexdump_t h;
	
	if (2 > ac) {
		std::cerr << options;
		return EXIT_FAILURE;
	}

	try {
		pm3_t* 		pm(nullptr);
		signed int 	opt(0);

		while (-1 != (opt = ::getopt(ac, av, "vVts:r:phm?"))) {
			switch (opt) {
				case 'v':
					std::cout << PROGRAM_NAME << " " << VERSION_STR << std::endl;
					break;

				case 'V':
					pm = new pm3_t;
					std::cout << pm->version() << std::endl;
					break;

				case 't':
					pm = new pm3_t;
					std::cout << pm->tune() << std::endl;
					break;

				case 's':
					pm = new pm3_t;
					std::cout << "Writing dump to " << optarg << "..." << std::endl;
					pm->otfta_raw(std::string(optarg));
					break;
				
				case 'r':
					std::cout << "Reading dump from " << optarg << "..." << std::endl;
					{
						std::vector< decoded_t > 	dvec;
						decoder_t 					decoder(optarg);
						//otfta_parse_t				parser;
						otfta_state_t				fsm(STATE_XMIT_REQWUP);
						//hexdump_t					hex;

						decoder.parse();
						dvec = decoder.get_decoded();

						for (auto rec : dvec) 
							std::cout << decoder.to_string(rec, fsm) << std::endl << std::endl;
							
						/*for (auto rec : dvec) {
							if (1 > rec.demodulated.size()) 
								throw std::runtime_error("Invalid length encountered while parsing demodulated records");

							hex.set(rec.modulated);

							if (DECODED_MILLER_T == rec.type) {
								switch (rec.demodulated.at(0)) {
									case REQA_VALUE:
									case WUPA_VALUE:
										std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										//parser.set_state(STATE_SELC_ONE);
										break;

									case ACCO_VALUE: // 0x93
										
										if (2 > rec.demodulated.size())
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										else if (2 == rec.demodulated.size())  { // 0x93 0x20
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
											parser.set_state(STATE_SELC_ONE_CONTINUE);
										} else if (2 < rec.demodulated.size()) {
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
											parser.set_state(STATE_SELC_TWO);
										}
										break;

									case ACCW_VALUE: // 0x95
										if (2 > rec.demodulated.size())
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										else if (2 == rec.demodulated.size())  { // 0x93 0x20
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
											parser.set_state(STATE_SELC_TWO_CONTINUE);
										} else if (2 < rec.demodulated.size()) {
											std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
											parser.set_state(STATE_SELC_THREE);
										}

										break;
									case ACCT_VALUE: // 0x97
										std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										parser.set_state(STATE_XMIT_RATS);
										break;

									case RATS_VALUE: 
									case HALT_VALUE:
										std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										parser.set_state(STATE_XMIT_REQWUP);
										break;

									default:
										std::cout << "DEFAULT: " << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
										break;
								}

							} else {
								std::cout << parser.to_string(rec.demodulated) << std::endl << std::endl << hex.to_string() << std::endl << std::endl;
							}
						}*/
					}
			
					break;

				case 'p':
					pm = new pm3_t;
					pm->read14a();
					break;

				case 'm':
					pm = new pm3_t;
					pm->msniff();
					break;

				case 'h':
				case '?':
				default:
					std::cerr << options; 
					return EXIT_FAILURE;
					break;
			}
		}

	} catch (std::exception& e) {
		std::cerr << "Fatal exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
