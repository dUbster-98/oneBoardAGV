/* Utils.em - a small collection of useful editing macros */



/*-------------------------------------------------------------------------
	I N S E R T   H E A D E R

	Inserts a comment header block at the top of the current function. 
	This actually works on any type of symbol, not just functions.

	To use this, define an environment variable "MYNAME" and set it
	to your email name.  eg. set MYNAME=raygr
-------------------------------------------------------------------------*/
macro get_today()
{
	today = GetSysTime(1);

	fdate = "";
	fdate = cat(fdate, (today.Year));

	if (today.Month < 10)
		fdate = cat(fdate,"0");

	fdate = cat(fdate, today.Month);

	if (today.Day < 10)
		fdate = cat(fdate, "0");

	fdate = cat(fdate, today.Day);

	return fdate;
}
macro InsertHeader()
{
	szMyName = jhshin	/*getenv(MYNAME)*/

	hwnd = GetCurrentWnd()
	lnFirst = GetWndSelLnFirst(hwnd)
	lnLast = GetWndSelLnLast(hwnd)

	hbuf = GetCurrentBuf()
	InsBufLine(hbuf, lnFirst, "//*********************************************************************************************")
	InsBufLine(hbuf, lnFirst+1, "// Function : ")
	InsBufLine(hbuf, lnFirst+2, "//")
	InsBufLine(hbuf, lnFirst+3, "// Date : ")
	InsBufLine(hbuf, lnFirst+4, "//")
	InsBufLine(hbuf, lnFirst+5, "// Author : jhshin ")
	InsBufLine(hbuf, lnFirst+6, "//")
	InsBufLine(hbuf, lnFirst+7, "// Description : ")
	InsBufLine(hbuf, lnFirst+8, "//")
	InsBufLine(hbuf, lnFirst+9, "// Input Parameters")
	InsBufLine(hbuf, lnFirst+10, "// none")
	InsBufLine(hbuf, lnFirst+11, "//")
	InsBufLine(hbuf, lnFirst+12, "// Output Parameters")
	InsBufLine(hbuf, lnFirst+13, "// none")
	InsBufLine(hbuf, lnFirst+14, "//")
	InsBufLine(hbuf, lnFirst+15, "//*********************************************************************************************")
}


/* InsertFileHeader:

   Inserts a comment header block at the top of the current function. 
   This actually works on any type of symbol, not just functions.

   To use this, define an environment variable "MYNAME" and set it
   to your email name.  eg. set MYNAME=raygr
*/

macro InsertFileHeader()
{
	sz = "Shin jin-ho"

	hbuf = GetCurrentBuf()
	InsBufLine(hbuf, 0, "//*********************************************************************************************")
	InsBufLine(hbuf, 1, "//")
	InsBufLine(hbuf, 2, "// Proprietary and Confidential Information")	
	InsBufLine(hbuf, 3, "//")
	InsBufLine(hbuf, 4, "// Copyright (c) 2022 by Canon Korea. All Rights Reserved.")
	InsBufLine(hbuf, 5, "//")
	InsBufLine(hbuf, 6, "// This source code and the algorithms implemented therein constitute")
	InsBufLine(hbuf, 7, "// confidential information and may compromise trade secretes of Canon Korea.")
	InsBufLine(hbuf, 8, "// or its associates, and any unaythorized use thereof is prohibited.")
	InsBufLine(hbuf, 9, "//")
	InsBufLine(hbuf, 10, "//*******************************************************************************************//")
	InsBufLine(hbuf, 11, "")
	InsBufLine(hbuf, 12, "//*******************************************************************************************//")
	InsBufLine(hbuf, 13, "//")
	InsBufLine(hbuf, 14, "// File Name:")
	InsBufLine(hbuf, 15, "//") 
	InsBufLine(hbuf, 16, "// Date: ")
	InsBufLine(hbuf, 17, "//")
	InsBufLine(hbuf, 18, "// Auther : @sz@")
	InsBufLine(hbuf, 19, "//")
	InsBufLine(hbuf, 20, "// Description: ")
	InsBufLine(hbuf, 21, "//")
	InsBufLine(hbuf, 22, "//*******************************************************************************************//")
	InsBufLine(hbuf, 23, "")
	InsBufLine(hbuf, 24, "/*********************************************************************************************")
	InsBufLine(hbuf, 25, "* 1. INCLUDES")
	InsBufLine(hbuf, 26, "*********************************************************************************************/")
	InsBufLine(hbuf, 27, "")
	InsBufLine(hbuf, 28, "/*********************************************************************************************")
	InsBufLine(hbuf, 29, "* -. Internal Definition")
	InsBufLine(hbuf, 30, "*********************************************************************************************/")
	InsBufLine(hbuf, 31, "")
	InsBufLine(hbuf, 32, "/*********************************************************************************************")
	InsBufLine(hbuf, 33, "* 2. GLOBAL VALUE")
	InsBufLine(hbuf, 34, "*********************************************************************************************/")
	InsBufLine(hbuf, 35, "")
	InsBufLine(hbuf, 36, "/*********************************************************************************************")
	InsBufLine(hbuf, 37, "* 3. INLINE FUNCTION")
	InsBufLine(hbuf, 38, "*********************************************************************************************/")
	InsBufLine(hbuf, 39, "")
	InsBufLine(hbuf, 40, "/*********************************************************************************************")
	InsBufLine(hbuf, 41, "* 4. FUNCTION")
	InsBufLine(hbuf, 42, "*********************************************************************************************/")
	InsBufLine(hbuf, 43, "")
}



