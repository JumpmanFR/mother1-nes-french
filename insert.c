// you are a hot dog

// i hope you like messy code

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const int WRITE_FLAG_NORMAL = 0;
const int WRITE_FLAG_PREFILL = 1;
const int WRITE_FLAG_AFTER_PREFILL = 2;

struct tableEntry
{
	int  hexVal;
	char str[500];
};

struct romArea
{
	int  address;
	int  size;
	int  flag;
	const char* comment;
	int  isAllowed;
	int  isReported;
};

int        tableLen = 0;
struct 	   tableEntry table[500];

void 		  LoadTable(void);
void          PrepString(char[], char[], int);
unsigned char ConvChar(unsigned char);
void          ConvComplexString(char[], int&);
void          CompileCC(char[], int&, unsigned char[], int&);
int           CharToHex(char);
int  		  hstrtoi(char*);
void          StartWritingInRom(int address, int flag, const char * comment);
void          WriteInRom(int character);
void          WriteReport();
int           PostProcessWriteAreas();
void          InsertMainStuff(void);
void          InsertNames();
void          InsertSpecialText(void);
void          InsertAltWindowData(void);
void          InsertItemArticles(void);
void          InsertEnemyArticles(void);
void          InsertItemClasses(void);
void          InsertEnemyClasses(void);
void          InsertEnemyLongNames(void);

void          ConvComplexMenuString(char[], int&);

//void          UpdatePointers(int, int, FILE*, char*);
//void          InsertEnemies(FILE*);
//void          InsertMenuStuff1(FILE*);
//void          InsertStuff(FILE*, char[], int, int, int);
//void          InsertMenuStuff2(FILE*);

int quoteCount = 0;

struct romArea writtenAreas[10000];
int writtenAreasCount = 0;
int currentWrittenArea = 0;

FILE* romStream;

//=================================================================================================

int main(int argc, char *argv[])
{
	romStream = fopen("test.nes", "rb+");
	if (romStream == NULL)
	{
		printf("Can't open test.nes\n");
		fclose(romStream);
		return -1;
	}
	
	printf("\r\n MOTHER 1 STUFF\r\n");
	printf("=====================================\r\n");
	LoadTable();
	InsertMainStuff();
	InsertSpecialText();
	InsertNames();
	//InsertAltWindowData();
	//InsertItemArticles();
    //InsertEnemyArticles();
    //InsertItemClasses();
    //InsertEnemyClasses();

	fclose(romStream);

    printf("\r\nDone!\r\n");
	
	int countReportedAreas = PostProcessWriteAreas();
	printf("\r\n%d areas written\r\n", countReportedAreas);
	
	if (argc > 1) {
		WriteReport();
	}

	return 0;
}

//=================================================================================================

void ConvComplexString(char str[5000], int& newLen)
{
	char          newStr[5000] = "";
	unsigned char newStream[100];
	int           streamLen =  0;
	int           len = strlen(str) - 1; // minus one to take out the newline
	int           counter = 0;
	int           i;

	newLen = 0;

    quoteCount = 0;

    while (counter < len)
    {
		//printf("%c", str[counter]);
		if (str[counter] == '[')
		{
		   CompileCC(str, counter, newStream, streamLen);
		   for (i = 0; i < streamLen; i++)
		   {
			   newStr[newLen] = newStream[i];
			   newLen++;
		   }
		   counter++; // to skip past the ]
		}
		else
		{
		   newStr[newLen] = ConvChar(str[counter]);
		   newLen++;

		   counter++;
		}
	}

    for (i = 0; i < 5000; i++)
       str[i] = '\0';
	for (i = 0; i < newLen; i++)
	   str[i] = newStr[i];
}

void StartWritingInRom(int address, int flag, const char * comment) {
	int i;
	int foundArea = 0;
	for (i = 0; i < writtenAreasCount; i++) {
		if (writtenAreas[i].address + writtenAreas[i].size == address
		&& strcmp(writtenAreas[i].comment, comment) == 0 && writtenAreas[i].flag == flag) {
			currentWrittenArea = i;
			foundArea = 1;
			break;
		}
	}
	if (!foundArea) {
		currentWrittenArea = writtenAreasCount;
		writtenAreas[currentWrittenArea].address = address;
		writtenAreas[currentWrittenArea].size = 0;
		writtenAreas[currentWrittenArea].comment = comment;
		writtenAreas[currentWrittenArea].flag = flag;
		writtenAreasCount++;
	}
		
	fseek(romStream, address, SEEK_SET);
}

