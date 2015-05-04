#include "parse.hpp"

std::string
otfta_parse_t::parse_reqwup(std::vector< uint8_t >& v)
{
	std::string r("");

	switch (v.at(0)) {
		case REQA_VALUE:
			r = "Request (REQA)";
			break;
		case WUPA_VALUE:
			r = "Wake-up (WUPA)";
			break;
		default: // ATQA?
			throw std::runtime_error("otfta_parse_t::parse_reqwup(): Invalid value encountered");
			break;
	}

	return r;
}

std::string
otfta_parse_t::parse_nvb(uint8_t nvb)
{
	std::string r("NVB");
	uint8_t		b(0);

	if (! NVB_IS_VALID(nvb))
		r += " (INVALID): ";
	else
		r += ": ";

	switch (nvb & NVB_BYTES) {
		case NVB_BYC02:
			b += 2*8;
			r += "2 bytes, ";
			break;

		case NVB_BYC03:
			b += 3*8;
			r += "3 bytes, ";
			break;

		case NVB_BYC04:
			b += 4*8;
			r += "4 bytes, ";
			break;

		case NVB_BYC05:
			b += 5*8;
			r += "5 bytes, ";
			break;

		case NVB_BYC06:
			b += 6*8;
			r += "6 bytes, ";
			break;

		case NVB_BYC07:
			b += 7*8;
			r += "7 bytes, ";
			break;

		default:
			b += 0;
			r += "Invalid byte count ";
			break;
	}

	switch (nvb & NVB_BITS) {
		case NVB_BI00:
			b += 0;
			r += "0 bits (Total ";
			break;

		case NVB_BI01:
			b += 1;
			r += "1 bit (Total ";
			break;

		case NVB_BI02:
			b += 2;
			r += "2 bits (Total ";
			break;

		case NVB_BI03:
			b += 3;
			r += "3 bits (Total ";
			break;

		case NVB_BI04:
			b += 4;
			r += "4 bits (Total ";
			break;

		case NVB_BI05:
			b += 5;
			r += "5 bits (Total ";
			break;

		case NVB_BI06:
			b += 6;
			r += "6 bits (Total ";
			break;

		case NVB_BI07:
			b += 7;
			r += "7 bits (Total ";
			break;

		default:
			b += 0;
			r += "Invalid bit count (Total ";
			break;
	}

	if (0 == b)
		r += "INVALID) ";
	else
		r += std::to_string(b) + " bits), ";
	
	return r;
}

std::string
otfta_parse_t::parse_selanc(std::vector< uint8_t >& v)
{
	//const std::size_t 	l(v.size());
	//std::size_t			o(0);
	//std::size_t			m(0);
	std::string			r("");

	if (2 > v.size())
		return std::string("Malformed SELECT/ANTI-COLLISION packet frame encounered");

	if (SELC_VALUE == v.at(1)) 
		r = "Select (SEL) Cascade level ";
	else if (SELA_VALUE == v.at(1))
		r = "Select All (SEL) Cascade level ";
	else if (SECC_VALUE != v.at(0))
		r = "Anti-Collision (ANTICOLLISION) Cascade level ";
	else 
		r = "Anti-Collision Response";

	switch (v.at(0)) {
		case ACCO_VALUE:
			r += "1";
			break;
		case ACCW_VALUE:
			r += "2";
			break;

		case ACCT_VALUE:
			r += "3";
			break;

		case SECC_VALUE:
			break;
	}

	if (SELA_VALUE == v.at(1)) 
		return r;

	r += ": ";

	if (SELC_VALUE == v.at(1)) {
		uint8_t bcc(0);

		m_hcol = false;

		r += "40-bits present, ";

		if (2 + 4 > v.size())
			return r + "Remainder of request is malformed (UID)";

		r += "UID: ";

		for (std::size_t idx = 2; idx < 6; idx++)
			r += to_hex_string(v.at(idx)) + ":";

		r = r.substr(0, r.size() - 1);
		
		if (2 + 4 + 1 > v.size())
			return r + ", Reaminder of request is malformed (BCC)";

		r += ", BCC: ";
		for (std::size_t idx = 2; idx < 7; idx++)
			bcc ^= v.at(idx);

		r += to_hex_string(v.at(6));

		if (0 != bcc)
			r += "(INVALID) ";

		if (2 + 4 + 1 + 2 > v.size())
			return r + ", Remainder of request is malformed (CRC)";

		r += ", CRC: ";
		r += to_hex_string(*reinterpret_cast< uint16_t* >(&v[7]));
		r += " (\?\?\?)";

	} else if (SECC_VALUE != v.at(0)) {
		r += parse_nvb(v.at(1) - 0x20);

		m_hcol = true;
		if (2 + 4 > v.size())
			return r + " Remainder of request is malformed";

		r += "UID: ";

		for (std::size_t idx = 2; idx < v.size(); idx++)
			r += to_hex_string(v.at(idx)) + ":";
		
		r = r.substr(0, r.size() - 1);

	} else {
		uint8_t bcc(0);

		if (4 > v.size())
			return r + "Remainder of request is malformed (UID)";

		r += "UID: ";

		for (std::size_t idx = 0; idx < v.size(); idx++)
			bcc ^= v.at(idx);

		for (std::size_t idx = 1; idx < v.size() - 1; idx++) 
			r += to_hex_string(v.at(idx)) + ":";

		r = r.substr(0, r.size() - 1);
		r += " ";

		if (4 + 1 > v.size())
			return r + "Remainder of request is malformed (BCC)";

		r += "BCC: " + to_hex_string(v.at(4)) + " ";
		
		if (0 != bcc)
			r += "(MALFORMED)" ; //, CRC: ";
	}	

	return r;	
}

