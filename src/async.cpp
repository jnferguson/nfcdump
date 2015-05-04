#include "async.hpp"


/*rx_queue_t::rx_queue_t(void) : m_raw(sizeof(usb_cmd_t)), m_off(0), m_max(sizeof(usb_cmd_t))
{
	std::lock_guard< std::mutex > lck(m_mutex);
	return;
}

rx_queue_t::~rx_queue_t(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);
	
	while (m_queue.size()) {
		cmd_ptr_t ptr = m_queue.front();
		m_queue.pop();
		ptr.reset();
	}

	while (m_defer.size()) {
		cmd_ptr_t ptr = m_defer.front();
		m_defer.pop();
		ptr.reset();
	}

	return;
}

uint8_t*
rx_queue_t::data(void)
{
	std::lock_guard< std::mutex > 	lck(m_mutex);
	uint8_t* 						ptr(&m_raw[0]);

	if (m_off > m_max)
		throw std::runtime_error("rx_queue_t::data(): The offset into the contained buffer is invalid; this shouldnt be possible");

	ptr += m_off;

	return ptr;
}

std::size_t
rx_queue_t::left(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (m_off > m_max)
		throw std::runtime_error("rx_queue_t::left(): Offset is greater than max length; this shouldn't be possible");

	return m_max - m_off;
}

void
rx_queue_t::inc(std::size_t len)
{
//	std::lock_guard< std::mutex > lck(m_mutex);


	if (len > left())
		throw std::runtime_error("rx_queue_t::inc(): If we reach here, there was memory corruption somehow (magic presumably).");

	m_mutex.lock();
	m_off += len;
	m_mutex.unlock();

	if (0 == left()) {
		cmd_ptr_t ptr = std::make_shared< cmd_t >();

		if (m_max != ptr->length_left())
			throw std::runtime_error("rx_queue_t::inc(): Here be evil magic, again. This shouldn't be possible.");

		m_mutex.lock();
		ptr->base().insert(ptr->base().begin(), m_raw.begin(), m_raw.end());

		m_queue.push(ptr);
		m_raw.clear();
		m_raw.resize(m_max);
		m_off = 0;
		::memset(m_raw.data(), 0, m_max);
		m_mutex.unlock();
	}

	return;
}

bool
rx_queue_t::empty(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	return m_queue.empty();
}

bool
rx_queue_t::defer_empty(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	return m_defer.empty();
}

cmd_ptr_t
rx_queue_t::pop(void)
{
	cmd_ptr_t	ptr(nullptr);

	if (true == empty())
		throw std::runtime_error("rx_queue_t::pop(): Attempted to retrieve item from empty queue");

	m_mutex.lock();
	ptr = m_queue.front();
	m_queue.pop();
	m_mutex.unlock();
	return ptr;
}

cmd_ptr_t
rx_queue_t::defer_pop(void)
{
	cmd_ptr_t	ptr(nullptr);

	if (true == defer_empty())
		throw std::runtime_error("rx_queue_t::defer_pop(): Attempted to retrieve item from empty queue");

	m_mutex.lock();
	ptr = m_defer.front();
	m_defer.pop();
	m_mutex.unlock();

	return ptr;
}

void
rx_queue_t::push(cmd_ptr_t& ptr)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	m_defer.push(ptr);
	return;
}*/

async_usb_t::async_usb_t(void) : m_ctxt(nullptr), m_hnd(nullptr) /*, m_xfer(nullptr)*/ 
{
	std::lock_guard< std::mutex > 	l(m_mutex);
	err_t							e(static_cast< err_t >(::libusb_init(&m_ctxt)));

	if (LIBUSB_SUCCESS != e)
		throw std::runtime_error(usb_t::usb_error("Error initializing libUSB", e));

//	::libusb_set_debug(m_ctxt, LIBUSB_LOG_LEVEL_DEBUG);

	return;

}

async_usb_t::~async_usb_t(void)
{
	std::lock_guard< std::mutex > l(m_mutex);

	/*if (nullptr != m_xfer) {
		::libusb_cancel_transfer(m_xfer);
		::libusb_free_transfer(m_xfer);
		m_xfer = nullptr;
	}*/

	if (nullptr != m_hnd) {
		::libusb_close(m_hnd);
		m_hnd = nullptr;
	}

	if (nullptr != m_ctxt) {
		::libusb_exit(m_ctxt);
		m_ctxt = nullptr;
	}

	return;
}

