#include "card.hpp"

void 
otfta_card_t::set(std::vector< uint8_t >& dst, std::vector< uint8_t >& src)
{
	dst = src;
	return;
}

void
otfta_card_t::set(std::vector< uint8_t >& dst, uint8_t* src, std::size_t len)
{
	if (nullptr == src || 0 == len)
		throw std::runtime_error("otfta_card_t::set(): One or more invald parameters specified");

	dst.clear();
	dst.resize(len);
	std::memcpy(dst.data(), src, len);

	return;
}

void
otfta_card_t::append(std::vector< uint8_t >& dst, std::vector< uint8_t >& src)
{
	dst.insert(dst.end(), src.begin(), src.end());
	return;
}

void
otfta_card_t::append(std::vector< uint8_t >& dst, uint8_t* src, std::size_t len)
{
	if (nullptr == src || 0 == len)
		throw std::runtime_error("otfta_card_t::append(): One or more invalid parameters specified");

	for (std::size_t idx = 0; idx < len; idx++)
		dst.push_back(src[idx]);

	return;
}

otfta_card_t::otfta_card_t(void) : m_atq(0) {}
otfta_card_t::~otfta_card_t(void) { m_uid.clear(); m_ats.clear(); m_atq = 0; return; }

std::vector< uint8_t >&
otfta_card_t::get_uid(void)
{
	return m_uid;
}

std::size_t
otfta_card_t::uid_size(void)
{
	return m_uid.size();
}

std::string
otfta_card_t::uid(void)
{
	std::string retval("");
			
	for (auto& byte : m_uid)
		retval += otfta_card_t::fmt("%.02x ", byte);

	if (0 != retval.size())
		retval = retval.substr(0, retval.size() - 1);

	return retval;
}

void 
otfta_card_t::set_uid(std::vector< uint8_t >& uid)
{
	set(m_uid, uid);
	return;
}

void
otfta_card_t::set_uid(uint8_t* ptr, std::size_t len)
{
	set(m_uid, ptr, len);
	return;
}

void
otfta_card_t::append_uid(uint8_t byte)
{
	append(m_uid, &byte, sizeof(uint8_t));
	return;
}

void
otfta_card_t::append_uid(uint8_t* ptr, std::size_t len)
{
	append(m_uid, ptr, len);
	return;
}

void
otfta_card_t::append_uid(std::vector< uint8_t >& uid)
{
	append(m_uid, uid);
	return;
}

std::vector< uint8_t >
otfta_card_t::get_ats(void)
{
	return m_ats;
}

std::size_t
otfta_card_t::ats_size(void)
{
	return m_ats.size();
}

std::string
otfta_card_t::ats(void)
{	
	std::string retval("");

	for (auto& byte : m_ats)
		retval += otfta_card_t::fmt("%.02x ", byte);

	if (0 != retval.size())
		retval = retval.substr(0, retval.size() - 1);
				
	return retval;
}

void
otfta_card_t::set_ats(std::vector< uint8_t >& ats)
{
	set(m_ats, ats);
	return;
}

void
otfta_card_t::set_ats(uint8_t* ptr, std::size_t len)
{
	set(m_ats, ptr, len);
	return;
}

void
otfta_card_t::append_ats(std::vector< uint8_t >& ats)
{
	append(m_ats, ats);
	return;
}

void
otfta_card_t::append_ats(uint8_t* ptr, std::size_t len)
{
	append(m_ats, ptr, len);
	return;
}

void
otfta_card_t::set_atq(uint16_t atq)
{
	m_atq = atq;
	return;
}

uint16_t
otfta_card_t::get_atq(void)
{
	return m_atq;
}

std::string
otfta_card_t::atq(void)
{
	return otfta_card_t::fmt("%.02x", m_atq);
}

