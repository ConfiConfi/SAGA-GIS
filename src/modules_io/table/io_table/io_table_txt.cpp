
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       table_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    io_table_txt.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "io_table_txt.h"


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Export::CTable_Text_Export(void)
{
	Set_Name		(_TL("Export Text Table"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("Save Headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Value(
		NULL	, "STRQUOTA"	, _TL("Strings in Quota"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, true
	);
}

//---------------------------------------------------------
bool CTable_Text_Export::On_Execute(void)
{
	CSG_String	StrFormat, Separator;
	CSG_File	Stream;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable		= Parameters("TABLE")		->asTable();
	StrFormat	= Parameters("STRQUOTA")	->asBool() ? SG_T("\"%s\"") : SG_T("%s");

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		Message_Add(_TL("file could not be opened."));
	}

	//-----------------------------------------------------
	else
	{
		if( Parameters("HEADLINE")->asBool() )
		{
			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				Stream.Printf(StrFormat, pTable->Get_Field_Name(iField));
				Stream.Printf(iField < pTable->Get_Field_Count() - 1 ? Separator.c_str() : SG_T("\n"));
			}
		}

		//-------------------------------------------------
		for(int iRecord=0; iRecord<pTable->Get_Record_Count() && Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

			for(int iField=0; iField<pTable->Get_Field_Count(); iField++)
			{
				switch( pTable->Get_Field_Type(iField) )
				{
				default:
				case SG_DATATYPE_Char:
				case SG_DATATYPE_String:
				case SG_DATATYPE_Date:
					Stream.Printf(StrFormat		, pRecord->asString(iField));
					break;

				case SG_DATATYPE_Short:
				case SG_DATATYPE_Int:
				case SG_DATATYPE_Long:
				case SG_DATATYPE_Color:
					Stream.Printf(SG_T("%d")	, pRecord->asDouble(iField));

				case SG_DATATYPE_Float:
				case SG_DATATYPE_Double:
					Stream.Printf(SG_T("%f")	, pRecord->asDouble(iField));
					break;
				}

				Stream.Printf(iField < pTable->Get_Field_Count() - 1 ? Separator.c_str() : SG_T("\n"));
			}
		}

		//-------------------------------------------------
		Stream.Close();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import::CTable_Text_Import(void)
{
	Set_Name		(_TL("Import Text Table"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("File contains headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("space"),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CTable_Text_Import::On_Execute(void)
{
	bool		bHeader;
	CSG_String	Separator;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")		->asTable();
	bHeader	= Parameters("HEADLINE")	->asBool();

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	case 3:		Separator	=  " ";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !pTable->Create(Parameters("FILENAME")->asString(), bHeader ? TABLE_FILETYPE_Text : TABLE_FILETYPE_Text_NoHeadLine, Separator) )
	{
		Message_Add(_TL("table could not be opened."));
	}

	//-----------------------------------------------------
	else
	{
		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import_Numbers::CTable_Text_Import_Numbers(void)
{
	Set_Name		(_TL("Import Text Table with Numbers only"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("File contains headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("tabulator"),
			_TL(";"),
			_TL(","),
			_TL("other")
		), 0
	);

	Parameters.Add_String(
		NULL	, "SEP_OTHER"	, _TL("Separator (other)"),
		_TL(""),
		SG_T("*")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("CSV Files (*.csv)")	, SG_T("*.csv"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);
}

//---------------------------------------------------------
bool CTable_Text_Import_Numbers::On_Execute(void)
{
	bool		bHeader;
	double		Value;
	CSG_String	sLine, sHead, Separator;
	CSG_File	Stream;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")		->asTable();
	bHeader	= Parameters("HEADLINE")	->asBool();

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:		Separator	= "\t";	break;
	case 1:		Separator	=  ";";	break;
	case 2:		Separator	=  ",";	break;
	default:	Separator	= Parameters("SEP_OTHER")->asString();	break;
	}

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, false) )
	{
		Message_Add(_TL("file could not be opened"));
	}

	else if( !Stream.Read_Line(sLine) || sLine.Length() == 0 )
	{
		Message_Add(_TL("empty or corrupted file"));
	}

	else if( bHeader && (!Stream.Read_Line(sLine) || sLine.Length() == 0) )
	{
		Message_Add(_TL("empty or corrupted file"));
	}

	//-----------------------------------------------------
	else
	{
		pTable->Destroy();
		pTable->Set_Name(SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

		sLine.Replace(Separator, SG_T("\t"));
		sHead	= sLine;

		do
		{
			sHead.Trim();

			if( sHead.asDouble(Value) )
			{
				pTable->Add_Field(CSG_String::Format(SG_T("%d"), 1 + pTable->Get_Field_Count()), SG_DATATYPE_Double);
			}

			sHead	= sHead.AfterFirst('\t');
		}
		while( sHead.Length() > 0 );

		//-------------------------------------------------
		if( pTable->Get_Field_Count() > 0 )
		{
			int		fLength	= Stream.Length();

			do
			{
				sLine.Replace(Separator, SG_T("\t"));

				CSG_Table_Record	*pRecord	= pTable->Add_Record();

				for(int i=0; i<pTable->Get_Field_Count(); i++)
				{
					sLine.Trim();

					pRecord->Set_Value(i, sLine.asDouble());

					sLine	= sLine.AfterFirst('\t');
				}
			}
			while( Stream.Read_Line(sLine) && sLine.Length() > 0 && Set_Progress(Stream.Tell(), fLength) );
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Import_Fixed_Cols::CTable_Text_Import_Fixed_Cols(void)
{
	Set_Name		(_TL("Import Text Table (Fixed Column Sizes)"));

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "HEADLINE"	, _TL("File contains headline"),
		_TL(""),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "FIELDDEF"	, _TL("Field Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("mark breaks in first line"),
			_TL("specify fields with type")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "NFIELDS"		, _TL("Numver of Fields"),
		_TL(""),
		PARAMETER_TYPE_Int		, 1, 1, true
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("Text Files (*.txt)")	, SG_T("*.txt"),
			_TL("All Files")			, SG_T("*.*")
		), NULL, false
	);

	Add_Parameters("BREAKS", _TL("Breaks"), _TL(""));
	Add_Parameters("FIELDS", _TL("Fields"), _TL(""));
}

//---------------------------------------------------------
bool CTable_Text_Import_Fixed_Cols::On_Execute(void)
{
	bool			bHeader;
	int				i, nChars, iField, nFields, *iFirst, *iLength;
	CSG_String		sLine;
	CSG_File		Stream;
	CSG_Table		*pTable;

	//-----------------------------------------------------
	pTable	= Parameters("TABLE")		->asTable();
	bHeader	= Parameters("HEADLINE")	->asBool();

	//-----------------------------------------------------
	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, true) )
	{
		Message_Add(_TL("file could not be opened"));

		return( false );
	}

	if( !Stream.Read_Line(sLine) || (nChars = sLine.Length()) <= 0 )
	{
		Message_Add(_TL("empty or corrupted file"));

		return( false );
	}

	//-----------------------------------------------------
	pTable->Destroy();
	pTable->Set_Name(SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

	//-----------------------------------------------------
	if( Parameters("FIELDDEF")->asInt() == 0 )
	{
		CSG_Parameters	*pBreaks	= Get_Parameters("BREAKS");

		pBreaks->Del_Parameters();

		for(i=0; i<nChars; i++)
		{
			pBreaks->Add_Value(NULL,
				CSG_String::Format(SG_T("%03d"), i),
				CSG_String::Format(SG_T("%03d %c"), i + 1, sLine[i]),
				_TL(""), PARAMETER_TYPE_Bool, false
			);
		}

		if( !Dlg_Parameters("BREAKS") )
		{
			return( false );
		}

		//-------------------------------------------------
		for(i=0, nFields=1; i<pBreaks->Get_Count(); i++)
		{
			if( pBreaks->Get_Parameter(i)->asBool() )
			{
				nFields++;
			}
		}

		//-------------------------------------------------
		iFirst		= new int[nFields];
		iLength		= new int[nFields];

		iFirst[0]	= 0;

		for(i=0, iField=1; i<pBreaks->Get_Count() && iField<nFields; i++)
		{
			if( pBreaks->Get_Parameter(i)->asBool() )
			{
				iFirst[iField++]	= i + 1;
			}
		}

		//-------------------------------------------------
		for(iField=0; iField<nFields; iField++)
		{
			iLength[iField]	= (iField < nFields - 1 ? iFirst[iField + 1] : sLine.Length()) - iFirst[iField];

			pTable->Add_Field(bHeader ? sLine.Mid(iFirst[iField], iLength[iField]) : CSG_String::Format(SG_T("FIELD%03d"), iField + 1), SG_DATATYPE_String);
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Parameters	*pFields	= Get_Parameters("FIELDS");

		pFields->Del_Parameters();

		nFields	= Parameters("NFIELDS")->asInt();

		for(iField=0; iField<nFields; iField++)
		{
			CSG_String		s		= CSG_String::Format(SG_T("%03d"), iField);
			CSG_Parameter	*pNode	= pFields->Add_Node(NULL, SG_T("NODE") + s, _TL("Field") + s, _TL(""));
			pFields->Add_Value	(pNode, SG_T("LENGTH") + s, _TL("Length"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
		//	pFields->Add_Value	(pNode, SG_T("IMPORT") + s, _TL("Import"), _TL(""), PARAMETER_TYPE_Bool, true);
			pFields->Add_Choice	(pNode, SG_T("TYPE")   + s, _TL("Type")  , _TL(""), CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
				_TL("text"),
				_TL("2 byte integer"),
				_TL("4 byte integer"),
				_TL("4 byte float"),
				_TL("8 byte float"))
			);
		}

		if( !Dlg_Parameters("FIELDS") )
		{
			return( false );
		}

		//-------------------------------------------------
		iFirst		= new int[nFields];
		iLength		= new int[nFields];

		iFirst[0]	= 0;

		for(iField=0, i=0; iField<nFields && i<nChars; iField++)
		{
			CSG_String		s		= CSG_String::Format(SG_T("%03d"), iField);

			iFirst [iField]	= i;
			iLength[iField]	= pFields->Get_Parameter(SG_T("LENGTH") + s)->asInt();

			i	+= iLength[iField];

			CSG_String		Name	= bHeader ? sLine.Mid(iFirst[iField], iLength[iField]) : CSG_String::Format(SG_T("FIELD%03d"), iField + 1);

			switch( pFields->Get_Parameter(SG_T("TYPE") + s)->asInt() )
			{
			default:
			case 0:	pTable->Add_Field(Name, SG_DATATYPE_String);	break;
			case 1:	pTable->Add_Field(Name, SG_DATATYPE_Short);		break;
			case 2:	pTable->Add_Field(Name, SG_DATATYPE_Int);		break;
			case 3:	pTable->Add_Field(Name, SG_DATATYPE_Float);		break;
			case 4:	pTable->Add_Field(Name, SG_DATATYPE_Double);	break;
			}
		}
	}

	//-----------------------------------------------------
	if( bHeader )
	{
		Stream.Read_Line(sLine);
	}

	//-----------------------------------------------------
	int		fLength	= Stream.Length();

	do
	{
		if( sLine.Length() == nChars )
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			for(iField=0; iField<nFields; iField++)
			{
				pRecord->Set_Value(iField, sLine.Mid(iFirst[iField], iLength[iField]));
			}
		}
	}
	while( Stream.Read_Line(sLine) && Set_Progress(Stream.Tell(), fLength) );

	//-----------------------------------------------------
	delete[](iFirst);
	delete[](iLength);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
