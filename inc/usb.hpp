#ifndef HAVE_USB_T_HPP
#define HAVE_USB_T_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <map>
#include <limits>

#include <string.h>
#include <libusb.h>

#include "util.hpp"

#define CON_DESC_ATTR_BUS_POWERED 0x80
#define CON_DESC_ATTR_SELF_POWERED 0x40
#define CON_DESC_ATTR_REMOTE_WAKE 0x20

typedef libusb_context usb_ctxt_t;
typedef libusb_device usb_dev_t;
typedef libusb_device_handle usb_handle_t;
typedef libusb_device_descriptor usb_dev_desc_t;
typedef libusb_config_descriptor usb_conf_desc_t;
typedef libusb_interface usb_iface_t;
typedef libusb_interface_descriptor usb_iface_desc_t;
typedef libusb_endpoint_descriptor usb_endpoint_desc_t;
typedef libusb_error err_t;
typedef libusb_transfer usb_xfer_t;

typedef std::map< std::string, std::string > string_map_t;
typedef std::vector< string_map_t > string_map_vector_t;

class usb_desc_base_t {
	private:
	protected:
		string_map_t m_vars;

	public:
		usb_desc_base_t(void) { }
		virtual ~usb_desc_base_t(void) { }
		
		virtual std::string 
		value(std::string key)
		{
			if (m_vars.end() == m_vars.find(key))
				return std::string("");

			return m_vars[key];
		}

		virtual string_map_t&
		variables(void) 
		{
			return m_vars;
		}

		virtual std::size_t
		var_size(void)
		{
			return m_vars.size();
		}

		virtual std::string to_string(void) = 0;
};

class usb_endpoint_info_t : public usb_desc_base_t {
	private:
		uint8_t	m_addr;
		uint8_t m_attr;
		uint8_t	m_dir;

	protected:
	public:
		usb_endpoint_info_t(const usb_endpoint_desc_t&);
		~usb_endpoint_info_t(void);
		virtual std::string to_string(void);
		uint8_t get_addr(void) { return m_addr; }
		uint8_t get_attr(void) { return m_attr; }
		uint8_t get_dir(void) { return m_dir; }
};

class usb_altsettings_t : public usb_desc_base_t {
	private:
		std::vector< usb_endpoint_info_t > 	m_endpoints;

	protected:
	public:
		usb_altsettings_t(usb_handle_t&, const usb_iface_desc_t&);
		virtual ~usb_altsettings_t(void);

		virtual std::string to_string(void);
		virtual usb_endpoint_info_t& endpoint(std::size_t);

		virtual std::vector< usb_endpoint_info_t >& 
		endpoints(void) 
		{ 
			return m_endpoints; 
		}

		virtual std::size_t 
		size(void) 
		{ 
			return m_endpoints.size(); 
		}
};

class usb_interface_t : public usb_desc_base_t {
	private:
		std::vector< usb_altsettings_t > m_alternates;
	protected:
	public:
		usb_interface_t(usb_handle_t&, const usb_iface_t&);
		virtual ~usb_interface_t(void);

		virtual std::string to_string(void);
		virtual usb_altsettings_t& alternate(std::size_t);
		
		virtual std::vector< usb_altsettings_t >& 
		alternates(void) 
		{ 
			return m_alternates; 
		}

		virtual std::size_t size(void) 
		{ 
			return m_alternates.size(); 
		}
};

class usb_configuration_t : public usb_desc_base_t {
	private:
		std::vector< usb_interface_t > m_interfaces;
	protected:
	public:
		usb_configuration_t(usb_handle_t&, const usb_conf_desc_t&);
		virtual ~usb_configuration_t(void);

		virtual std::string to_string(void);
		virtual usb_interface_t& interface(std::size_t);
		
		virtual std::vector< usb_interface_t >& 
		interfaces(void) 
		{ 
			return m_interfaces; 
		}

		virtual std::size_t 
		size(void) 
		{ 
			return m_interfaces.size(); 
		}
};

class usb_info_t : public usb_desc_base_t {
	private:
		/*uint8_t									m_class;
		uint8_t									m_subclass;
		uint16_t								m_vendorid;
		uint16_t								m_productid;
		uint8_t									m_protocol;
		uint8_t									m_nconfigs;
		uint16_t								m_usb;
		uint16_t								m_release;
		std::string								m_manufacturer;
		std::string								m_product;
		std::string								m_serial;*/
		std::vector< usb_configuration_t >		m_confs;

	protected:
		usb_conf_desc_t* get_config_descriptor(usb_dev_t&, uint8_t);
		void init_dev(usb_handle_t&, usb_dev_desc_t&);
	public:
		usb_info_t(usb_dev_t&, usb_handle_t&, usb_dev_desc_t&);
		virtual ~usb_info_t(void);
		
		virtual std::string to_string(void);
		virtual usb_configuration_t& configuration(std::size_t);

		virtual std::vector< usb_configuration_t >& 
		configurations(void) 
		{
			return m_confs;
		}

		virtual std::size_t
		size(void)
		{
			return m_confs.size();
		}
		
		/*virtual uint8_t get_class(void) { return m_class; }
		virtual uint8_t get_subclass(void) { return m_subclass; }
		virtual uint16_t get_vendorid(void) { return m_vendorid; }
		virtual uint16_t get_productid(void) { return m_productid; }
		virtual uint8_t get_protocol(void) { return m_protocol; }
		virtual uint8_t get_configs_size(void) { return m_nconfigs; }
		virtual uint16_t get_usb_version(void) { return m_usb; }
		virtual uint16_t get_dev_release_version(void) { return m_release; }
		virtual std::string get_manufacturer(void) { return m_manufacturer; }
		virtual std::string get_product(void) { return m_product; }
		virtual std::string get_serial(void) { return m_serial; }*/
};

class usb_t
{
	private:
		std::mutex						m_mutex;
		usb_ctxt_t*						m_ctxt;
		usb_handle_t*					m_handle;
		usb_info_t*						m_info;
		std::size_t						m_max;

	protected:
		usb_dev_desc_t get_device_descriptor(usb_dev_t&);
		usb_conf_desc_t* get_config_descriptor(usb_dev_t&, uint8_t idx = 0);

		void detach_kernel(uint8_t interface = 0);
		bool kernel_is_attached(uint8_t interface = 0);

	public:
		usb_t(void);
		~usb_t(void);

		static inline std::string
		usb_error(const char* msg, err_t err)
		{
			return std::string(msg) + ": " + std::string(::libusb_strerror(err));
		}

		static std::string lookup_class(uint8_t, uint8_t, uint8_t);

		usb_info_t& info(void); 
		std::string to_string(void);
		std::vector< usb_info_t* > enumerate_devices(void);
		std::vector< usb_endpoint_info_t > get_endpoints(void); 

		void claim(uint8_t inteferface = 0);
		void claim_all(void);
		void release(uint8_t interface = 0);
		void release_all(void);

		void set_config(int8_t conf = 1);

		bool open(uint16_t, uint16_t);
		void close(void);
		
		bool ctrl_xfer(uint8_t, uint8_t, uint16_t, uint16_t, std::vector< uint8_t >&, std::size_t&, unsigned int to = 0);
		bool bulk_xfer(uint8_t, std::vector< uint8_t >&, std::size_t&, unsigned int to = 0);
		void bulk_send(uint8_t, std::vector< uint8_t >&, std::size_t&, unsigned int to = 0);
		void bulk_recv(uint8_t, std::vector< uint8_t >&, unsigned int to = 0);
		bool int_xfer(uint8_t, std::vector< uint8_t >&, std::size_t&, unsigned int to = 0);

};

#endif
