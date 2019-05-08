/*
    LAC (Linux Air combat)
    Copyright 2015 by Robert J. Bosen. Major portions of
    this code were derived from "gl-117", by Thomas A. Drexl and
    other contributors, who are mentioned in the "Credits" menu.

    This file is part of LAC.

    LAC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    LAC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LAC; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
   ****************************************************************************
   * Almost all of this logic was originally written by "Testing Solutions"   *
   * Reuter, Christian                         *
   * Von-Leoprechting-Str. 14, 86899 Landsberg / Lech Germany                 *
   * Phone: +49 8191 305202, Fax: +49 8191 305244                             *
   * Internet: www.testingsolutions.de                                        *
   ****************************************************************************

   DOCUMENTATION:
     This is a low level library to split text files into tokens.
*/

#ifndef IS_FILELOADER_H

#include <stdio.h>
#include <string.h>

#include "fileloader.h"

/**************************************************************************
  This is a low level library to split text files into tokens.
  Customize strings for quotes, whitespace and comments.
  Then parse one token after each other calling file_nextToken()
  or set the file cursor to a new position calling file_find(mystr)

  Documentation notes:
  - all int-functions return 0 on error/EOF, 1 on success
  - all documented functions are public, the rest is private

  Example to parse MEA-files:
    char token [256];
    file_setWhitespace (" \t\r\n-=");
    file_setQuotes ("\'{}");
    file_addComment ("$", "\n");
    file_open ("test.mea");
    while (file_find ("[MEA_"))         // search for string [MEA_
    {
      file_nextToken (token, 256);      // read the [MEA_ token
      printf ("%s\n", token);           // and print it on stdout
    }
    file_rewind ();                     // again from the beginning
    while (file_nextToken (token, 256)) // read every token
    {
      printf ("%s\n", token);           // print out every token
    }
**************************************************************************/

int file_quoteremove = 0;

FILE *file_file = NULL;         /* FILE pointer */
char file_buffer [FILE_MAXLEN+1]; /* file buffer in RAM */
int file_ptr = FILE_MAXLEN;     /* file_buffer pointer to current char = "file cursor" */
char file_whitespace [256];     /* string containing the whitespace chars */
char file_separator [256];      /* string containing the separator chars */
int file_commentnum = 0;        /* number of comment strings */
char file_quotes [256];         /* string containing the quote chars */
char file_commentstart [FILE_MAXCOMMENTS] [256]; /* strings for comment beginnings */
char file_commentend [FILE_MAXCOMMENTS] [256];   /* strings for corresponding comment ends */
int file_bufferlen = FILE_MAXLEN;                /* current length of the file buffer */
int file_line = 1;
char file_currentquotes = 0;
char file_currentcomment = 0;
XLONG file_bufferstart = 0;
int file_bufferread = 0;
char file_lastchar;
int file_reverse = 0;

/**************************************************************************
  Init data structure
  Example: file_init ();
**************************************************************************/
void file_init ()
    {
    memset (file_whitespace, 0, 256);
    memset (file_separator, 0, 256);
    memset (file_quotes, 0, 256);
    memset (file_commentstart, 0, FILE_MAXCOMMENTS * 256);
    memset (file_commentend, 0, FILE_MAXCOMMENTS * 256);
    file_commentnum = 0;
    file_quoteremove = 0;
    file_currentquotes = 0;
    file_currentcomment = 0;
    file_reverse = 0;
    file_bufferread = 0;
    file_bufferstart = 0;
    file_buffer [FILE_MAXLEN] = 0;
    }

void file_initComments ()
    {
    memset (file_commentstart, 0, FILE_MAXCOMMENTS * 256);
    memset (file_commentend, 0, FILE_MAXCOMMENTS * 256);
    file_commentnum = 0;
    }

/**************************************************************************
  Get current position of the file pointer
  Example: file_getPosition ();
**************************************************************************/
XLONG file_getPosition ()
    {
    if (!file_reverse)
        {
        return (XLONG) ftell (file_file) - FILE_MAXLEN + file_ptr;
        }
    else
        {
        return (XLONG) ftell (file_file) - file_ptr;
        }
    }

