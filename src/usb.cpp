#include "usb.hpp"

usb_conf_desc_t*
usb_info_t::get_config_descriptor(usb_dev_t& dev, uint8_t idx)
{
	usb_conf_desc_t*    retval(nullptr);
	err_t               err(LIBUSB_SUCCESS);

	err = static_cast< err_t >(::libusb_get_config_descriptor(&dev, idx, &retval));

	if (LIBUSB_SUCCESS != err) {
		if (LIBUSB_ERROR_NOT_FOUND == err)
			return nullptr;

		throw std::runtime_error(usb_t::usb_error("Error while retrieving USB device configuration descriptor", err));
	}

	return retval;
}

std::string
usb_t::lookup_class(uint8_t cls, uint8_t scls, uint8_t proto)
{
	std::string retval("");

	switch (cls) {
		case LIBUSB_CLASS_PER_INTERFACE:
			if (0 == scls && 0 == proto)
				retval = "PER INTERFACE CLASS";

			break;

		case LIBUSB_CLASS_AUDIO:
			retval = "AUDIO";
			break;

		case LIBUSB_CLASS_COMM:
			retval = "COMMUNICATIONS AND CDC CONTROL";
			break;

		case LIBUSB_CLASS_HID:
			retval = "HID DEVICE";
			break;

		case LIBUSB_CLASS_PHYSICAL:
			retval = "PHYSICAL DEVICE";
			break;

		case LIBUSB_CLASS_PRINTER:
			retval = "PRINTER DEVICE";
			break;

		case LIBUSB_CLASS_IMAGE:
			if (1 == scls && 1 == proto)
				retval = "STILL IMAGING";

			break;

		case LIBUSB_CLASS_MASS_STORAGE:
			retval = "MASS STORAGE";
			break;

		case LIBUSB_CLASS_HUB:
			if (0 == scls) {
				retval = "HUB";

				if (0 == proto)
					retval += " Full-speed";
				else if (1 == proto) 
					retval += "High-speed with single TT";
				else if (2 == proto)
					retval += "High-speed with multiple TTs";
			}
			
			break;

		case LIBUSB_CLASS_DATA:
			retval = "CDC-DATA";
			break;

		case LIBUSB_CLASS_SMART_CARD:
			retval =  "SMART CARD";
			break;

		case LIBUSB_CLASS_CONTENT_SECURITY:
			if (0 == scls && 0 == proto)
				retval = "CONTENT SECURITY";

			break;

		case LIBUSB_CLASS_VIDEO:
			retval = "VIDEO";
			break;

		case LIBUSB_CLASS_PERSONAL_HEALTHCARE:
			retval = "PERSONAL HEALTHCARE";
			break;

		case 0x10:	// AV
			if (1 == scls && 0 == proto) 
				retval = "A/V CONTROL";
			if (2 == scls && 0 == proto)
				retval = "A/V DATA VIDEO";
			if (3 == scls && 0 == proto)
				retval = "A/V DATA AUDIO";

			break;

		case 0x11:  // Billboard
			if (0 == scls && 0 == proto)
				retval = "BILLBOARD";

			break;

		case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:
			if (1 == scls && 1 == proto)
				retval = "DIAGNOSTIC";

			break;

		case LIBUSB_CLASS_WIRELESS:
			if (1 == scls) {
				switch (proto) {
					case 1:
						retval = "WIRELESS BLUETOOTH PROGRAMMING INTERFACE";
						break;

					case 2:
						retval = "WIRELESS UWB RADIO CONTROL INTERFACE";
						break;

					case 3:
						retval = "WIRELESS REMOTE NDIS";
						break;

					case 4:
						retval = "WIRELESS BLUETOOTH AMP CONTROLLER";
						break;

					default:
						break;
				}
				
			} else if (2 == scls) {
				if (1 == proto)
					retval = "WIRELESS HOST WIRE ADAPTER CONTROL/DATA INTERFACE";
				else if (2 == proto)
					retval = "WIRELESS DEVICE WIRE ADAPTER CONTROL/DATA INTERFACE";
				else if (3 == proto)
					retval = "WIRELESS DEVICE WIRE ADAPTER ISOCHRONOUS INTERFACE";
			}
			
			break;

		case 0xEF: // misc
			switch (scls) {
				case 1:
					if (1 == proto)
						retval = "MISC ACTIVESYNC";
					else if (2 == proto)
						retval = "MISC PALM SYNC";

					break;

				case 2:
					if (1 == proto)
						retval = "MISC INTERFACE ASSOCIATION DESCRIPTOR";
					else if (2 == proto)
						retval = "MISC WIRE ADAPTER MULTIFUNCTION PERIPHERAL";

					break;

				case 3:
					if (1 == proto)
						retval = "MISC CABLE BASED ASSOCIATION FRAMEWORK";

					break;

				case 4:
					switch (proto) {
						case 1:
							retval = "MISC RNDIS OVER ETHERNET";
							break;

						case 2:
							retval = "MISC RNDIS OVER WIFI";
							break;

						case 3:
							retval = "MISC RNDIS OVER WIMAX";
							break;

						case 4:
							retval = "MISC RNDIS OVER WWAN";
							break;

						case 5:
							retval = "MISC RNDIS FOR RAW IPV4";
							break;

						case 6:
							retval = "MISC RNDIS FOR RAW IPV6";
							break;

						case 7:
							retval = "MISC RNDIS FOR GPRS";
							break;

						default:
							break;
					}

					break;

				case 5:
					if (0 == proto)
						retval = "MISC USB3 VISION CONTROL INTERFACE";
					else if (1 == proto)
						retval = "MISC USB3 VISION EVENT INTERFACE";
					else if (2 == proto)
						retval = "MISC USB3 VISION STREAMING INTERFACE";
			}

			break;

		case LIBUSB_CLASS_APPLICATION:
			if (1 == scls && 1 == proto)
				retval = "DEVICE FIRMWARE UPGRADE";	
			else if (2 == scls && 0 == proto)
				retval = "IRDA BRIDGE DEVICE";
			else if (3 == scls && 0 == proto)
				retval = "USB TEST AND MEASUREMENT DEVICE";
			else if (3 == scls && 1 == proto)
				retval = "USB TEST AND MEASUREMENT DEVICE (USB488 COMPLIANT)";
			
			break;

		case LIBUSB_CLASS_VENDOR_SPEC:
			retval = "VENDOR SPECIFIC DEVICE";
			break;

		default:
			break;
	}

	retval += " (" + to_hex_string(cls) + ":" + to_hex_string(scls) + ":" + to_hex_string(proto) + ")";
	return retval;
}

