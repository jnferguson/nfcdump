#ifndef HAVE_CMD2_T_HPP
#define HAVE_CMD2_T_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <mutex>
#include <vector>
#include <sstream>
#include <iomanip>

//#include <string.h>
//#include "14a.hpp"

#include "otfta.hpp"

#define USB_CMD_DSIZE 512 

typedef enum ISO14A_COMMAND {
    ISO14A_CONNECT = 1,
    ISO14A_NO_DISCONNECT = 2,
    ISO14A_APDU = 4,
	ISO14A_NO_PARITY = 4,
    ISO14A_RAW = 8,
    ISO14A_REQUEST_TRIGGER = 0x10,
    ISO14A_APPEND_CRC = 0x20,
    ISO14A_SET_TIMEOUT = 0x40,
    ISO14A_NO_SELECT = 0x80
} iso14a_command_t;

typedef enum OTFTA_PARAMS {
	OTFTA_ENABLE 		= 1,
	OTFTA_DISABLE		= 2,
	OTFTA_NO_PARITY		= 3,
	OTFTA_APPEND_CRC	= 4,
	OTFTA_NO_RESPONSE	= 8,
	OTFTA_XMIT_BITS		= 0x10
} otfta_params_t;

typedef struct {
    uint8_t uid[10];
    uint8_t uidlen;
    uint8_t atqa[2];
    uint8_t sak;
    uint8_t ats_len;
    uint8_t ats[256];
} __attribute__((__packed__)) iso14a_card_select_t;

#define DIRECTION_PICC 0xEE
#define DIRECTION_PCD 0xFF
#define CMD_OTFTA 0xB000UL
#define DIR_MASK 0xFF

typedef struct {
	uint64_t		cmd;
//	uint64_t		dir;
	uint64_t		col;
//	uint8_t		par;
	uint64_t	dlen;
	uint64_t	mlen;
	uint8_t		demodulated[USB_CMD_DSIZE/2];
	uint8_t		modulated[USB_CMD_DSIZE/2];
} __attribute__((packed)) otfta_cmd;

#define CMD_OTFTA_RAW_SNIFF				0xB000
#define CMD_OTFTA_RAW_DUMP				0xB001

#define CMD_READER_ISO_14443a_CUSTOM	0xA001
#define CMD_ISO14443a_READER_DUMP		0xA002
#define CMD_ISO14443a_CARD_DUMP			0xA003   
#define CMD_ISO14443a_READER_MOD_DUMP	0xA004
#define CMD_ISO14443a_CARD_MOD_DUMP		0xA005

// For the bootloader
#define CMD_DEVICE_INFO								0x0000
#define CMD_SETUP_WRITE								0x0001
#define CMD_FINISH_WRITE							0x0003
#define CMD_HARDWARE_RESET							0x0004
#define CMD_START_FLASH								0x0005
#define CMD_NACK									0x00fe
#define CMD_ACK									  	0x00ff
// For general mucking around
#define CMD_DEBUG_PRINT_STRING						0x0100
#define CMD_DEBUG_PRINT_INTEGERS					0x0101
#define CMD_DEBUG_PRINT_BYTES						0x0102
#define CMD_LCD_RESET								0x0103
#define CMD_LCD										0x0104
#define CMD_BUFF_CLEAR								0x0105
#define CMD_READ_MEM								0x0106
#define CMD_VERSION									0x0107

// For low-frequency tags
#define CMD_READ_TI_TYPE							0x0202
#define CMD_WRITE_TI_TYPE							0x0203
#define CMD_DOWNLOADED_RAW_BITS_TI_TYPE				0x0204
#define CMD_ACQUIRE_RAW_ADC_SAMPLES_125K			0x0205
#define CMD_MOD_THEN_ACQUIRE_RAW_ADC_SAMPLES_125K	0x0206
#define CMD_DOWNLOAD_RAW_ADC_SAMPLES_125K			0x0207
#define CMD_DOWNLOADED_RAW_ADC_SAMPLES_125K			0x0208
#define CMD_DOWNLOADED_SIM_SAMPLES_125K				0x0209
#define CMD_SIMULATE_TAG_125K						0x020A
#define CMD_HID_DEMOD_FSK							0x020B
#define CMD_HID_SIM_TAG								0x020C
#define CMD_SET_LF_DIVISOR							0x020D
#define CMD_LF_SIMULATE_BIDIR						0x020E
#define CMD_SET_ADC_MUX								0x020F
#define CMD_HID_CLONE_TAG							0x0210
#define CMD_EM410X_WRITE_TAG						0x0211
#define CMD_INDALA_CLONE_TAG						0x0212
// for 224 bits UID
#define CMD_INDALA_CLONE_TAG_L						0x0213
#define CMD_T55XX_READ_BLOCK						0x0214
#define CMD_T55XX_WRITE_BLOCK						0x0215
#define CMD_T55XX_READ_TRACE						0x0216
#define CMD_PCF7931_READ							0x0217
#define CMD_EM4X_READ_WORD							0x0218
#define CMD_EM4X_WRITE_WORD							0x0219
#define CMD_IO_DEMOD_FSK							0x021A
#define CMD_IO_CLONE_TAG							0x021B
/* CMD_SET_ADC_MUX: ext1 is 0 for lopkd, 1 for loraw, 2 for hipkd, 3 for hiraw */

