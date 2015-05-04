#include "manchester.hpp"


inline manchester_lut_t
manchester_t::init_lut(void)
{
	manchester_lut_t retval({	
							false, false, false, false, false, false, false, true,
							false, false, false, true, false, true, true, true
						});

	return retval;
}

inline void
manchester_t::reset(void)
{
	m_state				= MANCHESTER_STATE_UNSYNCHED;
	m_two_bits			= 0;
	m_high				= 0;
	m_bits				= 0;
	m_collision_pos 	= 0;
	m_sync_bit			= 0;
	m_parity			= 0;
	m_shift				= 0;
	m_samples			= 0;
	m_length			= 0;
	m_start				= 0;
	m_end				= 0;
	
	m_input.clear();
	m_output.clear();

	return;
}

inline bool
manchester_t::is_mod_nibble_top(uint16_t b)
{
	return m_lut[(b & 0x00F0) >> 4];
}

inline bool
manchester_t::is_mod_nibble_bottom(uint16_t b)
{
	return m_lut[b & 0x000F];
}

manchester_t::manchester_t(void) : m_lut(init_lut()) 
{ 
	reset(); 

	return; 
}

manchester_t::~manchester_t(void) 
{ 
	reset(); 

	return; 
}

bool
manchester_t::has_sync(uint16_t b)
{
    uint16_t sync = 0xFFFF;

    if (0x7000 == (b & 0x7700))
        sync = 7;
    else if (0x3800 == (b & 0x3B80))
        sync = 6;
    else if (0x1C00 == (b & 0x1DC0))
        sync = 5;
    else if (0x0E00 == (b & 0x0EE0))
        sync = 4;
    else if (0x0700 == (b & 0x0770))
        sync = 3;
    else if (0x0380 == (b & 0x03B8))
        sync = 2;
    else if (0x01C0 == (b & 0x01DC))
        sync = 1;
    else if (0x00E0 == (b & 0x00EE))
        sync = 0;

    if (0xFFFF != sync)
        return true;

    return false;
}


bool
manchester_t::has_sync(void)
{
	m_sync_bit = 0xFFFF;

	if (0x7000 == (m_two_bits & 0x7700))
		m_sync_bit = 7;
	else if (0x3800 == (m_two_bits & 0x3B80))
		m_sync_bit = 6;
	else if (0x1C00 == (m_two_bits & 0x1DC0))
		m_sync_bit = 5;
	else if (0x0E00 == (m_two_bits & 0x0EE0))
		m_sync_bit = 4;
	else if (0x0700 == (m_two_bits & 0x0770))
		m_sync_bit = 3;
	else if (0x0380 == (m_two_bits & 0x03B8))
		m_sync_bit = 2;
	else if (0x01C0 == (m_two_bits & 0x01DC))
		m_sync_bit = 1;
	else if (0x00E0 == (m_two_bits & 0x00EE))
		m_sync_bit = 0;

	if (0xFFFF != m_sync_bit) 
		return true;

	return false;
}

bool
manchester_t::is_modulated_both_halves(void)
{
	if (is_mod_nibble_top(m_two_bits >> m_sync_bit) && is_mod_nibble_bottom(m_two_bits >> m_sync_bit))
		return true;

	return false;
}

void
manchester_t::process_logic_one(void)
{
	if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit))
		if (0 == m_collision_pos)
			m_collision_pos = (m_length << 3) + m_bits;

	m_bits 	+= 1;
	m_shift	= (m_shift >> 1) | 0x100;

	if (9 == m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity 	<<= 1;
		m_parity 	|= (m_shift >> 0x08) & 0x01;
		m_bits 		= 0;
		m_shift 	= 0;
		m_length	+= 1;
	}
	
	m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 4;

	return;
}

void
manchester_t::process_logic_zero(void)
{
	m_bits++;
	m_shift = (m_shift >> 1);

	if (9 <= m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity	<<= 1;
		m_parity	|= (m_shift >> 0x08) & 0x01;
		m_bits		= 0;
		m_shift		= 0;
		m_length	+= 1;
	}

	m_end = m_start + 8 * (9 * m_length + m_bits + 1);

	return;
}

bool
manchester_t::is_sof(uint16_t b, uint16_t& off)
{
	off = 0xFFFF;
   
	if (0x7000 == (b & 0x7700))
		off = 7;
	else if (0x3800 == (b & 0x3B80))
		off = 6;
	else if (0x1C00 == (b & 0x1DC0))
		off = 5;
	else if (0x0E00 == (b & 0x0EE0))
		off = 4;
	else if (0x0700 == (b & 0x0770))
		off = 3;
	else if (0x0380 == (b & 0x03B8))
		off = 2;
	else if (0x01C0 == (b & 0x01DC))
		off = 1;
	else if (0x00E0 == (b & 0x00EE))
		off = 0;
        
	if (0xFFFF != off)
		return true;
        
	return false;
}   

