#ifndef HAVE_OTFTA_T_HPP
#define HAVE_OTFTA_T_HPP

#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>


typedef enum {
	scl_one = 0,
	scl_two = 1,
	scl_three = 2
} select_cascade_level_t;

typedef enum {
	STATE_XMIT_REQWUP = 1,
	STATE_SELC_ONE,
	STATE_SELC_ONE_CONTINUE,
	STATE_SELC_TWO,
	STATE_SELC_TWO_CONTINUE,
	STATE_SELC_THREE ,
	STATE_XMIT_RATS,
	STATE_XMIT_PPS,
	STATE_CARD_HALT,
	STATE_DISCONNECT
} otfta_state_t;

#define REQA_VALUE 0x26
#define WUPA_VALUE 0x52
#define ACCO_VALUE 0x93
#define ACCW_VALUE 0x95
#define ACCT_VALUE 0x97 //0x75
#define SELC_VALUE 0x70
#define SELA_VALUE 0x20
#define SECC_VALUE 0x88
#define RATS_VALUE 0xE0
#define HALT_VALUE 0x50

#define ATSA_FMT_RFU_MASK 0x80
#define ATSA_TAO_MASK 0x40
#define ATSA_TBO_MASK 0x20
#define ATSA_TCO_MASK 0x10
#define ATSA_FSCI_MASK 0x0F
#define ATSA_FSCI_16 0
#define ATSA_FSCI_24 1
#define ATSA_FSCI_32 2
#define ATSA_FSCI_40 3
#define ATSA_FSCI_48 4
#define ATSA_FSCI_64 5
#define ATSA_FSCI_96 6
#define ATSA_FSCI_128 7
#define ATSA_FSCI_256 8
#define ATSA_FMT_IS_VALID(x) (0 == (x & ATSA_FMT_RFU_MASK))

#define ATSA_TAO_DIFFDIV_MASK 0x80
#define ATSA_TAO_DS_MASK 0x70
#define ATSA_TAO_RFU_MASK 0x08
#define ATSA_TAO_DR_MASK 0x07
#define ATSA_TAO_IS_VALID(x) (0 == (x & ATSA_TAO_RFU_MASK))
#define ATSA_TAO_DRT 0x01
#define ATSA_TAO_DRF 0x02
#define ATSA_TAO_DRH 0x04
#define ATSA_TAO_DST 0x10
#define ATSA_TAO_DSF 0x20
#define ATSA_TAO_DSH 0x40
#define ATSA_TBO_FWIFWT_MASK 0xF0
#define ATSA_TBO_SFGI_MASK 0x0F
#define ATSA_TBO_SFGI_RFU_VALUE 15
#define ATSA_TBO_IS_VALID(x) (ATSA_TBO_SFGI_RFU_VALUE != (x & ATSA_TBO_SFGI_MASK))
#define ATSA_TCO_RFU_BITS 0xFC
#define ATSA_TCO_HAS_NAD 0x01
#define ATSA_TCO_HAS_CID 0x02
#define ATSA_TCO_IS_VALID(x) (0 == (x & ATSA_TCO_RFU_BITS))

#define ATSA_TK_CATI_NOTTLV 0x00
#define ATSA_TK_CATI_DIR 0x10
#define ATSA_TK_CATI_TLV 0x80
#define ATSA_TK_RFU_MASK 0x8F

#define ATSA_TK_MIFARE 0xC1

#define RATSA_FSDI_16 (0x00 << 4)
#define RATSA_FSDI_24 (0x01 << 4)
#define RATSA_FSDI_32 (0x02 << 4)
#define RATSA_FSDI_40 (0x03 << 4)
#define RATSA_FSDI_48 (0x04 << 4)
#define RATSA_FSDI_64 (0x05 << 4)
#define RATSA_FSDI_96 (0x06 << 4)
#define RATSA_FSDI_128 (0x07 << 4)
#define RATSA_FSDI_256 (0x08 << 4)
#define RATSA_FSDI_RFU0 (0x09 << 4)
#define RATSA_FSDI_RFU1 (0x0A << 4)
#define RATSA_FSDI_RFU2 (0x0B << 4)
#define RATSA_FSDI_RFU3 (0x0C << 4)
#define RATSA_FSDI_RFU4 (0x0E << 4)
#define RATSA_FSDI_RFU5 (0x0F << 4)
#define RATSA_FSDI_RFU (RATSA_FSDI_RFU0|RATSA_FSDI_RFU1|RATSA_FSDI_RFU2|RATSA_FSDI_RFU3|RATSA_FSDI_RFU4|RATSA_FSDI_RFU5)
#define RATSA_FSDI_IS_VALID(x) (0 != (x & RATSA_FSDI_RFU))

#define RATSA_CID_BITS 0xF

#define NVB_BYC02 0x20
#define NVB_BYC03 0x30
#define NVB_BYC04 0x40
#define NVB_BYC05 0x50
#define NVB_BYC06 0x60
#define NVB_BYC07 0x70
#define NVB_BYTES (NVB_BYC02|NVB_BYC03|NVB_BYC04|NVB_BYC05|NVB_BYC06|NVB_BYC07)

#define NVB_BI00 0x00
#define NVB_BI01 0x01
#define NVB_BI02 0x02
#define NVB_BI03 0x03
#define NVB_BI04 0x04
#define NVB_BI05 0x05
#define NVB_BI06 0x06
#define NVB_BI07 0x07
#define NVB_BITS (NVB_BI00|NVB_BI01|NVB_BI02|NVB_BI03|NVB_BI04|NVB_BI05|NVB_BI06|NVB_BI07)

#define NVB_IBIT 0x08
#define NVB_IBYT 0x80

#define NVB_IS_VALID(x) (0 == (x & (NVB_IBIT|NVB_IBYT)))