// For the 13.56 MHz tags
#define CMD_ACQUIRE_RAW_ADC_SAMPLES_ISO_15693		0x0300
#define CMD_ACQUIRE_RAW_ADC_SAMPLES_ISO_14443		0x0301
#define CMD_READ_SRI512_TAG							0x0303
#define CMD_READ_SRIX4K_TAG							0x0304
#define CMD_ISO_14443B_COMMAND						0x0305
#define CMD_READER_ISO_15693						0x0310
#define CMD_SIMTAG_ISO_15693						0x0311
#define CMD_RECORD_RAW_ADC_SAMPLES_ISO_15693		0x0312
#define CMD_ISO_15693_COMMAND						0x0313
#define CMD_ISO_15693_COMMAND_DONE					0x0314
#define CMD_ISO_15693_FIND_AFI						0x0315
#define CMD_ISO_15693_DEBUG							0x0316
#define CMD_LF_SNOOP_RAW_ADC_SAMPLES				0x0317

// For Hitag2 transponders
#define CMD_SNOOP_HITAG								0x0370
#define CMD_SIMULATE_HITAG							0x0371
#define CMD_READER_HITAG							0x0372

#define CMD_SIMULATE_TAG_HF_LISTEN					0x0380
#define CMD_SIMULATE_TAG_ISO_14443					0x0381
#define CMD_SNOOP_ISO_14443							0x0382
#define CMD_SNOOP_ISO_14443a						0x0383
#define CMD_SIMULATE_TAG_ISO_14443a					0x0384
#define CMD_READER_ISO_14443a						0x0385
#define CMD_SIMULATE_TAG_LEGIC_RF					0x0387
#define CMD_READER_LEGIC_RF							0x0388
#define CMD_WRITER_LEGIC_RF							0x0389
#define CMD_EPA_PACE_COLLECT_NONCE					0x038A

#define CMD_SNOOP_ICLASS							0x0392
#define CMD_SIMULATE_TAG_ICLASS						0x0393
#define CMD_READER_ICLASS							0x0394
#define CMD_READER_ICLASS_REPLAY					0x0395
#define CMD_ICLASS_ISO14443A_WRITE					0x0397

// For measurements of the antenna tuning
#define CMD_MEASURE_ANTENNA_TUNING					0x0400
#define CMD_MEASURE_ANTENNA_TUNING_HF				0x0401
#define CMD_MEASURED_ANTENNA_TUNING					0x0410
#define CMD_LISTEN_READER_FIELD						0x0420

// For direct FPGA control
#define CMD_FPGA_MAJOR_MODE_OFF						0x0500

// For mifare commands
#define CMD_MIFARE_SET_DBGMODE						0x0600
#define CMD_MIFARE_EML_MEMCLR						0x0601
#define CMD_MIFARE_EML_MEMSET						0x0602
#define CMD_MIFARE_EML_MEMGET						0x0603
#define CMD_MIFARE_EML_CARDLOAD						0x0604
#define CMD_MIFARE_EML_CSETBLOCK					0x0605
#define CMD_MIFARE_EML_CGETBLOCK					0x0606

#define CMD_SIMULATE_MIFARE_CARD					0x0610

#define CMD_READER_MIFARE							0x0611
#define CMD_MIFARE_NESTED							0x0612

