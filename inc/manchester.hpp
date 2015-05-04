#ifndef HAVE_MANCHESTER_T_HPP
#define HAVE_MANCHESTER_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <array>

#include <iostream>
#include <bitset>
#include <map>
#include "util.hpp"
#include "hexdump.hpp"

typedef enum {
	MANCHESTER_STATE_UNSYNCHED = 1,
	// MANCHESTER_STATE_HALF_SYNCHED, 
	// MANCHESTER_STATE_MOD_FIRST_HALF,
	// MANCHESTER_STATE_NOMOD_FIRST_HALF
	MANCHESTER_STATE_DATA
} manchester_state_t;

typedef std::array< bool, 16 > manchester_lut_t;

class manchester_t {

	private:
		manchester_state_t		m_state;
		uint16_t				m_two_bits;
		uint16_t				m_high;
		uint16_t				m_bits;
		uint16_t				m_collision_pos;
		uint16_t				m_sync_bit;
		uint32_t				m_parity;
		uint16_t				m_shift;
		uint16_t				m_samples;
		uint16_t				m_length;
		uint32_t				m_start;
		uint32_t				m_end;
		std::vector< uint8_t >	m_output;
		std::vector< uint8_t >	m_input;
		const manchester_lut_t	m_lut;

	protected:
		inline manchester_lut_t init_lut(void);

		bool is_mod_nibble_top(uint16_t);
		bool is_mod_nibble_bottom(uint16_t);
		static bool has_sync(uint16_t);
		bool has_sync(void);
		bool is_modulated_both_halves(void);
		void process_logic_zero(void);
		void process_logic_one(void);

		bool is_sof(uint16_t, uint16_t&);
		bool is_eof(uint16_t);

	public:
		manchester_t(void);
		~manchester_t(void);

		std::vector< uint8_t >& input(void) { return m_input; }

		inline bool is_active(void) { return m_state != MANCHESTER_STATE_UNSYNCHED; }
		void reset(void);
		bool decode_bits(uint8_t, uint16_t, uint32_t);
		bool decode_all(std::vector< uint8_t >&, uint16_t offset = 0);
		bool decode(uint8_t, uint16_t, uint32_t);
		std::vector< uint8_t >& output(void) { return m_output; }
		bool has_output(void) { return 0 != m_output.size(); }
		void clear(void) { m_output.clear(); m_length = 0; return; }
		inline manchester_state_t state(void) { return m_state; }
};

#endif
