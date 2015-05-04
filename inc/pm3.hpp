#ifndef HAVE_PM3_T_HPP
#define HAVE_PM3_T_HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "async.hpp"
#include "cmd.hpp"
#include "hexdump.hpp"
#include "parse.hpp"
#include "ncap.hpp"
#include "decoder.hpp"

class pm3_t 
{
	private:
		async_usb_t		m_usb;
		//std::ostream	m_out;
		//std::ostream	m_err;
		std::vector< uint8_t > m_uid;
		uint8_t				m_col;

	protected:
		std::string get_proc_info(uint64_t);
		std::string get_mem_info(uint64_t);
		std::string get_arch_info(uint64_t);

		cmd_t* select_major(select_cascade_level_t, usb_cmd_t&);
		cmd_t* select_minor(select_cascade_level_t, usb_cmd_t&);

	public:
		pm3_t(uint16_t v = 0x2D2D, uint16_t p = 0x504D); 
		virtual ~pm3_t(void);

		//void set_out(std::ostream& out = std::cout);
		//void set_err(std::ostream& err = std::cerr);

		std::string version(void);
		std::string tune(void);
		//std::vector< uint8_t > read14a(void);
		void read14a(const std::size_t interval_ms = 1000);
		void msniff(void);
		void otfta_raw(std::string);
		//void otfta_disconnect(void);
};

#endif
