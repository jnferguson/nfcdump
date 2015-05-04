#ifndef HAVE_TECL_APDU_T_HPP
#define HAVE_TECL_APDU_T_HPP

#define IBLOCK_BIT_VALUE (0x00 << 6)
#define IBLOCK_SET_CHAIN (0x01 << 4)
#define IBLOCK_CID_FOLLOWS (0x01 << 3)
#define IBLOCK_NAD_FOLLOWS (0x01 << 2)
#define IBLOCK_RFU (0x01 << 1)
#define IBLOCK_TOGGLE_BLOCK(x) (x ^= 0x01)

#define IBLOCK_WITH_CID(x) (x|IBLOCK_BIT_VALUE|IBLOCK_CID_FOLLOWS|~(IBLOCK_NAD_FOLLOWS)|IBLOCK_RFU)
#define IBLOCK_WITH_NAD(x) (x|IBLOCK_BIT_VALUE|IBLOCK_NAD_FOLLOWS|~(IBLOCK_CID_FOLLOWS)|IBLOCK_RFU)

#define RBLOCK_BIT_VALUE (0x01 << 6)
#define RBLOCK_ACK (0x00 << 4)
#define RBLOCK_NACK (0x01 << 4)
#define RBLOCK_CID_FOLLOWS (0x01 << 3)
#define RBLOCK_NAD_CONST (0x00 << 2)
#define RBLOCK_RFU (0x01 << 1)
#define RBLOCK_TOGGLE_BLOCK(x) (x ^= 0x01)

#define RBLOCK_WITH_CID(x) (x|RBLOCK_BIT_VALUE|RBLOCK_CID_FOLLOWS|RBLOCK_NAD_FOLLOWS|RBLOCK_RFU)

#define SBLOCK_BIT_VALUE (0x03 << 6)
#define SBLOCK_DESELECT (0x00 << 4)
#define SBLOCK_WTX (0x03 << 4)
#define SBLOCK_CID_FOLLOWS (0x01 << 3)
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

typedef enum {
	TECL_IBLOCK_TYPE = 0,
	TECL_RBLOCK_TYPE,
	TECL_SBLOCK_TYPE,
	TECL_INVALID_TYPE
} tecl_block_t;

class tecl_prologue_base_t
{
	private:
	protected:
		tecl_block_t	m_type;
		uint8_t			m_pcb;
		uint8_t			m_cid;

	public:
		tecl_prologue_base_t(tecl_block_t type = TECL_IBLOCK_TYPE) : m_type(type), m_pcb(0x00), m_cid(0x00)
		{
			return;
		}

		virtual 
		~tecl_prologue_t(void)
		{
			m_type	= TECL_INVALID_TYPE;
			m_pcb 	= 0x00;
			m_cid	= 0x00;

			return;
		}

		tecl_block_t type(void) { return m_type; }

		virtual std::vector< uint8_t > data(void) = 0; 

};

class tecl_sblock_t : public tecl_prologue_base_t
{
	private:
	protected:
		virtual bool
		has_cid(void)
		{
			return (SBLOCK_CID_FOLLOWS == (m_pcb & SBLOCK_CID_FOLLOWS));
		}

	public:
		tecl_sblock_t(void) : m_type(TECL_SBLOCK_TYPE), m_pcb(SBLOCK_BIT_VALUE|SBLOCK_NAD_CONST|SBLOCK_RFU), m_cid(0x00)
		{
			return;
		}

		virtual 
		~tecl_sblock_t(void)
		{
			m_type 	= TECL_INVALID_TYPE;
			m_pcb	= 0x00;
			m_cid	= 0x00;

			return;
		}

		virtual std::vector< uint8_t >
		data(void)
		{
			std::vector< uint8_t > r;

			r.push_back(m_pcb);

			if (true == has_cid())
				r.push_back(m_cid);

			return r;
		}

		virtual void
		set_deselect(void)
		{
			m_pcb |= SBLOCK_DESELECT|SBLOCK_WTX;
			m_pcb ^= SBLOCK_WTX;
			return;
		}

		virtual void
		set_wtx(void)
		{
			m_pcb |= SBLOCK_WTX|SBLOCK_DESELECT;
			m_pcb ^= SBLOCK_DESELECT;
			return;
		}
}

class tecl_rblock_t : public tecl_prologue_base_t
{
	private:
	protected:
		virtual inline bool
		has_cid(void)
		{
			return (RBLOCK_CID_FOLLOWS == (m_pcb & RBLOCK_CID_FOLLOWS));
		}

