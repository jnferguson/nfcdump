#ifndef HAVE_OTFTA_PARSE_T_HPP
#define HAVE_OTFTA_PARSE_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdexcept>

#include "otfta.hpp"
#include "util.hpp"
#include "hexdump.hpp"

class otfta_parse_t
{
	private:
		otfta_state_t	m_state;
		hexdump_t		m_hex;
		bool			m_hcol;

	protected:

		static inline bool is_valid_idx(std::size_t idx, std::size_t max) { return idx < max; }
	
		virtual std::string parse_reqwup(std::vector< uint8_t >&);
		virtual std::string parse_atqa(std::vector< uint8_t >&);
		virtual std::string parse_nvb(uint8_t);
		virtual std::string parse_selanc(std::vector< uint8_t >&);
		virtual std::string parse_saka(std::vector< uint8_t >&);
		virtual std::string parse_rats(std::vector< uint8_t >&);
		virtual std::string parse_halt(std::vector< uint8_t >&);
		virtual std::string parse_other(std::vector< uint8_t >&);
		virtual std::string cmd(std::vector< uint8_t >&);

	public:
		otfta_parse_t(void) : m_state(STATE_XMIT_REQWUP), m_hcol(false) { return; }
		virtual ~otfta_parse_t(void) { return; }
		
		virtual std::string 
		to_string(std::vector< uint8_t >& v)
		{
			std::string r(cmd(v));
			r += "\r\n";
			m_hex.set(v);
			r += m_hex.to_string();
			return r;
		}

		virtual otfta_state_t get_state(void) { return m_state; }
		virtual void set_state(otfta_state_t s) { m_state = s; return; }

		
};

#endif
