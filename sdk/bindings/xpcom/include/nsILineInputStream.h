/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM /home/vbox/tinderbox/sdk/src/libs/xpcom18a4/xpcom/io/nsILineInputStream.idl
 */

#ifndef __gen_nsILineInputStream_h__
#define __gen_nsILineInputStream_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsILineInputStream; /* forward declaration */


/* starting interface:    nsILineInputStream */
#define NS_ILINEINPUTSTREAM_IID_STR "c97b466c-1e6e-4773-a4ab-2b2b3190a7a6"

#define NS_ILINEINPUTSTREAM_IID \
  {0xc97b466c, 0x1e6e, 0x4773, \
    { 0xa4, 0xab, 0x2b, 0x2b, 0x31, 0x90, 0xa7, 0xa6 }}

class NS_NO_VTABLE nsILineInputStream : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ILINEINPUTSTREAM_IID)

  /**
   * Read a single line from the stream, where a line is a 
   * possibly zero length sequence of 8bit chars terminated by a
   * CR, LF, CRLF, LFCR, or eof.
   * The line terminator is not returned.
   * Return false for end of file, true otherwise
   */
  /* boolean readLine (out ACString aLine); */
  NS_IMETHOD ReadLine(nsACString & aLine, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSILINEINPUTSTREAM \
  NS_IMETHOD ReadLine(nsACString & aLine, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSILINEINPUTSTREAM(_to) \
  NS_IMETHOD ReadLine(nsACString & aLine, PRBool *_retval) { return _to ReadLine(aLine, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSILINEINPUTSTREAM(_to) \
  NS_IMETHOD ReadLine(nsACString & aLine, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadLine(aLine, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsLineInputStream : public nsILineInputStream
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSILINEINPUTSTREAM

  nsLineInputStream();

private:
  ~nsLineInputStream();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsLineInputStream, nsILineInputStream)

nsLineInputStream::nsLineInputStream()
{
  /* member initializers and constructor code */
}

nsLineInputStream::~nsLineInputStream()
{
  /* destructor code */
}

/* boolean readLine (out ACString aLine); */
NS_IMETHODIMP nsLineInputStream::ReadLine(nsACString & aLine, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsILineInputStream_h__ */
