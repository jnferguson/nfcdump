#ifndef HAVE_DECODER_T_HPP
#define HAVE_DECODER_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <list>
#include <vector>

#include "miller.hpp"
#include "manchester.hpp"
#include "ncap.hpp"
#include "parse.hpp"

#define DECODED_MILLER_T 0x01
#define DECODED_MANCHESTER_T 0x02

typedef struct _decoded_t {
	std::vector< uint8_t > 	demodulated;
	std::vector< uint8_t > 	modulated;
	uint8_t					type;
	_decoded_t(std::vector< uint8_t >& d, std::vector< uint8_t >& m, uint8_t t = DECODED_MILLER_T) : demodulated(d), modulated(m), type(t) {}
	~_decoded_t(void) { demodulated.clear(); modulated.clear(); return; }
} decoded_t;

class decoder_t
{
	private:
		std::vector< uint8_t >		m_data;
		std::vector< uint8_t >		m_output;
		std::vector< decoded_t >	m_decoded;
		std::size_t					m_offset;
		miller_t					m_miller;
		manchester_t				m_manchester;

	protected:
	public:
		decoder_t(void);
		decoder_t(std::vector< uint8_t >&);
		decoder_t(std::string&);
		decoder_t(const char*);
		~decoder_t(void);

		inline void reset(void);

		bool parse(void);
		bool parse(std::vector< uint8_t >&);
		static bool parse(std::vector< uint8_t >&, std::vector< decoded_t >&, miller_t&, manchester_t&);

		void set_data(std::vector< uint8_t >&); 
		std::vector< uint8_t > get_data(void);
		inline std::vector< decoded_t > get_decoded(void) { return m_decoded; }
		std::string to_string(decoded_t&, otfta_state_t&);

};

#endif