void
usb_info_t::init_dev(usb_handle_t& hnd, usb_dev_desc_t& desc)
{
	signed int		rval(0);
	unsigned char	buf[4096] = {0};

	m_vars["device_class"]		= usb_t::lookup_class(desc.bDeviceClass, desc.bDeviceSubClass, desc.bDeviceProtocol);
	m_vars["vendor_id"]			= "0x" + to_hex_string(desc.idVendor);
	m_vars["product_id"]		= "0x" + to_hex_string(desc.idProduct);
	m_vars["num_configs"]		= "0x" + to_hex_string(desc.bNumConfigurations);
	m_vars["usb"] 				= "USB v" + to_hex_string(static_cast< uint8_t >(+(desc.bcdUSB >> 8))) + "." 
								+ to_hex_string(static_cast< uint8_t >(+(desc.bcdUSB & 0xFF)));
	m_vars["device_version"]	= "v" + to_hex_string(static_cast< uint8_t >(+(desc.bcdDevice & 0xFF))) + "." 
								+ to_hex_string(static_cast< uint8_t >(+(desc.bcdDevice >> 8)));

	printf("num_configs: %u\n", desc.bNumConfigurations);

	rval = ::libusb_get_string_descriptor_ascii(&hnd, desc.iManufacturer, buf, sizeof(buf));

	if (0 < rval)
		m_vars["manufacturer"] = std::string(reinterpret_cast< const char* >(&buf[0]));

	rval = ::libusb_get_string_descriptor_ascii(&hnd, desc.iProduct, buf, sizeof(buf));

	if (0 < rval)
		m_vars["product"] = std::string(reinterpret_cast< const char* >(&buf[0]));

	rval = ::libusb_get_string_descriptor_ascii(&hnd, desc.iSerialNumber, buf, sizeof(buf));

	if (0 < rval)
		m_vars["serial_number"] = std::string(reinterpret_cast< const char* >(&buf[0]));

	return;
}