void
async_usb_t::get_device_descriptor(usbdev_t& d, devdes_t& des)
{
	err_t		err(LIBUSB_SUCCESS);

	err = static_cast< err_t >(::libusb_get_device_descriptor(&d, &des));

	if (LIBUSB_SUCCESS != err)
		throw std::runtime_error(usb_t::usb_error("async_usb_t::get_device_descriptor(): Error retrieving device descriptor", err));

	return;
}

confdes_t* 
async_usb_t::get_config_descriptor(usbdev_t& d, uint8_t idx)
{
	confdes_t*	retval(nullptr);
	err_t		err(LIBUSB_SUCCESS);

	err = static_cast< err_t >(::libusb_get_config_descriptor(&d, idx, &retval));

	if (LIBUSB_SUCCESS != err) 
		throw std::runtime_error(usb_t::usb_error("async_usb_t::get_config_descriptor(): Error retrieving config descriptor", err));

	return retval;
}

void
async_usb_t::open(uint16_t v, uint16_t p)
{
	std::lock_guard< std::mutex >	lock(m_mutex);
	usb_dev_t**						devs(nullptr);
	ssize_t							cnt(0);
	err_t							err(LIBUSB_SUCCESS);
	bool							fnd(false);

	if (nullptr != m_hnd) 
		::libusb_close(m_hnd);
	
	cnt = ::libusb_get_device_list(m_ctxt, &devs);

	if (0 > cnt) 
		throw std::runtime_error(usb_t::usb_error("async_usb_t::open(): Error retrieving USB device list", static_cast< err_t >(cnt)));

	for (ssize_t idx = 0; idx < cnt; idx++) {
		usb_dev_desc_t 		desc = {0}; 

		err = static_cast< err_t >(::libusb_get_device_descriptor(devs[idx], &desc));

		if (LIBUSB_SUCCESS != err) {
			::libusb_free_device_list(devs, 1);
			throw std::runtime_error(usb_t::usb_error("async_usb_t::open(): Error retrieving device descriptor", err));
		}

		if (v == desc.idVendor && p == desc.idProduct) {
			libusb_device_descriptor desc = {0};

			err = static_cast< err_t >(::libusb_open(devs[idx], &m_hnd));

			if (LIBUSB_SUCCESS != err) {
				::libusb_free_device_list(devs, 1);
				throw std::runtime_error(usb_t::usb_error("async_usb_t::open(): Error opening USB device", err));
			}

			get_device_descriptor(*devs[idx], desc);

			m_nconfigs = desc.bNumConfigurations;

			for (uint8_t cnum = 0; cnum < desc.bNumConfigurations; cnum++) {
				confdes_t* config = get_config_descriptor(*devs[idx], cnum);
				m_icnt[cnum] = config->bNumInterfaces;

				for (uint8_t inum = 0; inum < config->bNumInterfaces; inum++) {
					const interface_t* interface = &config->interface[inum];

					for (signed int anum = 0; anum < interface->num_altsetting; anum++) {
						const interfacedes_t* interfacedes = &interface->altsetting[anum];

						for (uint8_t e_num = 0; e_num < interfacedes->bNumEndpoints; e_num++) {
							const endpointdes_t* epdes = &interfacedes->endpoint[e_num];
							m_endpoints.push_back(endpoint_t(*epdes));
						}
					}
				}
			}

			/*err = static_cast< err_t >(::libusb_reset_device(m_hnd));

			if (LIBUSB_SUCCESS != err) {
				::libusb_free_device_list(devs, 1);
				::libusb_close(m_hnd);
				m_hnd = nullptr;
				throw std::runtime_error(usb_t::usb_error("async_usb_t::open(): Error reseting USB device", err));
			}*/
			
			
			for (auto& ep : m_endpoints) {
				if (LIBUSB_ENDPOINT_IN == ep.dir && (LIBUSB_TRANSFER_TYPE_BULK == (LIBUSB_TRANSFER_TYPE_MASK & ep.attr))) 
					m_iaddr = ep.addr|LIBUSB_ENDPOINT_IN;
				else if (LIBUSB_ENDPOINT_OUT == ep.dir && (LIBUSB_TRANSFER_TYPE_BULK == (LIBUSB_TRANSFER_TYPE_MASK & ep.attr))) 
					m_oaddr = ep.addr|LIBUSB_ENDPOINT_OUT;
				else if (LIBUSB_TRANSFER_TYPE_INTERRUPT == (LIBUSB_TRANSFER_TYPE_MASK & ep.attr)) 
					m_intaddr = ep.addr|LIBUSB_ENDPOINT_IN;
				
			}

			fnd = true;
			break;
		}
	}

	::libusb_free_device_list(devs, 1);

	if (false == fnd)
		throw std::runtime_error("async_usb_t::open(): Failed to locate requested USB device");

//	set_config(1);
//	claim_all();
//	initialize();

	return;
}

