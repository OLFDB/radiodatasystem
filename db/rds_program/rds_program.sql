/* EON table */
create table EON_LI (
	PI		CHAR(4),
	ECC		CHAR(2),
	EON_PI		CHAR(4),
	EON_LA		NUMERIC(1),
	EON_EG		NUMERIC(1),
	EON_ILS		NUMERIC(1),
	EON_LSN		NUMERIC(4),
	PRIMARY KEY (PI, ECC, EON_PI)
);

/* radio frequency table */
/* - including PI code, transmitter location */
/* - local availability check by receiver location */
/* - synchronization and collaboration with fmscan, fmlist databases */

/* ODA_RTP table */
create table ODA_RTP (
	PI	CHAR(4),
	ECC	CHAR(2),
	CT	CHAR(2),
	RTP	CHAR(128),
	PRIMARY KEY (PI, ECC, CT)
);

/* RDS table */
create table RDS (
	PI		CHAR(4),
	PS		CHAR(8),
	PTY		NUMERIC(2),
	TP		NUMERIC(1),
	AF		VARCHAR,
	TA		NUMERIC(1),
	DI_ST		NUMERIC(1),
	DI_AH		NUMERIC(1),
	DI_CO		NUMERIC(1),
	DI_DP		NUMERIC(1),
	MS		NUMERIC(1),
	PIN		CHAR(10),
	ECC		CHAR(2),
	ISO		CHAR(2),
	ITU		NUMERIC(1),
	LIC		NUMERIC(3),
	LA		NUMERIC(1),
	RT		CHAR(64),
	ODA		CHAR(160),
	CT		CHAR(25),
	PTYN		CHAR(8),
	EON_MF		CHAR(45),
	/* EON_LI is in table EON_LI */
	ODA_RTP_CB	NUMERIC(1),
	ODA_RTP_SCB	NUMERIC(1),
	ODA_RTP_TN	NUMERIC(1),
	ODA_RTP_ITB	NUMERIC(1),
	ODA_RTP_IRB	NUMERIC(1),
	ODA_RTP_CT1	NUMERIC(2),
	ODA_RTP_CT2	NUMERIC(2),
	/* ODA_RTP is in table ODA_RTP */
	ODA_ERT		CHAR(128),
	ODA_TMC_CID	NUMERIC(3),
	ODA_TMC_LID	NUMERIC(3),
	ODA_TMC_SID	NUMERIC(2),
	PRIMARY KEY (PI, ECC)
);
