#include "miller.hpp"

inline miller_lut_t
miller_t::init_lut(void)
{
	miller_lut_t retval({	
							true, true, false, true, false, false, false, false,
							true, true, false, false, true, false, false, false
						});	

	return retval;
}

void
miller_t::reset(void)
{
	m_state 	= MILLER_STATE_UNSYNC;
	m_bits 		= 0;
	m_shift 	= 0;
	m_length	= 0;
	m_parity 	= 0;
	m_two_bits 	= 0;
	m_high 		= 0;
	m_start 	= 0;
	m_end 		= 0;

	m_input.clear();
	m_output.clear();

	return;
}

inline bool
miller_t::is_mod_nibble_top(uint16_t b)
{
	return m_lut[(b & 0x00F0) >> 4];
}

inline bool
miller_t::is_mod_nibble_bottom(uint16_t b)
{
	return m_lut[b & 0x000F];
}

miller_t::miller_t(void) : m_lut(init_lut()) 
{ 
	reset(); 
	return; 
}

miller_t::~miller_t(void) 
{ 
	m_output.clear(); 
	reset(); 
	return; 
}

bool
miller_t::has_sync(void)
{
	m_sync_bit = 0xFFFF;

	// 0x7FF0
	if (0x0780 == (m_two_bits & 0x6780))
		m_sync_bit = 7;
	else if (0x03C0 == (m_two_bits & 0x33C0))
		m_sync_bit = 6;
	else if (0x01E0 == (m_two_bits & 0x19E0))
		m_sync_bit = 5;
	else if (0x00F0 == (m_two_bits & 0x0CF0))
		m_sync_bit = 4;
	else if (0x0078 == (m_two_bits & 0x0678))
		m_sync_bit = 3;
	else if (0x003C == (m_two_bits & 0x033C))
		m_sync_bit = 2;
	else if (0x001E == (m_two_bits & 0x019E))
		m_sync_bit = 1;
	else if (0x00CF == (m_two_bits & 0x000F))
		m_sync_bit = 0;

	if (0xFFFF != m_sync_bit)
		return true;

	return false;
}

bool
miller_t::is_modulated_both_halves(void)
{
	if (is_mod_nibble_top(m_two_bits >> m_sync_bit) && is_mod_nibble_bottom(m_two_bits >> m_sync_bit))
		return true;

	return false;
}

void
miller_t::decode_top_nibble(void)
{
	
	if (MILLER_STATE_X == m_state) {
		reset();
		m_high = 6;
	}

	m_bits++;
	m_shift = (m_shift >> 1);
	m_state = MILLER_STATE_Z;
	m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 6;

	if (9 <= m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity 	<<= 1;
		m_parity 	|= ((m_shift >> 8) & 0x01);
		m_bits 		= 0;
		m_shift 	= 0;
		m_length	+= 1;
	}

	return;
}

void
miller_t::decode_bottom_nibble(void)
{
	m_bits++;
	m_shift = (m_shift >> 1) | 0x100;
	m_state = MILLER_STATE_X;
	m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 2;

	if (9 <= m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity 	<<= 1;
		m_parity 	|= ((m_shift >> 8) & 0x01);
		m_bits 		= 0;
		m_shift 	= 0;
		m_length 	+= 1;

	}

	return;
}

