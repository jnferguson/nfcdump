#ifndef HAVE_SEST_T_HPP
#define HAVE_SEST_T_HPP

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdexcept>

#define FICLA_LAST_ONLY_CHAIN_CMD (0x0 << 4)
#define FICLA_MORE_CHAIN_CMD (0x01 << 4)
#define FICLA_NO_SMA (0x00 << 2)
#define FICLA_PROP_SMA (0x01 << 2)
#define FICLA_UNAUTH_SMA (0x02 << 2)
#define FICLA_AUTH_SMA (0x03 << 2)
#define FICLA_LOGIC_CHANNEL_ZERO (0x00)
#define FICLA_LOGIC_CHANNEL_ONE (0x01)
#define FICLA_LOGIC_CHANNEL_TWO (0x02)
#define FICLA_LOGIC_CHANNEL_THREE (0x03)

#define FUCLA_NO_SMA (0x01 << 6)
#define FUCLA_UNAUTH_SMA (0x03 << 5)
#define FUCLA_LAST_ONLY_CHAIN_CMD 0x40
#define FUCLA_MORE_CHAIN_CMD 0x50
#define FUCLA_LOGIC_CHANNEL_NINETEEN (0x4F)
#define FUCLA_LOGIC_CHANNEL_EIGHTEEN (0x4E)
#define FUCLA_LOGIC_CHANNEL_SEVENTEEN (0x4D)
#define FUCLA_LOGIC_CHANNEL_SIXTEEN (0x4C)
#define FUCLA_LOGIC_CHANNEL_FIFTEEN (0x4B)
#define FUCLA_LOGIC_CHANNEL_FOURTEEN (0x4A)
#define FUCLA_LOGIC_CHANNEL_THIRTEEN (0x49)
#define FUCLA_LOGIC_CHANNEL_TWELVE (0x48)
#define FUCLA_LOGIC_CHANNEL_ELEVEN (0x47)
#define FUCLA_LOGIC_CHANNEL_TEN (0x46)
#define FUCLA_LOGIC_CHANNEL_NINE (0x45)
#define FUCLA_LOGIC_CHANNEL_EIGHT (0x44)
#define FUCLA_LOGIC_CHANNEL_SEVEN (0x43)
#define FUCLA_LOGIC_CHANNEL_SIX (0x42)
#define FUCLA_LOGIC_CHANNEL_FIVE (0x41)
#define FUCLA_LOGIC_CHANNEL_FOUR (0x40)

#define FUCLA_IS_PROPIETARY(x) (0x80 == (x & 0x80))
#define FUCLA_IS_VALID(x) (x != 0xFF)

#define CMD_ACTIVATE_CHANNEL 0x44
#define CMD_APPEND_RECORD 0xE2
#define CMD_CHANGE_REFERENCE_DATA 0x24
#define CMD_CREATE_FILE 0xE0
#define CMD_DEACTIVATE_FILE 0x04
#define CMD_DELETE_FILE 0xE4
#define CMD_DISABLE_VERIFICATION_REQUIREMENT 0x26
#define CMD_ENABLE_VERIFICATION_REQUIRED 0x28
#define CMD_ENVELOPE 0xC2 // 0xC3 ??
#define CMD_ERASE_BINARY 0x0E // 0x0F ??
#define CMD_ERASE_RECORDS 0x0C
#define CMD_EXTERNAL_AUTHENTICATE 0x82
#define CMD_GENERAL_AUTHENTICATE 0x86 // 0x87 ??
#define CMD_GENERATE_ASYMMETRIC_KEYPAIR 0x46
#define CMD_GET_CHALLENGE 0x84
#define CMD_GET_DATA 0xCA // 0xCB ??
#define CMD_GET_RESPONSE 0xC0
#define CMD_INTERNAL_AUTHENTICATE 0x88
#define CMD_MANAGE_CHANNEL 0x70
#define CMD_MANAGE_SECURITY_ENVIRONMENT 0x22
#define CMD_PERFORM_SCQL_OPERATION 0x10
#define CMD_SECRITY_OPERATION 0x2A
#define CMD_PERFORM_TRANSACTION_OPERATION 0x12
#define CMD_USER_OPERATION 0x14
#define CMD_PUT_DATA 0xDA // 0xDB ??
#define CMD_READ_BINARY 0xB0 // 0xB1 ??
#define CMD_READ_RECORDS 0xB2 // 0xB3 ??
#define CMD_RESET_RETRY_COUNTER 0x2C
#define CMD_SEARCH_BINARY 0xA0 // 0xA1 ??
#define CMD_SEARCH_RECORD 0xA2
#define CMD_SELECT 0xA4
#define CMD_TERMINATE_CARD_USAGE 0xFE
#define CMD_TERMINATE_DF 0xE6
#define CMD_TERMINATE_EF 0xE8
#define CMD_UPDATE_BINARY 0xD6 // 0xD7 ??
#define CMD_UPDATE_RECORD 0xDC // 0xDD ??
#define CMD_VERIFY 0x20 // 0x21 ??
#define CMD_WRITE_BINARY 0xD0 // 0xD1 ??
#define CMD_WRITE_RECORD 0xD2 

