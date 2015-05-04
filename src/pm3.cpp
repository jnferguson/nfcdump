#include "pm3.hpp"

pm3_t::pm3_t(uint16_t v, uint16_t p) : m_col(0) //: m_out(std::cout), m_err(std::cerr) 
{
	m_usb.open(v, p);
	m_usb.set_config(1);
	m_usb.claim_all();

	return;
}

pm3_t::~pm3_t(void)
{
//	m_out.flush();
//	m_err.flush();
	m_usb.release_all();
	m_usb.close();
}

/*void
pm3_t::set_out(std::ostream& out)
{
	m_out.flush();
	m_out = out;

	return;
}

void
pm3_t::set_err(std::ostream& err)
{
	m_err.flush();
	m_err = err;

	return;
}*/

std::string	
pm3_t::version(void)
{
	cmd_version_t 			ver;
	std::vector< uint8_t >	vec;
	usb_cmd_t*				ptr(nullptr);
	std::string				str("");

	m_usb.transmit(ver.vector());
		
	do {
		m_usb.receive(vec, sizeof(usb_cmd_t));
				
		if (sizeof(usb_cmd_t) != vec.size())
			throw std::runtime_error("pm3_t::version(): Invalid response received from ProxMark");

		ptr = reinterpret_cast< usb_cmd_t* >(&vec[0]);
				
		if (CMD_DEBUG_PRINT_STRING == ptr->cmd) {
			cmd_dbg_str_t dbg;
			dbg.set(vec);
			str += std::string(reinterpret_cast< const char* >(dbg.get_data()));
			str += "\r\n";
		} else if (CMD_ACK == ptr->cmd) { 
			str += get_proc_info(ptr->arg[0]);
			str += "\r\n";
			str += get_arch_info(ptr->arg[0]);
			str += "\r\n";
			str += get_mem_info(ptr->arg[0]);
			str += "\r\n";
			break;
		}

		vec.clear();
	} while(1);

	return str;
}

std::string
pm3_t::tune(void)
{
	cmd_tune_t				tune;
	std::vector< uint8_t >	vec;
	usb_cmd_t*				ptr(nullptr);
	std::string				str("");

	m_usb.transmit(tune.vector());

	do {
		m_usb.receive(vec, sizeof(usb_cmd_t));

		if (sizeof(usb_cmd_t) != vec.size())
			throw std::runtime_error("pm3_t::tune(): Invalid response received from ProxMark");

		ptr = reinterpret_cast< usb_cmd_t* >(&vec[0]);

		if (CMD_DEBUG_PRINT_STRING == ptr->cmd) {
			cmd_dbg_str_t dbg;
			dbg.set(vec);
			str += dbg.to_string();
			str += "\r\n";
		} else 
		
		if (CMD_MEASURED_ANTENNA_TUNING == ptr->cmd) {
			tune.set(vec);
			str = tune.to_string();
			str += "\r\n";
			break;

		} else 
			throw std::runtime_error("pm3_t::tune(): Invalid command type received from ProxMark");

	} while(1);

	return str;
}

void
pm3_t::msniff(void)
{
	hexdump_t 				hex;
	cmd_mifare_sniff_t		msni;
	std::vector< uint8_t >	vec;
	usb_cmd_t*				ptr(nullptr);

	m_usb.transmit(msni.vector());
	
	while (1) {
		m_usb.receive(vec, sizeof(usb_cmd_t));

		if (sizeof(usb_cmd_t) != vec.size())
			throw std::runtime_error("pm3_t::msniff(): Encountered short read from ProxMark");

		ptr = reinterpret_cast< usb_cmd_t* >(&vec[0]);

		if (CMD_ACK == ptr->cmd) {
			signed int res(ptr->arg[0]);
			signed int len(ptr->arg[1]);
			signed int num(ptr->arg[2]);
	
			printf("res: %d len: %d num: %d\n", res, len, num);

			if (0 == len)
				continue;

			if (0 == res)
				continue;
			
			if (2 == res) {
				std::vector< uint8_t > 	cp(ptr->d.bytes[0], len);
				uint8_t*				ptr(&cp[0]);
				bool					isTag(false);

				ptr += 6; // timing info apparently...
				len = *((uint16_t*)ptr);

				if (len & 0x8000) {
					isTag = true;
					len &= 0x7fff;
				} else 
					isTag = false;

				ptr += 2;
				hex.set(ptr, len);
				printf("DATA: %s:\n%s\n", (isTag == true ? "TAG" : "READER"), hex.to_string().c_str());

			} else { 
				hex.set(&ptr->d.bytes[0], len);
				printf("DATA:\n%s\n", hex.to_string().c_str());
			}

		} else if (CMD_DEBUG_PRINT_STRING == ptr->cmd) {
			cmd_dbg_str_t dbg;
			dbg.set(vec);
			printf("DBG: %s\n", dbg.to_string().c_str());

		} else {
			printf("OTHER: code: %#lx\n", ptr->cmd);
			vec.clear();
			continue;
		}
	}
}

