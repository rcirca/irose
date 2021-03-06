USE [SHO_Mall]
GO
/****** Object:  StoredProcedure [dbo].[GetItemLIST]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO



CREATE PROCEDURE [dbo].[GetItemLIST]
	@szAccount	nvarchar(30),
	@biStartID	bigint
AS
	SELECT top 48 intID, nItemTYPE,nItemNO,nDupCNT,txtFromCHAR,txtToCHAR,txtDESC,txtFromIP, nTime  from tblCART where txtACCOUNT = @szAccount and intID > @biStartID
GO
/****** Object:  StoredProcedure [dbo].[GiveITEM]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO

-- 리턴값은 남은 아이템 갯수...
CREATE PROCEDURE [dbo].[GiveITEM]  
	@biItemID		bigint,
	@szFromAccount	nvarchar(30),
	@szFromChar		nvarchar(30),
	@szToAccount		nvarchar(30),
	@szToChar		nvarchar(30),
	@szDesc		nvarchar(80),
	@szUserIP		nvarchar(20),
	@szServerIP		nvarchar(20),
	@nGiveCnt		smallint
AS
	DECLARE @nItemType int, @nItemNo int, @nDupCnt  int

	SELECT  @nItemType=nItemTYPE, @nItemNo=nItemNO, @nDupCnt=nDupCNT from tblCART where intID = @biItemID and txtAccount = @szFromAccount;
	IF @@ROWCOUNT <= 0
		RETURN -1;

	-- 분할 전달이 안될경우...
	IF @nDupCnt <= @nGiveCnt
	BEGIN
		-- 레코드 통채로 변경
		BEGIN TRAN
		UPDATE tblCART SET 	txtACCOUNT=@szToAccount,
				            	txtFromACC=@szFromAccount,
					txtFromCHAR=@szFromChar,
					txtToCHAR=@szToChar,
					txtDesc=@szDesc,
					txtFromIP=@szServerIP
					WHERE intID=@biItemID and txtACCOUNT=@szFromAccount
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN
			RETURN -1;
		END
	
		COMMIT TRAN
	
		-- 선물한 로그...
		INSERT ItemStorageHistory_T ( UserID,CharName,UserIP,ServerIP,Mallidx,ItemType,ItemNo,ItemCount,RecvUserID,RecvCharName,btLogType ) 
			VALUES ( @szFromAccount,@szFromChar,@szUserIP,@szServerIP,@biItemID, @nItemType,@nItemNo,@nDupCnt, @szToAccount,@szToChar,1 );
	
		RETURN 0;
	END


	BEGIN TRAN
	-- 아이템 @nGiveCn개짜리 추가...
	INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT,txtFromACC,txtFromCHAR,txtToCHAR,txtDesc,txtFromIP) VALUES
		 ( @szToAccount, @nItemType, @nItemNo, @nGiveCnt, @szFromAccount, @szFromChar, @szToChar, @szDesc, @szServerIP );
	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN
		RETURN -1;
	END

	-- 기존꺼에서 갯수 감소.	
	UPDATE tblCART SET	nDupCNT=@nDupCnt-@nGiveCnt WHERE intID=@biItemID and txtACCOUNT=@szFromAccount
	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN
		RETURN -1;
	END

	COMMIT TRAN

	-- 선물한 로그...
	INSERT ItemStorageHistory_T ( UserID,CharName,UserIP,ServerIP,Mallidx,ItemType,ItemNo,ItemCount,RecvUserID,RecvCharName,btLogType ) 
		VALUES ( @szFromAccount,@szFromChar,@szUserIP,@szServerIP,@biItemID, @nItemType,@nItemNo,@nGiveCnt, @szToAccount,@szToChar,1 );

	RETURN @nDupCnt-@nGiveCnt;

GO
/****** Object:  StoredProcedure [dbo].[InsertITEM]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[InsertITEM]
	@szAccount	nvarchar(30),
	@nItemType	smallint,
	@nItemNo	smallint,
	@nDupCnt	smallint
AS
	IF @nItemType >= 10 AND @nItemType < 14
	BEGIN
		-- 중복갯수 적용되는 아이템 :: 중복갯수는 최대 999개씩으로 끊어서...
		BEGIN TRAN ins_item
		WHILE @nDupCnt > 999
		BEGIN
			SET @nDupCnt = @nDupCnt - 999
			INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,999 );
			IF @@ERROR <> 0
			BEGIN
				ROLLBACK TRAN ins_item
				RETURN 0;
			END
		END
		
		IF @nDupCnt > 0 
		BEGIN
			INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,@nDupCnt );
		END
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN ins_item
			RETURN 0;
		END
	
		COMMIT TRAN ins_item
		RETURN 1;
	END

	BEGIN TRAN ins_item
	-- 장비 아이템..
	WHILE @nDupCnt > 0
	BEGIN
		SET @nDupCnt = @nDupCnt - 1

		INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,1 );
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN ins_item
			RETURN 0;
		END
	END

	COMMIT TRAN ins_item
	RETURN 1;

GO
/****** Object:  StoredProcedure [dbo].[InsertITEM_BACK]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO



CREATE PROCEDURE [dbo].[InsertITEM_BACK]
	@szAccount	nvarchar(30),
	@nItemType	smallint,
	@nItemNo	smallint,
	@nDupCnt	smallint
AS
	BEGIN TRAN ins_item
	INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,@nDupCnt );

	IF @@ERROR <> 0
	BEGIN
		ROLLBACK TRAN ins_item
		RETURN 0;
	END

	COMMIT TRAN ins_item
	RETURN 1;

GO
/****** Object:  StoredProcedure [dbo].[InsertITEM2]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO



CREATE PROCEDURE [dbo].[InsertITEM2] 
	@szAccount	nvarchar(30),
	@nItemType	smallint,
	@nItemNo	smallint,
	@nDupCnt	smallint
AS
	IF @nItemType >= 10 AND @nItemType < 14
	BEGIN
		-- 중복갯수 적용되는 아이템
		BEGIN TRAN ins_item
		INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,@nDupCnt );
	
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN ins_item
			RETURN 0;
		END
	
		COMMIT TRAN ins_item
		RETURN 1;
	END

	BEGIN TRAN ins_item
	-- 장비 아이템..
	WHILE @nDupCnt > 0
	BEGIN

		SET @nDupCnt = @nDupCnt - 1
		INSERT tblCART ( txtACCOUNT,nItemTYPE,nItemNO,nDupCNT ) VALUES ( @szAccount,@nItemType,@nItemNo,1 );
		IF @@ERROR <> 0
		BEGIN
			ROLLBACK TRAN ins_item
			RETURN 0;
		END
	END

	COMMIT TRAN ins_item
	RETURN 1;

GO
/****** Object:  StoredProcedure [dbo].[NewPickOutITEM]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO

CREATE PROCEDURE [dbo].[NewPickOutITEM]
	@szAccount	nvarchar(30),
	@szCharName	nvarchar(30),
	@szUserIP	nvarchar(20),
	@szServerIP	nvarchar(20),
	@biSqlID	bigint,
	@nPickOutCnt	smallint 
AS
	DECLARE @nItemType int, @nItemNo int, @nDupCnt  int, @nRemainCount int

	SELECT  @nItemType=nItemTYPE, @nItemNo=nItemNO, @nDupCnt=nDupCNT from tblCART where intID = @biSqlID and txtAccount = @szAccount;
	IF @@ROWCOUNT <= 0
		RETURN 0;

	BEGIN TRAN

	DELETE from tblCART where intID = @biSqlID and txtAccount = @szAccount;
	IF @@ERROR = 0
		BEGIN
		COMMIT TRAN
		-- 아이템 꺼내간 로그 남길곳....
		INSERT ItemStorageHistory_T ( UserID,CharName,UserIP,ServerIP,Mallidx,ItemType,ItemNo,ItemCount ) VALUES ( @szAccount,@szCharName,@szUserIP,@szServerIP,@biSqlID, @nItemType,@nItemNo,@nDupCnt );
		SELECT @nItemType, @nItemNo, @nDupCnt, 0;
		RETURN 1;
		END
	

	ROLLBACK TRAN
	RETURN 0;
GO
/****** Object:  StoredProcedure [dbo].[PickOutITEM]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO

CREATE PROCEDURE [dbo].[PickOutITEM]
	@szAccount	nvarchar(30),
	@szCharName	nvarchar(30),
	@szUserIP	nvarchar(20),
	@szServerIP	nvarchar(20),
	@biSqlID	bigint,
	@nPickOutCnt	smallint 
AS
	DECLARE @nItemType int, @nItemNo int, @nDupCnt  int, @nRemainCount int, @ntime int

	SELECT  @nItemType=nItemTYPE, @nItemNo=nItemNO, @nDupCnt=nDupCNT, @ntime=nTime from tblCART where intID = @biSqlID and txtAccount = @szAccount;
	IF @@ROWCOUNT <= 0
		RETURN 0;

	BEGIN TRAN

	IF @nDupCnt > @nPickOutCnt
	BEGIN
		-- 기존 레코드의 nDupCnt를 @nPickOutCnt만큼... 감소
		set @nRemainCount = @nDupCnt - @nPickOutCnt;
		update tblCART set nDupCNT=@nRemainCount where intID = @biSqlID and txtAccount = @szAccount;
		IF @@ERROR = 0
		BEGIN
			COMMIT TRAN
			-- 아이템 꺼내간 로그 남길곳....
			INSERT ItemStorageHistory_T ( UserID,CharName,UserIP,ServerIP,Mallidx,ItemType,ItemNo,ItemCount ) VALUES ( @szAccount,@szCharName,@szUserIP,@szServerIP,@biSqlID, @nItemType,@nItemNo,@nPickOutCnt );

			SELECT @nItemType, @nItemNo, @nPickOutCnt, @nRemainCount, @ntime;
			RETURN 1;
		END
	END
	ELSE
	BEGIN
		DELETE from tblCART where intID = @biSqlID and txtAccount = @szAccount;
		IF @@ERROR = 0
		BEGIN
			COMMIT TRAN
			-- 아이템 꺼내간 로그 남길곳....
			INSERT ItemStorageHistory_T ( UserID,CharName,UserIP,ServerIP,Mallidx,ItemType,ItemNo,ItemCount ) VALUES ( @szAccount,@szCharName,@szUserIP,@szServerIP,@biSqlID, @nItemType,@nItemNo,@nDupCnt );

			SELECT @nItemType, @nItemNo, @nDupCnt, 0, @ntime;
			RETURN 1;
		END
	END

	ROLLBACK TRAN
	RETURN 0;
GO
/****** Object:  StoredProcedure [dbo].[usp_ASPGetTableList]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE        Procedure [dbo].[usp_ASPGetTableList]
@tableName VarChar(100),
@PageSize INT,
@PageCnt INT,
@SelectedColumn VarChar(500),
@WhereCondition VarChar(200),
@OrderByColumn VarChar(100),
@OrderDirection Char(1),
@rCount INT OUTPUT,
@pCount INT OUTPUT

AS
Begin
/****************************************************************************************************************************
Procedure  : Get Table List
Parameter  : @tableName            VarChar(100)        -- 데이터를 읽어들일 테이블이나 뷰
Parameter  : @PageSize            INT        -- 한번에 읽어들일 레코드수
Parameter  : @PageCnt            INT        -- 읽어들이고자 하는 페이지
Parameter  : @SelectedColumn     VarChar(200)    -- 읽어들이고자 하는 칼럼 리스트 구분자 ,
Parameter  : @WhereCondition        VarChar(200)    -- 레코드 검색 조건식
Parameter  : @OrderByColumn        VarChar(200)        -- 정렬하고자 하는 조건식 , 읽어들이는 칼럼리스트에 반드시 포함되어야.
Parameter  : @OrderDerection         Char(1)    -- '1': ASC, '2':DESC
****************************************************************************************************************************/
SET NOCOUNT ON
    Declare @TotalRecordSize INT, @CurReadCount INT , @CmdLine NVarChar(1000) ,   @ConditionedCount INT
    Declare @SQLSTR1 VarChar(8000),@SQLSTR2 VarChar(8000)
    Declare @strParams NVarChar(100)
     
    IF Len(@WhereCondition) <> 0  
       Set  @WhereCondition = ' WHERE '+ @WhereCondition 
    Else
       Set  @WhereCondition = ''
  

    -- 조건식에 맞는 데이터의 전체 카운터가 필요하다.
    Select @CmdLine = 'SELECT @RecordCount = Count(*) From ' + @TableName + @WhereCondition 
    SET @strParams='@RecordCount INT OUTPUT'
    EXEC sp_executesql @CmdLine , @strParams  , @RecordCount = @ConditionedCount OUTPUT

     SET @rCount = @ConditionedCount

     SET  @pCount = CEILING(CONVERT(DECIMAL(8, 2), @ConditionedCount) / @PageSize)