/**************************************************************************
  Get current line of the file
  Example: file_getLine ();
**************************************************************************/
int file_getLine ()
    {
    return file_line;
    }

/**************************************************************************
  Set the whitespace chars for this file
  Example: file_setWhitespace (" \n\t");
**************************************************************************/
void file_setWhitespace (char *string)
    {
    strncpy (file_whitespace, string, 256);
    file_whitespace [255] = 0;
    }

/**************************************************************************
  Set the separator chars for this file
  Example: file_setSeparator (",;");
**************************************************************************/
void file_setSeparator (char *string)
    {
    strncpy (file_separator, string, 256);
    file_separator [255] = 0;
    }

/**************************************************************************
  Add comment strings for this file
  Example language C++:
    file_addComment ("//", "\n");
**************************************************************************/
void file_addComment (char *startstring, char *endstring)
    {
    if (file_commentnum >= FILE_MAXCOMMENTS)
        {
        return;
        }

    strncpy (file_commentstart [file_commentnum], startstring, 256);
    file_commentstart [file_commentnum] [255] = 0;
    strncpy (file_commentend [file_commentnum], endstring, 256);
    file_commentend [file_commentnum] [255] = 0;
    file_commentnum ++;
    }

/**************************************************************************
  Set the quote chars for string in this file
  Example: file_setQuotes ("\'\"");
**************************************************************************/
void file_setQuotes (char *string)
    {
    strncpy (file_quotes, string, 256);
    file_quotes [255] = 0;
    }

void file_revertBuffer (int start, int end)
    {
    int i;
    char c;

    for (i = 0; i <= (end - start) / 2; i ++)
        {
        c = file_buffer [start + i];
        file_buffer [start + i] = file_buffer [end - i];
        file_buffer [end - i] = c;
        }
    }

int file_fillBuffer ()
    {
    int bytesread = 0;
    int n = FILE_MAXLEN - file_ptr;
    int bytestoread = file_ptr;

    if (file_ptr < FILE_MAXLEN)
        if (file_buffer [file_ptr] == 0)
            {
            return 0;
            }

    if (file_ptr <= FILE_MAXLEN / 2)
        {
        return 1;
        }

    memcpy (file_buffer, &file_buffer [file_ptr], n);

    if (file_reverse)
        {
        if (file_bufferstart - bytestoread - file_bufferread >= 0)
            {
            fseek (file_file, -bytestoread - file_bufferread, SEEK_CUR);
            file_bufferstart += -bytestoread - file_bufferread;
            }
        else
            {
            bytestoread = (int) file_bufferstart - file_bufferread;
            fseek (file_file, 0, SEEK_SET);
            file_bufferstart = 0;
            memset (&file_buffer [n + bytestoread], 0, FILE_MAXLEN - (n + bytestoread));
            }
        }

    bytesread = fread (&file_buffer [n], 1, bytestoread, file_file);
    file_bufferlen = file_bufferlen + bytesread - file_ptr;
    file_bufferread = bytesread;
    file_bufferstart += bytesread;

    if (file_reverse)
        {
        if (bytesread > 0)
            {
            file_revertBuffer (n, n + bytesread - 1);
            }
        }

    if (bytesread != file_ptr)
        {
        if (file_bufferlen >= 0 && file_bufferlen < FILE_MAXLEN)
            {
            file_buffer [file_bufferlen] = 0;
            }
        }

    file_ptr = 0;
    return 1;
    }

/**************************************************************************
  Revert the file read order (forward/backward)
  Example: file_revert ();
**************************************************************************/
void file_revert ()
    {
    if (file_reverse)
        {
        fseek (file_file, -file_bufferread - file_ptr + file_bufferlen, SEEK_CUR);
        file_bufferstart += -file_bufferread - file_ptr + file_bufferlen;
        }

    file_bufferread = file_bufferlen - file_ptr;
    file_reverse = !file_reverse;
    file_ptr = FILE_MAXLEN;
    file_bufferlen = FILE_MAXLEN;
    file_fillBuffer ();
    }