void WriteInRom(int character) {
	writtenAreas[currentWrittenArea].size++;
	fputc(character, romStream);
}

int PostProcessWriteAreas() {
	int countReportedAreas = 0;
	int i;
	for (i = 0; i < writtenAreasCount; i++) {		
		writtenAreas[i].isReported = 1;
		if (writtenAreas[i].flag == WRITE_FLAG_AFTER_PREFILL) {
			int j;
			for (j = 0; j < writtenAreasCount; j++) {
				if (writtenAreas[j].flag == WRITE_FLAG_PREFILL
				&& writtenAreas[j].address <= writtenAreas[i].address
				&& writtenAreas[j].address + writtenAreas[j].size >= writtenAreas[i].address + writtenAreas[i].size) {
					writtenAreas[i].isAllowed = 1;
					writtenAreas[i].isReported = 0;
					break;
				}
			}
		} else {
			writtenAreas[i].isAllowed = 1;
		}
		countReportedAreas += writtenAreas[i].isReported;
	}
	return countReportedAreas;
}

void WriteReport() {
	FILE* reportStream;
	reportStream = fopen("insert_report.txt", "w+");
	
	int i;
	for (i = 0; i < writtenAreasCount; i++) {
		if (writtenAreas[i].isReported) {
			const char * warningStr = "";
			if (!writtenAreas[i].isAllowed) {
				warningStr = " FORBIDDEN";
			}
			fprintf(reportStream, "org $%05X; fill $%02X // %s%s\n", writtenAreas[i].address, writtenAreas[i].size, warningStr, writtenAreas[i].comment); 
		}
	}
	fclose(reportStream);
}

//=================================================================================================

void CompileCC(char str[5000], int& strLoc, unsigned char newStream[100], int& streamLen)
{
   char  str2[5000] = "";
   char* ptr[5000];
   int   ptrCount = 0;
   int   totalLength = strlen(str);
   int   i;
   int   j;
   FILE* fin;
   char  hexVal[100] = "";
   char  specialStr[100] = "";
   int   retVal = 0;


   // we're gonna mess with the original string, so make a backup for later
   strcpy(str2, str);

   // first we gotta parse the codes, what a pain
   ptr[ptrCount++] = &str[strLoc + 1];
   while (str[strLoc] != ']' && strLoc < totalLength)
   {
      if (str[strLoc] == ' ')
      {
         ptr[ptrCount++] = &str[strLoc + 1];
         str[strLoc] = 0;
      }

      strLoc++;
   }

   if (str[strLoc] == ']')
      str[strLoc] = 0;


   // Capitalize all the arguments for ease of use
   for (i = 0; i < ptrCount; i++)
   {
      for (j = 0; j < strlen(ptr[i]); j++)
         ptr[i][j] = toupper(ptr[i][j]);
   }

   // now the actual compiling into the data stream
   streamLen = 0;
   if (strcmp(ptr[0], "END") == 0)
       newStream[streamLen++] = 0x00;
   else if (strcmp(ptr[0], "BREAK") == 0)
       newStream[streamLen++] = 0x01;
   else if (strcmp(ptr[0], "PAUSE") == 0)
   {
	   newStream[streamLen++] = 0x03;
   }



   else if ((isalpha(ptr[0][0]) == true) && (strlen(ptr[0]) != 2))
   {
	    i = 0;

		while ((i < tableLen) && (retVal == 0))
		{
			if (strcmp(ptr[0], table[i].str) == 0)
			   retVal = table[i].hexVal;
			else
			   i++;
		}

		newStream[streamLen++] = retVal;
		//newStream[streamLen++] = 0x00;
		if (retVal == 0)
		   printf("Couldn't convert control code: %s\n", ptr[0]);


      //printf("%s\r\n", ptr[0]);

   }



   // going to assume raw codes now, in 2-char hex things, like [FA 1A 2C EE]
   else if (strlen(ptr[0]) == 2)
   {
      for (i = 0; i < ptrCount; i++)
         newStream[streamLen++] = hstrtoi(ptr[i]);
   }

   else
      printf("UNKNOWN CONTROL CODE: %s\n", ptr[0]);

   // restore backup string
   strcpy(str, str2);
}