void
async_usb_t::close(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (nullptr == m_hnd) 
		return;

	/*if (nullptr != m_xfer) {
		err_t e = static_cast< err_t >(::libusb_cancel_transfer(m_xfer));

		if (LIBUSB_SUCCESS != e) 
			printf("%s\n", usb_t::usb_error("libusb_cancel_transfer() error", e).c_str());

		::libusb_free_transfer(m_xfer);
		m_xfer = nullptr;
	}*/

	if (nullptr != m_hnd) {
		::libusb_reset_device(m_hnd);
		::libusb_close(m_hnd);
		m_hnd = nullptr;
	}

	return;
}

void
async_usb_t::claim(uint8_t i)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::claim(): invoked prior to opening device");

	if (1 == ::libusb_kernel_driver_active(m_hnd, +i)) {
		err = static_cast< err_t >(::libusb_set_auto_detach_kernel_driver(m_hnd, 1));

		if (LIBUSB_SUCCESS != err) {
			 err = static_cast< err_t >(::libusb_detach_kernel_driver(m_hnd, +i));

			if (LIBUSB_SUCCESS != err && LIBUSB_ERROR_NOT_FOUND != err)
				throw std::runtime_error(usb_t::usb_error("Error detaching kernel driver from device", err));
		}
	}

	err = static_cast< err_t >(::libusb_claim_interface(m_hnd, +i));

	if (LIBUSB_SUCCESS != err) 
		throw std::runtime_error(usb_t::usb_error("Error while attempting to claim device interface", err));

	return;	
}

void 
async_usb_t::claim_all(void)
{
	for (std::size_t nc = 0; nc < m_nconfigs; nc++)
		for (std::size_t ic = 0; ic < m_icnt[nc]; ic++) 
			claim(ic);
			
	return;
}

void 
async_usb_t::release(uint8_t i)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_hnd)
		throw std::runtime_error("Attempted to release device interface before opening the device");

	err = static_cast< err_t >(::libusb_release_interface(m_hnd, i));

	if (LIBUSB_SUCCESS != err && LIBUSB_ERROR_NOT_FOUND != err)
		throw std::runtime_error(usb_t::usb_error("Error while releasing device interface", err));

	return;
}

void 
async_usb_t::release_all(void)
{
	for (std::size_t nc = 0; nc < m_nconfigs; nc++)
		for (std::size_t ic = 0; ic < m_icnt[nc]; ic++)
			release(ic);
			
	return;
}

void
async_usb_t::set_config(int8_t c)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::set_config(): Attempted to set device configuration before opening the device");

	err = static_cast< err_t >(::libusb_set_configuration(m_hnd, c));

	if (LIBUSB_SUCCESS != err)
		throw std::runtime_error(usb_t::usb_error("async_usb_t::set_config(): Error while setting device configuration", err));

	return;	
}

