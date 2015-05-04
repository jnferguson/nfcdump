#include "decoder.hpp"

decoder_t::decoder_t(void) 
{
	reset();

	return;
}

decoder_t::decoder_t(std::vector< uint8_t >& v) : m_data(v) 
{
	reset();

	return;
}

decoder_t::~decoder_t(void)
{
	reset();

	return;
}

decoder_t::decoder_t(std::string & n)
{
	ncap_t 				ncap;
	ncap_record_list_t	list;

	ncap.read(n, list);

	for (auto& item : list) { 
		// NCAP_MODULATED_RECORD
		if (NCAP_MODULATED_RECORD == item.type())
			m_data.insert(m_data.end(), item.data().begin(), item.data().end());

	}

	return;
}

decoder_t::decoder_t(const char* n)
{
	ncap_t 				ncap;
	ncap_record_list_t	list;

	ncap.read(n, list);

	for (auto& item : list) {
		if (NCAP_MODULATED_RECORD == item.type())
			m_data.insert(m_data.end(), item.data().begin(), item.data().end());
	}

	return;
}

inline void
decoder_t::reset(void)
{
	m_data.clear();
	m_output.clear();
	m_decoded.clear();

	m_miller.reset();
	m_manchester.reset();

	return;
}

void 
decoder_t::set_data(std::vector< uint8_t >& v) 
{
	m_output.clear();
	m_data = v;

	return;
}

inline std::vector< uint8_t > 
decoder_t::get_data(void)
{
	return m_output;
}


bool
decoder_t::parse(void)
{
	return parse(m_data, m_decoded, m_miller, m_manchester);
}

bool
decoder_t::parse(std::vector< uint8_t >& v)
{
	m_data = v;
	return parse(m_data, m_decoded, m_miller, m_manchester);
}

bool
decoder_t::parse(std::vector< uint8_t >& src, std::vector< decoded_t >& dvec, miller_t& mi, manchester_t& ma)
{
	uint8_t 			prior(0x00);
	uint8_t 			byte(0x00);
	hexdump_t			hex;
	otfta_parse_t		parser;
	bool				readerActive(false);
	bool				tagActive(false);

	mi.clear();
	mi.reset();
	ma.clear();
	ma.reset();

	for (std::size_t idx = 0; idx < src.size(); idx++) {
		if (idx & 0x01) {
			if (false == tagActive) {
				byte = ((prior & 0xF0) | (src.at(idx) >> 4));		
				if (true == mi.decode_bits(byte, (idx - 1 ) * 4)) {
					if (mi.has_output()) {
						dvec.push_back(decoded_t(mi.output(), mi.input(), DECODED_MILLER_T));
					}
					mi.clear();
					ma.clear();
					mi.reset();
					ma.reset();
				}

				readerActive = (MILLER_STATE_UNSYNC != mi.state());
			}

			if (false == readerActive) {
				byte = ((prior << 4) | (src.at(idx) & 0x0F));
				
				if (true == ma.decode_bits(byte, 0, (idx - 1) * 4)) {
					if (true == ma.has_output()) {
						dvec.push_back(decoded_t(ma.output(), ma.input(), DECODED_MANCHESTER_T));
						//std::vector< uint8_t > 	v(ma.output());
						//std::cout << "MANCHESTER: " << std::endl << parser.to_string(v) << std::endl << std::endl;
						//hex.set(ma.input());
						//std::cout << "MODULATED: " << std::endl << hex.to_string() << std::endl << std::endl;
					}
					
					ma.clear();
					mi.clear();
					ma.reset();
					mi.reset();
				}

				tagActive = (MANCHESTER_STATE_UNSYNCHED != ma.state());
			}
		}

		prior = src.at(idx);
	}

	mi.clear();
	mi.reset();
	ma.clear();
	ma.reset();
	return true;
}

std::string 
decoder_t::to_string(decoded_t& rec, otfta_state_t& s)
{
	otfta_parse_t	parser;
	hexdump_t		hex;
	std::string		ret("");

	parser.set_state(s);

	if (1 > rec.demodulated.size())
		throw std::runtime_error("decoder_t::to_string(): Invalid length encountered while parsing demodulated record");

	hex.set(rec.modulated);
	ret = parser.to_string(rec.demodulated) + "\r\n\r\n" + hex.to_string();

	if (DECODED_MILLER_T == rec.type) {
		switch (rec.demodulated.at(0)) {
			case REQA_VALUE:
			case WUPA_VALUE:
				parser.set_state(STATE_XMIT_REQWUP);
				//parser.set_state(STATE_SELC_ONE);
				break;

			case ACCO_VALUE: // 0x93
				if (2 > rec.demodulated.size())
					break;
				if (2 == rec.demodulated.size())   // 0x93 0x20
					parser.set_state(STATE_SELC_ONE_CONTINUE);
				else if (2 < rec.demodulated.size())
					parser.set_state(STATE_SELC_TWO);

				break;

			case ACCW_VALUE: // 0x95
				if (2 > rec.demodulated.size())
					break;
				else if (2 == rec.demodulated.size()) // 0x93 0x20
					parser.set_state(STATE_SELC_TWO_CONTINUE);
				else if (2 < rec.demodulated.size())
					parser.set_state(STATE_SELC_THREE);

				break;

			case ACCT_VALUE:
				parser.set_state(STATE_XMIT_RATS);
				break;

			case RATS_VALUE:
			case HALT_VALUE:
				parser.set_state(STATE_XMIT_REQWUP);
				break;
			default:
				break;
		}
	}

	s = parser.get_state();
	return ret;
}
