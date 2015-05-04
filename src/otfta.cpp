#include "otfta.hpp"


otfta_base_t::otfta_base_t(void) 
{
	return;
}

otfta_base_t::~otfta_base_t(void) 
{
	return;
}

std::vector< uint8_t >& 
otfta_base_t::vector(void) 
{ 
	return m_raw; 
}

void* 
otfta_base_t::data(void) 
{ 
	return m_raw.data(); 
}

std::size_t 
otfta_base_t::size(void) 
{ 
	return m_raw.size(); 
}

void 
otfta_base_t::set(std::vector< uint8_t >& d)
{
	m_raw = d;
	return;
}

void
otfta_base_t::set(uint8_t* ptr, std::size_t len)
{
	if (nullptr == ptr || 0 == len)
		throw std::runtime_error("base_1443a_t::set(): Invalid parameters");

	m_raw.clear();
	m_raw.resize(len);
	std::memcpy(m_raw.data(), ptr, len);

	return;
}

void
otfta_base_t::set(int8_t* ptr, std::size_t len)
{
	set(reinterpret_cast< uint8_t* >(ptr), len);
	return;
}

/*std::string
otfta_parse_t::get_cmd(std::vector< uint8_t >& v)
{
	std::size_t len 	= v.size();
	std::size_t off		= 0;
	std::string retval	= "";

	
	if (0 == len)
		return retval;
	
	switch (v.at(off)) {
		case REQA_VALUE:
			retval = "Request Type A (REQA)";
			break;

		case WUPA_VALUE:
			retval = "Wake-up Type A (WUPA)";
			break;

		case ACCO_VALUE:
			off++;
			if (! is_valid_idx(off, len))
				return std::string("Malformed Select/Anti-Collision Type A Cascade Level 1 (TRUNCATED: LENGTH)");

			if (SELA_VALUE == v.at(off)) {
				retval = "Anti-Collision All Type A (ANTICOLLISION), Cascade Level 1";
			} else if (SELC_VALUE == v.at(off)) {
				uint8_t bcc(0);

				retval = "Select Type A (SEL), Cascade Level 1, 40-bits, UID: ";
				off++;

				if (! is_valid_idx(off, len)
					return std::string("Malformed Select Type A (SEL), Cascade Level 1 (TRUNCATED: UID)");

				if (off+7 != len)
					return std::string("Malformed Select Type A (SEL), Cascade Level 1 (OVERLONG: 40+ BITS)");
				for (; off < len - 3; off++) {
					bcc ^= v.at(off);
					retval += to_hex_string(v.at(off)) + ":";
				}
				
				retval = retval.substr(0, retval.size() - 1);
				retval += ", Block Check Character (BCC): "
				retval += to_hex_string(v.at(off));

				if (bcc != v.at(off)) 
					retval += " (INVALID), CRC: ";
				else
					retval += ", CRC: ";

				retval += to_hex_string(*reinterpret_cast< uint16_t* >(v.at(off)));
				
			} else {

			}
		case ACCW_VALUE:
		case ACCT_VALUE:
		case SECC_VALUE:
		case HALT_VALUE:
		default:

	}
}

std::string
parse_otfta(std::vector< uint8_t >& v, otfta_state_t s)
{
	std::string retval("");

	if (! v.size())
		throw std::runtime_error("parse_otfta(): empty vector parameter specified");
	
	switch (v.at(0)) {
		case ACCO_VALUE:
		case ACCW_VALUE:
		case ACCT_VALUE:
			
		case SECC_VALUE:
		case HALT_VALUE:
		default:
	}
	switch (s) {
		case STATE_XMIT_REQWUP:
			if (1 != v.size() && 2 != v.size())
				throw std::runtime_error("parse_otfta(): Data size and protocol state mismatch");

			if (1 == v.size()) {
				if (0x26 == v.at(0))
					retval = "Request Type A (REQA)";
				else if (0x52 == v.at(0))
					retval = "Wake-up Type A (WUPA)";
				else
					throw std::runtime_error("parse_otfta(): Unknown bytes received for protocol state");
			} else {
				uint16_t atqa = *reinterpret_cast< uint16_t* >(&v[0]);

				retval = "Answer to Request Type A (ATQA) ";

				if (! ATQA_IS_VALID(atqa))
					retval += "(INVALID): ";
				else
					retval += ": ";

				retval += "RFU: " + std::to_string(atqa & ATQA_RFU_BITS) + " (HIGH) ";
				retval += std::to_strig(atqa & ATQA_BFA_BITS) + " (LOW) ";
				retval += "PRO: " + std::to_string(atqa & ATQA_PRO_BITS) + " ";
				retval += "UID: ";

				if (ATQA_UID_SING(atqa)) 
					retval += "Single-length ";
				if (ATQA_UID_DOUB(atqa))
					retval += "Double-length ";
				if (ATQA_UID_TRIP(atqa))
					retval += "Triple-length ";
				if (! ATQA_UID_VALID(atqa))
					retval += "(INVALID) ";
				
				if (ATQA_HAS_ANC(atqa))
					retval += " Anti-collision is supported ";
					
			}

			break;

		case STATE_SELC_ONE:
		case STATE_SELC_ONE_CONTINUE:
		case STATE_SELC_TWO:
		case STATE_SELC_TWO_CONTINUE:
		case STATE_SELC_THREE:
			if (1 > v.size())
				throw std;:runtime_error("");

			if (ACCO_VALUE == v.at(0) || ACCW_VALUE == v.at(0) || ACCT_VALUE == v.at(0)) {
				std::size_t bits(0);

				if (2 > v.size())
					throw std::runtime_error("");

				if (SELC_VALUE == v.at(1))
					retval = "Select Type A (SEL): ";
				else
					retval = "Anti-Collision Type A (ANTICOLLISION): ";

				switch (v.at(0)) {
					case ACCO_VALUE:
						retval += "Cascade Level One, ";
						break;

					case ACCW_VALUE:
						retval += "Cascade Level Two, ";
						break;

					case ACCT_VALUE:
						retval += "Cascade Level Three, ";
						break;

					default:
						throw std::runtime_error("If you get this exception, there is a bug in your compiler");
						break;
				}

				if (SELC_VALUE != v.at(1)) {
					retval += "Number of Valid Bits (NVB) ";

					if (! NVB_IS_VALID(v.at(1))
						retval += "(INVALID): ";
					else
						retval += ": ";

					switch (v.at(1) & NVB_BYTES) {
						case NVB_BYC02:
							retval += "2 bytes, ";
							bits = 2*8;
							break;

						case NVB_BYC03:
							retval += "3 bytes, ";
							bits = 3*8;
							break;

						case NVB_BYC04:
							retval += "4 bytes, ";
							bits = 4*8;
							break;

						case NVB_BYC05:
							retval += "5 bytes, ";
							bits = 5*8;
							break;

						case NVB_BYC06:
							retval += "6 bytes, ";
							bits = 6*8;
							break;

						case NVB_BYC07:
							retval += "7 bytes, ";
							bits = 7*8;
							break;

						default:
							retval += "Invalid byte count, ";
							break;
					}

					switch (v.at(1) & NVB_BITS) {
						case NVB_BI00:
							retval += "0 bits (Total ";
							bits += 0;
							break;

						case NVB_BI01:
							retval += "1 bit (Total: ";
							bits += 1;
							break;

						case NVB_BI02:
							retval += "2 bits (Total: ";
							bits += 2;

						case NVB_BI03:
							retval += "3 bits (Total: ";
							bits += 3;
							break;

						case NVB_BI04:
							retval += "4 bits (Total: ";
							bits += 4;
							break;

						case NVB_BI05:
							retval += "5 bits (Total: ";
							bits += 5;
							break;

						case NVB_BI06:
							retval += "6 bits (Total: ";
							bits += 6;

						case NVB_BI07:
							retval += "7 bits (Total: ";
							bits += 7;
							break;

						default:
							retval += "Invalid number of bits (Total: ";
							bits += 0;
							break;
					}

					if (0 == bits)
						retval += "INVALID) ";
					else
						retval += std::to_string(bits) + " bits), ";
				}


				if (3 <= v.size()) {
					retval += "UID: "
					for (std::size_t idx = 2; idx < v.size(); idx++) {
						retval += std::to_string(v.at(idx)) + ":";
					
					retval = retval.substr(0, retval.size() - 1);
					retval += " ";
				}
			} else {
				if (SELECT_CONTINUE == v.at(0)) {
					uint8_t bcc(0);

					retval = "UID Collision-n Type A (UID CLn) ";
					

					if (5 != v.size())
						retval += "(INVALID): ";
					else
						retval += ": ";

					retval += "UID: ";

					for (std::size_t idx = 0; idx < v.size()-1; idx++) {
						bcc ^= v.at(idx);
						retval += std::to_string(v.at(idx)) + ":";
					}

					retval = retval.substr(0, retval.size() - 1);
					retval += ", Block Check Character (BCC): ";
					retval += std::to_string(v.at(v.size()-1)) + " ";

					if (bcc != v.at(v.size() - 1))
						retval += "(INVALID) ";
					
				} else if (SAK_IS_VALID(v.at(0))) {
					
				} else {
				}
				retval = "Select Acknowledgement (SAK): ";

			}
			break;

		case STATE_XMIT_RATS:
		case STATE_CARD_HALT:
		case STATE_DISCONNECT:
		default:
	};

}*/