//=================================================================================================

unsigned char ConvChar(unsigned char ch)
{
	unsigned char retVal = 0;
	char          origChar[100] = "";
	int           i = 0;

	while ((i < tableLen) && (retVal == 0))
	{
		sprintf(origChar, "%c", ch);


		if (strcmp(origChar, table[i].str) == 0)
		{
		   retVal = table[i].hexVal;
		   if (ch == '\"')
		   {
			   // implementing smart quotes
			   if (quoteCount % 2 == 0)
			      retVal = 0xAC;

			   quoteCount++;
		   }
	    }
		else
		   i++;
	}

	if (retVal == 0) {
		printf("UNABLE TO CONVERT CHARACTER: %c %02X\n", ch, ch);
	}

	return retVal;
}

//=================================================================================================

void LoadTable(void)
{
   FILE* fin;
   char  tempStr[500] = "";
   int i;

   tableLen = 0;

   fin = fopen("eng_table.txt", "r");
   if (fin == NULL)
   {
	   printf("Can't open eng_table.txt!\n");
	   return;
   }

   /*i = fgetc(fin);
   i = fgetc(fin);
   i = fgetc(fin);*/


   fscanf(fin, "%s", tempStr);
   table[tableLen].hexVal = hstrtoi(tempStr);
   fscanf(fin, "%s", table[tableLen].str);
   while (!feof(fin))
   {
	   tableLen++;

   	   fscanf(fin, "%s", tempStr);
  	   table[tableLen].hexVal = hstrtoi(tempStr);
       fscanf(fin, "%s", table[tableLen].str);
	   if (strcmp(table[tableLen].str, "_") == 0) {
		   printf("_\n");
		   table[tableLen].str[0] = ' ';
		   table[tableLen].str[1] = '\0';		   
	   }
       //printf("%s\n", table[tableLen].str);
   }

   //table[0x01].str[0] = ' ';
   //table[0x01].str[1] = '\0';

   fclose(fin);
}

//=================================================================================================

int hstrtoi(char* string)
{
   unsigned int retval = 0;
   int sign = 1;
   
   for (int i = 0; i < strlen(string); i++)
   {
      retval <<= 4;
      retval += CharToHex(string[i]);
   }
   
   if (string[0] == '-') {
	   sign = -1;
   }

   return retval * sign;
}

//=================================================================================================

int CharToHex(char ch)
{
   // Converts a single hex character to an integer.

   int retVal = 0;

   ch = toupper(ch);

   switch (ch)
   {
      case '0':
      {
         retVal = 0;
         break;
      }
      case '1':
      {
         retVal = 1;
         break;
      }
      case '2':
      {
         retVal = 2;
         break;
      }
      case '3':
      {
         retVal = 3;
         break;
      }
      case '4':
      {
         retVal = 4;
         break;
      }
      case '5':
      {
         retVal = 5;
         break;
      }
      case '6':
      {
         retVal = 6;
         break;
      }
      case '7':
      {
         retVal = 7;
         break;
      }
      case '8':
      {
         retVal = 8;
         break;
      }
      case '9':
      {
         retVal = 9;
         break;
      }
      case 'A':
      {
         retVal = 10;
         break;
      }
      case 'B':
      {
         retVal = 11;
         break;
      }
      case 'C':
      {
         retVal = 12;
         break;
      }
      case 'D':
      {
         retVal = 13;
         break;
      }
      case 'E':
      {
         retVal = 14;
         break;
      }
      case 'F':
      {
         retVal = 15;
         break;
      }
   }

   return retVal;
}

//=================================================================================================

void PrepString(char str[5000], char str2[5000], int startPoint)
{
	int j;
	int ctr;

    for (j = 0; j < 5000; j++)
	    str2[j] = '\0';

    ctr = 0;
	for (j = startPoint; j < strlen(str); j++)
	{
	   str2[ctr] = str[j];
	   ctr++;
	}

}

//=================================================================================================