bool
manchester_t::is_eof(uint16_t b)
{
	if (0x00 == (b & 0xF0 >> 4) && 0x00 == (b & 0x0F))
		return true; 
        
	return false;
}


bool
manchester_t::decode_all(std::vector< uint8_t >& vec, uint16_t offset)
{
	//uint32_t 	time(0);
	uint8_t*	ptr(nullptr);
	uint8_t*	end(nullptr);
	uint8_t		prior(0x00);
	uint8_t		byte(0x00);
	//uint32_t	dword(0x00);

	if (0 == vec.size())
		return false;

	ptr = &vec[0];
	end	= &vec[vec.size() - 1];

	m_output.clear();

	/*std::map< std::size_t, std::size_t > coms;

	for (std::size_t idx = 1; idx < vec.size()-4; idx++) {
		std::size_t start = 0;
		std::size_t stop = 0;
		uint16_t	sync = 0;

		dword = 0;

		for (uint8_t cnt = 0; cnt < 4; cnt++) {
			byte 	= ((vec.at(idx - 1 + cnt) << 4) | (vec.at(idx+cnt) & 0x0F));
			dword	= (dword << 8) | byte;
	//		dword 	<<= 4;
	//		dword	|= byte;
		}
	
		if (true == is_sof((uint16_t)dword, sync)) {
			start = (0 == (idx & 0x01) ? idx : idx+1); //(idx == 1 ? 0 : idx - 1);
			dword = 0;

			for (std::size_t e = start+4; e < vec.size()-4; e++) {
				for (std::size_t cnt = 0; cnt < 4; cnt++) {
					byte = ((vec.at(e - 1 + cnt) << 4) | (vec.at(e+cnt) & 0x0F));
					dword <<= 4;
					dword |= byte;
				}

				if (is_eof((uint16_t)dword)) {
					uint16_t word(0), word2(0);

					for (std::size_t w = start+1; w < start+4; w++) {
						byte = ((vec.at(w - 1) << 4) | (vec.at(w) & 0x0F));
						word <<= 4;
						word |= byte;
					}
					stop = e; //(e + 4 - sync - 1); //(0 == (e & 0x01) ? e : e+1);
					coms.insert(std::pair< std::size_t, std::size_t >(start, stop));

					for (std::size_t w = e+1; w < e+4; w++) {
						byte = ((vec.at(w - 1) << 4) | (vec.at(w) & 0x0F));
						word2 <<= 4;
						word2 |= byte;
					}

					//printf("start: %lx (%.4x) stop: %lx (%.04x)\n", start, word, stop, word2);
					idx = e+4;
					dword = 0;
					break;
				}
			}
		}
		
		dword = 0;*/
//		bits = dword;
//		std::cout << to_hex_string(dword) << " " << to_hex_string((uint16_t)dword) << " " << bits << std::endl;
//		dwvec.push_back(dword);
//		dword = 0;
	//}

//	for (std::size_t idx = 0; idx < dwvec.size(); idx++)
//		printf("%x\n", dwvec.at(idx));

	for (uint32_t time = 0; ptr < end; time++) {
		if (time & 0x01) {
			//printf("two_bits: %.04x\n", m_two_bits);
			byte = ((prior << 4) | (*ptr & 0x0F));
			m_two_bits = (m_two_bits << 8) | byte;

			//printf("two_bits: %.04x\n", m_two_bits);

			if (MANCHESTER_STATE_UNSYNCHED == m_state) {
				if (2 > m_high) {
					if (0x0000 == m_two_bits) {
						m_high++;
					} else {
						m_high = 0;
					}
				} else if (has_sync()) {
					m_start	= (time-1)*4;
					m_start	-= m_sync_bit;
					m_bits	= offset;
					m_state	= MANCHESTER_STATE_DATA;
					//printf("decode_all(): %x: %#x (%.04hx)\n", time-1, m_sync_bit, m_two_bits);
				}
			} else {
				if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit))
					process_logic_one();
				else if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit))
					process_logic_zero();
				else {
					if (0 < m_length || 0 < m_bits) {
						if (0 < m_bits) {
							m_shift >>= (9 - m_bits);
							m_output.push_back(m_shift & 0xFF);
							m_parity <<= 1;
							m_length++;
						}

						//printf("START: %#x\n", m_start/4);
						//printf("END: %#x\n", m_end/4);
						break;
					} else {
						reset();
					}
				}
			}
		}

		prior = *ptr;
		ptr++;
	}

	return true;
}

std::size_t g_count;

