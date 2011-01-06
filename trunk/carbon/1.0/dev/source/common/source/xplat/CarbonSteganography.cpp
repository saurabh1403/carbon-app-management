

#include "CarbonSteganography.h"

//All third party code. Slightly modified. Lots of improvement needed. poorly written code.
char ExtractChar( BMP& Image, int i, int j)
{
	RGBApixel Pixel1 = *Image(i,j);
	i++; 
	if( i == Image.TellWidth() ){ i=0; j++; }

	RGBApixel Pixel2 = *Image(i,j);

	// convert the two pixels to a character

	unsigned int T = 0;
	T += (Pixel1.Red%2);
	T += (2* (Pixel1.Green%2));
	T += (4* (Pixel1.Blue%2));
	T += (8* (Pixel1.Alpha%2));

	T += (16*  (Pixel2.Red%2));
	T += (32*  (Pixel2.Green%2));
	T += (64*  (Pixel2.Blue%2));
	T += (128* (Pixel2.Alpha%2));

	char c = (char) T;
	return c;
}

class EasyBMPstegoInternalHeader
{
public:

	EasyBMPstegoInternalHeader()
	{
		FileName = NULL;
	}

	~EasyBMPstegoInternalHeader()
	{
		if(FileName)
			delete FileName;
		FileName = NULL;
	}

	char* FileName;
	int FileNameSize;

	int FileSize;

	unsigned char* CharsToEncode;
	int NumberOfCharsToEncode;

	void InitializeFromFile( const char* input , int BMPwidth, int BMPheight );
	void InitializeFromImage( BMP& Input );
	void InitializeFromFileData( const char* fileData, int fileDataSize, int BMPwidth, int BMPheight );
};

void EasyBMPstegoInternalHeader::InitializeFromImage( BMP& Image )
{ 
	if( Image.TellWidth()*Image.TellHeight() < 2*(12+2+4) )
	{
		CharsToEncode = NULL;
		NumberOfCharsToEncode = 0;
		FileName = NULL;
		FileNameSize = 0;
		FileSize = 0;
		return; 
	}

	// extract the first few purported characters to see if this thing
	// has hidden data

	//*****dont change this constant string... if to be changed, change the next initialization of ComparisonString char array variable also
	char* StegoIdentifierString = "EasyBMPstego";
	//ugly coding
	//	char ComparisonString [strlen(StegoIdentifierString)+1];
	char ComparisonString[13];

	int i=0; 
	int j=0;
	int k=0;

	while( k < strlen(StegoIdentifierString) )
	{
		ComparisonString[k] = ExtractChar( Image, i, j);
		i += 2;
		while( i >= Image.TellWidth() )
		{ i -= Image.TellWidth(); j++; }
		k++;
	} 
	ComparisonString[k] = '\0';

	if( strcmp( StegoIdentifierString , ComparisonString ) )
	{
		cout << "Error: No (compatible) hidden data found in image!\n";
		FileSize = 0;
		FileNameSize = 0;
		return;
	}

	// get the next two characters to determine file size 
	unsigned char C1 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }
	unsigned char C2 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }

	FileNameSize = C1 + 256*C2;
	FileName = new char[ FileNameSize+1];

	// read the filename

	k=0;
	while( k < FileNameSize )
	{
		FileName[k] = ExtractChar( Image, i, j);
		i += 2;
		while( i >= Image.TellWidth() )
		{ i -= Image.TellWidth(); j++; }
		k++;
	} 
	FileName[k] = '\0';

	// find the actual data size

	C1 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }
	C2 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }
	unsigned char C3 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }
	unsigned char C4 = (unsigned char) ExtractChar( Image, i,j );
	i += 2;
	while( i >= Image.TellWidth() )
	{ i -= Image.TellWidth(); j++; }

	FileSize = C1 + 256*C2 + 65536*C3 + 16777216*C4;
	NumberOfCharsToEncode = FileNameSize + 2 + 4 
		+ strlen(StegoIdentifierString);
	return;
}

