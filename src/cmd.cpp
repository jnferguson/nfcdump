#include "cmd.hpp"

cmd_t::cmd_t(void) : m_raw(sizeof(usb_cmd_t)), m_cmd(nullptr), m_data(nullptr), m_dlen(0)
{
	return;
}

cmd_t::~cmd_t(void)
{
	m_cmd   = nullptr;
	m_data  = nullptr;
	m_raw.clear();

	return;
}

void
cmd_t::set(std::vector< uint8_t >& v)
{
	if (v.size() != sizeof(usb_cmd_t))
		throw std::runtime_error("cmd_t::set(): Vector parameter is invalid");

	m_raw   = v;
	m_cmd   = reinterpret_cast< usb_cmd_t* >(&m_raw[0]);

	parse();
	return;
}

void
cmd_t::set(const uint8_t* ptr, std::size_t len)
{
	if (len != sizeof(usb_cmd_t) || nullptr == ptr)
		throw std::runtime_error("cmd_t::set(): Pointer parameter is invalid");

	m_raw.clear();
	m_raw.insert(m_raw.begin(), ptr, ptr+len);
	parse();

	return;
}

uint64_t
cmd_t::get_cmd(void) const
{
	if (nullptr == m_cmd)
		throw std::runtime_error("cmd_t::get_cmd(): Called before initialized");

	return m_cmd->cmd;
}

void
cmd_t::set_cmd(uint64_t c)
{
	if (nullptr == m_cmd)
		throw std::runtime_error("cmd_t::set_cmd(): Called before initialized");
	
	m_cmd->cmd = c;
	return;
}

uint64_t
cmd_t::get_arg(uint8_t idx) const
{
	if (nullptr == m_cmd || idx >= 3)
		throw std::runtime_error("cmd_t::get_cmd(): Called before initialized");

	return m_cmd->arg[idx];
}

void
cmd_t::set_arg(uint8_t idx, uint64_t val)
{
	if (nullptr == m_cmd || idx >= 3)
		throw std::runtime_error("cmd_t::set_cmd(): Called before initialized");

	m_cmd->arg[idx] = val;
	return;
}

uint8_t*
cmd_t::get_data(std::size_t off) const
{
	if (nullptr == m_cmd || nullptr == m_data || USB_CMD_DSIZE <= off)
		throw std::runtime_error("cmd_t::get_data(): Called before initialized or invalid parameter");

	return m_data+off;
}

uint64_t
cmd_t::get_dlen(void) const
{
	if (nullptr == m_cmd || nullptr == m_data)
		throw std::runtime_error("cmd_t::get_dlen(): Called while in an invalid state");

	return m_dlen;
}

void
cmd_t::reset(void)
{
	uint64_t cmd(0);

	if (nullptr == m_cmd)
		return;
	
	cmd = m_cmd->cmd;
	::memset(m_raw.data(), 0, m_raw.size());
	m_cmd->cmd = cmd;

	return;
}

void 
cmd_dbg_str_t::parse(void)
{
	if (m_raw.size() != sizeof(usb_cmd_t))
		throw std::runtime_error("cmd_dbg_str_t::parse(): Invalid data size encountered");

	m_cmd 	= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
	m_data	= &m_cmd->d.bytes[0];
	m_dlen	= m_cmd->arg[0];
		
	if (CMD_DEBUG_PRINT_STRING != m_cmd->cmd)
		throw std::runtime_error("cmd_dbg_str_t::parse(): Command encountered was not CMD_DEBUG_PRINT_STRING");

	if (USB_CMD_DSIZE <= m_dlen)
		throw std::runtime_error("cmd_dbg_str_t::parse(): Data length specified in command is invalid");

	return;
}
		

