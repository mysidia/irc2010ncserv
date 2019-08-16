/*
 *   pIrcd protocol control file                               src/msgs/msgs.c
 *   Copyright C 1998 pIrcd Authors
 *   Copyright C 1998 Mysidia <***REMOVED***> <Mysidia>
 *   see file 'AUTHORS' for a comprehensive list of pIrcd authors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the file 'LICENSE' included with this package
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

 /* 
  *  server<->server protocol need not be the same as client<->server protocol
  *  any longer
  */

// We need a table of function -> token mappings
// Secondly, the parser should call the proper function given
// a token is incoming.

// The parameters the (*ccParse) pointer is called with should
// be independent of the underlying protocol

struct MessageTokenTable
{
	CommandToken		tok;
	ParserFunctionPtr	ccParse;
}
tokens[]
=
{
};

#if 0
static void vsendto_one( aClient *target, const char *fmt, va_list ap)
{
              /* used by msgtoclient() */
}
#endif


#if 0
void msgtoclient( aClient *target, const char *fmt, ...)
{
          /* find client in hash tables */
          /* deliver message and return if it's my client*/
          /* lookup next hop the message has to get to */
          /* transmit message to next hop */
}
#endif

