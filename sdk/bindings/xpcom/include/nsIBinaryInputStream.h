/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM /home/vbox/tinderbox/sdk/src/libs/xpcom18a4/xpcom/io/nsIBinaryInputStream.idl
 */

#ifndef __gen_nsIBinaryInputStream_h__
#define __gen_nsIBinaryInputStream_h__


#ifndef __gen_nsIInputStream_h__
#include "nsIInputStream.h"
#endif

#ifndef __gen_nsrootidl_h__
#include "nsrootidl.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIBinaryInputStream */
#define NS_IBINARYINPUTSTREAM_IID_STR "7b456cb0-8772-11d3-90cf-0040056a906e"

#define NS_IBINARYINPUTSTREAM_IID \
  {0x7b456cb0, 0x8772, 0x11d3, \
    { 0x90, 0xcf, 0x00, 0x40, 0x05, 0x6a, 0x90, 0x6e }}

/**
 * This interface allows consumption of primitive data types from a "binary
 * stream" containing untagged, big-endian binary data, i.e. as produced by an
 * implementation of nsIBinaryOutputStream.  This might be used, for example,
 * to implement network protocols or to read from architecture-neutral disk
 * files, i.e. ones that can be read and written by both big-endian and
 * little-endian platforms.
 *
 * @See nsIBinaryOutputStream
 */
class NS_NO_VTABLE nsIBinaryInputStream : public nsIInputStream {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IBINARYINPUTSTREAM_IID)

  /* void setInputStream (in nsIInputStream aInputStream); */
  NS_IMETHOD SetInputStream(nsIInputStream *aInputStream) = 0;

  /* PRBool readBoolean (); */
  NS_IMETHOD ReadBoolean(PRBool *_retval) = 0;

  /* PRUint8 read8 (); */
  NS_IMETHOD Read8(PRUint8 *_retval) = 0;

  /* PRUint16 read16 (); */
  NS_IMETHOD Read16(PRUint16 *_retval) = 0;

  /* PRUint32 read32 (); */
  NS_IMETHOD Read32(PRUint32 *_retval) = 0;

  /* PRUint64 read64 (); */
  NS_IMETHOD Read64(PRUint64 *_retval) = 0;

  /* float readFloat (); */
  NS_IMETHOD ReadFloat(float *_retval) = 0;

  /* double readDouble (); */
  NS_IMETHOD ReadDouble(double *_retval) = 0;

  /**
     * Read a NUL-terminated 8-bit char* string from a binary stream.
     */
  /* ACString readCString (); */
  NS_IMETHOD ReadCString(nsACString & _retval) = 0;

  /**
     * Read a NUL-terminated 16-bit PRUnichar* string from a binary stream.
     */
  /* AString readString (); */
  NS_IMETHOD ReadString(nsAString & _retval) = 0;

  /**
     * Read an opaque byte array from a binary stream.
     */
  /* void readBytes (in PRUint32 aLength, [size_is (aLength), retval] out string aString); */
  NS_IMETHOD ReadBytes(PRUint32 aLength, char **aString) = 0;

  /**
     * Read an opaque byte array from a binary stream, storing the results
     * as an array of PRUint8s.
     */
  /* void readByteArray (in PRUint32 aLength, [array, size_is (aLength), retval] out PRUint8 aBytes); */
  NS_IMETHOD ReadByteArray(PRUint32 aLength, PRUint8 **aBytes) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIBINARYINPUTSTREAM \
  NS_IMETHOD SetInputStream(nsIInputStream *aInputStream); \
  NS_IMETHOD ReadBoolean(PRBool *_retval); \
  NS_IMETHOD Read8(PRUint8 *_retval); \
  NS_IMETHOD Read16(PRUint16 *_retval); \
  NS_IMETHOD Read32(PRUint32 *_retval); \
  NS_IMETHOD Read64(PRUint64 *_retval); \
  NS_IMETHOD ReadFloat(float *_retval); \
  NS_IMETHOD ReadDouble(double *_retval); \
  NS_IMETHOD ReadCString(nsACString & _retval); \
  NS_IMETHOD ReadString(nsAString & _retval); \
  NS_IMETHOD ReadBytes(PRUint32 aLength, char **aString); \
  NS_IMETHOD ReadByteArray(PRUint32 aLength, PRUint8 **aBytes); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIBINARYINPUTSTREAM(_to) \
  NS_IMETHOD SetInputStream(nsIInputStream *aInputStream) { return _to SetInputStream(aInputStream); } \
  NS_IMETHOD ReadBoolean(PRBool *_retval) { return _to ReadBoolean(_retval); } \
  NS_IMETHOD Read8(PRUint8 *_retval) { return _to Read8(_retval); } \
  NS_IMETHOD Read16(PRUint16 *_retval) { return _to Read16(_retval); } \
  NS_IMETHOD Read32(PRUint32 *_retval) { return _to Read32(_retval); } \
  NS_IMETHOD Read64(PRUint64 *_retval) { return _to Read64(_retval); } \
  NS_IMETHOD ReadFloat(float *_retval) { return _to ReadFloat(_retval); } \
  NS_IMETHOD ReadDouble(double *_retval) { return _to ReadDouble(_retval); } \
  NS_IMETHOD ReadCString(nsACString & _retval) { return _to ReadCString(_retval); } \
  NS_IMETHOD ReadString(nsAString & _retval) { return _to ReadString(_retval); } \
  NS_IMETHOD ReadBytes(PRUint32 aLength, char **aString) { return _to ReadBytes(aLength, aString); } \
  NS_IMETHOD ReadByteArray(PRUint32 aLength, PRUint8 **aBytes) { return _to ReadByteArray(aLength, aBytes); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIBINARYINPUTSTREAM(_to) \
  NS_IMETHOD SetInputStream(nsIInputStream *aInputStream) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetInputStream(aInputStream); } \
  NS_IMETHOD ReadBoolean(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadBoolean(_retval); } \
  NS_IMETHOD Read8(PRUint8 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Read8(_retval); } \
  NS_IMETHOD Read16(PRUint16 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Read16(_retval); } \
  NS_IMETHOD Read32(PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Read32(_retval); } \
  NS_IMETHOD Read64(PRUint64 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Read64(_retval); } \
  NS_IMETHOD ReadFloat(float *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadFloat(_retval); } \
  NS_IMETHOD ReadDouble(double *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadDouble(_retval); } \
  NS_IMETHOD ReadCString(nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadCString(_retval); } \
  NS_IMETHOD ReadString(nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadString(_retval); } \
  NS_IMETHOD ReadBytes(PRUint32 aLength, char **aString) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadBytes(aLength, aString); } \
  NS_IMETHOD ReadByteArray(PRUint32 aLength, PRUint8 **aBytes) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadByteArray(aLength, aBytes); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsBinaryInputStream : public nsIBinaryInputStream
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIBINARYINPUTSTREAM

  nsBinaryInputStream();