cmd_t*
pm3_t::select_major(select_cascade_level_t cl, usb_cmd_t& cmd)
{
	uint64_t				len(cmd.arg[0]);
	uint64_t				col(cmd.arg[2]);
	uint64_t				ocol(m_col);
	uint8_t*				ptr(&cmd.d.bytes[0]);
	cmd_seluida_t*			csu(nullptr);
	cmd_hlta_t*				halt(nullptr);
	std::vector< uint8_t >	tmp;

	if (0 == len) {
		halt = new cmd_hlta_t;
		return halt;
		throw std::runtime_error("pm3_t::select_major(): Received invalid zero-byte buffer");
	}

	if (0 != col && 0 == m_col) 
		m_col = col;
	else if (0 == col && 0 != m_col) 
		m_col = 0;

	csu = new cmd_seluida_t(m_uid, ocol, cl);

	tmp.resize(len - 1);
	::memcpy(tmp.data(), ptr, len - 1);

	csu->set_uid(tmp, col);
	tmp = csu->get_uid();

	if (0 != tmp.size())
		for (std::size_t idx = 0; idx < tmp.size(); idx++) //(SELECT_CONTINUE == tmp.at(0) ? 1 : 0); idx < tmp.size(); idx++)
			m_uid.push_back(tmp.at(idx));

	return csu;
}

cmd_t*
pm3_t::select_minor(select_cascade_level_t cl, usb_cmd_t& cmd)
{
	uint64_t		len(cmd.arg[0]);
	uint8_t*		ptr(&cmd.d.bytes[0]);
	cmd_selalla_t*	csa(nullptr);
	cmd_ratsa_t*	rats(nullptr);
	cmd_hlta_t*		halt(nullptr);

	if (0 == len) {
		halt = new cmd_hlta_t;
		return halt;
		throw std::runtime_error("pm3_t::select_minor(): Received invalid zero-byte buffer");
	}

	if (! SAK_IS_VALID(*ptr)) {
		halt = new cmd_hlta_t;
		return halt;
		throw std::runtime_error("pm3_t::select_minor(): Select error, invalid SAK.");
	}

	csa = new cmd_selalla_t(cl);

	if (SAK_UID_CASCADE & *ptr)
		return csa;
    
	rats = new cmd_ratsa_t;
	return rats;
}

void
pm3_t::otfta_raw(std::string n)
{
	std::vector< uint8_t > 	r;
	hexdump_t				h;
	usb_cmd_t*				p(nullptr);
	uint64_t				i(0);

	{
		cmd_raw_sniff_t rs;
		m_usb.transmit(rs.vector());
	}

	while (1) {
		m_usb.receive(r, sizeof(usb_cmd_t));

		if (sizeof(usb_cmd_t) != r.size())
			throw std::runtime_error("pm3_t::otfta_raw(): Encountered short read from ProxMark!");

		p = reinterpret_cast< usb_cmd_t* >(&r[0]);

		if (0 > p->arg[0]) {
			std::cout << "pm3_t::otfta_raw(): Zero byte read??" << std::endl;
			continue;
		}

		switch (p->cmd) {
			case CMD_DEBUG_PRINT_STRING:
				{
					cmd_dbg_str_t d;
					d.set(r);
					std::cout << "DBG: " << d.to_string() << std::endl;
				}
				break;
			
			case CMD_OTFTA_RAW_DUMP:
				{
					ncap_t 					ncap;
					ncap_record_list_t 		list;
					decoder_t				decoder;
					std::size_t				max(p->arg[0]);
					std::vector< uint8_t >	vec(&p->d.bytes[4], &p->d.bytes[USB_CMD_DSIZE - 1]);

					do {
						std::size_t tmax(vec.size());
						usb_cmd_t*	tptr(nullptr);

						m_usb.receive(r, sizeof(usb_cmd_t));
						tptr = reinterpret_cast< usb_cmd_t* >(&r[0]);

						vec.resize(tmax + USB_CMD_DSIZE);
						std::memcpy(&vec[tmax], &tptr->d.bytes[4], USB_CMD_DSIZE);

					} while (vec.size() < max);

					list.push_back(ncap_modulated_record_t(i++, vec));
					ncap.append(n, list);
					std::cout << "Dumping record " << (1 == i ? 0 : i-1) << " to disk" << std::endl;
					//decoder.set_data(vec);
					//decoder.parse();
					//h.set(vec);
				}
				break;
			default:
				std::cout << "OTHER: " << p->cmd << std::endl;
				break;

		}
	}

	return;
}