/*void
async_usb_t::output_cb(async_xfer_t* xfer)
{
	async_usb_t* 	obj(nullptr);
	err_t			err(LIBUSB_SUCCESS);	

	if (nullptr == xfer) 
		return;

	switch (xfer->status) {
		case LIBUSB_TRANSFER_COMPLETED:
			break;

		case LIBUSB_TRANSFER_TIMED_OUT:
		case LIBUSB_TRANSFER_CANCELLED:
			return;

		case LIBUSB_TRANSFER_NO_DEVICE:
			throw std::runtime_error("async_usb_t::output_cb(): Device was removed");
			break;

		default:
			throw std::runtime_error("async_usb_t::output_cb(): Error encountered in callback");
			break;
	}

	obj = reinterpret_cast< async_usb_t* >(xfer->user_data);

	if (nullptr == obj)
		throw std::runtime_error("async_usb_t::output_cb(): User data pointer is null");

	if (nullptr == obj->m_xfer || nullptr == obj->m_hnd)
		throw std::runtime_error("async_usb_t::output_cb(): Transfer buffer pointer or USB device handle is invalid");

	obj->m_mutex.lock();
	
	if (0 != xfer->actual_length) 
		obj->m_rqueue.inc(xfer->actual_length);

	::libusb_fill_bulk_transfer(obj->m_xfer, obj->m_hnd, obj->m_iaddr,
								static_cast< unsigned char* >(obj->m_rqueue.data()),
								obj->m_rqueue.left(),
								&async_usb_t::output_cb, obj, 5000);



	err = static_cast< err_t >(::libusb_submit_transfer(obj->m_xfer));

	if (LIBUSB_SUCCESS != err) {
		::libusb_free_transfer(obj->m_xfer);
		obj->m_xfer = nullptr;
		obj->m_mutex.unlock();

		throw std::runtime_error(usb_t::usb_error("Error submitting bulk transfer", err));
	}

	obj->m_mutex.unlock();
	return;
}

void
async_usb_t::initialize(void)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_hnd) 
		throw std::runtime_error("async_usb_t::initialize(): Attempted to initialize async i/o before opening device");
	
	if (nullptr != m_xfer)
		throw std::runtime_error("async_usb_t::initialize(): Attempted to reinitialize object");
	
	m_xfer = ::libusb_alloc_transfer(0);

	if (nullptr == m_xfer) 
		throw std::runtime_error("async_usb_t::initialize(): Failed to allocated transfer structure");

	::libusb_fill_bulk_transfer(m_xfer, m_hnd, 
								m_iaddr, 
								static_cast< unsigned char* >(m_rqueue.data()),
								m_rqueue.left(), 
								&async_usb_t::output_cb, this, 5000);

	err = static_cast< err_t >(::libusb_submit_transfer(m_xfer));

	if (LIBUSB_SUCCESS != err) {
		::libusb_free_transfer(m_xfer);
		m_xfer 	= nullptr;

		throw std::runtime_error(usb_t::usb_error("Error while submitting async i/o structure", err));
	}
	
	return;
}*/

/*void
async_usb_t::get_version(void)
{
	std::vector< uint8_t >	vec(sizeof(usb_cmd_t));
	usb_cmd_t*				cmd(reinterpret_cast< usb_cmd_t* >(&vec[0]));

	::memset(vec.data(), 0, vec.size());
	cmd->cmd = CMD_VERSION;

	transmit(vec, 63);
	return;
}*/

void
async_usb_t::receive(std::vector< uint8_t >& vec, const std::size_t max)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	std::size_t						cnt(max);
	std::size_t						off(0);
	signed int						rval(0);
	signed int						len(0);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::receive(): Handle to USB device is invalid");

	vec.clear();
	vec.resize(max);
	::memset(vec.data(), 0, max);

	while (0 != cnt) {
		len = 0;
	
		rval = ::libusb_bulk_transfer(m_hnd, m_iaddr, static_cast< unsigned char* >(vec.data()+off), cnt, &len, 0);

		if (0 > rval) 
			throw std::runtime_error(usb_t::usb_error("async_usb_t::receive(): Error while receiving bulk data", static_cast< err_t >(rval)));
	
		if (0 > len || static_cast< std::size_t >(len)+off > max)
			throw std::runtime_error("async_usb_t::receive(): Error receive length exceeds maximum requested length");

		off += len;
		cnt -= len;
	}

	return;

}

bool
async_usb_t::receive_timeout(std::vector< uint8_t >& vec, const std::size_t max, const std::size_t timeout)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	std::size_t						cnt(max);
	std::size_t						off(0);
	signed int						rval(0);
	signed int						len(0);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::receive_timeout(): Handle to USB device is invalid");

	vec.clear();
	vec.resize(max);
	::memset(vec.data(), 0, vec.size());

	while (0 != cnt) {
		len = 0;

		rval = ::libusb_bulk_transfer(m_hnd, m_iaddr, static_cast< unsigned char* >(vec.data()+off), cnt, &len, timeout);

		if (0 == rval)
			return false;

		if (0 > rval) {
			if (LIBUSB_ERROR_TIMEOUT == static_cast< err_t >(rval) && (cnt == max && 0 == len))
				return false;
			else if (LIBUSB_ERROR_TIMEOUT == static_cast< err_t >(rval) && (cnt != max && 0 != len))
				return true; // partial read is okay??

			throw std::runtime_error(usb_t::usb_error("async_usb_t::receive_timeout(): Error while receiving bulk data", static_cast< err_t >(rval)));
		}

		if (0 > len || static_cast< std::size_t >(len)+off > max)
			throw std::runtime_error("async_usb_receive_timeout(): Error receive length exceeds maximum requested length");

		off += len;
		cnt -= len;
	}

	return true;
}

