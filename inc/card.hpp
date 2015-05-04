#ifndef HAVE_OTFTA_CARD_T_HPP
#define HAVE_OTFTA_CARD_T_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <climits>
#include <cstring>
#include <memory>



class otfta_card_t 
{
	private:
		std::vector< uint8_t > 	m_uid;
		std::vector< uint8_t > 	m_ats;
		uint16_t				m_atq;

	protected:
		template <typename ...Ts>
		static inline std::string
		fmt(const std::string fmt, Ts... var)
		{
			char						b(0);
			ssize_t						l(std::snprintf(&b, 0, fmt.c_str(), var...));
			ssize_t						r(0);
			std::unique_ptr< char[] >	p(nullptr);

			if (0 == l)
				return std::string("");

			if (0 > l)
				throw std::runtime_error("hexdump_t::fmt(): Error encountered in std::snprintf(): '" + fmt + "' ret: " + std::to_string(l));


			if (SSIZE_MAX == l)
				throw std::runtime_error("hexdump_t::fmt(): Overly large input encountered");

			l += 1;
			p.reset(new char[l]);

			r = std::snprintf(p.get(), l, fmt.c_str(), var...);

			if (0 > r)
				throw std::runtime_error("hexdump_t::fmt(): Error while formatting output");

			return std::string(p.get(), p.get()+r);
		}
	
		void set(std::vector< uint8_t >&, std::vector< uint8_t >&);
		void set(std::vector< uint8_t >&, uint8_t*, std::size_t);
		void append(std::vector< uint8_t >&, std::vector< uint8_t >&);
		void append(std::vector< uint8_t >&, uint8_t*, std::size_t);

	public:
		otfta_card_t(void);
		~otfta_card_t(void);

		std::vector< uint8_t >& get_uid(void);
		std::size_t uid_size(void);
		std::string uid(void);

		void set_uid(std::vector< uint8_t >&);
		void set_uid(uint8_t*, std::size_t);
		void append_uid(uint8_t);
		void append_uid(uint8_t*, std::size_t);
		void append_uid(std::vector< uint8_t >&);

		std::vector< uint8_t > get_ats(void);
		std::size_t ats_size(void);
		std::string ats(void);
		void set_ats(std::vector< uint8_t >&);
		void set_ats(uint8_t*, std::size_t);
		void append_ats(std::vector< uint8_t >&);
		void append_ats(uint8_t*, std::size_t);

		void set_atq(uint16_t);
		uint16_t get_atq(void);
		std::string atq(void);

};

#endif
