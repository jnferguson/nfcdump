#include "sest.hpp"

void
class_byte_t::parse(bool validate) 
{
	if (0xFF == m_class && true == validate)
		throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (0xFF)");

	if (0x80 == (m_class & 0x80))
		m_propietary = true;
			
	if (0x00 == (m_class & 0x40))
		m_fi_inter = true;
	else
		m_fu_inter = true;
			
	if ((FICLA_LAST_ONLY_CHAIN_CMD == (m_class & FICLA_LAST_ONLY_CHAIN_CMD)) ||
		FUCLA_LAST_ONLY_CHAIN_CMD == (m_class & FUCLA_LAST_ONLY_CHAIN_CMD)) {
			m_last_only = true;

			if (true == validate) {
				if ((FICLA_LAST_ONLY_CHAIN_CMD == (m_class & FICLA_LAST_ONLY_CHAIN_CMD)) && true == m_fu_inter)
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_LAST+Further)");
				else if (FUCLA_LAST_ONLY_CHAIN_CMD == (m_class & FUCLA_LAST_ONLY_CHAIN_CMD) && true == m_fi_inter)
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FUCLA_LAST+First");
				else if ((FICLA_LAST_ONLY_CHAIN_CMD == (m_class & FICLA_LAST_ONLY_CHAIN_CMD)) && 
						(FUCLA_LAST_ONLY_CHAIN_CMD == (m_class & FUCLA_LAST_ONLY_CHAIN_CMD)))
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_LAST+FUCLA_LAST)");
			}
	}

	if ((FICLA_MORE_CHAIN_CMD == (m_class & FICLA_MORE_CHAIN_CMD)) ||
		FUCLA_MORE_CHAIN_CMD == (m_class & FUCLA_MORE_CHAIN_CMD)) {
			m_more_chain = true;
			
			if (true == validate) {
				if ((FICLA_MORE_CHAIN_CMD == (m_class & FICLA_MORE_CHAIN_CMD)) && true == m_fu_inter)
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_MORE+Further)");
				else if ((FUCLA_MORE_CHAIN_CMD == (m_class & FUCLA_MORE_CHAIN_CMD)) && true == m_fi_inter)
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FUCLA_MORE+First)");
				else if ((FICLA_MORE_CHAIN_CMD == (m_class & FICLA_MORE_CHAIN_CMD)) && (FUCLA_MORE_CHAIN_CMD == (m_class & FUCLA_MORE_CHAIN_CMD)))
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_MORE_CHAIN_CMD+FUCLA_MORE_CHAIN_CMD)");
			}
	}

	if (true == validate && true == m_last_only && true == m_more_chain) 
		throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (Last/Only+More)");

	if ((FICLA_NO_SMA == (FICLA_NO_SMA & m_class)) || (FUCLA_NO_SMA == (FUCLA_NO_SMA & m_class))) {
		m_no_sma = true;

		if (true == validate) {
			if ((FICLA_NO_SMA == (FICLA_NO_SMA & m_class)) && true == m_fu_inter)
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_NO_SMA+Further)");
			else if ((FUCLA_NO_SMA == (FUCLA_NO_SMA & m_class)) && true == m_fi_inter)
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FUCLA_NO_SMA+First");
			else if ((FUCLA_NO_SMA == (FUCLA_NO_SMA & m_class)) && (FICLA_NO_SMA == (FICLA_NO_SMA & m_class)))
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FUCLA_NO_SMA+FICLA_NO_SMA)");
		}
	}

	if ((FICLA_UNAUTH_SMA == (FICLA_UNAUTH_SMA & m_class)) || (FUCLA_UNAUTH_SMA == (FUCLA_UNAUTH_SMA & m_class))) {
		m_unauth_sma = true;

		if (true == validate) {
			if ((FICLA_UNAUTH_SMA == (FICLA_UNAUTH_SMA & m_class)) && true == m_fu_inter)
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_UNAUTH_SMA+Further)");
			else if ((FUCLA_UNAUTH_SMA == (FUCLA_UNAUTH_SMA & m_class)) && true == m_fi_inter)
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FUCLA_UNAUTH_SMA+First)");
			else if ((FICLA_UNAUTH_SMA == (FICLA_UNAUTH_SMA & m_class)) && (FUCLA_UNAUTH_SMA == (FUCLA_UNAUTH_SMA & m_class)))
				throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_UNAUTH_SMA+FUCLA_UNAUTH_SMA)");
		}
	}

	if (FICLA_PROP_SMA == (FICLA_PROP_SMA & m_class)) {
		m_prop_sma = true;

		if (true == validate && true == m_fu_inter)
			throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_PROP_SMA+Further)");
	}

	if (FICLA_AUTH_SMA == (FICLA_AUTH_SMA & m_class)) {
		m_auth_sma = true;

		if (true == validate && true == m_fu_inter)
			throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (FICLA_AUTH_SMA+Further)");
	}

	if (true == m_fi_inter) {
		m_channel = (m_class & (FICLA_LOGIC_CHANNEL_ONE|FICLA_LOGIC_CHANNEL_TWO|FICLA_LOGIC_CHANNEL_THREE));
			
		switch (m_channel) {
			case FICLA_LOGIC_CHANNEL_ONE:
			case FICLA_LOGIC_CHANNEL_TWO:
			case FICLA_LOGIC_CHANNEL_THREE:
				break;
			default:
				if (true == validate)
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (channel)");

				break;
		}
	} else {
		m_channel = (m_class & 0x03);

		switch (m_class & 0x0F) {
			case FUCLA_LOGIC_CHANNEL_NINETEEN:
			case FUCLA_LOGIC_CHANNEL_EIGHTEEN:
			case FUCLA_LOGIC_CHANNEL_SEVENTEEN:
			case FUCLA_LOGIC_CHANNEL_SIXTEEN:
			case FUCLA_LOGIC_CHANNEL_FIFTEEN:
			case FUCLA_LOGIC_CHANNEL_FOURTEEN:
			case FUCLA_LOGIC_CHANNEL_THIRTEEN:
			case FUCLA_LOGIC_CHANNEL_TWELVE:
			case FUCLA_LOGIC_CHANNEL_ELEVEN:
			case FUCLA_LOGIC_CHANNEL_TEN:
			case FUCLA_LOGIC_CHANNEL_NINE:
			case FUCLA_LOGIC_CHANNEL_EIGHT:
			case FUCLA_LOGIC_CHANNEL_SEVEN:
			case FUCLA_LOGIC_CHANNEL_SIX:
			case FUCLA_LOGIC_CHANNEL_FIVE:
			case FUCLA_LOGIC_CHANNEL_FOUR:
				break;
			default:
				if (true == validate) 
					throw std::runtime_error("class_byte_t::parse(): Class byte value is invalid (channel)");
				break;

		}
	}

	m_is_set = true;
	return;
}