void
async_usb_t::transmit_interrupt(const std::vector< uint8_t >& vec)
{
	std::vector< uint8_t >	dat(vec);
	std::size_t				cnt(0);
	signed int				rval(0);
	signed int				len(0);
	std::size_t				max(0);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::transmit_interrupt(): Handle to USB device is invalid");
	
	for (auto& epd : m_endpoints) 
		if (LIBUSB_TRANSFER_TYPE_INTERRUPT == (LIBUSB_TRANSFER_TYPE_MASK & epd.attr)) {
			max = epd.max;
			break;
		}
	
	if (0 == max)
		throw std::runtime_error("async_usb_t::transmit_interrupt(): Unable to determine maximum segment length");
	
	while (0 != dat.size()) {
		len = 0;
		cnt = std::min(max, dat.size());

		rval = ::libusb_interrupt_transfer(m_hnd, m_intaddr, static_cast< unsigned char* >(dat.data()), cnt, &len, 0);

		if (0 > rval)
			throw std::runtime_error(usb_t::usb_error("async_usb_t::transmit_interrupt(): Error while transmitting interrupt data", static_cast< err_t >(rval)));

		if (0 > len || static_cast< std::size_t >(len) > cnt)
			throw std::runtime_error("async_usb_t::transmit(): Transmit length exceeds requested length");

		dat.erase(dat.begin(), dat.begin()+len);
	}

	return;
}

void
async_usb_t::transmit(const std::vector< uint8_t >& vec, const std::size_t max) 
{
	std::lock_guard< std::mutex >   lck(m_mutex);
	std::vector< uint8_t >			dat(vec);
	std::size_t 					cnt(0);
	signed int						rval(0);
	signed int						len(0);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::transmit(): Handle to USB device is invalid");

	while (0 != dat.size()) {
		len = 0;
		cnt = std::min(max, dat.size());
		
		rval = ::libusb_bulk_transfer(m_hnd, m_oaddr, static_cast< unsigned char* >(dat.data()), cnt, &len, 0);

		if (0 > rval)
			throw std::runtime_error(usb_t::usb_error("async_usb_t::transmit(): Error while transmitting bulk data", static_cast< err_t >(rval)));

		if (0 > len || static_cast< std::size_t >(len) > cnt) 
			throw std::runtime_error("async_usb_t::transmit(): Transmit length exceeds requested length");

		dat.erase(dat.begin(), dat.begin()+len);
	}

	return;

}

bool
async_usb_t::transmit_timeout(const std::vector< uint8_t >& vec, const std::size_t max, const std::size_t timeout)
{
	std::lock_guard< std::mutex >   lck(m_mutex);
	std::vector< uint8_t >          dat(vec);
	std::size_t                     cnt(0);
	signed int                      rval(0);
	signed int                      len(0);

	if (nullptr == m_hnd)
		throw std::runtime_error("async_usb_t::transmit(): Handle to USB device is invalid");

	while (0 != dat.size()) {
		len = 0;
		cnt = std::min(max, dat.size());

		rval = ::libusb_bulk_transfer(m_hnd, m_oaddr, static_cast< unsigned char* >(dat.data()), cnt, &len, timeout);

		if (0 == rval)
			return false;

		if (0 > rval) {
			if (LIBUSB_ERROR_TIMEOUT == static_cast< err_t >(rval) && (cnt == max && 0 == len))
				return false;
			
			throw std::runtime_error(usb_t::usb_error("async_usb_t::transmit(): Error while transmitting bulk data", static_cast< err_t >(rval)));
		}

		if (0 > len || static_cast< std::size_t >(len) > cnt)
			throw std::runtime_error("async_usb_t::transmit(): Transmit length exceeds requested length");

		dat.erase(dat.begin(), dat.begin()+len);
	}

	return true;
}

/*void
async_usb_t::handle_events(void)
{
	err_t e(LIBUSB_SUCCESS);

	e = static_cast< err_t >(::libusb_handle_events(m_ctxt));

	if (LIBUSB_SUCCESS != e) {
		if (LIBUSB_ERROR_INTERRUPTED == e)
			return;

		throw std::runtime_error(usb_t::usb_error("Error while handling USB events", e));
	}

	return;
}*/
