#include "ncap.hpp"

ncap_base_record_t::ncap_base_record_t(void) : m_magic(NCAP_MAGIC), m_id(0), m_type(NCAP_INVALID_RECORD), m_length(0) 
{
	return;
}

ncap_base_record_t::ncap_base_record_t(uint64_t id, uint64_t type, std::vector< uint8_t >& vec) : 	m_magic(NCAP_MAGIC), m_id(id), 
																									m_type(type), 
																									m_length(vec.size()), m_data(vec)
{
	return;
}

ncap_base_record_t::~ncap_base_record_t(void)
{
	m_id 		= 0;
	m_length	= 0;
	m_magic		= 0;
	m_type		= NCAP_INVALID_RECORD;
	
	m_data.clear();

	return;
}

uint64_t
ncap_base_record_t::byte_swap(uint64_t v)
{
	const int 	one = 1;
	const char*	sig = reinterpret_cast< const char* >(&one);

	uint64_t val(v);

	if (0 != sig) {
		const uint32_t hi = ::htonl(static_cast< uint32_t >(v >> 32));
		const uint32_t lo = ::htonl(static_cast< uint32_t >(v & 0x00000000FFFFFFFF));

		val = (static_cast< uint64_t >(lo) << 32) | hi;
	}

	return val;
}

std::vector< uint8_t >
ncap_base_record_t::vector(void)
{
	return ncap_base_record_t::to_disk(m_id, m_type, m_data);
}

std::vector< uint8_t >
ncap_base_record_t::to_disk(uint64_t id, uint64_t type, std::vector< uint8_t >& vec)
{
	std::vector< uint8_t > 	r;
	uint64_t				t(NCAP_MAGIC);
	uint64_t				l(vec.size());

	r.resize(sizeof(t) + sizeof(id) + sizeof(type) + sizeof(l) + l);

	/*
	 * MAGIC
	 * ID
	 * TYPE
	 * LENGTH
	 * DATA
	 */

	std::memcpy(r.data(), &t, sizeof(uint64_t));

	t = byte_swap(id);

	std::memcpy(r.data()+sizeof(uint64_t), &t, sizeof(id));

	t = byte_swap(type);

	std::memcpy(r.data()+sizeof(uint64_t)+sizeof(id), &t, sizeof(t));

	t = byte_swap(l);

	std::memcpy(r.data()+sizeof(t)+sizeof(id)+sizeof(type), &t, sizeof(l));
	std::memcpy(r.data()+sizeof(t)+sizeof(id)+sizeof(type)+sizeof(l), vec.data(), l);

	return r;
}

ncap_record_list_t
ncap_base_record_t::from_disk(std::vector< uint8_t >& vec)
{
	ncap_record_list_t 	list;
	const std::size_t	min(sizeof(uint64_t)*4);

	for (std::size_t off = 0; off < vec.size(); ) {
		uint64_t				magic(0);
		uint64_t 				id(0);
		uint64_t				type(NCAP_INVALID_RECORD);
		uint64_t 				length(0);
		std::vector< uint8_t > 	data;

		
		if (vec.size() >= off && (vec.size() - off) < min)
			throw std::runtime_error("ncap_base_record_t::from_disk(): Malformed input file encountered (Min)");

		magic	= *reinterpret_cast< uint64_t* >(&vec[off]);

		if (NCAP_MAGIC != magic)
			throw std::runtime_error("ncap_base_record_t::from_disk(): Malformed input file encountered (Magic)");

		id 		= *reinterpret_cast< uint64_t* >(&vec[off] + sizeof(magic));
		type	= *reinterpret_cast< uint64_t* >(&vec[off] + sizeof(magic) + sizeof(id));
		length	= *reinterpret_cast< uint64_t* >(&vec[off] + sizeof(magic) + sizeof(id) + sizeof(type));

		off += min;

		id		= ncap_base_record_t::byte_swap(id);
		type	= ncap_base_record_t::byte_swap(type);
		length	= ncap_base_record_t::byte_swap(length);

		if (false == ncap_base_record_t::is_valid_type(type & NCAP_RECORD_TYPE_MASK))
			throw std::runtime_error("ncap_base_record_t::from_disk(): Malformed input file encountered (Type): " + to_hex_string(type));

		if (vec.size() >= off && (vec.size() - off) < length)
			throw std::runtime_error("ncap_base_record_t::from_disk(): Malformed input file encountered (Length)");

		data.resize(length);
		std::memcpy(data.data(), &vec[off], length);
		off += length;
		list.push_back(ncap_base_record_t(id, type, data));
	}

	return list;
}

ncap_t::ncap_t(void)
{
	return;
}

ncap_t::~ncap_t(void)
{
	m_file.flush();
	m_file.close();

	return;
}

void
ncap_t::read(std::string& n, ncap_record_list_t& l)
{
	std::vector< uint8_t > 	r;
	std::size_t 			e(0);


	if (false == m_file.open(n, FILE_READ_DEFAULT))
		throw std::runtime_error("ncap_t::read(): Error opening file: " + m_file.error());

	e = m_file.size();

	if (false == m_file.read(r, e))
		throw std::runtime_error("ncap_t::read(): Error reading file: " + m_file.error());

	for (auto& rec : ncap_base_record_t::from_disk(r))
		l.push_back(rec);
	
	return;
}

void 
ncap_t::read(const char* n, ncap_record_list_t& l)
{
	std::string a(n);

	read(a, l);

	return;
}

void
ncap_t::write(const std::string& n, ncap_record_list_t& l, file_mode_t m)
{
	output_file_t			f(n, m);
	std::vector< uint8_t > 	v;

	if (false == f.open(n, m))
		throw std::runtime_error("ncap_t::write(): Error opening file: " + m_file.error());

	for (auto& rec : l) {
		std::vector< uint8_t > rv(rec.vector());

		v.insert(v.end(), rv.begin(), rv.end());
	}

	if (false == f.write(v))
		throw std::runtime_error("ncap_t::write(): Error writing file: " + m_file.error());

	if (false == f.sync())
		throw std::runtime_error("ncap_t::write(): Error syncing file: " + m_file.error());

	if (false == f.flush())
		throw std::runtime_error("ncap_t::write(): Error flushing file: " + m_file.error());

	f.close();

	return;
}

void 
ncap_t::write(const char* n, ncap_record_list_t& l, file_mode_t m)
{
	std::string a(n);

	write(a, l, m);

	return;
}

void
ncap_t::append(const std::string& n, ncap_record_list_t& l)
{
	write(n, l, FILE_WRITE_APPEND);

	return;
}

void
ncap_t::append(const char* n, ncap_record_list_t& l)
{
	std::string a(n);

	write(a, l, FILE_WRITE_APPEND);

	return;
}