usb_info_t::usb_info_t(usb_dev_t& dev, usb_handle_t& hnd, usb_dev_desc_t& desc) 
{
	usb_conf_desc_t*    conf(nullptr);

	init_dev(hnd, desc);

	for (std::size_t idx = 0; idx < desc.bNumConfigurations; idx++) {
		conf = get_config_descriptor(dev, idx);

		if (nullptr == conf) 
			continue;

		m_confs.push_back(usb_configuration_t(hnd, *conf));

		::libusb_free_config_descriptor(conf);
	}

	return;
}

usb_info_t::~usb_info_t(void)
{
	m_vars.clear();
	m_confs.clear();
	return;
}

std::string
usb_info_t::to_string(void)
{
	std::string retval("");

	for (auto itr = m_vars.begin(); itr != m_vars.end(); itr++)
		retval += itr->first + ": " + itr->second + "\r\n";
	
	for (std::size_t idx = 0; idx < m_confs.size(); idx++) {
		retval += "Configuration 0x" + to_hex_string(idx) + "\r\n";
		retval += m_confs.at(idx).to_string();
	}

	return retval;
}

usb_configuration_t&
usb_info_t::configuration(std::size_t idx)
{
	if (idx >= size())
		throw std::runtime_error("usb_info_t::configuration(): invalid index specified");
	
	return m_confs.at(idx);
}

usb_configuration_t::usb_configuration_t(usb_handle_t& hnd, const usb_conf_desc_t& desc)
{
	signed int 		rval(0);
	unsigned char	buf[4096] = {0};

	m_vars["number_interfaces"] 	= "0x" + to_hex_string(desc.bNumInterfaces);
	m_vars["configuration_value"]	= "0x" + to_hex_string(desc.bConfigurationValue);
	m_vars["max_power"]				= "0x" + to_hex_string(desc.MaxPower);

	printf("number_interfaces: %u\n", desc.bNumInterfaces);

	if ((desc.bmAttributes & CON_DESC_ATTR_BUS_POWERED) == CON_DESC_ATTR_BUS_POWERED)
		m_vars["attributes"] += "BUS_POWERED ";
	
	if ((desc.bmAttributes & CON_DESC_ATTR_SELF_POWERED) == CON_DESC_ATTR_SELF_POWERED)
		m_vars["attributes"] += "SELF_POWERED ";
	
	if ((desc.bmAttributes & CON_DESC_ATTR_REMOTE_WAKE) == CON_DESC_ATTR_REMOTE_WAKE)
		m_vars["attributes"] += "REMOTE WAKEUP ";

	m_vars["attributes"] += " (0x" + to_hex_string(desc.bmAttributes) + ")";

	rval = ::libusb_get_string_descriptor_ascii(&hnd, desc.iConfiguration, buf, sizeof(buf));

	if (0 < rval)
		m_vars["configuration"] = std::string(reinterpret_cast< const char* >(&buf[0]));
	
	for (std::size_t idx = 0; idx < desc.bNumInterfaces; idx++) 
		m_interfaces.push_back(usb_interface_t(hnd, desc.interface[idx]));

	return;
}

usb_configuration_t::~usb_configuration_t(void)
{
	m_vars.clear();
	m_interfaces.clear();
	return;
}

usb_interface_t&
usb_configuration_t::interface(std::size_t idx)
{
	if (idx >= size())
		throw std::runtime_error("usb_configuration_t::interface(): invalid index specified");

	return m_interfaces.at(idx);
}