void InsertMainStuff(void)
{
	FILE* fin;
	char  str[5000];
	char  str2[5000];
	int   lineNum = 0;
	int   textAreaStart = 0x60010;
	int   textAreaStop = 0x7400F;
	int   textAreaBis = 0x76810;
    int   loc;
	int   ptrLoc;
	int   ptrValue;
	int   len;
	int   i;

	loc = textAreaStart;

	fin = fopen("script.txt", "r");
	if (fin == NULL)
	{
		printf("Can't open script.txt\n");
		return;
	}

	fgets(str, 5000, fin);
	while(strstr(str, "-E: ") == NULL)
	{
	   fgets(str, 5000, fin);
	}
    while(!feof(fin))
    {
  		PrepString(str, str2, 7);

  		if (str2[0] != '\n')
  		{
			ConvComplexString(str2, len);
			str2[len] = 0x00;
			len++;
			
			if (loc < textAreaBis && loc + len > textAreaStop) {
				loc = textAreaBis;
			}

			ptrLoc = 0x30010 + lineNum * 3;
			StartWritingInRom(ptrLoc, WRITE_FLAG_NORMAL, "Insert M1 main text pointer");

			ptrValue = loc - textAreaStart;
	        WriteInRom(ptrValue & 0x0000FF);
            WriteInRom((ptrValue & 0x00FF00) >> 8);
	        WriteInRom(ptrValue >> 16);

            StartWritingInRom(loc, WRITE_FLAG_NORMAL, "Insert M1 main text");
            for (i = 0; i < len; i++)
            {
			   //printf("%02X ", str2[i]);
               WriteInRom(str2[i]);
			}
			//printf("\n");

		    loc += len;
		}


        lineNum++;
	    fgets(str, 5000, fin);
	    while(strstr(str, "-E: ") == NULL && !feof(fin))
	    {
			fgets(str, 5000, fin);
    	}

	}

    printf(" Main text:\t\tINSERTED\r\n");

	fclose(fin);
}

//=================================================================================================

void InsertSpecialText(void)
{
	FILE* fin;
	char  str[5000];
	char  str2[5000];
	char  line[5000];
	int   loc;
	int   temp;
	int   len;
	int   i;
	
	char*  commentStr = (char*) "Insert M1 misc text";

    fin = fopen("misc_text.txt", "r");
    if (fin == NULL)
    {
		printf("Can't open misc_text.txt");
		return;
	}

	//fscanf(fin, "%x", &loc);
	//fscanf(fin, "%s", str);
	//fscanf(fin, "%s", line);
    fgets(line, 5000, fin);
    while(!feof(fin))
    {
		if (line[0] != '/' && line[0] != '\r' && line[0] != 10)
		{
		   sscanf(line, "%x %[^\t\n]", &loc, str);
           strcat(str, " ");

           //printf("%2d %X - %s\n", line[0], loc, str);
     	   PrepString(str, str2, 0);
		   ConvComplexString(str2, len);
		   
		   StartWritingInRom(loc, WRITE_FLAG_NORMAL, commentStr);

           for (i = 0; i < len; i++)
	          WriteInRom(str2[i]);
		  
	    } else {
			if (line[0] == '/' && line[1] == '/' && line[2] != '=') {
				commentStr = (char*) malloc(strlen(line) - 1);
				strcpy(commentStr, line + 2);
				commentStr[strlen(commentStr)-1] = '\0';
			}
		}

	    //fscanf(fin, "%s", line);
	    fgets(line, 5000, fin);

    	//fscanf(fin, "%x", &loc);
//    	fscanf(fin, "%s", str);
	}

    printf(" Misc. text:\t\tINSERTED\r\n");

	fclose(fin);
}


//=================================================================================================