reqa_t::reqa_t(void) : otfta_base_t()
{
	uint8_t val(0x26);
	set(&val, sizeof(uint8_t));

	return;
}

reqa_t::~reqa_t(void) 
{
	return;
}

wupa_t::wupa_t(void) : otfta_base_t()
{
	uint8_t val(0x52);
	set(&val, sizeof(uint8_t));

	return;
}

wupa_t::~wupa_t(void) 
{
	return;
}

hlta_t::hlta_t(void) : otfta_base_t()
{
	uint8_t val[2] = { 0x50, 0x00 };
	set(&val[0], sizeof(val));

	return;
}

hlta_t::~hlta_t(void) 
{
	return;
}

ratsa_t::ratsa_t(uint16_t fsd, uint8_t cid) : otfta_base_t()
{
	uint8_t parm(0);
	uint8_t	bytes[2] = { 0xE0, 0x00 };

	switch (fsd) {
		case 16:
			parm |= RATSA_FSDI_16;
			break;

		case 24:
			parm |= RATSA_FSDI_24;
			break;

		case 32:
			parm |= RATSA_FSDI_32;
			break;

		case 40:
			parm |= RATSA_FSDI_40;
			break;

		case 48:
			parm |= RATSA_FSDI_48;
			break;

		case 64:
			parm |= RATSA_FSDI_64;
			break;

		case 96:
			parm |= RATSA_FSDI_96;
			break;

		case 128:
			parm |= RATSA_FSDI_128;
			break;

		case 256:
			parm |= RATSA_FSDI_256;
			break;
		default:
			throw std::runtime_error("ratsa_t::ratsa_t(): Invalid FSDI value specified");
			break;
	}

	if (cid & ~RATSA_CID_BITS)
		throw std::runtime_error("rats_t::rats_t(): Invalid CID value specified");

	parm |= cid;
	bytes[1] = parm;
	set(&bytes[0], sizeof(bytes));

	return;
}

