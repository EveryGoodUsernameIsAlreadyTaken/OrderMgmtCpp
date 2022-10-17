USE CPPDB

DROP TABLE CUSTOMERS;

CREATE TABLE CUSTOMERS(
CUSTNO		VARCHAR(5) PRIMARY KEY NOT NULL,
FIRSTNAME	VARCHAR(30) NOT NULL DEFAULT(''),
LASTNAME	VARCHAR(30) NOT NULL DEFAULT(''),
CITY		VARCHAR(30) NULL DEFAULT(''),
ADDR		VARCHAR(30) NULL DEFAULT(''),
PHONE		VARCHAR(12) NULL DEFAULT('')
);

INSERT INTO CUSTOMERS VALUES('00001', 'Peter', 'Park', 'Oakland', '7 Manito Ave', '111-222-2222');
INSERT INTO CUSTOMERS VALUES('00002', 'Paul', 'Park', 'Oakland', '8 Manito Ave', '112-333-3333');
INSERT INTO CUSTOMERS VALUES('00003', 'Lydia', 'Park', 'Oakland', '9 Manito Ave', '113-444-4444');
INSERT INTO CUSTOMERS VALUES('00004', 'JOseph', 'Park', 'Oakland', '10 Manito Ave', '114-555-5555');

DROP TABLE ORDERHISTORY;

CREATE TABLE ORDERHISTORY(
ORDERNO		INT PRIMARY KEY NOT NULL,
CUSTNO		INT NOT NULL,
ITEM		VARCHAR(20) NOT NULL DEFAULT(''),
QUANTITY	INT,
UNITPRICE	DECIMAL,
REMARKS		VARCHAR(30) )

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_CRTTBL_ORDERHISTORY
-- ORDER HISTORY TABLE을 만든다
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_CRTTBL_ORDERHISTORY' AND TYPE = 'P' )
  DROP PROC PQ_CRTTBL_ORDERHISTORY
GO
CREATE PROC PQ_CRTTBL_ORDERHISTORY
AS
BEGIN
	CREATE TABLE ORDERHISTORY(
	ORDERNO		INT PRIMARY KEY NOT NULL,
	CUSTNO		INT NOT NULL,
	ITEM		VARCHAR(20) NOT NULL DEFAULT(''),
	QUANTITY	INT,
	UNITPRICE	DECIMAL,
	REMARKS		VARCHAR(30) )
END
GO


---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_CUSTOMERLIST
-- QUERY ALL CUSTOMER LIST
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_CUSTOMERLIST' AND TYPE = 'P' )
  DROP PROC PQ_CUSTOMERLIST
GO
CREATE PROC PQ_CUSTOMERLIST
AS
BEGIN
	SELECT * FROM CUSTOMERS
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_CUSTOMERINFO
-- QUERY CUSTOMER INFO BY CUSTNO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_CUSTOMERINFO' AND TYPE = 'P' )
  DROP PROC PQ_CUSTOMERINFO
GO
CREATE PROC PQ_CUSTOMERINFO 
  @CUSTNO VARCHAR(6)= '00001'
AS
BEGIN
	SELECT * FROM CUSTOMERS WHERE CUSTNO = @CUSTNO
END
GO

EXEC PQ_CUSTOMERINFO '00003'

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_ORDERINFO
-- QUERY ORDER INFO BY ORDERNO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_ORDERINFO' AND TYPE = 'P' )
  DROP PROC PQ_ORDERINFO
GO
CREATE PROC PQ_ORDERINFO 
  @ORDERNO INT= 1
AS
BEGIN
	SELECT * FROM ORDERHISTORY WHERE ORDERNO = @ORDERNO
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_INSERT_ORDER
-- INSERT ORDER INFO INTO ORDERHISTORY
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_INSERT_ORDER' AND TYPE = 'P' )
  DROP PROC PQ_INSERT_ORDER
GO
CREATE PROC PQ_INSERT_ORDER 
  @ORDERNO INT,
  @CUSTNO VARCHAR(5),
  @DATE VARCHAR(10),
  @PRODUCT VARCHAR(20),
  @PRICE DECIMAL(8,2),
  @QUANTITY INT
AS
BEGIN
	INSERT INTO ORDERHISTORY VALUES(@ORDERNO, @CUSTNO, @DATE, @PRODUCT, @PRICE, @QUANTITY)
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_MODIFY_ORDER
-- MODIFY ORDER BY ORDERNO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_UPDATE_ORDER' AND TYPE = 'P' )
  DROP PROC PQ_UPDATE_ORDER
GO
CREATE PROC PQ_UPDATE_ORDER 
  @ORDERNO INT,
  @PRODUCT VARCHAR(20),
  @PRICE DECIMAL(8,2),
  @QUANTITY INT
AS
BEGIN
	UPDATE ORDERHISTORY SET PRODUCT = @PRODUCT, PRICE = @PRICE, QUANTITY = @QUANTITY WHERE ORDERNO = @ORDERNO
END
GO

EXEC PQ_ORDERINFO '00001'

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_ORDERINFO
-- SELECT ORDER BY ONLY ORDERNO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_ORDERINFO' AND TYPE = 'P' )
  DROP PROC PQ_ORDERINFO
GO
CREATE PROC PQ_ORDERINFO 
  @ORDERNO INT
AS
BEGIN
	SELECT * FROM ORDERHISTORY WHERE ORDERNO = @ORDERNO
END
GO

---------------------------------------------------------------------------------------
-- [PROCEDURE]: PQ_ORDERLIST
-- SELECT ORDERS BY CUSTNO
---------------------------------------------------------------------------------------
IF EXISTS(SELECT * FROM SYS.PROCEDURES WHERE NAME = 'PQ_ORDERLIST' AND TYPE = 'P' )
  DROP PROC PQ_ORDERLIST
GO
CREATE PROC PQ_ORDERLIST 
  @CUSTNO VARCHAR(5)
AS
BEGIN
	SELECT * FROM ORDERHISTORY WHERE CUSTNO = @CUSTNO
END
GO


EXEC PQ_ORDERINFO '00001'