std::string 
otfta_parse_t::parse_halt(std::vector< uint8_t >& v)
{
	std::string r = "Halt (HALTA): ";

	if (4 != v.size())
		return std::string("Halt (HALTA): Malformed");

	if (0 != v.at(1)) 
		r += "Invalid request: " + to_hex_string(v.at(1)) + ", ";

	r += "CRC: ";
	r += to_hex_string(*reinterpret_cast< uint16_t* >(&v[2]));
	r += std::string(" (\?\?\?)");

	return r;
}

std::string
otfta_parse_t::parse_rats(std::vector< uint8_t >& v)
{
	//const std::size_t 	len 	= v.size();
	std::string			retval 	= "Request to Answer Select (RATSA): ";

	if (2 > v.size())
		return retval + std::string("Malformed packet encountered");

	retval += "FSD: ";

	switch (v.at(1) & ~(RATSA_CID_BITS)) {
		case RATSA_FSDI_16:
			retval += "16 ";
			break;

		case RATSA_FSDI_24:
			retval += "24 ";
			break;

		case RATSA_FSDI_32:
			retval += "32 ";
			break;

		case RATSA_FSDI_40:
			retval += "40 ";
			break;

		case RATSA_FSDI_48:
			retval += "48 ";
			break;

		case RATSA_FSDI_64:
			retval += "64 ";
			break;

		case RATSA_FSDI_96:
			retval += "96 ";
			break;

		case RATSA_FSDI_128:
			retval += "128 ";
			break;

		case RATSA_FSDI_256:
			retval += "256 ";
			break;

		default:
			retval += "Invalid frame size ";
			break;
	}
	
	if (! RATSA_FSDI_IS_VALID((v.at(1) & ~(RATSA_CID_BITS))))
		retval += "(INVALID), CID: ";
	else
		retval += "CID: ";
	
	retval += to_hex_string(v.at(1) & RATSA_CID_BITS);

	if (4 > v.size())
		retval += " Remainder of request is malformed (CRC)";

	retval += ", CRC: ";
	retval += to_hex_string(*reinterpret_cast< uint16_t* >(&v.at(2)));
	retval += std::string(" (\?\?\?)");

	return retval;
}

std::string
otfta_parse_t::parse_atqa(std::vector< uint8_t >& v)
{
	std::string retv("Answer to Request (ATQA)");
	uint16_t	atqa(0);

	if (2 > v.size())
		return retv + ": Remainder of request is malformed. (TRUNCATED)";
	
	atqa = *reinterpret_cast< uint16_t* >(&v[0]);

	if (! ATQA_IS_VALID(atqa))
		retv += "(INVALID): ";
	else
		retv += ": ";

	if (0 != (atqa & ATQA_PRO_BITS))
		retv += "Propietary bits: " + to_hex_string(atqa & ATQA_PRO_BITS) + " ";
		
	retv += "UID: ";
	
	if (ATQA_UID_SING(atqa)) 
		retv += "Single-length ";

	if (ATQA_UID_DOUB(atqa))
		retv += "Double-length ";
		
	if (ATQA_UID_TRIP(atqa))
		retv += "Triple-length ";
		
	if (! ATQA_UID_VALID(atqa))
		retv += "(INVALID) ";

	retv = retv.substr(0, retv.size() - 1);
	retv += ", Anti-collision: ";

	if (ATQA_HAS_ANC(atqa))
		retv += "supported ";
	else 
		retv += "not-supported";
	
	return retv;
}