private:
  ~nsBinaryInputStream();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsBinaryInputStream, nsIBinaryInputStream)

nsBinaryInputStream::nsBinaryInputStream()
{
  /* member initializers and constructor code */
}

nsBinaryInputStream::~nsBinaryInputStream()
{
  /* destructor code */
}

/* void setInputStream (in nsIInputStream aInputStream); */
NS_IMETHODIMP nsBinaryInputStream::SetInputStream(nsIInputStream *aInputStream)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRBool readBoolean (); */
NS_IMETHODIMP nsBinaryInputStream::ReadBoolean(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRUint8 read8 (); */
NS_IMETHODIMP nsBinaryInputStream::Read8(PRUint8 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRUint16 read16 (); */
NS_IMETHODIMP nsBinaryInputStream::Read16(PRUint16 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRUint32 read32 (); */
NS_IMETHODIMP nsBinaryInputStream::Read32(PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRUint64 read64 (); */
NS_IMETHODIMP nsBinaryInputStream::Read64(PRUint64 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* float readFloat (); */
NS_IMETHODIMP nsBinaryInputStream::ReadFloat(float *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double readDouble (); */
NS_IMETHODIMP nsBinaryInputStream::ReadDouble(double *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* ACString readCString (); */
NS_IMETHODIMP nsBinaryInputStream::ReadCString(nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString readString (); */
NS_IMETHODIMP nsBinaryInputStream::ReadString(nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void readBytes (in PRUint32 aLength, [size_is (aLength), retval] out string aString); */
NS_IMETHODIMP nsBinaryInputStream::ReadBytes(PRUint32 aLength, char **aString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void readByteArray (in PRUint32 aLength, [array, size_is (aLength), retval] out PRUint8 aBytes); */
NS_IMETHODIMP nsBinaryInputStream::ReadByteArray(PRUint32 aLength, PRUint8 **aBytes)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

inline nsresult
NS_ReadOptionalCString(nsIBinaryInputStream* aStream, nsACString& aResult)
{
    PRBool nonnull;
    nsresult rv = aStream->ReadBoolean(&nonnull);
    if (NS_SUCCEEDED(rv)) {
        if (nonnull)
            rv = aStream->ReadCString(aResult);
        else
            aResult.Truncate();
    }
    return rv;
}
inline nsresult
NS_ReadOptionalString(nsIBinaryInputStream* aStream, nsAString& aResult)
{
    PRBool nonnull;
    nsresult rv = aStream->ReadBoolean(&nonnull);
    if (NS_SUCCEEDED(rv)) {
        if (nonnull)
            rv = aStream->ReadString(aResult);
        else
            aResult.Truncate();
    }
    return rv;
}

#endif /* __gen_nsIBinaryInputStream_h__ */