#define CMD_MIFARE_READBL							0x0620
#define CMD_MIFAREU_READBL							0x0720
#define CMD_MIFARE_READSC							0x0621
#define CMD_MIFAREU_READCARD						0x0721
#define CMD_MIFARE_WRITEBL							0x0622
#define CMD_MIFAREU_WRITEBL_COMPAT					0x0722
#define CMD_MIFAREU_WRITEBL					   		0x0723
#define CMD_MIFARE_CHKKEYS							0x0623
#define CMD_MIFARE_SNIFFER							0x0630

#define CMD_UNKNOWN									0xFFFF

#define USB_CMD_DSIZE 512 

typedef struct _usb_cmd_t {
	uint64_t		cmd;
	uint64_t		arg[3];

	union {
		uint8_t 	bytes[USB_CMD_DSIZE ];
		uint32_t	dwords[USB_CMD_DSIZE/sizeof(uint32_t)];
	} d;
} __attribute__((packed)) usb_cmd_t;

class cmd_t 
{
	private:
	protected:
		std::vector< uint8_t >	m_raw;
		usb_cmd_t*				m_cmd;
		uint8_t*				m_data;
		uint64_t				m_dlen;

		virtual void parse(void) = 0;

	public:
		cmd_t(void);
		virtual ~cmd_t(void);

		virtual void set(std::vector< uint8_t >&);
		virtual void set(const uint8_t*, std::size_t);

		virtual uint64_t get_cmd(void) const;
		virtual void set_cmd(uint64_t);

		virtual uint64_t get_arg(uint8_t) const;
		virtual void set_arg(uint8_t, uint64_t);

		virtual uint8_t* get_data(std::size_t off = 0) const;
		virtual uint64_t get_dlen(void) const;

		virtual void reset(void);

		virtual const std::vector< uint8_t >& vector(void) { return m_raw; }
};

class cmd_dbg_str_t : public cmd_t
{
	private:
	protected:
		void parse(void);

	public:
		cmd_dbg_str_t(void) : cmd_t() { } 
		virtual ~cmd_dbg_str_t(void) { }
		
		std::string to_string(void) 
		{ 
			if (sizeof(usb_cmd_t) != m_raw.size())
				throw std::runtime_error("cmd_dbg_str_t::to_string(): Invalid data encountered");

			if (nullptr == m_cmd)
				throw std::runtime_error("cmd_dbg_str_t::to_string(): Object is in an invalid state");

			if (CMD_DEBUG_PRINT_STRING != m_cmd->cmd)
				throw std::runtime_error("cmd_dbg_str_t::to_string(): Command is not a debug string");

			return std::string(reinterpret_cast< const char* >(get_data())); 
		}
};

class cmd_raw_sniff_t : public cmd_t
{
	private:
	protected:
		virtual void parse(void) {}

	public:
		cmd_raw_sniff_t(void) 
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 		= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd	= CMD_OTFTA_RAW_SNIFF;
			m_data		= &m_cmd->d.bytes[0];

			return;
		}

		virtual ~cmd_raw_sniff_t(void)
		{
			m_raw.clear();
			m_cmd 	= nullptr;
			m_data	= nullptr;

			return;
		}
};

class cmd_version_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}

	public:
		cmd_version_t(void) : cmd_t() 
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 		= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd 	= CMD_VERSION;
			m_data		= &m_cmd->d.bytes[0];

			return;
		}

		~cmd_version_t(void)
		{
			m_raw.clear();
			m_cmd 	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_tune_t : public cmd_t
{
	private:
		signed int m_peakv;
		signed int m_peakf;
		signed int m_vlf125;
		signed int m_vlf134;
		signed int m_vhf;

	protected:
		void 
		parse(void) 
		{
			if (sizeof(usb_cmd_t) != m_raw.size())
				throw std::runtime_error("cmd_tune_t::parse(): Input buffer is invalid");

			if (nullptr == m_cmd)
				m_cmd = reinterpret_cast< usb_cmd_t* >(&m_raw[0]);

			if (CMD_MEASURED_ANTENNA_TUNING != m_cmd->cmd)
				throw std::runtime_error("cmd_tune_t::parse(): Input command is not CMD_MEASURED_ATENNA_TUNING");

			m_vlf125 	= m_cmd->arg[0] & 0xffff;
			m_vlf134 	= m_cmd->arg[0] >> 16;
			m_vhf		= m_cmd->arg[1] & 0xffff;
			m_peakf		= m_cmd->arg[2] & 0xffff;
			m_peakv		= m_cmd->arg[2] >> 16;
		}

	public:
		cmd_tune_t(void) : cmd_t(), m_peakv(0), m_peakf(0), m_vlf125(0), m_vlf134(0), m_vhf(0)
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 		= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd	= CMD_MEASURE_ANTENNA_TUNING;
			m_data		= &m_cmd->d.bytes[0];

			return;
		}

		~cmd_tune_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}

		std::string
		to_string(void)
		{
			std::stringstream ss;

			ss << "LF 125.00 kHz: " << std::setprecision(7) << static_cast< float >((0 != m_vlf125 ? m_vlf125/1000.0 : 0.0)) << "v\r\n";
			ss << "LF 134.00 kHz: " << std::setprecision(7) << static_cast< float >((0 != m_vlf134 ? m_vlf134/1000.0 : 0.0)) << "v\r\n";
			ss << "LF optimal " << std::setprecision(11) << static_cast< float >(12000.0/(m_peakf+1)) << " kHz: ";
			ss << std::setprecision(7) << static_cast< float >((0 != m_peakv ? m_peakv/1000.0 : 0.0)) << "v\r\n";
			ss << "HF 13.56 MHz: " << std::setprecision(7) << static_cast< float >((0 != m_vhf ? m_vhf/1000.0 : 0.0)) << "v";
			
			return ss.str();
		}
};

