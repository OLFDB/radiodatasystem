/* Eventlist */
create table EVENTS (
	Class	CHAR(50),
	Type	CHAR(1000),
	Text_Q0	CHAR(1000),
	Text_Q	CHAR(1000),
	Quantifier	CHAR(30),
	Speed	CHAR(10),
	Since	CHAR(40),
	Notes	CHAR(1000),
	PRIMARY KEY (Class, Type)
)