/**************************************************************************
  Set current position of the file pointer
  Example: file_setPosition ();
**************************************************************************/
int file_setPosition (XLONG pointer, int whence)
    {
    if (fseek (file_file, 0, whence))
        {
        return 0;
        }

    if (whence == 1)
        {
        if (!file_reverse)
            {
            pointer += -FILE_MAXLEN + file_ptr;
            }
        else
            {
            pointer += -file_ptr;
            }
        }

    while (pointer > 0x77777777)
        {
        if (fseek (file_file, 0x77777777, 1))
            {
            return 0;
            }

        pointer -= 0x77777777;
        }

    while (pointer < -0x77777777)
        {
        if (fseek (file_file, -0x77777777, 1))
            {
            return 0;
            }

        pointer += 0x77777777;
        }

    if (!fseek (file_file, (long) pointer, 1))
        {
        file_bufferstart = ftell (file_file);
        file_bufferread = 0;
        file_ptr = FILE_MAXLEN;
        file_bufferlen = FILE_MAXLEN;
        file_fillBuffer ();
        return 1;
        }

    return 0;
    }

/**************************************************************************
  Rewind file to an initial state to restart parsing
**************************************************************************/
void file_rewind ()
    {
    fseek (file_file, 0, SEEK_SET);
    file_reverse = 0;
    file_ptr = FILE_MAXLEN;
    file_bufferlen = FILE_MAXLEN;
    file_bufferstart = 0;
    file_fillBuffer ();
    file_line = 1;
    }

/**************************************************************************
  Open file for parsing
**************************************************************************/
int file_open (char *filename)
    {
    file_file = fopen (filename, "rb");

    if (!file_file)
        {
        return 0;
        }

    file_rewind ();
    return 1;
    }

/**************************************************************************
  Close file
**************************************************************************/
int file_close ()
    {
    if (file_file)
        {
        return !fclose (file_file);
        }
    else
        {
        return 1;
        }
    }

int file_isChar (char *string)
    {
    int i = 0;

    while (string [i] != 0 && i < 256)
        {
        if (file_buffer [file_ptr] == string [i])
            {
            return 1;
            }

        i ++;
        }

    return 0;
    }

int file_isCommentStart ()
    {
    int i, i2;

    for (i = 0; i < file_commentnum; i ++)
        {
        for (i2 = 0; i2 < (int) strlen (file_commentstart [i]); i2 ++)
            if (file_buffer [file_ptr + i2] != file_commentstart [i] [i2])
                {
                break;
                }

        if (i2 == (int) strlen (file_commentstart [i]))
            {
            file_currentcomment = i;
            return 1;
            }
        }

    return 0;
    }

int file_isCommentEnd ()
    {
    int i, i2;
    i = file_currentcomment;

    for (i2 = 0; i2 < (int) strlen (file_commentend [i]); i2 ++)
        if (file_buffer [file_ptr + i2] != file_commentend [i] [i2])
            {
            break;
            }

    if (i2 == (int) strlen (file_commentend [i]))
        {
        for (i = 0; i < i2; i ++)
            {
            (void) file_nextChar ();
            }

        return 1;
        }

    return 0;
    }

/**************************************************************************
  Find the first occurance of string from the cursor and set cursor
  to the new position, this is quite fast!
**************************************************************************/
int file_find (char *string)
    {
    char *result = NULL;
    int stringlen = strlen (string);

    if (stringlen >= FILE_MAXLEN / 2)
        {
        return 0;
        }

    while (result == NULL)
        {
        result = strstr (&file_buffer [file_ptr], string);

        if (result == NULL)
            {
            file_ptr = file_bufferlen - stringlen;

            if (file_bufferlen < FILE_MAXLEN)
                {
                return 0;
                }

            if (!file_fillBuffer ())
                {
                return 0;
                }
            }
        else
            {
            file_ptr += (int) (result - (&file_buffer [file_ptr]));
            file_fillBuffer ();
            return 1;
            }
        }

    return 1;
    }