class cmd_wupa_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}

	public:
		cmd_wupa_t(void) : cmd_t()
		{
			wupa_t wupa;

			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd           = reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd      = CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]   = OTFTA_ENABLE; //ISO14A_CONNECT|ISO14A_NO_PARITY|ISO14A_RAW|ISO14A_NO_DISCONNECT;
			m_cmd->arg[1]   = wupa.size();
			m_cmd->arg[2]	= 7;
			m_data          = &m_cmd->d.bytes[0];
			m_dlen			= wupa.size();
			
			::memcpy(m_data, wupa.data(), wupa.size());

			return;
		}

		~cmd_wupa_t(void) 
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_pps_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_pps_t(void)
		{
			pps_t pps;
			
			std::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_APPEND_CRC;
			m_cmd->arg[1]	= pps.size();
			m_cmd->arg[2]	= 0;
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= pps.size();

			std::memcpy(m_data, pps.data(), pps.size());

			return;
		}

		~cmd_pps_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_get_data_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_get_data_t(void) : cmd_t()
		{
			get_data_t gd;

			std::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_APPEND_CRC;
			m_cmd->arg[1]	= gd.size();
			m_cmd->arg[2]	= 0;
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= gd.size();

			std::memcpy(m_data, gd.data(), gd.size());

			return;
		}

		~cmd_get_data_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_reqa_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_reqa_t(void) : cmd_t()
		{
			reqa_t reqa;
			
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_ENABLE; //ISO14A_CONNECT|ISO14A_NO_PARITY|ISO14A_RAW|ISO14A_NO_DISCONNECT;
			m_cmd->arg[1]	= reqa.size();
			m_cmd->arg[2]	= 7;
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= reqa.size();

			::memcpy(m_data, reqa.data(), reqa.size());

			return;
		}

		~cmd_reqa_t(void)
		{
			m_raw.clear();
			m_cmd 	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_hlta_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}

	public:
		cmd_hlta_t(void) : cmd_t()
		{
			hlta_t hlta;

			std::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_APPEND_CRC|OTFTA_NO_RESPONSE;
			m_cmd->arg[1]	= hlta.size();
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= hlta.size();

			std::memcpy(m_data, hlta.data(), hlta.size());

			return;
		}

		~cmd_hlta_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_selalla_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_selalla_t(select_cascade_level_t cl = scl_one) : cmd_t()
		{
			selalla_t sa(cl);

			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd      = CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= 0; //ISO14A_RAW; //ISO14A_CONNECT|ISO14A_RAW|ISO14A_NO_DISCONNECT;
			m_cmd->arg[1]	= sa.size();
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= sa.size();

			std::memcpy(m_data, sa.data(), sa.size());

			return;
		}
	
		~cmd_selalla_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_seluida_t : public cmd_t
{
	private:
		seluida_t m_uid;

	protected:
		void parse(void) {}
	public:
		cmd_seluida_t(select_cascade_level_t cl = scl_one) : cmd_t(), m_uid(cl)
		{
			std::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
//			m_cmd->arg[0]   = OTFTA_APPEND_CRC; //ISO14A_RAW|ISO14A_APPEND_CRC; //|ISO14A_NO_DISCONNECT;
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= 0;

			return;
		}	

		cmd_seluida_t(std::vector< uint8_t >& uid, uint8_t pcp = 0, select_cascade_level_t cl = scl_one) : cmd_t(), m_uid(uid, pcp, cl)
		{
			std::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= 0;

			return;
		}

		~cmd_seluida_t(void)
		{
			m_cmd 	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;
		}

		void
		set_uid(std::vector< uint8_t >& v, uint8_t cp = 0)
		{
			m_uid.set_uid(v, cp);
			std::memcpy(m_data, m_uid.data(), m_uid.size());
			m_dlen = m_uid.size();
			m_cmd->arg[1] = m_dlen;

			if (0 == cp) 
				m_cmd->arg[0] = OTFTA_APPEND_CRC;
			else {
				m_cmd->arg[0] = OTFTA_XMIT_BITS; 
				m_cmd->arg[2] = (2 * sizeof(uint8_t) * 8) + cp + 1;
			}

			return;

		}

		std::vector< uint8_t >& 
		get_uid(void)
		{
			return m_uid.get_uid();
		}
		void
		set_cascade_level(select_cascade_level_t cl)
		{
			m_uid.set_cascade_level(cl);
			return;
		}
		uint64_t get_bits(void) { return m_cmd->arg[2]; }
};