class_byte_t::class_byte_t(void) : m_class(0x00), m_is_set(false), m_channel(0)
{
	m_fi_inter 		= false;
	m_fu_inter 		= false;
	m_propietary	= false;
	m_last_only		= false;
	m_more_chain	= false;
	m_no_sma		= false;
	m_unauth_sma	= false;
	m_prop_sma		= false;
	m_auth_sma		= false;

	return;
}

class_byte_t::class_byte_t(uint8_t v, bool validate) : m_class(v), m_is_set(false), m_channel(0)
{
	m_fi_inter		= false;
	m_fu_inter		= false;
	m_propietary	= false;
	m_last_only		= false;
	m_more_chain	= false;
	m_no_sma		= false;
	m_unauth_sma	= false;
	m_prop_sma		= false;
	m_auth_sma		= false;

	parse(validate);
	return;
}

class_byte_t::~class_byte_t(void) 
{ 
	m_is_set = false;
	return; 
}

uint8_t 
class_byte_t::get(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::get(): Called with object uninitialized");

	return m_class; 
}

void 
class_byte_t::set(uint8_t v, bool validate) 
{ 
	m_class = v; 
	parse(validate); 
	return; 
}

bool 
class_byte_t::isFirstInterIndustry(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isFirstInterIndustry(): Called with object uninitialized");

	return m_fi_inter; 
}

bool 
class_byte_t::isFurtherInterIndustry(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isFurtherInterIndustry(): Called with object uninitialized");

	return m_fu_inter; 
}

bool 
class_byte_t::isPropietary(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isPropietary(): Called with object uninitialized");

	return m_propietary; 
}

bool 
class_byte_t::isLastOrOnlyChain(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isLastOrOnlyChain(): Called with object uninitialized");

	return m_last_only; 
}

bool 
class_byte_t::isMoreChain(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isMoreChain(): Called with object uninitialized");

	return m_more_chain; 
}

bool 
class_byte_t::isNoSMA(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isNoSMA(): Called with object uninitialized");

	return m_no_sma; 
}

bool 
class_byte_t::isUnauthSMA(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isUnauthSMA(): Called with object uninitialized");

	return m_unauth_sma; 
}

bool 
class_byte_t::isPropietarySMA(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isPropietarySMA(): Called with object uninitialized");

	return m_prop_sma; 
}

bool 
class_byte_t::isAuthSMA(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::isAuthSMA(): Called with object uninitialized");

	return m_auth_sma; 
}

