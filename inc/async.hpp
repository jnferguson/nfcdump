#ifndef HAVE_ASYNC_USB_T_HPP
#define HAVE_ASYNC_USB_T_HPP

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdexcept>
#include <mutex>
#include <memory>
#include <queue>
#include <atomic>

#include "usb.hpp"
//#include "pm3.hpp"
//#include "hexdump.hpp"
//#include "cmd.hpp"

//typedef libusb_transfer async_xfer_t;

//typedef std::shared_ptr< cmd_t > cmd_ptr_t;

/*class rx_queue_t 
{
	private:
	protected:
		std::mutex				m_mutex;
		std::queue< cmd_ptr_t > m_queue;
		std::queue< cmd_ptr_t > m_defer;
		std::vector< uint8_t >	m_raw;
		std::size_t				m_off;
		const std::size_t		m_max;
		
	public:
		rx_queue_t(void);
		virtual ~rx_queue_t(void);

		virtual uint8_t* data(void);

		virtual std::size_t left(void);
		virtual void inc(std::size_t);
		
		virtual bool empty(void);
		virtual bool defer_empty(void);
		
		virtual cmd_ptr_t pop(void);
		virtual cmd_ptr_t defer_pop(void);
		virtual void push(cmd_ptr_t&);
};*/

typedef libusb_device usbdev_t;
typedef libusb_device_descriptor devdes_t;
typedef libusb_config_descriptor confdes_t;
typedef libusb_interface interface_t;
typedef libusb_interface_descriptor interfacedes_t;
typedef libusb_endpoint_descriptor endpointdes_t;

typedef struct _endpoint_t {
	uint8_t		attr;
	uint8_t		addr;
	uint8_t		dir;
	uint16_t 	max;
	uint8_t		interval;

	_endpoint_t(void) : attr(0), addr(0), dir(0), max(0), interval(0) {}
	_endpoint_t(const endpointdes_t& epd) : attr(epd.bmAttributes), addr(epd.bEndpointAddress & LIBUSB_ENDPOINT_ADDRESS_MASK), 
											dir(epd.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK), max(epd.wMaxPacketSize),
											interval(epd.bInterval)
	{
		return;
	}

	~_endpoint_t(void) { attr = 0; addr = 0; max = 0; interval = 0; return; }
} endpoint_t;

typedef std::map< std::size_t, std::size_t > interface_cntmap_t;

class async_usb_t 
{
	private:
		std::mutex					m_mutex;
		usb_ctxt_t*					m_ctxt;
		usb_handle_t*				m_hnd;
		//async_xfer_t*				m_xfer;
		std::size_t					m_nconfigs;
		interface_cntmap_t			m_icnt;
		std::vector< endpoint_t >	m_endpoints;
		//rx_queue_t				m_rqueue;
		uint8_t					m_iaddr;
		uint8_t					m_oaddr;
		uint8_t					m_intaddr;

	protected:
	//	virtual uint8_t bulk_in(void);
	//	virtual uint8_t bulk_out(void);

		//static void output_cb(async_xfer_t*);
		virtual void get_device_descriptor(usbdev_t&, devdes_t&);
		virtual confdes_t* get_config_descriptor(usbdev_t&, uint8_t);
		
	public:
		async_usb_t(void);
		virtual ~async_usb_t(void);
		
		virtual void open(uint16_t, uint16_t);
		virtual void close(void);

		virtual void set_config(int8_t c = -1);

		virtual void claim(uint8_t i = 0);
		virtual void claim_all(void);

		virtual void release(uint8_t i = 0);
		virtual void release_all(void);

		virtual std::size_t num_configs(void) { return m_nconfigs; }
		
		virtual std::size_t num_interfaces(std::size_t cnf) 
		{ 
			if (cnf < m_nconfigs) 
				return m_icnt[cnf]; 
				
			throw std::runtime_error("async_usb_t::num_interfaces(): Invalid configuration index specified");
		}

		//void initialize(void);

		//void get_version(void);

		//virtual bool empty(void) { return m_rqueue.empty(); }
		//virtual cmd_ptr_t pop(void) { return m_rqueue.pop(); }
		//virtual void push(cmd_ptr_t& ptr) { m_rqueue.push(ptr); return; }
		//virtual void handle_events(void);

		virtual void receive(std::vector< uint8_t >&, const std::size_t);
		virtual bool receive_timeout(std::vector< uint8_t >&, const std::size_t, const std::size_t);
		virtual void transmit(const std::vector< uint8_t >&, const std::size_t max = 64);
		virtual bool transmit_timeout(const std::vector< uint8_t >&, const std::size_t max = 64, const std::size_t timeout = 1000);
		virtual void transmit_interrupt(const std::vector< uint8_t >&);
};

#endif