void
pm3_t::read14a(const std::size_t interval_ms)
{
	hexdump_t				hex;
	otfta_state_t			fsm(STATE_XMIT_REQWUP);
	cmd_wupa_t				wupa;
	cmd_reqa_t				reqa;
	std::vector< uint8_t >	retval;
	std::vector< uint8_t >	uid;
	usb_cmd_t*				ptr(nullptr);
	cmd_dbg_str_t			dbg;
	otfta_parse_t			parser;
	std::vector< uint8_t >	msg;

	m_usb.transmit(reqa.vector());

	while (1) {
		m_usb.receive(retval, sizeof(usb_cmd_t)); 

		if (sizeof(usb_cmd_t) != retval.size())
			throw std::runtime_error("pm3_t::14a_read(): Encountered short read from ProxMark");

		
		ptr = reinterpret_cast< usb_cmd_t* >(&retval[0]);
		
		if (0 >= ptr->arg[0])
			continue;

		switch (ptr->cmd) {
			case CMD_DEBUG_PRINT_STRING:
				dbg.set(retval);
				std::cout << "[DBG]: " << dbg.to_string() << std::endl << std::endl;
				break;

			case CMD_ACK:
				if (0 == ptr->arg[0])
					break;

				hex.set(&ptr->d.bytes[0], USB_CMD_DSIZE);

			case CMD_ISO14443a_READER_DUMP:
				if (STATE_DISCONNECT == fsm) {
					cmd_14443a_disconnect_t disco;

					m_usb.transmit(disco.vector());
					return;
				}
				
				if (0 != ptr->arg[0]) {
					msg.clear();
					msg.resize(ptr->arg[0]);
					std::memcpy(msg.data(), &ptr->d.bytes[0], ptr->arg[0]);
					std::cout << "PCD: " << parser.to_string(msg) << std::endl << std::endl;
				}

				break;

			case CMD_ISO14443a_CARD_DUMP:
				msg.clear();
				msg.resize(ptr->arg[0]);
				std::memcpy(msg.data(), &ptr->d.bytes[0], ptr->arg[0]);
				std::cout << "PICC: " << parser.to_string(msg) << std::endl << std::endl;

				switch (fsm) {
					case STATE_XMIT_REQWUP:
						{
							cmd_selalla_t sa(scl_one);
								
							fsm = STATE_SELC_ONE;
							m_usb.transmit(sa.vector());
							parser.set_state(fsm);
						}

						break;

					case STATE_SELC_ONE:
					{
						cmd_t* resp = select_major(scl_one, *ptr);

						if (nullptr != dynamic_cast< cmd_hlta_t* >(resp)) 
							fsm = STATE_CARD_HALT;

						if (0 == ptr->arg[2])
							fsm = STATE_SELC_ONE_CONTINUE;

						m_usb.transmit(resp->vector());
						delete resp;

						parser.set_state(fsm);
					}
							
					break;

					case STATE_SELC_ONE_CONTINUE:
					{
						// validate CRC
						cmd_t* resp = select_minor(scl_two, *ptr);

						if (nullptr != dynamic_cast< cmd_hlta_t* >(resp))
							fsm = STATE_CARD_HALT;
						else if (nullptr != dynamic_cast< cmd_ratsa_t* >(resp)) 
							fsm = STATE_XMIT_RATS;
						else 
							fsm = STATE_SELC_TWO;
						
						m_usb.transmit(resp->vector());
						delete resp;

						parser.set_state(fsm);
					}
						
					break;

					case STATE_SELC_TWO:
						{
							cmd_t* resp = select_major(scl_two, *ptr);

							if (nullptr != dynamic_cast< cmd_hlta_t* >(resp))
								fsm = STATE_CARD_HALT;
							else
								fsm = STATE_SELC_TWO_CONTINUE;

							m_usb.transmit(resp->vector());
							delete resp;
							
							parser.set_state(fsm);
						}

						break;
					case STATE_SELC_TWO_CONTINUE:
						{
							// validate CRC
							
							cmd_t* resp = select_minor(scl_three, *ptr);
							
							if (nullptr != dynamic_cast< cmd_hlta_t* >(resp))
								fsm = STATE_CARD_HALT;
							else if (nullptr != dynamic_cast< cmd_ratsa_t* >(resp)) 
								fsm = STATE_XMIT_RATS;
							else
								fsm = STATE_SELC_THREE;

							m_usb.transmit(resp->vector());
							delete resp;

							parser.set_state(fsm);
						}

						break;

					case STATE_SELC_THREE:
						{
							cmd_t* resp = select_major(scl_three, *ptr);

							if (nullptr != dynamic_cast< cmd_hlta_t* >(resp))
								fsm = STATE_CARD_HALT;
							else
								fsm = STATE_XMIT_RATS;

							m_usb.transmit(resp->vector());
							delete resp;

							parser.set_state(fsm);
						}

						break;
					
					case STATE_XMIT_RATS:
						{
							//cmd_pps_t gd;
							//m_usb.transmit(gd.vector());
							//fsm = STATE_XMIT_PPS;
							cmd_get_data_t gd;
							fsm = STATE_CARD_HALT;
							m_usb.transmit(gd.vector());
							parser.set_state(fsm);
						}

						break;

					case STATE_XMIT_PPS:
						{
							//cmd_hlta_t hlt;
							//fsm = STATE_DISCONNECT;
							//m_usb.transmit(hlt.vector());
							cmd_get_data_t gd;
							fsm = STATE_CARD_HALT;
							m_usb.transmit(gd.vector());
							parser.set_state(fsm);
						}
						break;

					case STATE_CARD_HALT:
						{
							cmd_hlta_t hlt;

							fsm = STATE_DISCONNECT;
							m_usb.transmit(hlt.vector());
							parser.set_state(fsm);
						}
						break;

					case STATE_DISCONNECT:
					default:
						{
							cmd_14443a_disconnect_t	disco;

							m_usb.transmit(disco.vector());
							parser.set_state(fsm);
						}
						return;
						break;
				}
				break;

			default:
				std::cout << "UNKNOWN CMD: " << ptr->cmd << std::endl;

		}
	}

	return; 
}