--PRINT @rCount
--PRINT @pCount

    -- 읽어 오고자 하는 페이지의 정보가 최대 Size의 어느 부분에 해당하는지 계산한다.(List에서 마지막 페이지에 대한 처리를 위해 ㅠ.ㅠ)
    Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt -  1))

    IF @CurReadCount > 0
        IF @CurReadCount > @PageSize
            Select @CurReadCount = @PageSize
    Else
        Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt - 1))    

SET @SQLSTR1 = 'SELECT *, ' + Convert(VarChar(10), @ConditionedCount) + ' as TotalCount  From'
SET @SQLSTR1 = @SQLSTR1 +' (SELECT TOP ' + Convert(VarChar(10), @CurReadCount)  + ' ' + @SelectedColumn 
SET @SQLSTR1 = @SQLSTR1 +' From ' + '(Select TOP '+Convert(VarChar(10), @PageSize*@PageCnt) + ' ' + @SelectedColumn
SET @SQLSTR2 = ' From ' + @TableName+'  ' + IsNull(@WhereCondition,' 1=1 ') + ' Order By ' + @OrderByColumn
SET @SQLSTR2 = @SQLSTR2 + CASE WHEN @OrderDirection = '1' THEN ' DESC ' ELSE ' ASC ' END + ') as X'  
SET @SQLSTR2 = @SQLSTR2 +' ORDER BY ' + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN ' ASC ' ELSE ' DESC ' END + ')  as Y ORDER BY ' 
SET @SQLSTR2 = @SQLSTR2 + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN '  DESC ' ELSE ' ASC ' END