std::string
usb_configuration_t::to_string(void)
{
	std::string retval("");

	for (auto itr = m_vars.begin(); itr != m_vars.end(); itr++)
		retval += itr->first + ": " + itr->second + "\r\n";

	for (std::size_t idx = 0; idx < size(); idx++) {
		retval += "Interface 0x" + to_hex_string(idx) + "\r\n";
		retval += m_interfaces.at(idx).to_string();
	}

	return retval;
}

usb_interface_t::usb_interface_t(usb_handle_t& hnd, const usb_iface_t& interface)
{
	for (signed int idx = 0; idx < interface.num_altsetting; idx++) 
		m_alternates.push_back(usb_altsettings_t(hnd, interface.altsetting[idx]));

	return;
}

usb_interface_t::~usb_interface_t(void)
{
	return;
}

usb_altsettings_t&
usb_interface_t::alternate(std::size_t idx)
{
	if (idx >= size())
		throw std::runtime_error("usb_interface_t::alternate(): invalid index specified");

	return m_alternates.at(idx);
}

std::string
usb_interface_t::to_string(void)
{
	std::string retval("");

	for (auto itr = m_vars.begin(); itr != m_vars.end(); itr++) 
		retval += itr->first + ": "  + itr->second + "\r\n";
	
	for (std::size_t idx = 0; idx < size(); idx++) {
		retval += "Alternate 0x" + to_hex_string(idx) + "\r\n";
		retval += m_alternates.at(idx).to_string();
	}

	return retval;
}


usb_altsettings_t::usb_altsettings_t(usb_handle_t& hnd, const usb_iface_desc_t& desc)
{
	signed int 		rval(0);
	unsigned char	buf[4096] = {0};

	m_vars["interface_class"]		= usb_t::lookup_class(desc.bInterfaceClass, desc.bInterfaceSubClass, desc.bInterfaceProtocol);
//	m_vars["interface_class"]		= "0x" + to_hex_string(desc.bInterfaceClass);
//	m_vars["interface_sub_class"]	= "0x" + to_hex_string(desc.bInterfaceSubClass);
//	m_vars["interface_protocol"]	= "0x" + to_hex_string(desc.bInterfaceProtocol);
	m_vars["alternate_setting"]		= "0x" + to_hex_string(desc.bAlternateSetting);
	m_vars["interface_number"]		= "0x" + to_hex_string(desc.bInterfaceNumber);

	rval = ::libusb_get_string_descriptor_ascii(&hnd, desc.iInterface, buf, sizeof(buf));

	if (0 < rval) 
		m_vars["interface"] = std::string(reinterpret_cast< const char* >(&buf[0]));
	
	for (std::size_t idx = 0; idx < desc.bNumEndpoints; idx++)
		m_endpoints.push_back(usb_endpoint_info_t(desc.endpoint[idx]));

	return;
}

usb_altsettings_t::~usb_altsettings_t(void)
{
	m_endpoints.clear();
	m_vars.clear();
}

usb_endpoint_info_t&
usb_altsettings_t::endpoint(std::size_t idx)
{
	if (idx > m_endpoints.size())
		throw std::runtime_error("usb_altsettings_t::endpoint(): invalid index specified");
	
	return m_endpoints.at(idx);
}

std::string
usb_altsettings_t::to_string(void)
{
	std::string retval("");

	for (auto itr = m_vars.begin(); itr != m_vars.end(); itr++)
		retval += itr->first + ": "  + itr->second + "\r\n";

	for (std::size_t idx = 0; idx < size(); idx++) {
		retval += "Endpoint 0x" + to_hex_string(idx) + "\r\n";
		retval += m_endpoints.at(idx).to_string();
	}

	return retval;
}