std::string 
pm3_t::get_proc_info(uint64_t data)
{
	std::string str("Processor: ");

	switch (data) {
		case 0x270B0A40:
			str += "AT91SAM7S512 Rev A";
			break;

		case 0x270B0A4F:
			str += "AT91SAM7S512 Rev B";
			break;

		case 0x270D0940:
			str += "AT91SAM7S256 Rev A";
			break;

		case 0x270B0941:
			str += "AT91SAM7S256 Rev B";
			break;

		case 0x270B0942:
			str += "AT91SAM7S256 Rev C";
			break;

		case 0x270B0943:
			str += "AT91SAM7S256 Rev D";
			break;

		case 0x270C0740:
			str += "AT91SAM7S128 Rev A";
			break;

		case 0x270A0741:
			str += "AT91SAM7S128 Rev B";
			break;

		case 0x270A0742:
			str += "AT91SAM7S128 Rev C";
			break;

		case 0x270A0743:
			str += "AT91SAM7S128 Rev D";
			break;

		case 0x27090540:
			str += "AT91SAM7S64 Rev A";
			break;

		case 0x27090543:
			str += "AT91SAM7S64 Rev B";
			break;

		case 0x27090544:
			str += "AT91SAM7S64 Rev C";
			break;

		case 0x27080342:
			str += "AT91SAM7S321 Rev A";
			break;

		case 0x27080340:
			str += "AT91SAM7S32 Rev B";
			break;

		case 0x27080341:
			str += "AT91SAM7S32 Rev B";
			break;

		case 0x27050241:
			str += "AT9SAM7S161 Rev A";
			break;

		case 0x27050240:
			str += "AT91SAM7S16 Rev A";
			break;

		default:
			str += "UNKNOWN PROCESSOR (" + std::to_string(data) + ")";
			break;
	}
					
	str += " ";
			
	switch ((data & 0xE0) >> 5) {
		case 1:
			str += "(ARM946ES)";
			break;

		case 2:
			str += "(ARM7TDMI)";
			break;

		case 4:
			str += "(ARM920T)";
			break;

		case 5:
			str += "(ARM926EJS)";
			break;

		default:
			str += "(UNKNOWN)";
			break;
	}
	
	return str;
}