	public:
		tecl_rblock_t(void) : m_type(TECL_RBLOCK_TYPE), m_pcb(RBLOCK_BIT_VALUE|RBLOCK_NAD_CONST|RBLOCK_RFU), m_cid(0x00);
		
		virtual
		~tecl_rblock_t(void)
		{
			m_type 	= TECL_INVALID_TYPE;
			m_cid	= 0x00;
			m_pcb	= 0x00;

			return;
		}

		virtual std::vector< uint8_t >
		data(void)
		{
			std::vector< uint8_t > r;

			r.push_back(m_pcb);

			if (true == has_cid)
				r.push_back(m_cid);

			return r;
		}

		virtual void
		set_ack(void)
		{
			m_pcb |= RBLOCK_ACK;
			return;
		}

		virtual void
		set_nack(void)
		{
			m_pcb |= RBLOCK_NACK;
			return;
		}

		virtual void
		set_cid(uint8_t cid)
		{
			m_pcb |= RBLOCK_CID_FOLLOWS;
			m_cid = cid;
			
			return;
		}

		virtual void
		set_block(bool set = true)
		{
			if (true == set)
				m_pcb |= 0x01;
			else
				m_pcb |= ~(0x01);

			return;
		}
};

class tecl_iblock_t : public tecl_prologue_base_t
{
	private:
		uint8_t m_nad;

	protected:
		virtual inline bool
		has_cid(void)
		{
			return (IBLOCK_CID_FOLLOWS == (m_pcb & IBLOCK_CID_FOLLOWS));
		}

		virtual inline bool
		has_nad(void)
		{
			return (IBLOCK_NAD_FOLLOWS == (m_pcb & IBLOCK_NAD_FOLLOWS));
		}

	public:
		tecl_iblock_t(void) : m_type(TECL_IBLOCK_TYPE), m_pcb(IBLOCK_BIT_VALUE|IBLOCK_RFU), m_cid(0x00), m_nad(0x00)
		{
			return;
		}

		virtual ~tecl_iblock_t(void)
		{
			m_type 	= TECL_INVALID_TYPE;
			m_pcb	= 0x00;
			m_cid	= 0x00;
			m_nad	= 0x00;

			return;
		}
	
		virtual std::vector< uint8_t >
		data(void)
		{
			std::vector< uint8_t > r;

			r.push_back(m_pcb);

			if (true == has_cid())
				r.push_back(m_cid);

			if (true == has_nad())
				r.push_back(m_nad);

			return r;
		}

		virtual void
		set_chain(void)
		{
			m_pcb |= IBLOCK_SET_CHAIN;
			return;
		}

		virtual void
		set_cid(uint8_t cid)
		{
			m_pcb |= IBLOCK_CID_FOLLOWS;
			m_cid = cid;

			return;
		}

		virtual void
		set_nad(uint8_t nad)
		{
			m_pcb |= IBLOCK_NAD_FOLLOWS;
			m_nad = nad;

			return;
		}

		virtual void
		set_block(bool set = true)
		{
			if (true == set)
				m_pcb |= 0x01;
			else
				m_pcb |= ~(0x01);

			return;
		}
};

class tecl_apdu_t
{
	private:
		tecl_block_t			m_type;
		tecl_prologue_base_t*	m_pcb;
		uint8_t					m_cla;
		uint8_t					m_ins;
		uint8_t 				m_p1;
		uint8_t 				m_p2;
		uint16_t				m_lc;
		uint16_t				m_le;
		std::vector< uint8_t >	m_data;

	protected:
		virtual void
		init(void)
		{
			m_type	= TECL_INVALID_TYPE;
			m_pcb 	= nullptr;
			m_cid	= 0x00;
			m_nad	= 0x00;
			m_cla	= 0x00;
			m_ins	= 0x00;
			m_p1	= 0x00;
			m_p2	= 0x00;
			m_lc	= 0x00;
			m_le	= 0x00;

			m_data.clear();

			return;
		}

	public:
		tecl_apdu_t(void) 
		{
			init();
			return;
		}

		tecl_apdu_t(tecl_block_t type) : m_type(type)
		{
			init();

			switch (type) {
				case TECL_IBLOCK_TYPE:
					m_pcb = new tecl_iblock_t();
					break;

				case TECL_RBLOCK_TYPE:
					m_pcb = new tecl_rblock_t();
					break;

				case TECL_SBLOCK_TYPE:
					m_pcb = new tecl_sblock_t();
					break;

				default:
					throw std::runtime_error("tecl_apdu_t::tecl_apdu_t(): Invalid block type specified");
					break;
			}

			return;
		}

		virtual
		~tecl_apdu_t(void)
		{
			if (nullptr != m_pcb)
				delete m_pcb;

		}

};
#endif