bool
miller_t::decode_bits(uint8_t bit, uint32_t time)
{
	uint8_t last(m_two_bits & 0xFF);

	m_two_bits = (m_two_bits << 8) | bit;

	//m_input.push_back(bit);

	if (MILLER_STATE_UNSYNC == m_state) {
		if (7 > m_high) {
			if (0xFFFF == m_two_bits)
				m_high++;
			else
				m_high = 0;
		} else {
			if (true == has_sync()) {
				m_start = time;
				m_start -= m_sync_bit;
				m_end   = m_start;
				m_state = MILLER_STATE_START_OF_COMMUNICATION;

				m_output.clear();
				m_input.push_back(last);
				m_input.push_back(bit);
			} else
				m_input.clear();

		}
	} else {
		m_input.push_back(bit);

		if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit)) {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				reset();
				m_high = 6;
			} else {
				if (MILLER_STATE_X == m_state) {
					reset();
					m_high = 6;
				} else {
					m_bits++;
					m_shift = (m_shift >> 1);
					m_state = MILLER_STATE_Z;
					m_end   = m_start + 8 * (9 * m_length + m_bits + 1) - 6;

					if (9 <= m_bits) {
						m_output.push_back(m_shift & 0xFF);
						m_parity    <<= 1;
						m_parity    |= ((m_shift >> 8) & 0x01);
						m_bits		= 0;
						m_shift		= 0;
					}
				}
			}
		 } else {
		 	if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				m_bits++;
				m_shift 	= (m_shift >> 1) | 0x100;
				m_state		= MILLER_STATE_X;
				m_end		= m_start + 8 * (9 * m_length + m_bits + 1) - 2;

				if (9 <= m_bits) {
					m_output.push_back(m_shift & 0xFF);
					m_parity 	<<= 1;
					m_parity 	|= ((m_shift >> 8) & 0x01);
					m_bits 		= 0;
					m_shift 	= 0;
				}
			} else {
				if (MILLER_STATE_Z == m_state || MILLER_STATE_Y == m_state) {
					m_state = MILLER_STATE_UNSYNC;

					if (0 == m_length && 0 < m_bits) {
						m_shift 	>>= (9 - m_bits);
						
						if ((m_shift & 0xFF)) {
							m_output.push_back(m_shift & 0xFF);
							m_length++;
						}
						m_parity	<<= 1;
						m_bits--; 
					}

					return true;
				}

				if (MILLER_STATE_START_OF_COMMUNICATION == m_state) {
					reset();
					m_high = 6;
				} else {
					m_bits++;
					m_shift = (m_shift >> 1);
					m_state = MILLER_STATE_Y;

					if (9 <= m_bits) {
						m_output.push_back(m_shift & 0xFF);
						m_parity 	<<= 1;
						m_parity 	|= ((m_shift >> 8) & 0x01);
						m_bits 		= 0;
						m_shift 	= 0;
					}
				}
			}
		}
	}

	return false;
}

bool
miller_t::decode_all(std::vector< uint8_t >& vec)
{
	//uint32_t 	time(0);
	uint8_t*	ptr(nullptr);
	uint8_t*	end(nullptr);
	uint8_t		prior(0x00);
	uint8_t		byte(0x00);

	if (0 == vec.size())
		return false;
	
	ptr = &vec[0];
	end = &vec[vec.size() - 1];

	m_output.clear();

	for (uint32_t time = 0; ptr < end; time++) {
		if (time & 0x01) {
			byte = ((prior & 0xF0) | (*ptr >> 4));
			m_two_bits = (m_two_bits << 8) | byte;

			if (MILLER_STATE_UNSYNC == m_state) {
					if (7 > m_high) {
						if (0xFFFF == m_two_bits)
							m_high++;
						else
							m_high = 0;
					} else {
						if (true == has_sync()) {	
							m_start = (time - 1)*4;
							m_start -= m_sync_bit;
							m_end   = m_start;
							m_state = MILLER_STATE_START_OF_COMMUNICATION;
						}
					}

			} else {
				if (true == is_modulated_both_halves()) {
					reset();
					m_high = 6;
					continue;
				}

				if (is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) 
					process_logic_one();
				else if (is_mod_nibble_top(m_two_bits >> m_sync_bit)) 
					process_logic_zero();
				else {
					switch (m_state) {
						case MILLER_STATE_Y:
						case MILLER_STATE_Z:

							m_state = MILLER_STATE_UNSYNC;

							if (0 == m_length && 0 != m_bits) {
								m_shift >>= (9 - m_bits);
								m_output.push_back(m_shift & 0xFF);
								m_parity <<= 1;
								m_bits--;
								m_length += 1;
							}

							break;
						case MILLER_STATE_START_OF_COMMUNICATION:
							reset();
							m_high = 6;

							break;

						default:
							m_bits += 1;
							m_shift >>= 1;
							m_state = MILLER_STATE_Y;

							if (9 <= m_bits) {
								m_output.push_back(m_shift & 0xFF);
								m_parity	<<= 1;
								m_parity 	|= ((m_shift >> 8) & 0x01);
								m_bits 		= 0;
								m_shift 	= 0;
								m_length 	+= 1;
							}
							break;
					}
				}
			}
		}

		prior = *ptr;
		ptr++;
	}	

	return true;
}