uint8_t 
class_byte_t::getChannel(void) 
{ 
	if (false == m_is_set)
		throw std::runtime_error("class_byte_t::getChannel(): Called with object uninitialized");

	return m_channel; 
}


sest_base_t::sest_base_t(void) : m_ins(0x00), m_p1(0x00), m_p2(0x00), m_lc(0x00), m_le(0x00) { return; }
sest_base_t::sest_base_t(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint16_t lc, uint16_t le, std::vector< uint8_t >& data) 
							: m_cla(cla), m_ins(ins), m_p1(p1), m_p2(p2), m_lc(lc), m_le(le), m_data(data) 
							{ 
								return; 
							}

sest_base_t::~sest_base_t(void) 
{
	m_ins 	= 0x00;
	m_lc 	= 0x00;
	m_le	= 0x00;

	m_data.clear();

	return;
}

std::vector< uint8_t > 
sest_base_t::data(bool validate)
{
	std::vector< uint8_t > 	r;
	bool					e(false);

	r.push_back(m_cla.get());
	r.push_back(m_ins);
	r.push_back(m_p1);
	r.push_back(m_p2);

	// lc 
	// if 0, then no data bytes follow
	// if 1-255, then 1-255 data bytes follow
	// if 0001-65535, then 0001-65535 data bytes follow and le/lc fields are 'extended'
	// either both are extended or both are short.
	
	if (true == validate && 0 != m_le && 0 != m_lc) {
		if (0 != m_lc && 65535 < m_lc) 
			throw std::runtime_error("sest_base_t::data(): Lc field is invalid (>65535)");
		
		if (0 != m_le && 65535 < m_le)
			throw std::runtime_error("sest_base_t::data(): Le field is invalid (>65535)");

		if (m_lc != m_data.size())
			throw std::runtime_error("sest_base_t::data(): Lc field is invalid (data size / value)");
	}

	if (255 < m_lc || 255 < m_le)
		e = true;

	if (0 != m_lc) {
		if (255 >= m_lc && false == e) 
			r.push_back(static_cast< uint8_t >(m_lc & 0xFF));
		else {
			r.push_back(0x00);
			r.push_back(static_cast< uint8_t >((m_lc & 0xFF00) >> 8));
			r.push_back(static_cast< uint8_t >(m_lc & 0xFF));
		}

		for (auto& v : m_data)
			r.push_back(v);
		
	}

	if (0 != m_le) {
		if (255 >= m_le && false == e)
			r.push_back(static_cast< uint8_t >(m_le & 0xFF));
		else {
			r.push_back(0x00);
			r.push_back(static_cast< uint8_t >((m_le & 0xFF00) >> 8));
			r.push_back(static_cast< uint8_t >(m_le & 0xFF));
		}
	} 

	return r;
}

void 
sest_base_t::set(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint16_t lc, uint16_t le, std::vector< uint8_t >& data)
{
	m_cla.set(cla);
	m_ins 	= ins;
	m_p1	= p1;
	m_p2	= p2;
	m_lc	= lc;
	m_le	= le;
	m_data	= data;

	return;
}

void 
sest_base_t::set(class_byte_t& cla, uint8_t ins, uint8_t p1, uint8_t p2, uint16_t lc, uint16_t le, std::vector< uint8_t >& data)
{
	m_cla 	= cla;
	m_ins	= ins;
	m_p1	= p1;
	m_p2	= p2;
	m_lc	= lc;
	m_le	= le;
	m_data	= data;

	return;
}

void 
sest_base_t::set_class(uint8_t v)
{
	m_cla.set(v);
	return;
}

void 
sest_base_t::set_instruction(uint8_t v)
{
	m_ins = v;
	return;
}

void
sest_base_t::set_p1(uint8_t p1)
{
	m_p1 = p1;
	return;
}

void
sest_base_t::set_p2(uint8_t p2)
{
	m_p2 = p2;
	return;
}

void 
sest_base_t::set_lc(uint16_t v)
{
	m_lc = v;
	return;
}

void 
sest_base_t::set_data(std::vector< uint8_t >& v)
{
	m_data = v;
	return;
}

void 
sest_base_t::set_le(uint16_t v)
{
	m_le = v;
	return;
}


class_byte_t& 
sest_base_t::cla(void) 
{ 
	return m_cla; 
}

uint8_t 
sest_base_t::ins(void) 
{ 
	return m_ins; 
}

uint16_t
sest_base_t::lc(void) 
{ 
	return m_lc; 
}

std::vector< uint8_t > 
sest_base_t::dat(void) 
{ 
	return m_data; 
}

uint16_t
sest_base_t::le(void) 
{ 
	return m_le; 
} 