int file_nextChar ()
    {
    if (file_buffer [file_ptr] == '\n' && file_lastchar != 92)
        {
        file_line ++;
        }

    file_lastchar = file_buffer [file_ptr];
    file_ptr ++;
    return file_fillBuffer ();
    }

int file_tokenpos = 0;

int file_fillToken (char *token, int tokenlen)
    {
    if (tokenlen == 0)
        {
        return 0;
        }

    if (file_tokenpos >= tokenlen - 1)
        {
        token [tokenlen - 1] = 0;
        return 0;
        }

    token [file_tokenpos ++] = file_buffer [file_ptr];
    token [file_tokenpos] = 0;
    return 1;
    }

/**************************************************************************
  Find the next token from the current cursor position and fill the
  token string. Use tokenlen to define a maximum length for token.
**************************************************************************/
int file_nextToken (char *token, int tokenlen)
    {
    char last = 0;
    file_tokenpos = 0;

    if (tokenlen <= 0)
        {
        return 1;
        }

    token [0] = '\0';

    if (file_buffer [file_ptr] == 0)
        {
        return 0;
        }

    while (1)
        {
        if (file_isChar (file_whitespace))
            {
            if (!file_nextChar ())
                {
                return 0;
                }
            }
        else if (file_isChar (file_separator))
            {
            if (!file_fillToken (token, tokenlen))
                {
                return 1;
                }

            if (!file_nextChar ())
                {
                return 0;
                }

            return 1;
            }
        else if (file_isCommentStart ())
            {
            do
                {
                if (!file_nextChar ())
                    {
                    return 0;
                    }
                }
            while (!file_isCommentEnd ());
            }
        else if (file_isChar (file_quotes))
            {
            file_currentquotes = file_buffer [file_ptr];

            if (!file_quoteremove)
                if (!file_fillToken (token, tokenlen))   /* opening quotes */
                    {
                    return 1;
                    }

            last = 0;

            while (1)
                {
                last = file_buffer [file_ptr];

                if (!file_nextChar ())
                    {
                    return 0;
                    }

                if (file_buffer [file_ptr] == file_currentquotes && last != '\\')
                    {
                    if (!file_quoteremove)
                        if (!file_fillToken (token, tokenlen))   /* closing quotes */
                            {
                            return 1;
                            }

                    if (!file_nextChar ())
                        {
                        return 0;
                        }

                    return 1;
                    }

                if (file_buffer [file_ptr] != '\\')
                    {
                    if (last != '\\')
                        {
                        if (!file_fillToken (token, tokenlen))
                            {
                            return 1;
                            }
                        }
                    else
                        {
                        if (file_buffer [file_ptr] == 'n')
                            {
                            file_buffer [file_ptr] = '\n';
                            }
                        else if (file_buffer [file_ptr] == 't')
                            {
                            file_buffer [file_ptr] = '\t';
                            }
                        else if (file_buffer [file_ptr] == 'r')
                            {
                            file_buffer [file_ptr] = '\r';
                            }
                        else if (file_buffer [file_ptr] == 'a')
                            {
                            file_buffer [file_ptr] = '\a';
                            }
                        else if (file_buffer [file_ptr] == '\\')
                            {
                            file_buffer [file_ptr] = '\\';
                            }

                        if (!file_fillToken (token, tokenlen))
                            {
                            return 1;
                            }
                        }
                    }
                }
            }
        else
            {
            while (1)
                {
                if (!file_fillToken (token, tokenlen))
                    {
                    return 1;
                    }

                if (!file_nextChar ())
                    {
                    return 0;
                    }

                if (file_isChar (file_whitespace))
                    {
                    return 1;
                    }

                if (file_isChar (file_separator))
                    {
                    return 1;
                    }

                if (file_isCommentStart ())
                    {
                    return 1;
                    }

                if (file_isChar (file_quotes))
                    {
                    return 1;
                    }
                }
            }
        }

    return 1;
    }

#endif