usb_endpoint_info_t::usb_endpoint_info_t(const usb_endpoint_desc_t& desc)
{
	m_vars["max_packet_size"]	= "0x" + to_hex_string(desc.wMaxPacketSize);
	m_vars["interval"]			= "0x" + to_hex_string(desc.bInterval);
	m_vars["refresh"]			= "0x" + to_hex_string(desc.bRefresh);
	m_vars["synch_address"]		= "0x" + to_hex_string(desc.bSynchAddress);

	if (LIBUSB_TRANSFER_TYPE_CONTROL  == (LIBUSB_TRANSFER_TYPE_MASK & desc.bmAttributes))
		m_vars["attributes"] += "CONTROL ";
	
	if (LIBUSB_TRANSFER_TYPE_ISOCHRONOUS == (LIBUSB_TRANSFER_TYPE_MASK & desc.bmAttributes))
		m_vars["attributes"] += "ISOCHRONOUS ";
	
	if (LIBUSB_TRANSFER_TYPE_BULK == (LIBUSB_TRANSFER_TYPE_MASK & desc.bmAttributes))
		m_vars["attributes"] += "BULK ";

	if (LIBUSB_TRANSFER_TYPE_INTERRUPT == (LIBUSB_TRANSFER_TYPE_MASK & desc.bmAttributes))
		m_vars["attributes"] += "INTERRUPT";

	if (LIBUSB_TRANSFER_TYPE_BULK_STREAM == (LIBUSB_TRANSFER_TYPE_MASK & desc.bmAttributes))
		m_vars["attributes"] += "BULK_STREAM";

	m_vars["attributes"] += " (0x" + to_hex_string(desc.bmAttributes) + ")";
	m_attr = desc.bmAttributes;

	m_vars["address"] += " 0x" + to_hex_string(desc.bEndpointAddress & LIBUSB_ENDPOINT_ADDRESS_MASK); 
	m_vars["address"] += " ";

	if (LIBUSB_ENDPOINT_IN == (LIBUSB_ENDPOINT_DIR_MASK & desc.bEndpointAddress))
		m_vars["address"] +=  "INBOUND ";
	
	if (LIBUSB_ENDPOINT_OUT == (LIBUSB_ENDPOINT_DIR_MASK & desc.bEndpointAddress))
		m_vars["address"] += "OUTBOUND";
	
	m_addr 	= (desc.bEndpointAddress & LIBUSB_ENDPOINT_ADDRESS_MASK);
	m_dir	= (desc.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK);

	m_vars["address"] += " (0x" + to_hex_string(desc.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) + ")";

	return;
}

usb_endpoint_info_t::~usb_endpoint_info_t(void)
{
	m_vars.clear();
	return;
}

std::string
usb_endpoint_info_t::to_string(void)
{
	std::string retval("");

	for (auto itr = m_vars.begin(); itr != m_vars.end(); itr++)
		retval += itr->first + ": "  + itr->second + "\r\n";

	return retval;
}

usb_t::usb_t(void) : 	m_ctxt(nullptr), m_handle(nullptr), m_info(nullptr), m_max(static_cast< std::size_t >(-1))
{
	std::lock_guard< std::mutex > 	lck(m_mutex);
	err_t							retval = static_cast< err_t >(::libusb_init(&m_ctxt));

	if (0 != retval) 
		throw std::runtime_error(std::string("Error while initializing libusb: ") + ::libusb_strerror(retval));

	//::libusb_set_debug(m_ctxt, LIBUSB_LOG_LEVEL_DEBUG);	

	return;
}

usb_t::~usb_t(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (nullptr != m_handle) {
		release_all();
		close();
	}

	if (nullptr != m_ctxt) {
		::libusb_exit(m_ctxt);
		m_ctxt = nullptr;
	}

	if (nullptr != m_info) {
		delete m_info;
		m_info = nullptr;
	}

	return;
}

usb_dev_desc_t
usb_t::get_device_descriptor(usb_dev_t& dev)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	usb_dev_desc_t 					retval = {0};
	err_t							err(LIBUSB_SUCCESS);

	err = static_cast< err_t >(::libusb_get_device_descriptor(&dev, &retval));

	if (LIBUSB_SUCCESS != err) 
		throw std::runtime_error(usb_t::usb_error("Error retrieving USB device descriptor", err));
	
	return retval;
}