bool
manchester_t::decode_bits(uint8_t bit, uint16_t offset, uint32_t time)
{
	uint8_t last(m_two_bits & 0xFF);

	m_two_bits = (m_two_bits << 8) | bit;
	
	if (MANCHESTER_STATE_UNSYNCHED == m_state) {
		if (2 > m_high) {
			if (0x0000 == m_two_bits)
				m_high++;
			else
				m_high = 0;
		} else {
			if (has_sync()) {
				m_start 	= time;
				m_start		-= m_sync_bit;
				m_bits		= offset;
				m_state		= MANCHESTER_STATE_DATA;

				m_input.push_back(last);
				m_input.push_back(bit);

				m_output.clear();

			} else
				m_input.clear();
		}
	} else {
		m_input.push_back(bit);

		if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit)) {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				if (0 == m_collision_pos) 	
					m_collision_pos = (m_length << 3) + m_bits;
			}

			m_bits++;
			m_shift = (m_shift >> 1) | 0x100;

			if (9 == m_bits) {
				m_output.push_back(m_shift & 0xFF);
				m_parity 	<<= 1;
				m_parity 	|= ((m_shift >> 8) & 0x01);
				m_bits 		= 0;
				m_shift 	= 0;
				m_length++;
			}

			m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 4;
		} else {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				m_bits++;
				m_shift = (m_shift >> 1);

				if (9 <= m_bits) {
					m_output.push_back(m_shift & 0xFF);
					m_parity 	<<= 1;
					m_parity 	|= ((m_shift >> 8) & 0x01);
					m_bits 		= 0;
					m_shift 	= 0;
					m_length++;
				}

				m_end = m_start + 8 * (9 * m_length + m_bits + 1);
			} else {
				if (0 < m_length || 0 < m_bits) {
					if (0 < m_bits) {
						m_shift >>= (9 - m_bits);
						m_output.push_back(m_shift & 0xFF);
						m_parity <<= 1;
					}

					return true;
				} else 
					reset();
			}
		}
	}

	return false;
}

bool 
manchester_t::decode(uint8_t bit, uint16_t offset, uint32_t time)
{
	//printf("m_two_bits %.04x bit: %.02x\n", m_two_bits, bit);

	m_two_bits = (m_two_bits << 8) | bit;

	g_count++;

	if (MANCHESTER_STATE_UNSYNCHED == m_state) {
		if (2 > m_high) {
			if (0x0000 == m_two_bits)
				m_high++;
			else 
				m_high = 0;

		} else {
			//printf("g_count: %lu\n", g_count);
			g_count = 0;

			m_sync_bit = 0xFFFF;

			// 0xFFE0
			if (0x7000 == (m_two_bits & 0x7700)) 
				m_sync_bit = 7;
			else if (0x3800 == (m_two_bits & 0x3B80))
				m_sync_bit = 6;
			else if (0x1C00 == (m_two_bits & 0x1DC0))
				m_sync_bit = 5;
			else if (0x0E00 == (m_two_bits & 0x0EE0))
				m_sync_bit = 4;
			else if (0x0700 == (m_two_bits & 0x0770))
				m_sync_bit = 3;
			else if (0x0380 == (m_two_bits & 0x03B8))
				m_sync_bit = 2;
			else if (0x01C0 == (m_two_bits & 0x01DC))
				m_sync_bit = 1;
			else if (0x00E0 == (m_two_bits & 0x00EE))
				m_sync_bit = 0;

			if (0xFFFF != m_sync_bit) {
				m_start 	= (time-1)*4;
				m_start		-= m_sync_bit;
				m_bits		= offset;
				m_state		= MANCHESTER_STATE_DATA;
			}
		}
	} else {
		if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit)) {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) 
				if (0 == m_collision_pos) 
					m_collision_pos = (m_length << 3) + m_bits;

			m_bits++;
			m_shift = (m_shift >> 1) | 0x100;

			if (9 == m_bits) {
				m_output.push_back(m_shift & 0xFF);
				m_parity <<= 1;
				m_parity |= (m_shift >> 0x08) & 0x01;
				m_bits = 0;
				m_shift = 0;
				m_length++;
			}

			m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 4;

		} else {
			if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit)) {
				m_bits++;
				m_shift = (m_shift >> 1);

				if (9 <= m_bits) {
					m_output.push_back(m_shift & 0xFF);
					m_parity <<= 1;
					m_parity |= (m_shift >> 0x08) & 0x01;
					m_bits = 0;
					m_shift = 0;
					m_length++;
				}

				m_end = m_start + 8 * (9 * m_length + m_bits + 1);
			} else {

				if (0 < m_length || 0 < m_bits) {
					if (0 < m_bits) {
						m_shift >>= (9 - m_bits);
						m_output.push_back(m_shift & 0xFF);
						m_parity <<= 1;
						m_length++;
					}

					return true;
				} else
					reset();
			}
		}
	}

	return false;
}