class cmd_ratsa_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_ratsa_t(uint16_t fsd = 256, uint8_t cid = 0x01)
		{
			ratsa_t rats(fsd, cid);
			
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_APPEND_CRC;
			m_cmd->arg[1]	= rats.size();
			m_data			= &m_cmd->d.bytes[0];
			m_dlen			= rats.size();

			::memcpy(m_data, rats.data(), rats.size());
			
			return;
		}

		~cmd_ratsa_t(void)
		{
			m_raw.clear();
			m_cmd 	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;
		}
};

class cmd_14443a_reader_t : public cmd_t 
{
	private:
	protected:
		void parse(void) {}

	public:
		cmd_14443a_reader_t(void) : cmd_t()
		{
			reqa_t reqa;

			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a;
			m_cmd->arg[0]	= ISO14A_CONNECT|ISO14A_NO_DISCONNECT; //|ISO14A_APPEND_CRC|ISO14A_RAW; //ISO14A_NO_DISCONNECT|ISO14A_RAW;
//			m_data			= &m_cmd->d.bytes[0];

//			::memset(m_data, 0, USB_CMD_DSIZE);

//			m_cmd->arg[2]	= 500;
//			m_cmd->arg[1]	= reqa.size(); //sizeof(m_reqa);// len
//			m_data			= &m_cmd->d.bytes[0];

//			::memcpy(m_data, reqa.data(), reqa.size());
//			m_data[0]		= m_reqa;
//			m_dlen			= reqa.size();

			return;
		}

		~cmd_14443a_reader_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_14443a_disconnect_t : public cmd_t
{
	private:
	protected:
		void parse(void) {}
	public:
		cmd_14443a_disconnect_t(void)
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_READER_ISO_14443a_CUSTOM;
			m_cmd->arg[0]	= OTFTA_DISABLE;
			m_cmd->arg[1]	= 0;
			

			return;
		}

		~cmd_14443a_disconnect_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_14a_sniffer_t : public cmd_t
{
	private:
	protected:
	public:
		cmd_14a_sniffer_t(void)
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd 			= reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd		= CMD_SNOOP_ISO_14443a;
			m_cmd->arg[0]	= 0x02;

			return;
		}

		~cmd_14a_sniffer_t(void)
		{
			m_raw.clear();
			m_cmd 	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;

			return;
		}
};

class cmd_mifare_sniff_t : public cmd_t
{
	private:
	protected:
		void parse(void) { }
	public:
		cmd_mifare_sniff_t(void) : cmd_t()
		{
			::memset(m_raw.data(), 0, m_raw.size());

			m_cmd           = reinterpret_cast< usb_cmd_t* >(&m_raw[0]);
			m_cmd->cmd      = CMD_MIFARE_SNIFFER;

			return;

		}

		~cmd_mifare_sniff_t(void)
		{
			m_raw.clear();
			m_cmd	= nullptr;
			m_data	= nullptr;
			m_dlen	= 0;
			return;
		}
};

#endif