usb_conf_desc_t*
usb_t::get_config_descriptor(usb_dev_t& dev, uint8_t idx)
{
	std::lock_guard< std::mutex > 	lck(m_mutex);
	usb_conf_desc_t*				retval(nullptr);
	err_t							err(LIBUSB_SUCCESS);
	
	err = static_cast< err_t >(::libusb_get_config_descriptor(&dev, idx, &retval));

	if (LIBUSB_SUCCESS != err) {
		if (LIBUSB_ERROR_NOT_FOUND == err)
			return nullptr;

		throw std::runtime_error(usb_t::usb_error("Error while retrieving USB device configuration descriptor", err));
	}

	return retval;
}

bool
usb_t::kernel_is_attached(uint8_t interface)
{
	if (nullptr == m_handle)
		throw std::runtime_error("usb_t::kernel_is_attached(): USB device handle is in an invalid state");

	if (1 == ::libusb_kernel_driver_active(m_handle, interface))
		return true;
	
	return false;
}

void
usb_t::detach_kernel(uint8_t interface)
{
//	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t 							err(LIBUSB_SUCCESS);

	if (nullptr == m_handle) 
		throw std::runtime_error("detach_kernel(): USB device handle is not valid");

	err = static_cast< err_t >(::libusb_set_auto_detach_kernel_driver(m_handle, 1));

	if (LIBUSB_SUCCESS != err) {

		if (1 != ::libusb_kernel_driver_active(m_handle, interface))
			return;

		err = static_cast< err_t >(::libusb_detach_kernel_driver(m_handle, interface));

		if (LIBUSB_SUCCESS != err && LIBUSB_ERROR_NOT_FOUND != err) 
			throw std::runtime_error(usb_t::usb_error("Error detaching kernel driver from USB device", err));
	}

	return;
}

void
usb_t::set_config(int8_t conf)
{
	std::lock_guard< std::mutex > 	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_handle)
		throw std::runtime_error("usb_t::set_config(): USB device handle is not valid");

	err = static_cast< err_t >(::libusb_set_configuration(m_handle, conf));

	if (LIBUSB_SUCCESS != err)
		throw std::runtime_error(usb_t::usb_error("Error while setting USB device configuration", err));
	
	return;
}

void
usb_t::claim(uint8_t interface)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_handle)
		throw std::runtime_error("usb_t::claim(): USB device handle is not valid");

	if (kernel_is_attached(+interface)) 
		detach_kernel(+interface);

	err = static_cast< err_t >(::libusb_claim_interface(m_handle, +interface));

	if (LIBUSB_SUCCESS != err) 
		throw std::runtime_error(usb_t::usb_error("Error claiming ownership of USB device", err));

	return;
}

void
usb_t::claim_all(void)
{
	if (nullptr == m_info)
		throw std::runtime_error("usb_t::claim_all(): Attempted to claim interface unopened USB device");


	for (auto& confs : m_info->configurations())
		for (std::size_t idx = 0; idx < confs.interfaces().size(); idx++)
			claim(idx);
			
	return;
}

void
usb_t::release(uint8_t interface)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	err_t							err(LIBUSB_SUCCESS);

	if (nullptr == m_handle)
		throw std::runtime_error("usb_t::release(): USB device handle is not valid");
	
	err = static_cast< err_t >(::libusb_release_interface(m_handle, interface));

	if (LIBUSB_SUCCESS != err && LIBUSB_ERROR_NOT_FOUND != err)
		throw std::runtime_error(usb_t::usb_error("usb_t::release(): Error releasing ownership of USB device", err));

	return;
}


void
usb_t::release_all(void)
{
	if (nullptr == m_info)
		throw std::runtime_error("usb_t::release_all(): Attempted to release interfaces on unopened USB device");

	for (auto& confs : m_info->configurations())
		for (std::size_t idx = 0; idx < confs.interfaces().size(); idx++)
			release(idx);

	return;
}

usb_info_t&
usb_t::info(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (nullptr == m_info)
		throw std::runtime_error("usb_t::info(): invoked during invalid state, please open the device first");

	return *m_info;
}

std::string
usb_t::to_string(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (nullptr == m_info)
		throw std::runtime_error("usb_t::to_string(): invoked during invalid state, please open the device first");

	return m_info->to_string();
}