ratsa_t::~ratsa_t(void)
{
	return;
}

selalla_t::selalla_t(select_cascade_level_t cl) : otfta_base_t(), m_sel(0x93), m_nvb(0x20)
{
	uint8_t val[] = { 0x00, 0x00 }; 
			
	val[0] = m_sel + cl * 2;
	val[1] = m_nvb;

	set(&val[0], sizeof(val));

	return;
}

selalla_t::~selalla_t(void) 
{
	return;
}

void 
selalla_t::set_cascade_level(select_cascade_level_t cl)
{
	uint8_t val[] = { 0x00, 0x00 };

	val[0] = m_sel + cl * 2;
	val[1] = m_nvb;
			
	set(&val[0], sizeof(val));

	return;
}

seluida_t::seluida_t(select_cascade_level_t cl) : otfta_base_t(), m_sel(0x93), m_nvb(0x20), m_col(0)
{
	m_sel = (m_sel + cl * 2);

	return;
}

seluida_t::seluida_t(std::vector< uint8_t >& uid, uint8_t pcp, select_cascade_level_t cl) : otfta_base_t(), 
																							m_sel(0x93), m_nvb(0x20), 
																							m_uid(uid), m_col(pcp)
{
	m_sel = (m_sel + cl * 2);

	return;
}

seluida_t::~seluida_t(void) 
{ 
	m_uid.clear(); 
	return; 
}

void 
seluida_t::set_cascade_level(select_cascade_level_t cl)
{
	m_sel = (0x93 + cl * 2);

	return;
}

