#ifndef HAVE_MILLER_T_HPP
#define HAVE_MILLER_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <array>

#include <iostream>
#include "hexdump.hpp"

typedef enum {
	MILLER_STATE_UNSYNC = 1,
	MILLER_STATE_START_OF_COMMUNICATION,
	MILLER_STATE_X,
	MILLER_STATE_Y,
	MILLER_STATE_Z
} miller_state_t;

typedef std::array< bool, 16 > miller_lut_t;

class miller_t {
	private:
		miller_state_t 			m_state;
		uint16_t				m_shift;
		uint16_t				m_bits;
		uint16_t				m_length;
		uint16_t				m_max_bytes;
		uint16_t				m_pos_cnt;
		uint16_t				m_sync_bit;
		uint32_t				m_parity;
		uint16_t				m_high;
		uint16_t				m_two_bits;
		uint32_t				m_start;
		uint32_t				m_end;
		std::vector< uint8_t >	m_output;
		std::vector< uint8_t >	m_input;
		const miller_lut_t		m_lut;

	protected:
		inline miller_lut_t init_lut(void);

		bool is_mod_nibble_top(uint16_t);
		bool is_mod_nibble_bottom(uint16_t);
		bool has_sync(void);
		void decode_top_nibble(void);
		void decode_bottom_nibble(void);
		bool is_modulated_both_halves(void);
		void process_logic_zero(void);
		void process_logic_one(void);

	public:
		miller_t(void);
		~miller_t(void);

		std::vector< uint8_t >& input(void) { return m_input; }
		inline bool is_active(void) { return m_state != MILLER_STATE_UNSYNC; }
		void reset(void);
		bool decode_bits(uint8_t, uint32_t);
		bool decode(uint8_t, uint32_t);
		bool decode_all(std::vector< uint8_t >&);

		std::vector< uint8_t >& output(void) { return m_output; }
		bool has_output(void) { return 0 != m_output.size(); }
		void clear(void) { m_output.clear(); m_length = 0; return; }

		inline miller_state_t state(void) { return m_state; }

};

#endif
