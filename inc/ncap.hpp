#ifndef HAVE_NCAP_T_HPP
#define HAVE_NCAP_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

#include <arpa/inet.h>

#include "file.hpp"
#include "util.hpp"

#define NCAP_MAGIC 0x4e4341505041434e

#define NCAP_RECORD_TYPE_BITS	48
#define NCAP_RECORD_TYPE_MASK 	(0xFFFFULL << NCAP_RECORD_TYPE_BITS) 
#define NCAP_RESERVED_0_BITS	32
#define NCAP_RESERVED_0_MASK	(0xFFFFULL << NCAP_RESERVED_0_BITS)
#define NCAP_RESERVED_1_BITS	16
#define NCAP_RESERVED_1_MASK	(0xFFFFULL << NCAP_RESERVED_1_BITS)
#define NCAP_RESERVED_2_BITS	0
#define NCAP_RESERVED_2_MASK	(0xFFFFULL << NCAP_RESERVED_2_BITS)
#define NCAP_RESERVED_MASK		(NCAP_RESERVED_0_MASK|NCAP_RESERVED_1_MASK|NCAP_RESERVED_2_MASK)

#define NCAP_INVALID_RECORD		(0xFFFFULL << NCAP_RECORD_TYPE_BITS)
#define NCAP_MODULATED_RECORD 	(0xFFFEULL << NCAP_RECORD_TYPE_BITS)

#define NCAP_RECORD_MIN_SIZE (sizeof(uint64_t) * 4)

class ncap_base_record_t
{
	private:
	protected:

		uint64_t				m_magic;
		uint64_t				m_id;
		uint64_t				m_type;
		uint64_t				m_length;

		std::vector< uint8_t >	m_data;

		static inline bool 
		is_valid_type(uint64_t type) 
		{
			bool r(false);

			switch (type) {
				case NCAP_MODULATED_RECORD:
					r = true;
					break;

				case NCAP_INVALID_RECORD:
					r = false;
					break;

				default:
					r = false;
					break;
			}

			return r;
		}

	public:
		ncap_base_record_t(void);
		ncap_base_record_t(uint64_t, uint64_t, std::vector< uint8_t >&);
		virtual ~ncap_base_record_t(void);

		inline static uint64_t byte_swap(uint64_t);

		inline uint64_t id(void) const { return m_id; }
		inline void set_id(uint64_t id) { m_id = id; return; }
		inline uint64_t type(void) const { return m_type & NCAP_RECORD_TYPE_MASK; }
		inline void set_type(uint64_t type) { m_type = ((type & NCAP_RECORD_TYPE_MASK) << NCAP_RECORD_TYPE_BITS); return; }
		inline void set_type(uint16_t type) { m_type = static_cast< uint64_t >(type) << NCAP_RECORD_TYPE_BITS; return; }

		inline uint64_t length(void) const { return m_length; }
		inline uint64_t magic(void) const { return m_magic; }

		inline std::vector< uint8_t >& data(void) { return m_data; }
		inline void set_data(std::vector< uint8_t >& data) { m_data = data; return; }

		std::vector< uint8_t > vector(void);

		static std::vector< uint8_t > to_disk(uint64_t, uint64_t, std::vector< uint8_t >&);
		static std::list< ncap_base_record_t > from_disk(std::vector< uint8_t >&);
};

typedef std::list< ncap_base_record_t > ncap_record_list_t;

class ncap_modulated_record_t : public ncap_base_record_t 
{
	private:
	protected:
	public:
		ncap_modulated_record_t(void) : ncap_base_record_t() { m_type = NCAP_MODULATED_RECORD; return; }
		ncap_modulated_record_t(uint64_t id, std::vector< uint8_t >& v) : ncap_base_record_t(id, NCAP_MODULATED_RECORD, v) { return; }

		virtual ~ncap_modulated_record_t(void) 
		{
			m_magic		= 0;
			m_length	= 0;
			m_id		= 0;
			m_type 		= NCAP_INVALID_RECORD;

			m_data.clear();

			return;
		}

};

class ncap_t 
{
	private:
		file_t	m_file;

	protected:
	public:

		ncap_t(void);
		virtual ~ncap_t(void);

		virtual void read(std::string&, ncap_record_list_t&);
		virtual void read(const char*, ncap_record_list_t&);

		virtual void write(const std::string&, ncap_record_list_t&, file_mode_t m = FILE_WRITE_TRUNCATE);
		virtual void write(const char*, ncap_record_list_t&, file_mode_t m = FILE_WRITE_TRUNCATE);
		virtual void append(const std::string&, ncap_record_list_t&);
		virtual void append(const char*, ncap_record_list_t&);
};

#endif