bool
usb_t::open(uint16_t vendor, uint16_t product)
{
	usb_dev_t**	devs(nullptr);
	ssize_t		cnt(0);
	err_t		err(LIBUSB_SUCCESS);
	bool		found(false);

	m_mutex.lock();

	if (nullptr != m_handle) {
		m_mutex.unlock();
		close();
		m_mutex.lock();
	}

	cnt = ::libusb_get_device_list(m_ctxt, &devs);
	m_mutex.unlock();

	if (0 > cnt) 
		throw std::runtime_error(usb_t::usb_error("Error retrieving USB device list", static_cast< err_t >(cnt)));

	try {
		for (ssize_t idx = 0; idx < cnt; idx++) {
			usb_dev_desc_t 		desc = get_device_descriptor(*devs[idx]); 

			if (vendor == desc.idVendor && product == desc.idProduct) {
				m_mutex.lock();
				err = static_cast< err_t >(::libusb_open(devs[idx], &m_handle));
				m_mutex.unlock();

				if (LIBUSB_SUCCESS != err) 
					throw std::runtime_error(usb_t::usb_error("Error opening USB device", err));

				m_mutex.lock();
				err = static_cast< err_t >(::libusb_reset_device(m_handle));
				m_mutex.unlock();

				if (LIBUSB_SUCCESS != err)
					throw std::runtime_error(usb_t::usb_error("Error reseting USB device", err));

				m_mutex.lock();
				m_info = new usb_info_t(*devs[idx], *m_handle, desc);
				m_mutex.unlock();

				for (auto& ep : get_endpoints()) {
					signed int max = ::libusb_get_max_packet_size(devs[idx], ep.get_addr());

					if (0 >= max && m_max > static_cast< std::size_t >(max))
						m_max = max;
				}

				//detach_kernel(0);
				//claim(0);
				found = true;
				break;
			}
			
		}

	} catch (std::runtime_error& e) {
		::libusb_free_device_list(devs, 1);
		
		if (nullptr != m_handle) {
			//release();
			close();
		}
		
		throw e;
	}

	::libusb_free_device_list(devs, 1);
	return found;	
}

void
usb_t::close(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (nullptr == m_handle)
		return;

	::libusb_close(m_handle);
	m_handle = nullptr;

	return;
}

std::vector< usb_endpoint_info_t >
usb_t::get_endpoints(void)
{
	std::lock_guard< std::mutex > 		lck(m_mutex);
	std::vector< usb_endpoint_info_t >	retval;

	if (nullptr == m_info)
		throw std::runtime_error("usb_t::get_endpoints(): called from invalid states");

	for (auto& conf : m_info->configurations())
		for (auto& interface : conf.interfaces())
			for (auto& alts : interface.alternates())
				for (auto& endpoint : alts.endpoints())
					retval.push_back(endpoint);
	
	return retval;
}

std::vector< usb_info_t* >
usb_t::enumerate_devices(void)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	usb_dev_t**						devs(nullptr);
	ssize_t							cnt(0);
	err_t							err(LIBUSB_SUCCESS);
	std::vector< usb_info_t* >		retval;

	cnt = ::libusb_get_device_list(m_ctxt, &devs);

	if (0 > cnt) 
		throw std::runtime_error(usb_t::usb_error("Error retrieving USB device list", static_cast< err_t >(cnt))); 

	try {
		for (ssize_t idx = 0; idx < cnt; idx++) {
			usb_dev_desc_t 	desc 	= get_device_descriptor(*devs[idx]);
			usb_handle_t*	hnd 	= nullptr;

			err = static_cast< err_t >(::libusb_open(devs[idx], &hnd));

			if (LIBUSB_SUCCESS != err || nullptr == hnd) 
				throw std::runtime_error(usb_t::usb_error("Error opening USB device", err));
			
			retval.push_back(new usb_info_t(*devs[idx], *hnd, desc));
			::libusb_close(hnd);
		}
	} catch (std::runtime_error& e) {
		for (std::size_t idx = 0; idx < retval.size(); idx++)
			delete retval.at(idx);

		::libusb_free_device_list(devs, 1);

		throw e;
	}


	::libusb_free_device_list(devs, 1);
	return retval;
}