#define SELECT_CONTINUE 0x88

#define SAK_UID_CASCADE 0x04
#define SAK_UID_COMPLETE 0x20
#define SAK_NFCIPONE 0x40
#define SAK_IS_VALID(x) (0 == (x & ~(SAK_UID_COMPLETE|SAK_UID_CASCADE)))

#define ATQA_RFU_HI16 0x8000
#define ATQA_RFU_HI15 0x4000
#define ATQA_RFU_HI14 0x2000
#define ATQA_RFU_HI13 0x1000
#define ATQA_RFU_BITS (ATQA_RFU_HI16|ATQA_RFU_HI15|ATQA_RFU_HI14|ATQA_RFU_HI13)

#define ATQA_PRO_BI12 0x0800
#define ATQA_PRO_BI11 0x0400
#define ATQA_PRO_BI10 0x0200
#define ATQA_PRO_BI09 0x0100
#define ATQA_PRO_BITS (ATQA_PRO_BI12|ATQA_PRO_BI11|ATQA_PRO_BI10|ATQA_PRO_BI09)

#define ATQA_UID_BI08 0x0080
#define ATQA_UID_BI07 0x0040
#define ATQA_UID_RFUB 0x00C0
#define ATQA_UID_BITS (ATQA_UID_BI08|ATQA_UID_BI07)
#define ATQA_UID_SING(x) (0 == (x & ATQA_UID_BITS))
#define ATQA_UID_DOUB(x) (ATQA_UID_BI07 == (x & ATQA_UID_BITS))
#define ATQA_UID_TRIP(x) (ATQA_UID_BI08 == (x & ATQA_UID_BITS))
#define ATQA_UID_VALID(x) (ATQA_UID_RFUB != (x & ATQA_UID_BITS) && (1 == ATQA_UID_SING(x) || 1 == ATQA_UID_DOUB(x) || 1 == ATQA_UID_TRIP(x)))

#define ATQA_RFU_LO06 0x0020

#define ATQA_BFA_BY05 0x0010
#define ATQA_BFA_BY04 0x0008
#define ATQA_BFA_BY03 0x0004
#define ATQA_BFA_BY02 0x0002
#define ATQA_BFA_BY01 0x0001
#define ATQA_BFA_BITS (ATQA_BFA_BY05|ATQA_BFA_BY04|ATQA_BFA_BY03|ATQA_BFA_BY02|ATQA_BFA_BY01)

#define ATQA_HAS_ANC(x) (0 != (x & ATQA_BFA_BITS))
#define ATQA_IS_VALID(x) (0 == (x & (ATQA_RFU_BITS|ATQA_RFU_LO06)) && ATQA_UID_VALID(x))

class otfta_base_t
{
	private:
	protected:
		std::vector< uint8_t > m_raw;

	public:
		otfta_base_t(void);
		virtual ~otfta_base_t(void);

		virtual std::vector< uint8_t >& vector(void);
		virtual void* data(void);
		virtual std::size_t size(void);

		virtual void set(std::vector< uint8_t >&);
		virtual void set(uint8_t*, std::size_t);
		virtual void set(int8_t*, std::size_t);
};

class reqa_t : public otfta_base_t
{
	private:
	protected:
	public:
		reqa_t(void);
		virtual ~reqa_t(void);
};

class wupa_t : public otfta_base_t
{
	private:
	protected:
	public:
		wupa_t(void);
		virtual ~wupa_t(void);
};

class hlta_t : public otfta_base_t 
{
	private:
	protected:
	public:
		hlta_t(void);
		virtual ~hlta_t(void);
};

class ratsa_t : public otfta_base_t
{
	private:
	protected:
	public:
		ratsa_t(uint16_t fsd = 256, uint8_t cid = 0x01);
		virtual ~ratsa_t(void);
};

class selalla_t : public otfta_base_t
{
	private:
		uint8_t	m_sel;
		uint8_t m_nvb;

	protected:
	public:
		selalla_t(select_cascade_level_t cl = scl_one);
		virtual ~selalla_t(void);

		virtual void set_cascade_level(select_cascade_level_t);
};

class seluida_t : public otfta_base_t
{
	private:
		uint8_t 				m_sel;
		uint8_t					m_nvb;
		std::vector< uint8_t > 	m_uid;
		uint8_t					m_col;

	protected:
	public:
		seluida_t(select_cascade_level_t cl = scl_one);
		seluida_t(std::vector< uint8_t >&, uint8_t pcp = 0, select_cascade_level_t cl = scl_one);
		virtual ~seluida_t(void);

		virtual void set_cascade_level(select_cascade_level_t);
		virtual void set_uid(std::vector< uint8_t >&, uint8_t cp = 0);
		virtual std::vector< uint8_t >& get_uid(void);
};

class pps_t : public otfta_base_t
{
	private:
	protected:
	public:
		pps_t(void)
		{
			uint8_t bytes[] = { 0xD1, 0x11, 0x0F };
			set(&bytes[0], sizeof(bytes));

			return;
		}

		~pps_t(void)
		{
			return;
		}
};

class get_data_t : public otfta_base_t
{
	private:
	protected:
	public:
		get_data_t(void)
		{
			uint8_t raw[] = { /*0x02,*/ 0x0A, 0x01, 0x90, 0x60, 0x00, 0x00, 0x00 /*, 0x14, 0x98*/}; //- GetVersion()
			//uint8_t raw[] = { 0x0A, 0x01, 0xB8, 0x84, 0x00, 0x00, 0x04 }; // GetChallenge()
			//uint8_t raw[] = { 0x02, 0x00, 0x84, 0x00, 0x00, 0x04 };
			set(&raw[0], sizeof(raw));
			return;
		}

		~get_data_t(void) { return; }
};

#endif