std::string
otfta_parse_t::parse_saka(std::vector< uint8_t >& v)
{
	std::string retval("Select Acknowledgement (SAKA)");

	if (! SAK_IS_VALID(v.at(0)))
		retval += " (INVALID): ";
	else
		retval += ": ";

	if ( v.at(0) & SAK_UID_CASCADE)
		retval += "UID cascade required, ";
	else if ( v.at(0) & SAK_UID_COMPLETE)
		retval += "UID is complete, ";
	else if ( v.at(0) & SAK_NFCIPONE)
		retval += "PICC is NFC-IP1 compliant";
	else if ( v.at(0) & ~(SAK_UID_CASCADE|SAK_UID_COMPLETE|SAK_NFCIPONE))
		retval += "PICC is not ISO/IEC 14443A-3 compliant, ";


	if (3 > v.size()) 
		return retval + "Remainder of request is malformed (CRC)";

	retval += "CRC: ";
	retval += to_hex_string(*reinterpret_cast< uint16_t* >(&v[1]));
	retval += " (\?\?\?)";

	if (3 < v.size())
		return retval + ", Request is overlong, potentially invalidly identified as SAK";

	return retval;
}

std::string
otfta_parse_t::parse_other(std::vector< uint8_t >& v)
{
	std::string r("");
	uint8_t		b(0);
	bool		valid_bcc(false);

	for (std::size_t idx = 0; idx < v.size(); idx++)
		b ^= v.at(idx);

	if (0 == b)
		valid_bcc = true;

	if (2 == v.size() && false == valid_bcc) {
		// ATQA ?
		if (STATE_XMIT_REQWUP == m_state) // ATQA.
			r = parse_atqa(v);
		else {
			r = parse_atqa(v);
			r += ", Protocol State does not match and ATQA is spurious";
		}
	} else if ( 3 == v.size() && false == m_hcol && STATE_XMIT_PPS != m_state) {
		// SAK ?
		switch (m_state) {
			// SAK.
			case STATE_SELC_ONE_CONTINUE:
			case STATE_SELC_TWO:
			case STATE_SELC_TWO_CONTINUE:
			case STATE_SELC_THREE:
			case STATE_XMIT_RATS:
				r = parse_saka(v);
				break;
			default:
				r = parse_saka(v);
				r += ", Protocol state does not match and SAKA is spurious";
				break;
		}
	} else if ( false == valid_bcc && (5 == v.size() || 3 == v.size()) && (STATE_XMIT_RATS == m_state || STATE_XMIT_PPS == m_state)) {
		if (5 == v.size()) {
                if (0xD0 == (v.at(0) & 0xD0)) {
                    r = "Protocol and Parameter Select (PPS): ";
                    r += "CID: " + to_hex_string((v.at(0) & 0x0F)) + ", ";

                    if (v.at(1) & 0x10)
                        r += "PPS1 present, ";

                    switch ((v.at(2) & 0x0C) >> 2) {
                        case 0: // 1
                            r += "DSI: 1, ";
                            break;

                        case 1: // 2
                            r += "DSI: 2, ";
                            break;

                        case 2: // 4
                            r += "DSI: 4, ";
                            break;

                        case 3: // 8
                            r += "DSI: 8, ";
                            break;

                        default:
                            r += "DSI: INVALID, ";
                            break;
                    }

                    switch (v.at(2) & 0x03) {
                        case 0:
                            r += "DRI: 1, ";
                            break;

                        case 1:
                            r += "DRI: 2, ";
                            break;

                        case 2:
                            r += "DRI: 4, ";
                            break;

                        case 3:
                            r += "DRI: 8, ";
                            break;

                        default:
                            r += "DRI: INVALID, ";
                            break;
                    }

                    r += "CRC: ";
                    r += to_hex_string(*reinterpret_cast< uint16_t* >(&v[3]));
                    r += " (\?\?\?)";
                }
		
		} else {
			r += "Protocol and Parameter Selection Response (PPSS): ";
			r += "CID: " + to_hex_string(v.at(0) & 0x0F) + ", ";
			r += "CRC: ";
			r += to_hex_string(*reinterpret_cast< uint16_t* >(&v[1]));
			r += " (\?\?\?)";
		}

	} else if ( true == valid_bcc || true == m_hcol || 5 == v.size()) { //5 == v.size()) {
		switch (m_state) {
			case STATE_SELC_ONE:
			case STATE_SELC_ONE_CONTINUE:
			case STATE_SELC_TWO:
			case STATE_SELC_TWO_CONTINUE:
			case STATE_SELC_THREE:
				{ // UID?
						r = "Select Response: UID: ";

					if (5 == v.size()) {
						for (std::size_t idx = 0; idx < v.size() - 1; idx++)
							r += to_hex_string(v.at(idx)) + ":";

						r = r.substr(0, r.size() - 1);
						r += ", BCC: " + to_hex_string(v.at(v.size() - 1));

						if (0 != b)
							r += " (INVALID)";
					} else {
						for (std::size_t idx = 0; idx < v.size(); idx++)
							r += to_hex_string(v.at(idx)) + ":";

						r = r.substr(0, r.size() - 1);
					}
				}
				break;
			
			default:
				r = "Unknown packet, Type A";
				break;
		}

	} else {
		
		if (STATE_XMIT_RATS == m_state) {
			uint8_t ats_len(0);
			uint8_t off(1);
			bool	tao(false), tbo(false), tco(false);

			if (0 == v.size())
				return std::string("Zero-byte packet encountered (bug?)");

			ats_len = v.at(0);
			
			
			if (2 > v.size()) 
				return std::string("Unknown Packet, Type A");

			if (ats_len == v.size() - 2) {
				r = "Answer to Select (ATS) ";

				if (! ATSA_FMT_IS_VALID(v.at(off)))
					r += "(INVALID): ";
				else
					r += ": ";

				if (v.at(off) & ATSA_TAO_MASK) 
					tao = true;

				if (v.at(off) & ATSA_TBO_MASK) 
					tbo = true;
	
				if (v.at(off) & ATSA_TCO_MASK) 
					tco = true;

				r += "FSC: ";
				switch (v.at(off) & ATSA_FSCI_MASK) {
					case ATSA_FSCI_16:	
						r += "16, ";
						break;

					case ATSA_FSCI_24:
						r += "24, ";
						break;

					case ATSA_FSCI_32:
						r += "32, ";
						break;

					case ATSA_FSCI_40:
						r += "40, ";
						break;

					case ATSA_FSCI_48:
						r += "48, ";
						break;

					case ATSA_FSCI_64:
						r += "64, ";
						break;

					case ATSA_FSCI_96:
						r += "96, ";
						break;

					case ATSA_FSCI_128:
						r += "128, ";
						break;

					case ATSA_FSCI_256:
						r += "256, ";
						break;

					default:
						r += "Unknown (" + to_hex_string(v.at(off) & ATSA_FSCI_MASK) + "), ";
						break;
				}


				off++;

				if (true == tao) {
					if (off < v.size()) {
						r += "TA(1)";

						if (! ATSA_TAO_IS_VALID(v.at(off)))
							r += " (INVALID): ";
						else
							r += ": ";

						r += std::string("Different divisors: ") + (0 == (v.at(off) & ATSA_TAO_DIFFDIV_MASK) ? "T" : "F");
						r += std::string(", DS: ");
				
						// D{S,R}=2 212Kb/s, D{S,R}=4 424Kb/s D{S,5}=8 847 Kb/s
						if (v.at(off) & ATSA_TAO_DST)
							r += "2 ";
						if (v.at(off) & ATSA_TAO_DSF)
							r += "4 ";
						if (v.at(off) & ATSA_TAO_DSH)
							r += "8";
						
						r += std::string(", DR: ");

						if (v.at(off) & ATSA_TAO_DRT)
							r += "2 ";
						if (v.at(off) & ATSA_TAO_DRF)
							r += "4 ";
						if (v.at(off) & ATSA_TAO_DRH)
							r += "8";

						off++;
					} else
						r += "TA(1) specified, but packet is truncated, ";
				}

				if (true == tbo) {
					if (off < v.size()) {
						if (true == tao)
							r += ", TB(1)";
						else
							r += "TB(1)";

						if (! ATSA_TBO_IS_VALID(v.at(off))) 
							r += " (INVALID): ";
						else
							r += ": ";

						r += "FWT: " + std::to_string(256.0 * 16.0 * (1 << ((v.at(off) & ATSA_TBO_FWIFWT_MASK) >> 4)) / 1356.0) + " ms, ";
						//to_hex_string(((1 << 12) << (v.at(off) & ATSA_TBO_FWIFWT_MASK))) + " ";
						r += "SFGT: ";

						if (0 == (v.at(off) & ATSA_TBO_SFGI_MASK))
							r += "None";
						else {
							r += std::to_string(256.0 * 16.0 * (1 << (v.at(off) & ATSA_TBO_SFGI_MASK))/1356.0) + " ms";
							//to_hex_string( ((1 << 12) << (v.at(off) & ATSA_TBO_SFGI_MASK)));
						}
						off++;
					} else
						r = "TB(1) specified, but packet is truncated, ";
				}

				if (true == tco) {
					if (off < v.size()) {
						if (true == tao || true == tbo)
							r += ", TC(1)";
						else
							r += "TC(1)";

						if (! ATSA_TCO_IS_VALID(v.at(off)))
							r += " (INVALID): ";
						else
							r += ": ";

						r += "NAD: ";
						if (v.at(off) & ATSA_TCO_HAS_NAD)
							r += "T, ";
						else
							r += "F, ";

						r += "CID: ";

						if (v.at(off) & ATSA_TCO_HAS_CID)
							r += "T, ";
						else
							r += "F, ";

						off++;

					} else
						r = "TC(1) specified, but packet is truncated, ";
			
				}

				if (off < v.size()) {
					if (ATSA_TK_CATI_NOTTLV == v.at(off) || ATSA_TK_CATI_DIR == v.at(off) || ATSA_TK_CATI_TLV == v.at(off))
						r += "ISO/IEC 7816-3 conformant historical bytes, encountered but currently unimplemented, ";
//					else if (v.at(off) & ATSA_TK_RFU_MASK) {
//						r += "ISO/IEC 7816-3 RFU value found in historical bytes.";
					else if (v.at(off) & ATSA_TK_MIFARE) {
						uint8_t ticl(0);

						off++;
						
						if (off <= v.size() - 2) {
							ticl = v.at(off++);

							if (ticl <= v.size() - 2 - off)	{
								r += "Chip type: ";

								switch (v.at(off) & 0xF0) {
									case 0x00:
										r += "Multiple Virtual Cards";
										break;

									case 0x10:
										r += "Mifare DESFire";
										break;

									case 0x20:
										r += "Mifare Plus";
										break;

									default:
										r += "RFU";
										break;
								}

								r += ", Memory size: ";
								switch (v.at(off) & 0x0F) {
									case 0x00:
										r += "<1KB";
										break;
									case 0x01:
										r += "1KB";
										break;

									case 0x02:
										r += "2KB";
										break;

									case 0x03:
										r += "3KB";
										break;

									case 0x04:
										r += "4KB";
										break;

									case 0x0F:
										r += "Unspecified";
										break;

									default:
										r += "RFU";
										break;
								}



							} else
								r += "Type identification coding length is invalid";

						} else 
							r += "Malformed Tk/T1 bytes encountered";

					}
				}
				r += ", CRC: ";
				r += to_hex_string(*reinterpret_cast< uint16_t* >(&v[v.size() - 2 ]));
			}

			
		} else 
			r = "Unknown Packet, Type A";
	}

	return r;
}

std::string
otfta_parse_t::cmd(std::vector< uint8_t >& v)
{
	const std::size_t 	len		= v.size();
	std::size_t			off		= 0;
	std::string			retval	= "";

	if (0 == len)
		return retval;

	if (true == m_hcol) {
		retval = parse_other(v);
		m_hcol = false;

	} else if (STATE_XMIT_PPS == m_state) {
		retval = parse_other(v);

	} else {
		switch (v.at(off)) {
			case REQA_VALUE:
			case WUPA_VALUE:
				retval = parse_reqwup(v);
				break;

			case ACCO_VALUE:
			case ACCW_VALUE:
			case ACCT_VALUE:
			case SECC_VALUE:
				retval = parse_selanc(v);
				break;

			case RATS_VALUE:
				retval = parse_rats(v);
				break;

			case HALT_VALUE:
				retval = parse_halt(v);
				break;

			default:
				retval = parse_other(v);
				break;
		}
	}

	return retval;
}