bool 
usb_t::ctrl_xfer(uint8_t type, uint8_t req, uint16_t val, uint16_t idx, std::vector< uint8_t >& data, std::size_t& len, unsigned int to)
{
	std::lock_guard< std::mutex > 	lck(m_mutex);
	signed int						rval(0);

	if (nullptr == m_handle)
		return false;

	if (USHRT_MAX <= data.size())
		return false;

	rval = ::libusb_control_transfer(m_handle, type, req, val, idx, data.data(), data.size(), to);

	if (0 > rval) 
		return false;

	len = rval;

	if (INT_MAX < len)
		return false;

	return true;
}

void
usb_t::bulk_send(uint8_t ep, std::vector< uint8_t >& data, std::size_t& len, unsigned int to)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	signed int						rval(0);
	std::size_t						cnt(0);

	if (nullptr == m_handle)
		throw std::runtime_error("usb_t::bulk_send(): called while in an invalid state");

	if (INT_MAX <= data.size())
		throw std::runtime_error("usb_t::bulk_send(): overly large outbound buffer encountered");

	while (data.size()) {
		if (m_max > data.size())
			cnt = data.size();
		else 
			cnt = m_max;

		rval = ::libusb_bulk_transfer(m_handle, ep, data.data(), cnt, reinterpret_cast< signed int* >(&len), to);

		if (0 > rval) 
			throw std::runtime_error(usb_t::usb_error("Error during bulk send", static_cast< err_t >(rval)));

		data.erase(data.begin(), data.begin()+len);
	}

	return;
}

void
usb_t::bulk_recv(uint8_t ep, std::vector< uint8_t >& data, unsigned int to)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	signed int						rval(0);
	signed int						len(0);

	rval = ::libusb_bulk_transfer(m_handle, ep, data.data(), data.size(), reinterpret_cast< signed int* >(&len), to);

	if (0 > rval) 
		throw std::runtime_error(usb_t::usb_error("Error during bulk recv", static_cast< err_t >(rval)));
	
	data.resize(len);
	return;
}

bool 
usb_t::bulk_xfer(uint8_t ep, std::vector< uint8_t >& data, std::size_t& len, unsigned int to)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	signed int						rval(0);
	std::size_t						cnt(0);
//	std::size_t						rem(0);

	if (nullptr == m_handle)
		return false;

	if (INT_MAX <= data.size())
		return false;

	if (data.size() > m_max && (ep & LIBUSB_ENDPOINT_OUT) == ep) {
		cnt = data.size() / m_max;
//		rem = data.size() % m_max;

		while (data.size()) {
			if (m_max > data.size())
				cnt = data.size();
			else
				cnt = m_max;

			rval = ::libusb_bulk_transfer(m_handle, ep, data.data(), cnt, reinterpret_cast< signed int* >(&len), to);

			if (0 > rval) {
				throw std::runtime_error(usb_t::usb_error("Error during bulk transfer", static_cast< err_t >(rval)));
				return false;
			}

			data.erase(data.begin(), data.begin()+len);
		}

	} else {
		rval = ::libusb_bulk_transfer(m_handle, ep, data.data(), data.size(), reinterpret_cast< signed int* >(&len), to);

		if (0 > rval) {
			throw std::runtime_error(usb_t::usb_error("Error calling libusb_bulk_transfer()", static_cast< err_t >(rval)));
			return false;
		}

	}

	return true;
}

bool 
usb_t::int_xfer(uint8_t ep, std::vector< uint8_t >& data, std::size_t& len, unsigned int to)
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	signed int						rval(0);

	if (nullptr == m_handle)
		return false;

	if (INT_MAX <= data.size())
		return false;

	rval = ::libusb_interrupt_transfer(m_handle, ep, data.data(), data.size(), reinterpret_cast< signed int* >(&len), to);

	if (0 > rval)
		return false;

	if (INT_MAX < len)
		return false;

	return true;
}
