/* (Extended) Country Code */
create table CC (
	ECC		CHAR(2),
	CCD		CHAR(1),
	ITU		NUMERIC(1),
	ISO		CHAR(2),
	CID		NUMERIC(3),
	NAME		CHAR(100),
	COMMENT		CHAR(50),
	PRIMARY KEY (ECC, CCD)
);

/* Language Identification Code */
create table LIC (
	LIC		NUMERIC(3),
	NAME		CHAR(50),
	PRIMARY KEY (LIC, NAME)
);

/* Music/Speech */
create table MS (
	MS		NUMERIC(1),
	TEXT		CHAR(10),
	PRIMARY KEY (MS, TEXT)
);

/* Programme Type */
create table PTY (
	PTY		NUMERIC(2),
	RDS		CHAR(50),
	RDS8		CHAR(8),
	RDS16		CHAR(16),
	RBDS		CHAR(50),
	RBDS8		CHAR(8),
	RBDS16		CHAR(16),
	PRIMARY KEY (PTY)
);

/* Radio Text Plus */
create table RTP (
	CODE		NUMERIC(2),
	CATEGORY	CHAR(20),
	CLASS		CHAR(40),
	PRIMARY KEY (CODE)
);