void 
seluida_t::set_uid(std::vector< uint8_t >& uid, uint8_t cp)
{
	std::vector< uint8_t > 	data;
	uint8_t					byte_cnt((0 != cp ? cp / 8 : 0));
	uint8_t					bit_cnt((0 != cp ? cp % 8 : 0));

	if (0 == uid.size())
		return;

	if (10 < m_uid.size()) 
		throw std::runtime_error("seluida_t::set_uid(): Invalid UID length (greater than maximum length)");

	data.push_back(m_sel);

	if (0 == cp && 0 != m_col) {
		std::vector< uint8_t > 	tmp;
		std::size_t				off(0);

		if (0 == m_uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Invalid UID length (stored UID is zero bytes in length)");

		m_uid.at(m_uid.size() - 1) |= uid.at(0);

		for (std::size_t idx = 1; idx < uid.size(); idx++)
			m_uid.push_back(uid.at(idx));
			
		m_nvb = 0x70;
		data.push_back(m_nvb);
		off = 0;

		tmp.clear();
		if (40 < m_uid.size()*8) {
			uint8_t bcc(0);

			off = m_uid.size() - 1;
			tmp.push_back(m_uid.at(off - 3));
			tmp.push_back(m_uid.at(off - 2));
			tmp.push_back(m_uid.at(off - 1));
			tmp.push_back(m_uid.at(off - 0));

			for (auto& byte : tmp) {
				data.push_back(byte);
				bcc ^= byte;
			}

		} else {
		
			for (auto& byte : m_uid)
				data.push_back(byte);
	
			data.push_back(m_uid.at(0) ^ m_uid.at(1) ^ m_uid.at(2) ^ m_uid.at(3));
		}

		m_col = 0;

	} else if (0 != cp && 0 != m_col) {
		std::vector< uint8_t >		tmp;
		uint8_t						fb(uid.at(0));
		uint8_t						lb(0);
		std::size_t					off(0);

		if (0 == m_uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Invalid UID length (stored UID is zero bytes in length)");

		if (40 < cp)
			throw std::runtime_error("seluida_t::set_uid(): Collision detected at bit count exceeding 5 bytes");

		if (byte_cnt > uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Mismatch between collision position and UID length");

		if (0 != bit_cnt && static_cast< std::size_t >(byte_cnt)+1 > uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Mismatch between collision position and UID length");

		lb = m_uid.at(m_uid.size() - 1);
		fb &= m_col;
		lb &= ~m_col;

//		off = m_uid.size();
//		m_uid.at(off - 1) = (fb|lb);
		m_uid.at(m_uid.size() - 1) = (fb|lb);
		tmp.resize(m_uid.size() + byte_cnt);

		if (0 != bit_cnt)
			tmp.resize(tmp.size() + 1);

		std::memset(tmp.data(), 0, tmp.size());
		std::memcpy(tmp.data(), m_uid.data(), m_uid.size());

		off = m_uid.size();

		for (std::size_t idx = 1; idx < cp; idx++, off++) {
			uint16_t uidbit = (uid.at(idx/8) >> (idx%8) & 0x01);
			tmp.at(off/8) |= (uidbit << (off%8));
		}

		if (0 != bit_cnt)
			tmp.back() |= 1 << (cp%8);
		else
			tmp.push_back(0x01);

		m_nvb = 0x20;
		m_nvb |= (byte_cnt << 4);
		m_nvb |= bit_cnt;

		if (! NVB_IS_VALID(m_nvb))
			throw std::runtime_error("seluida_t::set_uid(): Invalid NVB byte encountered");

		m_nvb += 0x01;
		m_nvb += 0x20; // WHY IS THIS CORRECT??
		
		data.push_back(m_nvb);

		m_uid = tmp;

		for (auto& byte : m_uid)
			data.push_back(byte);

		m_col = cp;

	} else if (0 != cp && 0 == m_col) {
		if (40 < cp)
			throw std::runtime_error("seluida_t::set_uid(): Collision detected at bit count exceeding 5 bytes");

		if (byte_cnt > uid.size()) 
			throw std::runtime_error("seluida_t::set_uid(): Mismatch between collision position and UID length");

		if (0 != bit_cnt && static_cast< std::size_t >(byte_cnt)+1 > uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Mismatch between collision position and UID length");

		m_col = cp;
		m_uid.resize(byte_cnt);

		if (0 != bit_cnt)
			m_uid.resize(m_uid.size()+1);

		std::memset(m_uid.data(), 0, m_uid.size());

		for (uint16_t idx = 0; idx < cp; idx++) {
			uint16_t uidbit = (uid.at(idx/8) >> (idx%8) & 0x01);		
			
			m_uid.at(idx/8) |= (uidbit << (idx%8));
		}

		// if we collided on a byte boundary than 
		// the (1 << (cp%8)) we add needs to go
		// to a new byte.
		if (0 != bit_cnt)
			m_uid.back() |= 1 << (cp%8);
		else 
			m_uid.push_back(0x01);

		// NVB = (number of bytes before collision)|(remaining bits)+0x20+0x01
		// 0000 0000
		// BYTE BITS
		m_nvb |= (byte_cnt << 4);
		m_nvb |= bit_cnt;
			
		if (! NVB_IS_VALID(m_nvb))
			throw std::runtime_error("seluida_t::set_uid(): Invalid NVB byte encountered");

		m_nvb += 0x01; // ?? why ?? is this the 0b or 1b 14443-3 speaks of on page 16 step 7 ??
		m_nvb += 0x20;
				
		data.push_back(m_nvb);

		for (auto& byte : m_uid) 
			data.push_back(byte);
	
	} else {
		m_uid = uid;
		m_nvb = 0x70;
		data.push_back(m_nvb);

		if (4 != m_uid.size())
			throw std::runtime_error("seluida_t::set_uid(): Invalid UID length encountered");

		data.push_back(m_uid.at(0));
		data.push_back(m_uid.at(1));
		data.push_back(m_uid.at(2));
		data.push_back(m_uid.at(3));
		data.push_back(m_uid.at(0) ^ m_uid.at(1) ^ m_uid.at(2) ^ m_uid.at(3));
	}

	set(data);

	return;
}

std::vector< uint8_t >&
seluida_t::get_uid(void) 
{
	return m_uid;
}