void InsertNames(void)
{
	FILE* fin;
	char  str[5000];
	char  str2[5000];
	int   lineNum = 0;
	int   textAreaStart = 0x10;
	int   textAreaStop = 0x9DB;
	int   textAreaBis = 0x31780;
    int   loc;
	int   ptrLoc;
	int   ptrOffset = 0;
	int   ptrStep = 1;
	
	int   ptrBaseValue = 0x8000;
	int   ptrValue;
	
	int   len;
	int   i;
	
	loc = textAreaStart;
	ptrLoc = textAreaStart;
	
	fin = fopen("names.txt", "r");
	if (fin == NULL)
	{
		printf("Can't open names.txt\n");
		return;
	}

	fgets(str, 5000, fin);
    while(!feof(fin))
    {

  		if (strncmp(str, "//", 2) == 0) {
		} else if (str[0] == '!') {
			str[strcspn(str,"\r\n")] = '\0';
			if (strncmp(str, "!START=", 7) == 0) {
				ptrLoc = hstrtoi(&str[7]);
			} else if (strncmp(str, "!OFFST=", 7) == 0) {
				ptrOffset = hstrtoi(&str[7]);
			} else if (strncmp(str, "!ENTRY=", 7) == 0) {
				ptrStep = hstrtoi(&str[7]);
			} else if (strncmp(str, "!REPEAT", 7) == 0) {
				StartWritingInRom(ptrLoc + ptrOffset, WRITE_FLAG_NORMAL, "Insert M1 name pointer");
				WriteInRom(ptrValue & 0x00FF);
				WriteInRom(ptrValue >> 8);
				ptrLoc += ptrStep;
			} else if (strncmp(str, "!NSKIP=", 7) == 0) {
				ptrLoc += ptrStep * hstrtoi(&str[7]);
			}
		} else {

			PrepString(str, str2, 0);	
			ConvComplexString(str2, len);
			str2[len] = 0x00;
			len++;
			
			if (loc < textAreaBis && loc + len > textAreaStop) {
				loc = textAreaBis;
			}

			ptrValue = loc - textAreaStart + ptrBaseValue;
			
			StartWritingInRom(ptrLoc + ptrOffset, WRITE_FLAG_NORMAL, "Insert M1 name pointer");
			WriteInRom(ptrValue & 0x00FF);
			WriteInRom(ptrValue >> 8);
			ptrLoc += ptrStep;

            StartWritingInRom(loc, WRITE_FLAG_NORMAL, "Insert M1 names");
            for (i = 0; i < len; i++)
            {
			   //printf("%02X ", str2[i]);
               WriteInRom(str2[i]);
			}
			//printf("\n");

		    loc += len;
			lineNum++;
		}


	    fgets(str, 5000, fin);

	}

    printf(" Names:\t\tINSERTED\r\n");

	fclose(fin);
}


void InsertAltWindowData(void)
{
	FILE* fin;
	char  str[1000];
	int   lineNum;
	int   insertLoc = 0xFED000;
	int   totalSize = 0x1000;
	int   totalFound = 0;

	fin = fopen("m1_small_windows_list.txt", "r");
	if (!fin)
	{
		printf("Can't open m1_small_windows_list.txt, doh\r\n");
		return;
	}

	StartWritingInRom(insertLoc, WRITE_FLAG_PREFILL, "Insert M1 alt window data");
	for (int i = 0; i < totalSize; i++)
	   WriteInRom(0);

	fscanf(fin, "%x", &lineNum);
    while(!feof(fin))
    {
		if (lineNum < totalSize)
		{
			StartWritingInRom(insertLoc + lineNum, WRITE_FLAG_AFTER_PREFILL, "Insert M1 alt window data");
			WriteInRom(1);
			totalFound++;
		}

	   fscanf(fin, "%x", &lineNum);
	}

	fclose(fin);

    printf(" Alt. windows:\t\tINSERTED (Total: %d)\r\n", totalFound);
    return;
}

void InsertItemArticles(void)
{
	FILE* fin;
	char  line[1000];
	char* str;
	int   lineNum = 0;
	int   startLoc = 0xFFE000;
	int   i;

	fin = fopen("m1_item_articles.txt", "r");
	if (!fin)
	{
		printf("Can't open m1_item_articles.txt, doh\r\n");
		return;
	}

	fgets(line, 1000, fin);
    while(!feof(fin))
    {
		line[strcspn(line, "\n")] = '\0';
        if (line[0] != '/' && line[0] != '\0') {
            str = &line[13];
            
            StartWritingInRom(startLoc + lineNum, WRITE_FLAG_NORMAL, "Insert M1 item articles");
            WriteInRom(hstrtoi(str));

            lineNum++;
        }
        fgets(line, 1000, fin);
	}

    printf(" Item articles:\t\tINSERTED\r\n");

	return;
}