#define SELECT_PARAM_MF_DF_EF (0x00)
#define SELECT_PARAM_CHILD_DF (0x01)
#define SELECT_PARAM_EF_OF_DF (0x02)
#define SELECT_PARAM_PARENT_DF (0x03)
#define SELECT_PARAM_DF_BY_NAME (0x04)
#define SELECT_PARAM_MF (0x08)
#define SELECT_PARAM_CURRENT_DF (0x09)

#define SELECT_PARAM_FIRST_ONLY (0x00)
#define SELECT_PARAM_LAST_OCCUR (0x01)
#define SELECT_PARAM_NEXT_OCCUR (0x02)
#define SELECT_PARAM_PREV_OCCUR (0x03)

#define SELECT_PARAM_RETURN_FCI (0x00)
#define SELECT_PARAM_RETURN_FCP (0x04)
#define SELECT_PARAM_RETURN_FMD (0x08)
#define SELECT_PARAM_RETURN_PROP (0x09)

#define IBLOCK_BIT_VALUE (0x00)
#define IBLOCK_SET_CHAIN (0x01 << 4)
#define IBLOCK_CID_FOLLOWS (0x01 << 3)
#define IBLOCK_NAD_FOLLOWS (0x01 << 2)
#define IBLOCK_RFU (0x01 << 1)
#define IBLOCK_TOGGLE_BLOCK(x) (x ^= 0x01)

#define IBLOCK_WITH_CID(x) (x|IBLOCK_BIT_VALUE|IBLOCK_CID_FOLLOWS|~(IBLOCK_NAD_FOLLOWS)|IBLOCK_RFU)
#define IBLOCK_WITH_NAD(x) (x|IBLOCK_BIT_VALUE|IBLOCK_NAD_FOLLOWS|~(IBLOCK_CID_FOLLOWS)|IBLOCK_RFU)

#define RBLOCK_BIT_VALUE (0x01)
#define RBLOCK_ACK (0x00 << 4)
#define RBLOCK_NACK (0x01 << 4)
#define RBLOCK_CID_FOLLOWS (0x01 << 3)
#define RBLOCK_NAD_CONST (0x00 << 2)
#define RBLOCK_RFU (0x01 << 1)
#define RBLOCKTOGGLE_BLOCK(x) (x ^= 0x01)

#define RBLOCK_WITH_CID(x) (x|RBLOCK_BIT_VALUE|RBLOCK_CID_FOLLOWS|RBLOCK_NAD_FOLLOWS|RBLOCK_RFU)

#define SBLOCK_BIT_VALUE (0x03)
#define SBLOCK_DESELECT (0x00 << 4)
#define SBLOCK_WTX (0x03 << 4)
#define SBLOCK_CID (0x01 << 3)
#define SBLOCK_NAD_CONST (0x00 << 2)
#define SBLOCK_RFU (0x2)
#define SBLOCK_WITH_CID(x) (x|SBLOCK_BIT_VALUE|SBLOCK_CID|SBLOCK_NAD_CONST|SBLOCK_RFU)

#define CID_POWER_MASK (0xC0)
#define CID_POWER_UNSUPPORTED (0x00 << 6)
#define CID_POWER_INSUFFICIENT (0x01 << 6)
#define CID_POWER_ADEQUATE (0x02 << 6)
#define CID_POWER_EXCESSIVE (0x03 << 6)
#define CID_RFU (0x00 << 4)
#define CID_CID(x) (x & 0x0F)