void EasyBMPstegoInternalHeader::InitializeFromFile( const char* input, int BMPwidth, int BMPheight )
{
	FileNameSize = strlen( input ) +1;
	FileName = new char [FileNameSize];
	strcpy( FileName , input );
	FileNameSize--;

	int SPACE = 32; 

	char* StegoIdentifierString = "EasyBMPstego";

	NumberOfCharsToEncode = FileNameSize 
		+ strlen(StegoIdentifierString) 
		+ 2 // indicate length of filename
		+ 4; // indicate length of data 

	int MaxCharsToEncode = (int) ( 0.5 * BMPwidth * BMPheight );
	if( NumberOfCharsToEncode >  MaxCharsToEncode )
	{
		cout << "Error: File is too small to even encode file information!\n"
			<< "       Terminating encoding.\n";
		FileSize = 0;	 
		CharsToEncode = NULL;
		NumberOfCharsToEncode = 0;
		return;
	} 

	CharsToEncode = new unsigned char [NumberOfCharsToEncode];

	FILE* fp;
	fp = fopen( FileName , "rb" );
	if( !fp )
	{ 
		FileSize = 0;

		return;
	}

	// determine the number of actual data bytes to encode

	FileSize = 0;
	while( !feof( fp ) )
	{
		char c;
		fread( &c, 1, 1, fp );
		FileSize++;
	}
	FileSize--;

	MaxCharsToEncode -= NumberOfCharsToEncode;
	if( FileSize > MaxCharsToEncode )
	{
		FileSize = MaxCharsToEncode; 
		cout << "Warning: Input file exceeds encoding capacity of the image\n"
			<< "         File will be truncated.\n";
	}
	fclose( fp );

	// create this "file header" string

	int k = 0;

	// this part gives the length of the filename
	while( k < strlen(StegoIdentifierString) )
	{ CharsToEncode[k] = StegoIdentifierString[k]; k++; }
	int TempInt = FileNameSize % 256;
	CharsToEncode[k] = (unsigned char) TempInt; k++;
	TempInt = FileNameSize - TempInt;
	if( TempInt < 0 ){ TempInt = 0; }
	TempInt = TempInt / 256; 
	CharsToEncode[k] = (unsigned char) TempInt; k++;

	// this part hides the filename
	int j=0;
	while( j < FileNameSize ) 
	{ CharsToEncode[k] = FileName[j]; k++; j++; }

	// this part gives the length of the hidden data
	int TempIntOriginal = FileSize;
	TempInt = FileSize % 256;
	CharsToEncode[k] = (unsigned char) TempInt; k++;
	TempIntOriginal -= TempInt;

	if( TempIntOriginal > 0 )
	{
		TempInt = TempIntOriginal % 65536; 
		CharsToEncode[k] = (unsigned char) (TempInt/256); k++;
		TempIntOriginal -= TempInt*256;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	if( TempIntOriginal > 0 )
	{ 
		TempInt = TempIntOriginal % 16777216; 
		CharsToEncode[k] = (unsigned char) (TempInt/65536); k++;
		TempIntOriginal -= TempInt*65536;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	if( TempIntOriginal > 0 )
	{
		TempInt = TempIntOriginal % 4294967296; 
		CharsToEncode[k] = (unsigned char) (TempInt/1677216); k++;
		TempIntOriginal -= TempInt*16777216;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	return; 
}


void EasyBMPstegoInternalHeader::InitializeFromFileData( const char* fileData, int fileDataSize, int BMPwidth, int BMPheight )
{
	const char *const_file_name = "carbonInfo";
	FileName = new char[strlen(const_file_name) + 1];
	FileNameSize = strlen(const_file_name) +1;
//	FileName = new char [FileNameSize];
	strcpy( FileName , const_file_name);
	FileNameSize--;

	int SPACE = 32;

	char* StegoIdentifierString = "EasyBMPstego";

	NumberOfCharsToEncode = FileNameSize 
		+ strlen(StegoIdentifierString) 
		+ 2 // indicate length of filename
		+ 4; // indicate length of data 

	int MaxCharsToEncode = (int) ( 0.5 * BMPwidth * BMPheight );
	if( NumberOfCharsToEncode >  MaxCharsToEncode )
	{
		cout << "Error: File is too small to even encode file information!\n"
			<< "       Terminating encoding.\n";
		FileSize = 0;	 
		CharsToEncode = NULL;
		NumberOfCharsToEncode = 0;
		return;
	} 

	CharsToEncode = new unsigned char [NumberOfCharsToEncode];

	//FILE* fp;
	//fp = fopen( FileName , "rb" );
	if( !fileData )
	{ 
		FileSize = 0;
		return;
	}

	// determine the number of actual data bytes to encode

	//FileSize = 0;
	//while( !feof( fp ) )
	//{
	//	char c;
	//	fread( &c, 1, 1, fp );
	//	FileSize++;
	//}

	FileSize = fileDataSize;
//	FileSize--;

	MaxCharsToEncode -= NumberOfCharsToEncode;
	if( FileSize > MaxCharsToEncode )
	{
		FileSize = MaxCharsToEncode; 
		cout << "Warning: Input file exceeds encoding capacity of the image\n"
			<< "         File will be truncated.\n";
	}

	//	fclose( fp );

	// create this "file header" string

	int k = 0;

	// this part gives the length of the filename
	while( k < strlen(StegoIdentifierString) )
	{ CharsToEncode[k] = StegoIdentifierString[k]; k++; }
	int TempInt = FileNameSize % 256;
	CharsToEncode[k] = (unsigned char) TempInt; k++;
	TempInt = FileNameSize - TempInt;
	if( TempInt < 0 ){ TempInt = 0; }
	TempInt = TempInt / 256; 
	CharsToEncode[k] = (unsigned char) TempInt; k++;

	// this part hides the filename
	int j=0;
	while( j < FileNameSize ) 
	{ CharsToEncode[k] = FileName[j]; k++; j++; }

	// this part gives the length of the hidden data
	int TempIntOriginal = FileSize;
	TempInt = FileSize % 256;
	CharsToEncode[k] = (unsigned char) TempInt; k++;
	TempIntOriginal -= TempInt;

	if( TempIntOriginal > 0 )
	{
		TempInt = TempIntOriginal % 65536; 
		CharsToEncode[k] = (unsigned char) (TempInt/256); k++;
		TempIntOriginal -= TempInt*256;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	if( TempIntOriginal > 0 )
	{ 
		TempInt = TempIntOriginal % 16777216; 
		CharsToEncode[k] = (unsigned char) (TempInt/65536); k++;
		TempIntOriginal -= TempInt*65536;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	if( TempIntOriginal > 0 )
	{
		TempInt = TempIntOriginal % 4294967296; 
		CharsToEncode[k] = (unsigned char) (TempInt/1677216); k++;
		TempIntOriginal -= TempInt*16777216;
	}
	else
	{ CharsToEncode[k] = 0; k++; }

	return; 
}


namespace carbonStegano
{
	bool hideFileInBmp(const std::string &inFilePath, const std::string &inBmpFilePath, const std::string &outBmpFilePath)
	{ 

		BMP Image;
		Image.ReadFromFile( inBmpFilePath.c_str());
		int MaxNumberOfPixels = Image.TellWidth() * Image.TellHeight() - 2;
		int k=1;

		// set image to 32 bpp

		Image.SetBitDepth( 32 );

		// open the input text file

		FILE* fp;
		fp = fopen( inFilePath.c_str() , "rb" );
		if( !fp )
		{
			//   cout << "Error: unable to read file " << argv[2] << " for text input!" << endl;
			return false; 
		}

		// figure out what we need to encode as an internal header

		EasyBMPstegoInternalHeader IH;
		IH.InitializeFromFile( inFilePath.c_str() , Image.TellWidth() , Image.TellHeight() );
		if( IH.FileNameSize == 0 || IH.NumberOfCharsToEncode == 0 )
		{ return false; }

		// write the internal header;
		k=0;
		int i=0;
		int j=0;
		while( !feof( fp ) && k < IH.NumberOfCharsToEncode )
		{
			// decompose the character 

			unsigned int T = (unsigned int) IH.CharsToEncode[k];

			int R1 = T % 2;
			T = (T-R1)/2;
			int G1 = T % 2;
			T = (T-G1)/2;
			int B1 = T % 2;
			T = (T-B1)/2;
			int A1 = T % 2;
			T = (T-A1)/2;

			int R2 = T % 2;
			T = (T-R2)/2;
			int G2 = T % 2;
			T = (T-G2)/2;
			int B2 = T % 2;
			T = (T-B2)/2;
			int A2 = T % 2;
			T = (T-A2)/2;

			RGBApixel Pixel1 = *Image(i,j);
			Pixel1.Red += ( -Pixel1.Red%2 + R1 );
			Pixel1.Green += ( -Pixel1.Green%2 + G1 );
			Pixel1.Blue += ( -Pixel1.Blue%2 + B1 );
			Pixel1.Alpha += ( -Pixel1.Alpha%2 + A1 );
			*Image(i,j) = Pixel1;

			i++; 
			if( i== Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			Pixel2.Red += ( -Pixel2.Red%2 + R2 );
			Pixel2.Green += ( -Pixel2.Green%2 + G2 );
			Pixel2.Blue += ( -Pixel2.Blue%2 + B2 );
			Pixel2.Alpha += ( -Pixel2.Alpha%2 + A2 );
			*Image(i,j) = Pixel2;

			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }
		}

		// encode the actual data 
		k=0;
		while( !feof( fp ) && k < 2*IH.FileSize )
		{
			char c;
			fread( &c , 1, 1, fp );

			// decompose the character 

			unsigned int T = (unsigned int) c;

			int R1 = T % 2;
			T = (T-R1)/2;
			int G1 = T % 2;
			T = (T-G1)/2;
			int B1 = T % 2;
			T = (T-B1)/2;
			int A1 = T % 2;
			T = (T-A1)/2;

			int R2 = T % 2;
			T = (T-R2)/2;
			int G2 = T % 2;
			T = (T-G2)/2;
			int B2 = T % 2;
			T = (T-B2)/2;
			int A2 = T % 2;
			T = (T-A2)/2;

			RGBApixel Pixel1 = *Image(i,j);
			Pixel1.Red += ( -Pixel1.Red%2 + R1 );
			Pixel1.Green += ( -Pixel1.Green%2 + G1 );
			Pixel1.Blue += ( -Pixel1.Blue%2 + B1 );
			Pixel1.Alpha += ( -Pixel1.Alpha%2 + A1 );
			*Image(i,j) = Pixel1;

			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			Pixel2.Red += ( -Pixel2.Red%2 + R2 );
			Pixel2.Green += ( -Pixel2.Green%2 + G2 );
			Pixel2.Blue += ( -Pixel2.Blue%2 + B2 );
			Pixel2.Alpha += ( -Pixel2.Alpha%2 + A2 );
			*Image(i,j) = Pixel2;


			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }
		}

		fclose( fp );
		Image.WriteToFile( outBmpFilePath.c_str());

		return true;

	}



	bool retriveFileFromBmp(const std::string &inBmpFilePath, const std::string &outFilePath)
	{ 
		BMP Image;

		Image.ReadFromFile(inBmpFilePath.c_str());
		if( Image.TellBitDepth() != 32 )
		{
			cout << "Error: File " <<inBmpFilePath << " not encoded with this program." << endl;
			return false;
		}

		EasyBMPstegoInternalHeader IH;
		IH.InitializeFromImage( Image );
		if( IH.FileSize == 0 || IH.FileNameSize == 0 || IH.NumberOfCharsToEncode == 0 )
		{
			cout << "No hiddent data detected. Exiting ... " << endl;
			return false;
		}

		cout << "Hidden data detected! Outputting to file " << IH.FileName << " ... " << endl;

		FILE* fp;
		fp = fopen( outFilePath.c_str() , "wb" );
		if( !fp )
		{
			cout << "Error: Unable to open file " << IH.FileName << " for output!\n";
			return false;
		}

		int MaxNumberOfPixels = Image.TellWidth() * Image.TellHeight();

		int k=0;
		int i=0;
		int j=0;

		// set the starting pixel to skip the internal header
		i = 2*IH.NumberOfCharsToEncode;
		while( i >= Image.TellWidth() )
		{
			i -= Image.TellWidth(); j++;
		}

		while( k < 2*IH.FileSize )
		{
			// read the two pixels

			RGBApixel Pixel1 = *Image(i,j);
			i++; k++;
			if( i == Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			i++; k++;
			if( i == Image.TellWidth() ){ i=0; j++; }

			// convert the two pixels to a character

			unsigned int T = 0;
			T += (Pixel1.Red%2);
			T += (2* (Pixel1.Green%2));
			T += (4* (Pixel1.Blue%2));
			T += (8* (Pixel1.Alpha%2));

			T += (16*  (Pixel2.Red%2));
			T += (32*  (Pixel2.Green%2));
			T += (64*  (Pixel2.Blue%2));
			T += (128* (Pixel2.Alpha%2));

			char c = (char) T;

			fwrite( &c , 1 , 1 , fp ); 
		}
		fclose( fp ); 
		return true;
	}


	bool retriveFileFromBmpStream(const std::string &inBmpFileDataStream, const std::string &outFilePath){ return true;}

	bool retrieveDataFromBmpStream(const std::string &inBmpFileDatastream, const std::string &outData){return true; }

	bool retrieveDataFromBmp(const std::string &inBmpFilePath, std::string &outData)
	{ 
		BMP Image;

		Image.ReadFromFile(inBmpFilePath.c_str());
		if( Image.TellBitDepth() != 32 )
		{
			cout << "Error: File " <<inBmpFilePath << " not encoded with this program." << endl;
			return false;
		}

		EasyBMPstegoInternalHeader IH;
		IH.InitializeFromImage( Image );
		if( IH.FileSize == 0 || IH.FileNameSize == 0 || IH.NumberOfCharsToEncode == 0 )
		{
			cout << "No hiddent data detected. Exiting ... " << endl;
			return false;
		}

		cout << "Hidden data detected! Outputting to file " << IH.FileName << " ... " << endl;

		//FILE* fp;
		//fp = fopen( outFilePath.c_str() , "wb" );
		//if( !fp )
		//{
		//	cout << "Error: Unable to open file " << IH.FileName << " for output!\n";
		//	return false;
		//}

		int MaxNumberOfPixels = Image.TellWidth() * Image.TellHeight();

		int k=0;
		int i=0;
		int j=0;

		//init the array of chars

		char * hiddenData = new char[IH.FileSize+1];

		// set the starting pixel to skip the internal header
		i = 2*IH.NumberOfCharsToEncode;
		while( i >= Image.TellWidth() )
		{
			i -= Image.TellWidth(); j++;
		}

		int charCount = 0;

		while( k < 2*IH.FileSize )
		{
			// read the two pixels

			RGBApixel Pixel1 = *Image(i,j);
			i++; k++;
			if( i == Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			i++; k++;
			if( i == Image.TellWidth() ){ i=0; j++; }

			// convert the two pixels to a character

			unsigned int T = 0;
			T += (Pixel1.Red%2);
			T += (2* (Pixel1.Green%2));
			T += (4* (Pixel1.Blue%2));
			T += (8* (Pixel1.Alpha%2));

			T += (16*  (Pixel2.Red%2));
			T += (32*  (Pixel2.Green%2));
			T += (64*  (Pixel2.Blue%2));
			T += (128* (Pixel2.Alpha%2));

			char c = (char) T;
			hiddenData[charCount] = c;
			charCount++;

			//			fwrite( &c , 1 , 1 , fp ); 
		}

		hiddenData[charCount] = '\0';
		//		fclose( fp ); 
		outData = hiddenData;

		return true;
	}

	bool hideDataInBmp(const std::string &inData, const std::string &inBmpFilePath, const std::string &outBmpFilePath)
	{ 

		BMP Image;
		Image.ReadFromFile( inBmpFilePath.c_str());
		int MaxNumberOfPixels = Image.TellWidth() * Image.TellHeight() - 2;
		int k=1;

		// set image to 32 bpp

		Image.SetBitDepth( 32 );

		// open the input text file

		//FILE* fp;
		//fp = fopen( inFilePath.c_str() , "rb" );
		//if( !fp )
		//{
		//	//   cout << "Error: unable to read file " << argv[2] << " for text input!" << endl;
		//	return false; 
		//}

		// figure out what we need to encode as an internal header

		EasyBMPstegoInternalHeader IH;
		IH.InitializeFromFileData(inData.c_str(), inData.length(), Image.TellWidth() , Image.TellHeight());
//		IH.InitializeFromFile( inFilePath.c_str() , Image.TellWidth() , Image.TellHeight() );
		if( IH.FileNameSize == 0 || IH.NumberOfCharsToEncode == 0 )
		{ return false; }

		// write the internal header;
		k=0;
		int i=0;
		int j=0;
		while( !inData.empty() && k < IH.NumberOfCharsToEncode )
		{
			// decompose the character 

			unsigned int T = (unsigned int) IH.CharsToEncode[k];

			int R1 = T % 2;
			T = (T-R1)/2;
			int G1 = T % 2;
			T = (T-G1)/2;
			int B1 = T % 2;
			T = (T-B1)/2;
			int A1 = T % 2;
			T = (T-A1)/2;

			int R2 = T % 2;
			T = (T-R2)/2;
			int G2 = T % 2;
			T = (T-G2)/2;
			int B2 = T % 2;
			T = (T-B2)/2;
			int A2 = T % 2;
			T = (T-A2)/2;

			RGBApixel Pixel1 = *Image(i,j);
			Pixel1.Red += ( -Pixel1.Red%2 + R1 );
			Pixel1.Green += ( -Pixel1.Green%2 + G1 );
			Pixel1.Blue += ( -Pixel1.Blue%2 + B1 );
			Pixel1.Alpha += ( -Pixel1.Alpha%2 + A1 );
			*Image(i,j) = Pixel1;

			i++; 
			if( i== Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			Pixel2.Red += ( -Pixel2.Red%2 + R2 );
			Pixel2.Green += ( -Pixel2.Green%2 + G2 );
			Pixel2.Blue += ( -Pixel2.Blue%2 + B2 );
			Pixel2.Alpha += ( -Pixel2.Alpha%2 + A2 );
			*Image(i,j) = Pixel2;

			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }
		}

		// encode the actual data 
		k=0;
		int charCount = 0;
		while( !inData.empty() && k < 2*IH.FileSize )
		{
			char c;
//			fread( &c , 1, 1, fp );
			c = inData[charCount];
			charCount++;

			// decompose the character 

			unsigned int T = (unsigned int) c;

			int R1 = T % 2;
			T = (T-R1)/2;
			int G1 = T % 2;
			T = (T-G1)/2;
			int B1 = T % 2;
			T = (T-B1)/2;
			int A1 = T % 2;
			T = (T-A1)/2;

			int R2 = T % 2;
			T = (T-R2)/2;
			int G2 = T % 2;
			T = (T-G2)/2;
			int B2 = T % 2;
			T = (T-B2)/2;
			int A2 = T % 2;
			T = (T-A2)/2;

			RGBApixel Pixel1 = *Image(i,j);
			Pixel1.Red += ( -Pixel1.Red%2 + R1 );
			Pixel1.Green += ( -Pixel1.Green%2 + G1 );
			Pixel1.Blue += ( -Pixel1.Blue%2 + B1 );
			Pixel1.Alpha += ( -Pixel1.Alpha%2 + A1 );
			*Image(i,j) = Pixel1;

			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }

			RGBApixel Pixel2 = *Image(i,j);
			Pixel2.Red += ( -Pixel2.Red%2 + R2 );
			Pixel2.Green += ( -Pixel2.Green%2 + G2 );
			Pixel2.Blue += ( -Pixel2.Blue%2 + B2 );
			Pixel2.Alpha += ( -Pixel2.Alpha%2 + A2 );
			*Image(i,j) = Pixel2;


			i++; k++;
			if( i== Image.TellWidth() ){ i=0; j++; }
		}

//		fclose( fp );
		Image.WriteToFile( outBmpFilePath.c_str());

		return true;

	}




}