EXEC (@SQLSTR1+@SQLSTR2)


    -- 만일 ADO 의 Command객체를 사용한다면
    -- Return (@ConditionedCount) --:)

SET NOCOUNT OFF
End
GO
/****** Object:  StoredProcedure [dbo].[usp_ASPGetTableList_itemlist]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO

CREATE              Procedure [dbo].[usp_ASPGetTableList_itemlist]
@tableName VarChar(100),
@PageSize INT,
@PageCnt INT,
@SelectedColumn VarChar(500),
@WhereCondition VarChar(200),
@OrderByColumn VarChar(100),
@OrderDirection Char(1),
@rCount INT OUTPUT,
@pCount INT OUTPUT

AS
Begin

SET NOCOUNT ON
    Declare @TotalRecordSize INT, @CurReadCount INT , @CmdLine NVarChar(1000) ,   @ConditionedCount INT
    Declare @SQLSTR1 VarChar(8000),@SQLSTR2 VarChar(8000),@OrderByColumn2 VarChar(200)
    Declare @strParams NVarChar(100) ,@nType VarChar(1000)
     
    IF Len(@WhereCondition) <> 0  
       Set  @WhereCondition = ' WHERE '+ @WhereCondition 
    Else
       Set  @WhereCondition = ''

     -- 조건식에 맞는 데이터의 전체 카운터가 필요하다.
    Select @CmdLine = 'SELECT @RecordCount = Count(*) From ' + @TableName + @WhereCondition 
    SET @strParams='@RecordCount INT OUTPUT'
    EXEC sp_executesql @CmdLine , @strParams  , @RecordCount = @ConditionedCount OUTPUT

     SET @rCount = @ConditionedCount

     SET  @pCount = CEILING(CONVERT(DECIMAL(8, 2), @ConditionedCount) / @PageSize)
--PRINT @rCount
--PRINT @pCount

    -- 읽어 오고자 하는 페이지의 정보가 최대 Size의 어느 부분에 해당하는지 계산한다.(List에서 마지막 페이지에 대한 처리를 위해 ㅠ.ㅠ)
    Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt -  1))

    IF @CurReadCount > 0
        IF @CurReadCount > @PageSize
            Select @CurReadCount = @PageSize
    Else
        Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt - 1))    


SET @SQLSTR1 = 'SELECT *, ' + Convert(VarChar(10), @ConditionedCount) + ' as TotalCount  From'
SET @SQLSTR1 = @SQLSTR1 +' (SELECT TOP ' + Convert(VarChar(10), @CurReadCount)  + ' ' + @SelectedColumn 
SET @SQLSTR1 = @SQLSTR1 +' From ' + '(Select TOP '+Convert(VarChar(10), @PageSize*@PageCnt) + ' ' + @SelectedColumn
SET @SQLSTR2 = ' From ' + @TableName+'  ' + IsNull(@WhereCondition,' 1=1 ') + ' Order By ' + @OrderByColumn
SET @SQLSTR2 = @SQLSTR2 + CASE WHEN @OrderDirection = '1' THEN ' DESC ' ELSE ' ASC ' END + ') as X'  
SET @SQLSTR2 = @SQLSTR2 +' ORDER BY ' + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN ' ASC ' ELSE ' DESC ' END + ')  as Y ORDER BY ' 
SET @SQLSTR2 = @SQLSTR2 + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN '  DESC ' ELSE ' ASC ' END

--PRINT @SQLSTR1+@SQLSTR2
EXEC (@SQLSTR1+@SQLSTR2)

    -- 만일 ADO 의 Command객체를 사용한다면
    -- Return (@ConditionedCount) --:)

SET NOCOUNT OFF
End
GO
/****** Object:  StoredProcedure [dbo].[usp_ASPGetTableList_ItemMain]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO
CREATE                      Procedure [dbo].[usp_ASPGetTableList_ItemMain]
AS
Begin
SET NOCOUNT ON
    Declare @SQLSTR1 VarChar(8000)

--SET @SQLSTR1 = 'select top 4 intID,itemNo,ItemName,itemBundle,ItemPoint,AttFile1,AttFile2,regdate   from ItemMall_TS Where DelCode= 1 and CateGory Like "스페샬몰%" and  ItemBest=1 order by itemSalecount  DESC;'
SET @SQLSTR1 = 'select top 4 intID,itemNo,ItemName,itemBundle,ItemPoint,AttFile1,AttFile2,regdate   from ItemMall_TS Where DelCode= 1 and CateGory Like "주논몰%" order by Regdate DESC;'
EXEC (@SQLSTR1)

SET @SQLSTR1 = 'select top 4 intID,itemNo,ItemName,itemBundle,ItemPoint,AttFile1,AttFile2,regdate   from ItemMall_TS Where DelCode= 1 and CateGory Like "아루아몰%" order by Regdate DESC;'
EXEC (@SQLSTR1)

SET @SQLSTR1 = 'select top 4 intID,itemNo,ItemName,itemBundle,ItemPoint,AttFile1,AttFile2,regdate   from ItemMall_TS Where DelCode= 1 and CateGory Like "스페샬몰%" order by Regdate DESC;'
EXEC (@SQLSTR1)

SET @SQLSTR1 = 'select top 4 intID,itemNo,ItemName,itemBundle,ItemPoint,AttFile1,AttFile2,regdate   from ItemMall_TS Where DelCode= 1 and CateGory Like "이벤트몰%" order by Regdate DESC;'
EXEC (@SQLSTR1)



--SET @SQLSTR1 = 'select top 4 intSeq ,nType,Subject,regdate   from Notice_T Where bViewTag = 1 and (nType = ''이벤트'')'
--EXEC (@SQLSTR1)

--SET @SQLSTR1 = 'select top 5 intSeq ,nType,Subject,regdate   from Notice_T Where bViewTag = 1 and (nType = ''업데이트'')'
--EXEC (@SQLSTR1)

--SET @SQLSTR1 = 'Select top 5  intSeq, Subject From Poll_T  where MainPageView = 1 '
--EXEC (@SQLSTR1)
SET NOCOUNT OFF
End
GO
/****** Object:  StoredProcedure [dbo].[usp_ASPGetTableList_ItemMain_D]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO

CREATE              Procedure [dbo].[usp_ASPGetTableList_ItemMain_D]
@tableName VarChar(100),
@PageSize INT,
@PageCnt INT,
@SelectedColumn VarChar(500),
@WhereCondition VarChar(200),
@OrderByColumn VarChar(100),
@OrderDirection Char(1),
@rCount INT OUTPUT,
@pCount INT OUTPUT

AS
Begin

SET NOCOUNT ON
    Declare @TotalRecordSize INT, @CurReadCount INT , @CmdLine NVarChar(1000) ,   @ConditionedCount INT
    Declare @SQLSTR1 VarChar(8000),@SQLSTR2 VarChar(8000),@OrderByColumn2 VarChar(200)
    Declare @strParams NVarChar(100) ,@nType VarChar(1000)
     
    IF Len(@WhereCondition) <> 0  
       Set  @WhereCondition = ' WHERE '+ @WhereCondition 
    Else
       Set  @WhereCondition = ''

     -- 조건식에 맞는 데이터의 전체 카운터가 필요하다.
    Select @CmdLine = 'SELECT @RecordCount = Count(*) From ' + @TableName + @WhereCondition 
    SET @strParams='@RecordCount INT OUTPUT'
    EXEC sp_executesql @CmdLine , @strParams  , @RecordCount = @ConditionedCount OUTPUT

     SET @rCount = @ConditionedCount

     SET  @pCount = CEILING(CONVERT(DECIMAL(8, 2), @ConditionedCount) / @PageSize)
--PRINT @rCount
--PRINT @pCount

    -- 읽어 오고자 하는 페이지의 정보가 최대 Size의 어느 부분에 해당하는지 계산한다.(List에서 마지막 페이지에 대한 처리를 위해 ㅠ.ㅠ)
    Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt -  1))

    IF @CurReadCount > 0
        IF @CurReadCount > @PageSize
            Select @CurReadCount = @PageSize
    Else
        Select @CurReadCount = @ConditionedCount - (@PageSize * (@PageCnt - 1))    


SET @SQLSTR1 = 'SELECT *, ' + Convert(VarChar(10), @ConditionedCount) + ' as TotalCount  From'
SET @SQLSTR1 = @SQLSTR1 +' (SELECT TOP ' + Convert(VarChar(10), @CurReadCount)  + ' ' + @SelectedColumn 
SET @SQLSTR1 = @SQLSTR1 +' From ' + '(Select TOP '+Convert(VarChar(10), @PageSize*@PageCnt) + ' ' + @SelectedColumn
SET @SQLSTR2 = ' From ' + @TableName+'  ' + IsNull(@WhereCondition,' 1=1 ') + ' Order By ' + @OrderByColumn
SET @SQLSTR2 = @SQLSTR2 + CASE WHEN @OrderDirection = '1' THEN ' DESC ' ELSE ' ASC ' END + ') as X'  
SET @SQLSTR2 = @SQLSTR2 +' ORDER BY ' + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN ' ASC ' ELSE ' DESC ' END + ')  as Y ORDER BY ' 
SET @SQLSTR2 = @SQLSTR2 + @OrderByColumn + CASE WHEN @OrderDirection = '1' THEN '  DESC ' ELSE ' ASC ' END

--PRINT @SQLSTR1+@SQLSTR2
EXEC (@SQLSTR1+@SQLSTR2)

Set @nType = 'select * from 
	(select top 1  intSeq,ItemType,ItemName,attfile1,attfile3 from ItemMall_T where nType = ''1'' and bViewTag = ''1'' and BestItem = ''1'' order by intSeq desc ) as nType1,
	(select top 1  intSeq,ItemType,ItemName,attfile1,attfile3 from ItemMall_T where nType = ''2'' and bViewTag = ''1'' and BestItem = ''1'' order by intSeq desc) as nType2,
	(select top 1  intSeq,ItemType,ItemName,attfile1,attfile3 from ItemMall_T where nType = ''3''  and bViewTag = ''1'' and BestItem = ''1'' order by intSeq desc ) as nType3,
	(select top 1  intSeq,ItemType,ItemName,attfile1,attfile3 from ItemMall_T where nType = ''4''  and bViewTag = ''1'' and BestItem = ''1'' order by intSeq desc ) as nType4'

EXEC (@nType)

    -- 만일 ADO 의 Command객체를 사용한다면
    -- Return (@ConditionedCount) --:)

SET NOCOUNT OFF
End
GO
/****** Object:  Table [dbo].[GM_USERINFO_TMP_T]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[GM_USERINFO_TMP_T](
	[seqNum] [int] NOT NULL,
	[UserID] [varchar](20) NOT NULL,
	[UserName] [varchar](10) NOT NULL,
	[UserPWD] [varchar](50) NOT NULL,
	[DeptName] [varchar](50) NOT NULL,
	[AdminLevel] [char](1) NOT NULL,
	[JoinDate] [datetime] NOT NULL
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[ItemBasket_T]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ItemBasket_T](
	[intSeq] [bigint] IDENTITY(1,1) NOT NULL,
	[UserID] [varchar](20) NOT NULL,
	[ItemAID] [int] NOT NULL,
	[ItemNo] [int] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemName] [varchar](50) NULL,
	[ItemCount] [int] NOT NULL,
	[Unit_Cost] [int] NOT NULL,
	[SellCount] [int] NULL,
	[attfile1] [varchar](255) NULL,
	[attfile2] [varchar](255) NOT NULL,
	[attfile3] [varchar](255) NULL,
	[regdate] [datetime] NOT NULL
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[ItemMall_T]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ItemMall_T](
	[intSeq] [bigint] IDENTITY(1,1) NOT NULL,
	[ItemNo] [int] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemName] [varchar](50) NOT NULL,
	[Level_Limit] [varchar](50) NULL,
	[ItemOption] [varchar](255) NULL,
	[ItemTransfer] [varchar](50) NULL,
	[ItemDesc] [varchar](255) NOT NULL,
	[SellCount] [int] NOT NULL,
	[Unit_Cost] [int] NOT NULL,
	[nType] [char](1) NOT NULL,
	[bViewTag] [char](1) NOT NULL,
	[SoldCount] [bigint] NULL,
	[attfile1] [varchar](255) NULL,
	[attfile2] [varchar](255) NULL,
	[attfile3] [varchar](255) NULL,
	[intCount] [int] NULL,
	[BestItem] [char](1) NULL,
	[regdate] [datetime] NOT NULL
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[ItemMall_TS]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ItemMall_TS](
	[intID] [int] IDENTITY(1,1) NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemNo] [int] NOT NULL,
	[CateNo] [int] NOT NULL,
	[ItemName] [varchar](60) NOT NULL,
	[AttFile1] [varchar](255) NULL,
	[AttFile2] [varchar](255) NULL,
	[AttFile3] [varchar](255) NULL,
	[ItemPeriod] [smallint] NOT NULL,
	[ItemBundle] [smallint] NOT NULL,
	[itemGive] [smallint] NOT NULL,
	[itemClass] [varchar](60) NULL,
	[ItemUnit] [smallint] NOT NULL,
	[ItemPoint] [smallint] NOT NULL,
	[ItemTime] [smallint] NOT NULL,
	[ItemAttack] [smallint] NULL,
	[ItemAttackSpeed] [smallint] NULL,
	[ItemHit] [smallint] NULL,
	[ItemSafe] [smallint] NULL,
	[ItemEvasion] [smallint] NULL,
	[ItemMagic] [smallint] NULL,
	[ItemEndurance] [smallint] NULL,
	[ItemLimit] [smallint] NULL,
	[ItemInUp1] [varchar](60) NULL,
	[ItemInUpPoint1] [smallint] NULL,
	[ItemInUp2] [varchar](60) NULL,
	[ItemInUpPoint2] [smallint] NULL,
	[ItemAttack_ADD] [smallint] NULL,
	[SameTime] [smallint] NULL,
	[Delay] [smallint] NULL,
	[ItemKeepTime] [smallint] NULL,
	[ItemOk] [smallint] NULL,
	[ItemContents] [text] NULL,
	[ItemSearchCount] [int] NOT NULL,
	[ItemSaleCount] [int] NOT NULL,
	[ItemBest] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[ItemStorageHistory_T]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[ItemStorageHistory_T](
	[intSeq] [bigint] IDENTITY(1,1) NOT NULL,
	[UserID] [varchar](20) NOT NULL,
	[CharName] [varchar](30) NOT NULL,
	[UserIP] [varchar](20) NOT NULL,
	[ServerIP] [varchar](50) NOT NULL,
	[Mallidx] [bigint] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemNo] [int] NOT NULL,
	[ItemCount] [int] NOT NULL,
	[regdate] [datetime] NOT NULL,
	[RecvUserID] [varchar](20) NULL,
	[RecvCharName] [varchar](30) NULL,
	[btLogType] [tinyint] NULL
) ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[tbl_ClanWar]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
SET ANSI_PADDING ON
GO
CREATE TABLE [dbo].[tbl_ClanWar](
	[intID] [bigint] IDENTITY(1,1) NOT NULL,
	[dataREG] [datetime] NULL,
	[nServerNo] [int] NOT NULL,
	[nWarType] [int] NOT NULL,
	[nWarWin] [int] NOT NULL,
	[nMoney] [money] NOT NULL,
	[nAClanID] [int] NOT NULL,
	[txtAClanName] [varchar](40) NULL,
	[nBClanID] [int] NOT NULL,
	[txtBClanName] [varchar](40) NULL,
	[nChannelNo] [int] NULL,
	[Comment] [text] NULL
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO
SET ANSI_PADDING OFF
GO
/****** Object:  Table [dbo].[tbl_SALE_ITEM]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_SALE_ITEM](
	[intID] [bigint] IDENTITY(1,1) NOT NULL,
	[nProductNum] [int] NOT NULL,
	[nItemType] [int] NOT NULL,
	[nItemNo] [int] NOT NULL,
	[nDupCNT] [int] NOT NULL,
	[nTime] [int] NOT NULL,
	[nItemCode] [int] NOT NULL
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[tblCART]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblCART](
	[intID] [bigint] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NULL,
	[txtACCOUNT] [nvarchar](35) NOT NULL,
	[nItemTYPE] [smallint] NOT NULL,
	[nItemNO] [smallint] NOT NULL,
	[nDupCNT] [smallint] NOT NULL,
	[txtFromACC] [nvarchar](35) NULL,
	[txtFromCHAR] [nvarchar](30) NULL,
	[txtToCHAR] [nvarchar](30) NULL,
	[txtDESC] [nvarchar](80) NULL,
	[txtFromIP] [nvarchar](20) NULL,
	[nTime] [int] NULL,
	[nTime2] [int] NULL
) ON [PRIMARY]

GO
/****** Object:  Table [dbo].[tblCART_BACKUP]    Script Date: 3/10/2012 15:38:54 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tblCART_BACKUP](
	[intID] [bigint] IDENTITY(1,1) NOT NULL,
	[dateREG] [datetime] NULL,
	[txtACCOUNT] [nvarchar](30) NOT NULL,
	[nItemTYPE] [smallint] NOT NULL,
	[nItemNO] [smallint] NOT NULL,
	[nDupCNT] [smallint] NOT NULL,
	[RegID] [nvarchar](20) NOT NULL
) ON [PRIMARY]

GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemPeriod]  DEFAULT (0) FOR [ItemPeriod]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemBundle]  DEFAULT (1) FOR [ItemBundle]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_itemGive]  DEFAULT (0) FOR [itemGive]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemUnit]  DEFAULT (1) FOR [ItemUnit]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemSearchCount]  DEFAULT (0) FOR [ItemSearchCount]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemSaleCount]  DEFAULT (0) FOR [ItemSaleCount]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_ItemBest]  DEFAULT (0) FOR [ItemBest]
GO
ALTER TABLE [dbo].[ItemMall_TS] ADD  CONSTRAINT [DF_ItemMall_TS_RegDate]  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_dataREG]  DEFAULT (getdate()) FOR [dataREG]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nServerNo]  DEFAULT (0) FOR [nServerNo]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nWarType]  DEFAULT (0) FOR [nWarType]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nWarWin]  DEFAULT (0) FOR [nWarWin]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nMoney]  DEFAULT (0) FOR [nMoney]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nAClanID]  DEFAULT (0) FOR [nAClanID]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nBClanID]  DEFAULT (0) FOR [nBClanID]
GO
ALTER TABLE [dbo].[tbl_ClanWar] ADD  CONSTRAINT [DF_tbl_ClanWar_nChannelNo]  DEFAULT (0) FOR [nChannelNo]
GO
ALTER TABLE [dbo].[tbl_SALE_ITEM] ADD  CONSTRAINT [DF_tbl_SALE_ITEM_nProductNum]  DEFAULT (0) FOR [nProductNum]
GO
ALTER TABLE [dbo].[tbl_SALE_ITEM] ADD  CONSTRAINT [DF_tbl_SALE_ITEM_nItemType]  DEFAULT (0) FOR [nItemType]
GO
ALTER TABLE [dbo].[tbl_SALE_ITEM] ADD  CONSTRAINT [DF_tbl_SALE_ITEM_nDupCNT]  DEFAULT (1) FOR [nDupCNT]
GO
ALTER TABLE [dbo].[tbl_SALE_ITEM] ADD  CONSTRAINT [DF_tbl_SALE_ITEM_nTime]  DEFAULT (0) FOR [nTime]
GO
ALTER TABLE [dbo].[tbl_SALE_ITEM] ADD  CONSTRAINT [DF_tbl_SALE_ITEM_nItemCode]  DEFAULT (0) FOR [nItemCode]
GO
ALTER TABLE [dbo].[tblCART] ADD  CONSTRAINT [DF_tblCART_nTime]  DEFAULT (0) FOR [nTime]
GO