std::string
pm3_t::get_mem_info(uint64_t data)
{
	std::string str("NVPRAM: ");


	switch ((data & 0xF00) >> 8) {
		case 0:
			str += "0KB";
			break;

		case 1:
			str += "8KB";
			break;

		case 2:
			str += "16KB";
			break;

		case 3:
			str += "32KB";
			break;

		case 5:
			str += "64KB";
			break;

		case 7:
			str += "128KB";
			break;

		case 9:
			str += "256KB";
			break;

		case 10:
			str += "512KB";
			break;

		case 12:
			str += "1024KB";
			break;

		case 14:
			str += "2048KB";
			break;

		default:
			str += "UNKNOWN";
			break;
	}
					
	str += " NVPRAM2: ";

	switch ((data & 0xF000) >> 12) {
		case 0:
			str += "0KB";
			break;

		case 1:
			str += "8KB";
			break;

		case 2:
			str += "16KB";
			break;

		case 3:
			str += "32KB";
			break;

		case 5:
			str += "64KB";
			break;

		case 7:
			str += "128KB";
			break;

		case 9:
			str += "256KB";
			break;

		case 10:
			str += "512KB";
			break;

		case 12:
			str += "1024KB";
			break;

		case 14:
			str += "2048KB";
			break;

		default:
			str += "UNKNOWN";
			break;
	}

	str += "\r\nInternal SRAM: ";

	switch ((data & 0xF0000) >> 16) {
		case 1:
			str += "1KB";
			break;

		case 2:
			str += "2KB";
			break;

		case 3:
			str += "6KB";
			break;

		case 4:
			str += "112KB";
			break;

		case 5:
			str += "4KB";
			break;

		case 6:
			str += "80KB";
			break;

		case 7:
			str += "160KB";
			break;

		case 8:
			str += "8KB";
			break;

		case 9:
			str += "16KB";
			break;

		case 10:
			str += "32KB";
			break;

		case 11:
			str += "64KB";
			break;

		case 12:
			str += "128KB";
			break;

		case 13:
			str += "256KB";
			break;

		case 14:
			str += "96KB";
			break;

		case 15:
			str += "512KB";
			break;

		default:
			str += "UNKNOWN";
			break;
	}

	str += "\r\nNVPRAM Type: ";

	switch ((data & 0x70000000) >> 28) {
		case 0:
			str += "ROM";
			break;

		case 1:
			str += "ROMless or on-chip flash";
			break;

		case 2:
			str += "Embedded flash memory";
			break;

		case 3:
			str += "ROM and Embedded Flash, NVPSIZ is ROM size, NVPSIZ2 is flash size";
			break;

		case 4:
			str += "SRAM emulating ROM";
			break;

		default:
			str += "UNKNOWN";
			break;
	}

	return str;
}

std::string
pm3_t::get_arch_info(uint64_t data)
{
	std::string str("Architecture Identifier: ");

	switch ((data & 0xFF00000) >> 20) {
		case 0x19:
			str += "AT91SAM9xx Series";
			break;

		case 0x29:
			str += "AT91SAM9XExx Series";
			break;

		case 0x34:
			str += "AT91x34 Series";
			break;

		case 0x37:
			str += "CAP7 Series";
			break;

		case 0x39:
			str += "CAP9 Series";
			break;

		case 0x3B:
			str += "CAP11 Series";
			break;

		case 0x40:
			str += "AT91x40 Series";
			break;

		case 0x42:
			str += "AT91x42 Series";
			break;

		case 0x55:
			str += "AT91x55 Series";
			break;

		case 0x60:
			str += "AT91SAM7Axx Series";
			break;

		case 0x61:
			str += "AT91SAM7AQxx Series";
			break;

		case 0x63:
			str += "AT91x63 Series";
			break;

		case 0x70:
			str += "AT91SAM7Sxx Series";
			break;

		case 0x71:
			str += "AT91SAM7XCxx Series";
			break;

		case 0x72:
			str += "AT91SAM7SExx Series";
			break;

		case 0x73:
			str += "AT91SAM7Lxx Series";
			break;

		case 0x75:
			str += "AT91SAM7Xxx Series";
			break;

		case 0x92:
			str += "AT91x92 Series";
			break;

		case 0xF0:
			str += "AT75Cxx Series";
			break;

		default:
			str += "UNKNOWN";
			break;
	}

	return str;
}

