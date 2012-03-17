-- Note: run this file will delete existed database named railway_ticket_system

-- delete and create the database
DROP DATABASE IF EXISTS railway_ticket_system;
CREATE DATABASE railway_ticket_system;

USE railway_ticket_system;
GRANT ALL ON railway_ticket_system.* TO admin@localhost IDENTIFIED BY 'admin';

-- the table member
CREATE TABLE member (
	id int(32) PRIMARY KEY AUTO_INCREMENT,
	name varchar(50) NOT NULL,
	password varchar(50) NOT NULL,
	UNIQUE(name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- the values inserted into member
INSERT INTO member VALUES
	(1, 'member1', 'member1'),
	(2, 'member2', 'member2'),
	(3, 'member3', 'member3');


-- the schedule table
CREATE TABLE schedule (
	id int(32) PRIMARY KEY AUTO_INCREMENT,
	beginning varchar(50) NOT NULL,
	destination varchar(50) NOT NULL,
	time datetime NOT NULL,
	duration int(32) NOT NULL,
	total_ticket int(32) NOT NULL,
	ticket_left int(32) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- the values inserted into schedule
INSERT INTO schedule VALUES
	(1, 'NanJing', 'BeiJing', '2012-3-17 12:00:00', 180, 300, 300),
	(2, 'NanJing', 'ShangHai', '2012-3-17 12:00:00', 30, 300, 300),
	(3, 'BeiJing', 'ShangHai', '2012-3-17 12:00:00', 210, 300, 300),
	(4, 'NanJing', 'BeiJing', '2012-3-31 12:00:00', 180, 300, 300),
	(5, 'NanJing', 'ShangHai', '2012-3-31 12:00:00', 30, 300, 300),
	(6, 'BeiJing', 'ShangHai', '2012-3-31 12:00:00', 210, 300, 300);

-- the sale table
CREATE TABLE sale (
	id int(32) PRIMARY KEY AUTO_INCREMENT,
	mid int(32) NOT NULL,
	sid int(32) NOT NULL,
	number int(32) NOT NULL,
	sale_time datetime NOT NULL,
	FOREIGN KEY (mid) REFERENCES member(id),
	FOREIGN KEY (sid) REFERENCES schedule(id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- the values inserted into sale