// Inserts "Returns True .. or False..." at the current line
macro ReturnTrueOrFalse()
{
	hbuf = GetCurrentBuf()
	ln = GetBufLineCur(hbuf)

	InsBufLine(hbuf, ln, "    Returns True if successful or False if errors.")
}



/* Inserts ifdef REVIEW around the selection */
macro IfdefReview()
{
	IfdefSz("REVIEW");
}


/* Inserts ifdef BOGUS around the selection */
macro IfdefBogus()
{
	IfdefSz("BOGUS");
}


/* Inserts ifdef NEVER around the selection */
macro IfdefNever()
{
	IfdefSz("DEBUG_MODE");
}



// Ask user for ifdef condition and wrap it around current
// selection.
macro InsertIfdef()
{
	sz = Ask("Enter ifdef condition:")
	if (sz != "")
		IfdefSz(sz);
}

// molink_slcho_20041004 : Source Insight에 주석 다는게 없어서리.. 추가함.
macro make_prefix()
{
	current_datetime = get_datetime();
	user_id = jhshin
	prefix = "ckr_@user_id@_@current_datetime@["
	return prefix;
}



macro InsertCPlusPlus()
{
	prefix = make_prefix()

	remark_text = Ask("주석은 상세하게!!")

	hWnd = GetCurrentWnd()
	first_line = GetWndSelLnFirst(hWnd)

	hBuffer = GetCurrentBuf()
	s = GetBufLine(hBuffer, first_line)

	DelBufLine(hBuffer, first_line);
	InsBufLine(hBuffer, first_line, "@s@ // @prefix@@remark_text@]");
}


// Wrap ifdef <sz> .. endif around the current selection
macro IfdefSz(sz)
{
	hwnd = GetCurrentWnd()
	lnFirst = GetWndSelLnFirst(hwnd)
	lnLast = GetWndSelLnLast(hwnd)
	 
	hbuf = GetCurrentBuf()
	InsBufLine(hbuf, lnFirst, "#ifdef @sz@")
	InsBufLine(hbuf, lnLast+2, "#endif /* @sz@ */")
}


// Delete the current line and appends it to the clipboard buffer
macro KillLine()
{
	hbufCur = GetCurrentBuf();
	lnCur = GetBufLnCur(hbufCur)
	hbufClip = GetBufHandle("Clipboard")
	AppendBufLine(hbufClip, GetBufLine(hbufCur, lnCur))
	DelBufLine(hbufCur, lnCur)
}


// Paste lines killed with KillLine (clipboard is emptied)
macro PasteKillLine()
{
	Paste
	EmptyBuf(GetBufHandle("Clipboard"))
}



// 현재 위치에 //ckr_jhshin_날짜[] 주석을 넣기 위한 매크로
macro remark_molink()
{
	prefix = make_prefix()

	remark_text = Ask("주석은 상세하게!!")

	hWnd = GetCurrentWnd()
	first_line = GetWndSelLnFirst(hWnd)

	hBuffer = GetCurrentBuf()
	s = GetBufLine(hBuffer, first_line)

	DelBufLine(hBuffer, first_line);
	InsBufLine(hBuffer, first_line, "@s@ // @prefix@@remark_text@]");
	//InsBufLine(hBuffer, first_line, "@s@ // @prefix@ : @remark_text@");
}


