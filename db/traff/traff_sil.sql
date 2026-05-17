/* Supplementary Informationlist */
create table SIL (
	Class	CHAR(50),
	Type	CHAR(1000),
	Text	CHAR(1000),
	Text_Q	CHAR(1000),
	Quantifier	CHAR(30),
	Since	CHAR(40),
	Comment	CHAR(200),
	PRIMARY KEY (Class, Type)
)