void InsertEnemyArticles(void)
{
	FILE* fin;
	char  line[1000];
	char* str;
	int   lineNum = 0;
	int   startLoc = 0xFFE080;
	int   i;

	fin = fopen("m1_enemy_articles.txt", "r");
	if (!fin)
	{
		printf("Can't open m1_enemy_articles.txt, doh\r\n");
		return;
	}

	fgets(line, 1000, fin);
    while(!feof(fin))
    {
		line[strcspn(line, "\n")] = '\0';
        if (line[0] != '/' && line[0] != '\0') {
            str = &line[14];
            
            StartWritingInRom(startLoc + lineNum, WRITE_FLAG_NORMAL, "Insert M1 enemy articles");
            WriteInRom(hstrtoi(str));

            lineNum++;
        }
	    fgets(line, 1000, fin);
	}

    printf(" Enemy articles:\tINSERTED\r\n");

	return;
}

void InsertItemClasses(void)
{
	FILE* fin;
	char  line[1000];
	char* str;
	int   lineNum = 0;
	int   startLoc = 0xFFE100;
	int   i;

	fin = fopen("m1_item_classes.txt", "r");
	if (!fin)
	{
		printf("Can't open m1_item_classes.txt, doh\r\n");
		return;
	}

	fgets(line, 1000, fin);
    while(!feof(fin))
    {
        line[strcspn(line, "\n")] = '\0';

        if (line[0] != '/') {
            StartWritingInRom(startLoc + lineNum * 0x8, WRITE_FLAG_PREFILL, "Insert M1 item classes");
            for (i = 0; i < 0x8; i++)
               WriteInRom(0);

			if (strlen(line) > 0) {
				StartWritingInRom(startLoc + lineNum * 0x8, WRITE_FLAG_AFTER_PREFILL, "Insert M1 item classes");
				for (i = 0; i < strlen(line); i++)
				   WriteInRom(ConvChar(line[i]));
		    }

            lineNum++;
        }
	    fgets(line, 1000, fin);
	}

    printf(" Item classes:\t\tINSERTED\r\n");

	return;
}

void InsertEnemyClasses(void)
{
	FILE* fin;
	char  line[1000];
	char* str;
	int   lineNum = 0;
	int   startLoc = 0xFFE500;
	int   i;

	fin = fopen("m1_enemy_classes.txt", "r");
	if (!fin)
	{
		printf("Can't open m1_enemy_classes.txt, doh\r\n");
		return;
	}

	fgets(line, 1000, fin);
    while(!feof(fin))
    {
        line[strcspn(line, "\n")] = '\0';

        if (line[0] != '/') {
            StartWritingInRom(startLoc + lineNum * 0x8, WRITE_FLAG_PREFILL, "Insert M1 enemy classes");
            for (i = 0; i < 0x8; i++)
               WriteInRom(0);

			if (strlen(line) > 0) {
				StartWritingInRom(startLoc + lineNum * 0x8, WRITE_FLAG_AFTER_PREFILL, "Insert M1 enemy classes");
				for (i = 0; i < strlen(line); i++)
				   WriteInRom(ConvChar(line[i]));
		    }

            lineNum++;
        }
	    fgets(line, 1000, fin);
	}

    printf(" Enemy classes:\t\tINSERTED\r\n");

	return;
}


void ConvComplexMenuString(char str[5000], int& newLen)
{
	char          newStr[5000] = "";
	unsigned char newStream[100];
	int           streamLen =  0;
	int           len = strlen(str) - 1; // minus one to take out the newline
	int           counter = 0;
	int           i;

	newLen = 0;

    quoteCount = 0;

    while (counter < len)
    {
		//printf("%c", str[counter]);
		if (str[counter] == '[')
		{
		   CompileCC(str, counter, newStream, streamLen);
		   for (i = 0; i < streamLen; i++)
		   {
			   newStr[newLen] = newStream[i];
			   newLen++;
		   }
		   counter++; // to skip past the ]
		}
		else
		{
		   newStr[newLen++] = 0x82;
		   newStr[newLen++] = str[counter] + 0x1F;

		   counter++;
		}
	}

    for (i = 0; i < 5000; i++)
       str[i] = '\0';
	for (i = 0; i < newLen; i++)
	   str[i] = newStr[i];
}