// 선택한 영역을 주석처리(/**/)하고, 처음과 마지막에 comment를 단다.
macro remark_all()
{
	prefix = make_prefix()

	remark_text = Ask("주석을 적어 주세요.")

	hWnd = GetCurrentWnd()
	first_line = GetWndSelLnFirst(hWnd)
	last_line = GetWndSelLnLast(hWnd)

	hBuffer = GetCurrentBuf()
	s = GetBufLine(hBuffer, first_line)

	i = 0
	length = strlen(s)

	white_space = "";

	while (i < length)
	{
		ch = s[i]
		if (ch == " " || ch == " ")
		white_space = cat(white_space, ch);
		else
		break

		i = i + 1
	}

	InsBufLine(hBuffer, first_line, white_space # "/* //[ @prefix@ BEGIN -- @remark_text@ ]");
	InsBufLine(hBuffer, last_line + 2, white_space # "*/ //] @prefix@ END -- @remark_text@ ]");
}


// delete all lines in the buffer
macro EmptyBuf(hbuf)
{
	lnMax = GetBufLineCount(hbuf)
	while (lnMax > 0)
		{
		DelBufLine(hbuf, 0)
		lnMax = lnMax - 1
		}
}


// Ask the user for a symbol name, then jump to its declaration
macro JumpAnywhere()
{
	symbol = Ask("What declaration would you like to see?")
	JumpToSymbolDef(symbol)
}

	
// list all siblings of a user specified symbol
// A sibling is any other symbol declared in the same file.
macro OutputSiblingSymbols()
{
	symbol = Ask("What symbol would you like to list siblings for?")
	hbuf = ListAllSiblings(symbol)
	SetCurrentBuf(hbuf)
}


// Given a symbol name, open the file its declared in and 
// create a new output buffer listing all of the symbols declared
// in that file.  Returns the new buffer handle.
macro ListAllSiblings(symbol)
{
	loc = GetSymbolLocation(symbol)
	if (loc == "")
		{
		msg ("@symbol@ not found.")
		stop
		}
	
	hbufOutput = NewBuf("Results")
	
	hbuf = OpenBuf(loc.file)
	if (hbuf == 0)
		{
		msg ("Can't open file.")
		stop
		}
		
	isymMax = GetBufSymCount(hbuf)
	isym = 0;
	while (isym < isymMax)
		{
		AppendBufLine(hbufOutput, GetBufSymName(hbuf, isym))
		isym = isym + 1
		}

	CloseBuf(hbuf)
	
	return hbufOutput

}


macro get_datetime()
{
	today = GetSysTime(1);

	fdate = "";
	fdate = cat(fdate, (today.Year - 2000));

	if (today.year < 10)
		fdate = cat(fdate, "0");

	if (today.Month < 10)
		fdate = cat(fdate,"0");

	fdate = cat(fdate, today.Month);

	if (today.Day < 10)
		fdate = cat(fdate, "0");

	fdate = cat(fdate, today.Day);

	return fdate;
}



// 선택한 영역을 if 0로 막고 처음과 마지막에 comment를 단다.
macro remark_if0()
{
	prefix = make_prefix()

	remark_text = Ask("주석을 적어 주세요.")

	hWnd = GetCurrentWnd()
	first_line = GetWndSelLnFirst(hWnd)
	last_line = GetWndSelLnLast(hWnd)

	hBuffer = GetCurrentBuf()
	s = GetBufLine(hBuffer, first_line)

	InsBufLine(hBuffer, first_line, "#if 0 //[[ @prefix@_BEGIN -- @remark_text@")
	InsBufLine(hBuffer, last_line + 2, "#endif //]] @prefix@_END -- @remark_text@");
}



macro remark_begin_end()
{
	prefix = make_prefix()
	remark_text = Ask("주석을 적어 주세요.")

	hWnd = GetCurrentWnd()
	first_line = GetWndSelLnFirst(hWnd)
	last_line = GetWndSelLnLast(hWnd)
	hBuffer = GetCurrentBuf()

	s = GetBufLine(hBuffer, first_line)
	i = 0
	length = strlen(s)

	white_space = "";

	while (i < length)
	{
		ch = s[i]
		if (ch == " " || ch == " ")
			white_space = cat(white_space, ch);
		else
			break
		i = i + 1
	}
	InsBufLine(hBuffer, first_line, white_space # "//[[ @prefix@_BEGIN -- @remark_text@")
	InsBufLine(hBuffer, last_line + 2, white_space # "//]] @prefix@_END -- @remark_text@");
}