void
miller_t::process_logic_one(void)
{

	m_bits++;
	m_shift = (m_shift >> 1) | 0x100;
	m_state = MILLER_STATE_X;
	m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 2;
		
	if (9 <= m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity	<<= 1;
		m_parity 	|= ((m_shift >> 8) & 0x01);
		m_bits 		= 0;
		m_shift 	= 0;
	}

	return;
}

void
miller_t::process_logic_zero(void)
{
	if (MILLER_STATE_X == m_state) {
		reset();
		m_high = 6;
		return;
	}

	m_bits += 1;
	m_shift = (m_shift >> 1);
	m_state = MILLER_STATE_Z;
	m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 6;

	if (9 <= m_bits) {
		m_output.push_back(m_shift & 0xFF);
		m_parity	<<= 1;
		m_parity 	|= ((m_shift >> 8) & 0x01);
		m_bits 		= 0;
		m_shift 	= 0;
		m_length 	+= 1;
	}

	return;
}

bool 
miller_t::decode(uint8_t bit, uint32_t time)
{
	m_two_bits = (m_two_bits << 8) | bit;

	if (MILLER_STATE_UNSYNC == m_state) {
		if (7 > m_high) {
			if (0xFFFF == m_two_bits)
				m_high++;
			else 
				m_high = 0;
		} else {
			m_sync_bit = 0xFFFF;

			// 0x7FF0
			if (0x0780 == (m_two_bits & 0x6780)) 
				m_sync_bit = 7;
			else if (0x03C0 == (m_two_bits & 0x33C0))
				m_sync_bit = 6;
			else if (0x01E0 == (m_two_bits & 0x19E0))
				m_sync_bit = 5;
			else if (0x00F0 == (m_two_bits & 0x0CF0))
				m_sync_bit = 4;
			else if (0x0078 == (m_two_bits & 0x0678))
				m_sync_bit = 3;
			else if (0x003C == (m_two_bits & 0x033C))
				m_sync_bit = 2;
			else if (0x001E == (m_two_bits & 0x019E))
				m_sync_bit = 1;
			else if (0x000F == (m_two_bits & 0x00CF))
				m_sync_bit = 0;

			if (0xFFFF != m_sync_bit) {
				m_start = time; //(time-1)*4;
				m_start -= m_sync_bit;
				m_end	= m_start;
				m_state = MILLER_STATE_START_OF_COMMUNICATION;
			}
		}
	} else {
		if (true == is_mod_nibble_top(m_two_bits >> m_sync_bit)) {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				reset();
				m_high = 6;
			} else {
				if (MILLER_STATE_X == m_state) {
					reset();
					m_high = 6;
				} else {
					m_bits++;
					m_shift = (m_shift >> 1);
					m_state = MILLER_STATE_Z;
					m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 6;

					if (9 <= m_bits) {
						m_output.push_back(m_shift & 0xFF);
						m_parity <<= 1;
						m_parity |= ((m_shift >> 8) & 0x01);
						m_bits = 0;
						m_shift = 0;
						m_length++;

					}
				}
			}
		} else {
			if (true == is_mod_nibble_bottom(m_two_bits >> m_sync_bit)) {
				m_bits++;
				m_shift = (m_shift >> 1) | 0x100;
				m_state = MILLER_STATE_X;
				m_end = m_start + 8 * (9 * m_length + m_bits + 1) - 2;

				if (9 <= m_bits) {
					m_output.push_back(m_shift & 0xFF);
					m_parity <<= 1;
					m_parity |= ((m_shift >> 8) & 0x01);
					m_bits = 0;
					m_shift = 0;
					m_length++;
				}
			} else {
				if (MILLER_STATE_Z == m_state || MILLER_STATE_Y) {
					m_state = MILLER_STATE_UNSYNC;

					if (0 == m_length && 0 < m_bits) {
						m_shift >>= (9 - m_bits);
						m_output.push_back(m_shift & 0xFF);
						m_parity <<= 1;
						m_bits--;
						m_length++;
					}

					return true;
				}

				if (MILLER_STATE_START_OF_COMMUNICATION == m_state) {
					reset();
					m_high = 6;
				} else {
					m_bits++;
					m_shift = (m_shift >> 1);
					m_state = MILLER_STATE_Y;

					if (9 <= m_bits) {
						m_output.push_back(m_shift & 0xFF);
						m_parity <<= 1;
						m_parity |= ((m_shift >> 0x08) & 0x01);
						m_bits = 0;
						m_shift = 0;
						m_length++;
					}
				}
			}
		}
	}

	return false;
}