class class_byte_t {

	private:
		uint8_t	m_class;
		bool	m_is_set;
		bool	m_fi_inter;
		bool	m_fu_inter;
		bool	m_propietary;
		bool	m_last_only;
		bool	m_more_chain;
		bool	m_no_sma;
		bool	m_unauth_sma; 
		bool 	m_prop_sma; 
		bool	m_auth_sma;
		uint8_t	m_channel;

	protected:
		virtual void parse(bool validate = true);

	public:
		class_byte_t(void);
		class_byte_t(uint8_t v, bool validate = true);

		virtual ~class_byte_t(void);

		virtual uint8_t get(void);
		virtual void set(uint8_t v, bool validate = true);
		virtual bool isFirstInterIndustry(void);
		virtual bool isFurtherInterIndustry(void);
		virtual bool isPropietary(void);
		virtual bool isLastOrOnlyChain(void);
		virtual bool isMoreChain(void);
		virtual bool isNoSMA(void);
		virtual bool isUnauthSMA(void);
		virtual bool isPropietarySMA(void);
		virtual bool isAuthSMA(void);
		virtual uint8_t getChannel(void);

};

class sest_prologue_t
{
	private:
		uint8_t	m_pcb;
		uint8_t m_cid;
		uint8_t m_nad;

	protected:
	public:
		sest_prologue_t(void) : m_pcb(0x00), m_cid(0x00), m_nad(0x00)
		{
			return;
		}

		virtual 
		~sest_prologue_t(void)
		{
			m_pcb = 0x00;
			m_cid = 0x00;
			m_nad = 0x00;
		}

		virtual uint8_t&
		pcb(void)
		{
			return m_pcb;
		}

		virtual uint8_t&
		cid(void)
		{
			return m_cid;
		}

		virtual uint8_t& 
		nad(void)
		{
			return m_nad;
		}

		std::vector< uint8_t >
		data(void)
		{
			std::vector< uint8_t > r;
			r.push_back(m_pcb);

			if (0x00 != m_cid)
				r.push_back(m_cid);

			if (0x00 != m_nad)
				r.push_back(m_nad);

			return r;
		}
};

class sest_base_t {
	private:
	protected:
		
		class_byte_t			m_cla;
		uint8_t					m_ins;
		uint8_t					m_p1;
		uint8_t					m_p2;
		uint16_t				m_lc;
		uint16_t				m_le;
		std::vector< uint8_t >	m_data;

	public:
		sest_base_t(void);
		sest_base_t(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t, std::vector< uint8_t >&);

		virtual ~sest_base_t(void); 

		virtual std::vector< uint8_t > data(bool validate = true);
		virtual void set(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t, std::vector< uint8_t >&);
		virtual void set(class_byte_t&, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t, std::vector< uint8_t >&);

		virtual void set_p1(uint8_t);
		virtual void set_p2(uint8_t);
		virtual void set_class(uint8_t);
		virtual void set_instruction(uint8_t);
		virtual void set_lc(uint16_t);
		virtual void set_data(std::vector< uint8_t >&);
		virtual void set_le(uint16_t);

		virtual class_byte_t& cla(void);
		virtual uint8_t ins(void);
		virtual uint16_t lc(void);
		virtual std::vector< uint8_t > dat(void);
		virtual uint16_t le(void);
};

class sest_select_cmd_t : public sest_base_t
{
	private:
	protected:
	public:
		sest_select_cmd_t(uint8_t cla, uint8_t p1, uint8_t p2, uint16_t lc, uint16_t le, std::vector< uint8_t >& d)
			: sest_base_t(cla, CMD_SELECT, p1, p2, lc, le, d) 
			{
				return; 
			}

		~sest_select_cmd_t(void) 
		{
			return;
		}

		virtual std::vector< uint8_t > 
		select_mf(bool validate = true)
		{
			m_p1 = 0x00;
			m_p2 = 0x00;
			m_lc = 2;
			m_data.push_back(0x3F);
			m_data.push_back(0x00);

			return data(validate);
		}
};

#endif
