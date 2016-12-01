/* awsFillAndSign.c Copyright 2016 MIB SOFTWARE, INC.

 PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4.
  
 LICENSE:   MIT (Free/OpenSource)
 
 PLATFORM:  C on GCC/MSVC/Clang/BSD/Linux/Windows.
 
 PACKAGE:   Compiles as a utility and a library function. Needs
                hmacsha256.c librock_sha256.c
            See details at [[Header include and compatibility]], below.
                
 STABLITY:  UNSTABLE as of 2016-11-22
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/
#if !defined(LIBROCK_UNSTABLE) //Guard for the entire file.
#   error "E-58 This file came from the unstable branch of librock. You have not defined LIBROCK_UNSTABLE."
#else
/*
The MIT License (MIT)

Copyright (C) 2016 MIB SOFTWARE INC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/*
*******************************************************************************
TABLE OF CONTENTS FOR THE REST OF THIS FILE
*******************************************************************************
    [[HOW TO USE]]
        HOW TO use this as a command-line utility
        HOW TO use this as a library function
        
    [[Header include and compatibility]]
        #include "librock_preinclude.h"
        #include <stdlib.h>, <sys/types.h>, <time.h>, <string.h>
        #if defined LIBROCK_WANT_INCLUDE_PTHREAD: <pthread.h>
        #if defined(LIBROCK_AWSFILLANDSIGN_MAIN): <stdio.h>, <fcntl.h>, "librock_fdio.h"
        #include "librock_postinclude.h"
        
    [[Predeclarations]]

    [[Predeclarations of functions from librock_sha256.c]]
        librock_sha256Init, librock_sha256Update, librock_sha256StoreFinal
    
    [[Predeclarations of functions from hmacsha256.c]]
        librock_hmacSha256

    [[standard templates]]

    [[function main()]]  #if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
               
    
    [[Implementation]]
    
        function librock_awsFillAndSign  The Workhorse
        
    [[PRIVATE implementation functions]]
        
    [[PRIVATE Utility Functions]]
        
*/
/********************************************************************************
[[HOW TO USE]]

   HOW TO use this as a command-line utility:
       1. Compile using -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN.
       This compiles as a command-line (console) utility on Windows,
       Linux, *BSD, using MSVC, gcc, or clang. E.g. To compile for gcc:
       gcc -o awsFillAndSign -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN -Werror -Wall awsFillAndSign.c hmacsha256.c librock_sha256.c 
       
       2. Create a template file in CURL config or RAW format, with numbered
       replaceable parts (inside @ @).  For example.
            @//Template AWS S3 LIST request, with a limit on results returned.
            url="https://@1_bucket@.s3.amazonaws.com/?prefix=@2_object@&max-keys=@3_max@&list-type=2"
            header = "Host: @1_bucket@.s3.amazonaws.com"
            header = "Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)"
            header = "x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
       
       3. Specify the template file name as the first command line parameter.
       
       4. Provide other template parameters in order. (In this example the bucket,
       the prefix, and the max-keys.)
       
       5. Give AWSCredentials scope,id,secret (comma separated, no spaces) on the input.
       
       6. Redirect the output to a file, that you provide to CURL.
       
       See github for more examples and the full set of templates for AWS S3 and
       other AWS services.
       
   HOW TO use this as a library function:
       You want to call librock_awsFillAndSign(), see details below.
 */
/**************************************************************/
//[[Header include and compatibility]]
#define LIBROCK_SRC_AWSFILLANDSIGN 1 //Identify the source module
#define LIBROCK_WANT_GMTIME_R 1 //We need this
#define LIBROCK_WANT_STRCASE 1 //We need this
#include "librock_preinclude.h"
/*************************************************************
A note about librock_preinclude.h and librock_postinclude.h:

   LIBROCK sources are designed to be very portable and very stable.
   Conditional includes in this file are based only on variables with a prefix
   of LIBROCK_.
   
   All compiler and platform-specific changes are to be accomplished in
   librock_preinclude.h and librock_postinclude.h.
   
For the purposes of awsFillAndSign, librock_preinclude should have
accomplished something like the following in order to have the
unchanged sources compile portably with MSVC, GCC, and CLANG on
Windows, and GCC on Linux and *BSD:
    #if !defined LIBROCK_WANT_GMTIME_R
    #elif defined __MINGW32__  && defined __MSVCRT__
    #   define LIBROCK_WANT_INCLUDE_PTHREAD
    #elif defined _MSC_VER
    #   define LIBROCK_WANT_GMTIME_R_FOR_MSC_VER
    #endif
    #if !defined LIBROCK_WANT_STRCASE
    #elif defined _MSC_VER
    #   define LIBROCK_WANT_STRCASE_FOR_MSC_VER
    #endif
    #if defined _MSC_VER
    #	define LIBROCK_WANT_GETENV_S_FOR_MSC_VER
    #endif
*/
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#if defined LIBROCK_WANT_INCLUDE_PTHREAD
#   include <pthread.h>
#endif
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
#   include <stdio.h> /* fwrite, fprintf */
#   include <fcntl.h>
#   include "librock_fdio.h" /* portable _open, read, close */
#endif


#include "librock_postinclude.h"

#if defined LIBROCK_WANT_STRCASE_FOR_MSC_VER
#   define strcasecmp _stricmp
#   define strncasecmp _strnicmp
#endif

#if defined LIBROCK_WANT_GMTIME_R_FOR_MSC_VER
    struct tm *gmtime_r(time_t *t, struct tm *p_tm) {
        gmtime_s(p_tm, t); /* NOTE: parameter order in _MSC_VER version is reversed compared to standard 
            https://connect.microsoft.com/VisualStudio/feedback/details/793092/gmtime-s-non-conformant
        */
        return p_tm;
    }
#endif



/********************************************************************/
//[[Predeclarations]]
#if !defined(PRIVATE)
#   define PRIVATE static /* names with module-level scope */
#endif
    
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)    // Compile as command line utility
    int main(int argc, char **argv);
    PRIVATE int write_to_FILE(void *id, const char *pSource, int count); /* helper function */
#endif

    /* Declare the workhorse function, which is public.  */
    const char *librock_awsFillAndSign( //Compute an AWS Version 4 signature
        const char *pRequestString //CURL one option per line, or HTTP headers one per line.
        ,const unsigned char *pMdContent32 /*32 bytes: the SHA 256 of the body, or NULL if
                                            the request has an upload-file */
        ,const char *pAwsCredentialsString //Account ID, secret key
        ,int (*fnOutput)(void *outputId, const char *pSource, int count) //called to write output
        ,void *outputId
        ,int (*fnDebugOutput)(void *outputId, const char *pSource, int count) //called to write output
        ,void *debugOutputId
        );
    
    /* Declare struct for the parsed request. Callers should treat it as opaque, 
       because it is subject to change. 
     */
    struct librock_awsRequest_s;

    /* (private) Implementing functions in this modules. 
            NOTE: Callers should use librock_awsSignatureVersion4(), not these. They
            are subject to change.
    */
    PRIVATE void librock_awsRequest_start_(
                        struct librock_awsRequest_s *pRequest,
                        const char *pRequestWithoutBody,
                        unsigned const char *pMdContent32,
                        const char *pAwsCredentialsCsv);
    PRIVATE const char *librock_awsSignature_findCredentials_(
                        struct librock_awsRequest_s *pRequest);
    PRIVATE const char *librock_awsSignature_parseRequest_(
                        struct librock_awsRequest_s *pRequest);
    PRIVATE const char *librock_awsSignature_getStringToSign_(
                        struct librock_awsRequest_s *pRequest,
                        char **ppToSign);
    PRIVATE const char *librock_awsSignature_outputWithAuthorization_(
                        struct librock_awsRequest_s *pRequest,
                        const char *pToSign);
     
    /* Utility functions in this module */
    PRIVATE char *librock_fillTemplate(
                        char **ppFilled,
                        const char *pTemplate,
                        int argc,
                        char * const * const argv);
    PRIVATE int qsort_strcmp_(const void *s1, const void *s2);  /* For sorting query parameters */
    PRIVATE int qsort_strcasecmp_(const void *pString1, const void *pString2);  /* For sorting HTTP headers */
    PRIVATE int countToCh(const char *pRead, char ch);
    PRIVATE int countToEol(const char *pRead); /* count to \n or \0 */
    PRIVATE int countOptionName(const char *pRead); /* count to ' ' or '=', for parsing CURL options */
    PRIVATE int countToValue(const char *pRead); /* count to value following '=', for parsing CURL options */
    struct librock_appendable {
        void *p;
        int cb;
        int iWriting;
        void (*realloc)(void *, size_t);
    };
    PRIVATE char *librock_safeAppend0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);
    
    PRIVATE void freeOnce(void **pp); /* free() a pointer and then set it NULL */
#ifdef LIBROCK_AWSFILLANDSIGN_MAIN
    PRIVATE const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32); /* store 32 bytes, returns error explanation or 0 */
    PRIVATE void *librock_fileGetContents(const char *fname); /* allocate memory, read entire file */
#endif
    PRIVATE void bToHex0(char *pWrite, unsigned char ch); /* To hexadecimal, avoid sprintf */
    PRIVATE void bToHex0_UC(char *pWrite, unsigned char ch); /* To upper case hexadecimal, avoid sprintf */
    PRIVATE const char *copyFromEnv_s(const char *pName, char *pWrite, int cMax);

/**************************************************************/
//[[Predeclarations of functions from librock_sha256.c]]

    extern int librock_sha256Init(void *c);
    extern int librock_sha256Update(void *c, const void *data_, size_t len);
    extern int librock_sha256StoreFinal(unsigned char *md, void *c);

/**************************************************************/
//[[Predeclarations of functions from librock_hmacsha256.c]]
    extern void librock_hmacSha256(
            char *pDigestResult,
            const char *pKey,
            int cKey,
            const char *pToSign,
            int cToSign);

/**************************************************************/
//[[Implementation]]
//struct for the parsed request Callers should NOT use because it is subject to change.
 struct librock_awsRequest_s {
    const char *pRequest;
    const char *pAwsCredentialsCsv;
    const char *pURI;
    const char *pHost;
    const char *pContentSha256; /* Or leave =0 */
    const char *pContentType;
    const char *pCredentialScopeWithoutDate;
    const char *pCanonicalQuery;
    char *pSignedHeaders;
    const char *pXAmzDate;
    char const * *pListOfHeaders;
    char amzDate[100];
    const char *pAuthorization;
    int bFormatCURL;
    const char *pAccessKeyId;
    const char *pKey;
    int cKey;
    const char *pVerb;
    int cHeaders;
    unsigned char mdContent32[32];
    int (*fnOutput)(void *outputId, const char *pSource, int count); //called to write output
    void *outputId;
    int (*fnDebugOutput)(void *outputId, const char *pSource, int count); //called to write output
    void *debugOutputId;
};



/**************************************************************/
/* The main workhorse function. Call this one */
const char *librock_awsFillAndSign(
        const char *pRequestWithoutBody //CURL one option per line, or HTTP headers one per line.
        ,const unsigned char *pMdContent32 /*32 bytes: the SHA 256 of the body */
        ,const char *pAwsCredentials //CSV, no spaces or quotes: CredentialScopeWithoutDate,AccountID,secret key
        ,int (*fnOutput)(void *fnOutputId, const char *pSource, int count) //called to write output
        ,void *outputId
        ,int (*fnDebugOutput)(void *fnOutputId, const char *pSource, int count) //called to write output
        ,void *debugOutputId
        )
{
    struct librock_awsRequest_s awsRequest;
    const char *pErrorMessage = 0;
    char *pToSign = 0;

    /* Initialize the structure */
    librock_awsRequest_start_(&awsRequest, pRequestWithoutBody, pMdContent32, pAwsCredentials);

    awsRequest.pListOfHeaders = 0;
    
    /* Initialize the output methods */
    awsRequest.fnOutput = fnOutput;
    awsRequest.outputId = outputId;

    awsRequest.fnDebugOutput = fnDebugOutput;
    awsRequest.debugOutputId = debugOutputId;

    /* Parse the request */
    pErrorMessage = librock_awsSignature_parseRequest_(&awsRequest);
    if (!pErrorMessage) {
        /* Parse the credentials */
        pErrorMessage = librock_awsSignature_findCredentials_(&awsRequest);
    }
    if (!pErrorMessage) {
        /* Build the string to sign, per the AWS specification */
        pErrorMessage = librock_awsSignature_getStringToSign_(&awsRequest, &pToSign);
    }
    if (!pErrorMessage) {
        /* Create the signing key and write out the signed request */
        pErrorMessage = librock_awsSignature_outputWithAuthorization_(&awsRequest, pToSign);
    }

    /* The internal functions called malloc.  Free the memory here */
    freeOnce((void **)&awsRequest.pListOfHeaders);
    freeOnce((void **)&awsRequest.pSignedHeaders);
    freeOnce((void **)&pToSign);
    
    return pErrorMessage;

} /* librock_awsSignature */
/**************************************************************/

//[[PRIVATE implementation functions]]

PRIVATE void librock_awsRequest_start_(
    struct librock_awsRequest_s *pRequest,
    const char *pRequestWithoutBody,
    const unsigned char *pMdContent32,
    const char *pAwsCredentialsCsv)
{ //Callers should use librock_awsSignatureVersion4(), not this.
    pRequest->pURI = 0;
    pRequest->pHost = 0;
    pRequest->pContentSha256 = 0;
    pRequest->pContentType = 0;
    pRequest->pXAmzDate = 0;
    pRequest->pAuthorization = 0;
    pRequest->bFormatCURL = 0;
    pRequest->pVerb = 0;
    pRequest->cHeaders = 0;
    pRequest->pRequest = pRequestWithoutBody;
    pRequest->pAwsCredentialsCsv = pAwsCredentialsCsv;
    pRequest->pCredentialScopeWithoutDate = 0;
    pRequest->pCanonicalQuery = 0;
    pRequest->pSignedHeaders = 0;
    pRequest->pAccessKeyId = 0;
    { /* prep the amzDate, in case we need it */
        time_t now;
        struct tm gmt;
        time(&now);
        gmtime_r(&now, &gmt);
        strftime(pRequest->amzDate, sizeof pRequest->amzDate, "X-Amz-Date:%Y%m%dT%H%M%SZ", &gmt);
    }
    if (pMdContent32) {
        memmove(pRequest->mdContent32, pMdContent32, 32);
        pRequest->pContentSha256 = (void *) pRequest->mdContent32;
    }
} /* librock_awsRequest_start */
/**************************************************************/


/**************************************************************/
PRIVATE const char *librock_awsSignature_findCredentials_(struct librock_awsRequest_s *pRequest)
{//Callers should use librock_awsSignatureVersion4(), not this.
    /* Set ->pKey and ->cKey */

    pRequest->pKey = pRequest->pAwsCredentialsCsv;
    if (!pRequest->pKey) {
        return("E-316 need credentials in AWS IAM format");
    }
    
    /* The Credential Scope */
    pRequest->pCredentialScopeWithoutDate = pRequest->pAwsCredentialsCsv;

    pRequest->pKey = strchr(pRequest->pKey, ',');
    if (!pRequest->pKey) {
        return("E-316 need credentials in AWS IAM format");
    }
    pRequest->pKey++;

    /* Access key */
    pRequest->pAccessKeyId = pRequest->pKey;

    pRequest->pKey = strchr(pRequest->pKey, ',');
    if (!pRequest->pKey) {
        return("E-316 need credentials in AWS IAM format");
    }
    pRequest->pKey++;
    pRequest->cKey = countOptionName(pRequest->pKey);

    return 0;
} /* librock_awsSignature_findCredentials */
/**************************************************************/



/**************************************************************/
PRIVATE const char *librock_awsSignature_parseRequest_(struct librock_awsRequest_s *pRequest)
{//Callers should use librock_awsSignatureVersion4(), not this.
    const char *pRead = pRequest->pRequest;
    const char *pStartParse;
    int iPass;

    if (!pRead) {
        return("E-309 need request");
    }


    if (strstr(pRequest->pRequest, "\nheader=")||strstr(pRequest->pRequest, "\nheader =")) {
        /* detected cURL --config format*/
        pRequest->bFormatCURL = 1;

        if (pRequest->fnDebugOutput) {
            const char *pLiteral = "I-171 Detected CURL\n";
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
        }

    } else {
        /* We will parse and output text headers */
        pRequest->pVerb = pRead;
        while (*pRead > ' ') {
            pRead++;
        }
        if (*pRead) {
            pRead++;
        }
        pRequest->pURI = pRead;
        while (*pRead && *pRead != '\n') {
            pRead++;
        }
        if (*pRead) pRead++;
    }
    iPass =0;
    pStartParse = pRead;
    while (iPass <= 1) {
        pRead = pStartParse;
        pRequest->cHeaders = 0;
        while (*pRead) {

            if (pRequest->bFormatCURL) {
                if (!strncmp(pRead, "upload-file", countOptionName(pRead))) {
                    if (iPass == 0) {
                        pRequest->pVerb = "PUT";
                    }
                    goto skip_line;
                } else if (!strncmp(pRead, "request", countOptionName(pRead))) {
                    pRead += countToValue(pRead);
                    if (iPass == 0) {
                        if (*pRead && *pRead != '\n') {
                            pRequest->pVerb = pRead;
                        }
                    }
                    goto skip_line;
                } else if (!strncmp(pRead, "url", countOptionName(pRead))) {
                    if (iPass == 0) {
                        pRead += countToValue(pRead);
                        if (*pRead && *pRead != '\n') {
                            pRequest->pURI = pRead;
                            if (!strncasecmp(pRead, "http://", 7)||
                                !strncasecmp(pRead, "https://", 8)) {
                                pRead = strchr(pRead, ':');
                                pRead += 3;
                                if (!pRequest->pHost) {
                                    pRequest->pHost = pRead;
                                }
                                while (*pRead && *pRead != '\n' && *pRead != '/') {
                                    pRead++;
                                }
                            }

                            if (pRequest->fnDebugOutput) {
                                const char *pLiteral = "I-222\n";
                                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
                            }

                        }
                    }
                    goto skip_line;
                } else if (strncmp(pRead, "header", countOptionName(pRead))) {
                    goto skip_line;
                }
                pRead += countToValue(pRead);
            }
            if (iPass == 0) {
                if (!strncasecmp(pRead, "x-amz-date:", 11)) {
                    pRequest->amzDate[0] = '\0';
                }
            }
            if (!strncasecmp(pRead, "authorization:", 14)) {
                /* Do not sign */
            } else if (1/*sign everything*/) {
                if (iPass==1) {
                    pRequest->pListOfHeaders[pRequest->cHeaders] = pRead;
                }
                pRequest->cHeaders++;
            }
            if (iPass != 0) {

            } else if (!strncasecmp(pRead, "authorization:", 14)) {
                pRequest->pAuthorization = pRead;
            } else if (!strncasecmp(pRead, "x-amz-content-sha256:", 21)) {
                pRead += 21;
                if (*pRead == ' ') pRead++;
                if (pRequest->pContentSha256 != (void *) pRequest->mdContent32) {
                    pRequest->pContentSha256 = pRead;
                }
            } else if (!strncasecmp(pRead, "content-type:", 13)) {
                pRead += 13;
                if (*pRead == ' ') pRead++;
                pRequest->pContentType = pRead;
            }
skip_line:
            while (*pRead && *pRead != '\n') {
                pRead++;
            }
            if (*pRead) pRead++; /* To next line */
        }
        if (pRequest->amzDate[0]) {
            /* Date was not found in the supplied headers.  Supply it. */
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = pRequest->amzDate;
            }
            pRequest->cHeaders++;
            
        }
        if (iPass == 0) {
            if (pRequest->cHeaders) {
                pRequest->pListOfHeaders = malloc(sizeof(char *) * pRequest->cHeaders);
                if (!pRequest->pListOfHeaders) {
                     return("E-523 malloc failed");
                }
            } else {
                iPass = 2;
            }
        }
        iPass++;
    }
    if (pRequest->bFormatCURL) {
        if (!pRequest->pVerb) {
            pRequest->pVerb = "GET";
        }
    }
    return 0;
} /* librock_awsSignature_parseRequest */
/**************************************************************/


/**************************************************************/
PRIVATE void librock_awsSignature_dump2_(struct librock_awsRequest_s *pRequest, const char *name, const char *pSource)
{ /* Special purpose dump routine to generate debugging output.
   * IMPORTANT: Call only when fnDebugOutput is set-up.
   */
    const char *pRead = pSource;
    const char *pLiteral = "#<";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = ">";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));

    while (*pRead) {
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pRead, countToEol(pRead));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\\n\n", 3);
        pRead += countToEol(pRead);
        if (*pRead) {
            pRead++;
        }
    }

    pLiteral = "</";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = ">\n";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));

    pLiteral = "<";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    pRead = pSource;
    while (*pRead) {
        char buffer[3];
        bToHex0(buffer, *pRead & 0xff);
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, buffer, strlen(buffer));
        pRead++;
        if (*pRead) {
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, " ", 1);
        }
    }
    pLiteral = "</";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>\n";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
} /* librock_awsSignature_dump2_ */
/**************************************************************/

PRIVATE const char *librock_awsSignature_canonicalQueryString_(struct librock_appendable *paParameter, const char **ppQueryString, int bFormatCURL)
{
    /* Canonicalize the % encoding into aParameter, then make a sorted copy, and store again to aParameter */
    const char *startChunk;
    int cParameters = 1;
    const char *pSource = *ppQueryString;
    int iStartString = paParameter->iWriting;
    
    if (*pSource == '?') {
        pSource++;
    }
    /* For efficiency, work in chunks */
    startChunk = pSource;
    while (*pSource > ' ') {
        char ch;
        if (bFormatCURL && *pSource == '\x22') {
            break;
        }
        ch = *pSource;
        if (ch == '%' && pSource[1] && pSource[2]) {
            if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
                return "E-601 would overflow pre-allocated block";
            }
            ch = 0;
            if (pSource[1] > '9') {
                ch += (pSource[1]&0x0f)+0x09;
            } else {
                ch += (pSource[1]&0x0f);
            }
            ch *= 16;
            if (pSource[2] > '9') {
                ch += (pSource[2]&0x0f)+0x09;
            } else {
                ch += (pSource[2]&0x0f);
            }
            if ((ch >= '0' && ch <= '9')
                ||(ch == '.')
                ||(ch >= 'A' && ch <= 'Z')
                ||(ch >= 'a' && ch <= 'z')
                ||(ch == '-')
                ||(ch == '_')
                ||(ch == '~')) {
                /* Not URL-encoded in the canonical query string */
                if (!librock_safeAppend0(paParameter, &ch, 1)) { 
                    return "E-601 would overflow pre-allocated block";
                }
            } else {
                /* Must be URL-encoded. Make sure it is upper case */
                if (!librock_safeAppend0(paParameter, "%XX", 3)) { 
                    return "E-601 would overflow pre-allocated block";
                }
                bToHex0_UC((char *) paParameter->p + paParameter->iWriting-2, ch & 0xff);
            }
            pSource += 3;
            startChunk = pSource;
        } else if ((*pSource >= '0' && *pSource <= '9')
            ||(*pSource == '.')
            ||(*pSource >= 'A' && *pSource <= 'Z')
            ||(*pSource >= 'a' && *pSource <= 'z')
            ||(*pSource == '-')
            ||(*pSource == '_')
            ||(*pSource == '~')
            ||(*pSource == '=') //Caller means this = not encoded.
        ) {
            pSource++;
        } else if (*pSource == '&') { //Caller means this not encoded.
            cParameters += 1;
            pSource++;
        } else {
            /* Append the string so far */
            if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
                return "E-601 would overflow pre-allocated block";
            }
            /* URI encoded */
            if (!librock_safeAppend0(paParameter, "%XX", 3)) { 
                return "E-601 would overflow pre-allocated block";
            }
            bToHex0_UC((char *) paParameter->p + paParameter->iWriting-2, *pSource & 0xff);
            pSource++;
            startChunk = pSource;
        }
    }
    /* Append rest of string */
    if (pSource > startChunk) {
        if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
            return "E-601 would overflow pre-allocated block";
        }
    }
    /* Store the end location */
    if (bFormatCURL && *pSource == '\x22') {
        pSource++;
    }
    *ppQueryString = pSource;
    
    if (cParameters > 1) { /*sort*/
        char **pList = 0;
        char *pCopy = 0;
        char *pFix;
        int i = 0;
        pList = malloc(cParameters * sizeof(char *));
        if (!pList) {
            return "E-680 malloc failed";
        }
        pCopy = malloc(strlen((char*)paParameter->p + iStartString)+1);
        if (!pCopy) {
            freeOnce((void **)&pList);
            return "E-683 malloc failed";
        }
        pFix = pCopy;
        
        memmove(pCopy, (char*)paParameter->p + iStartString,strlen((char*)paParameter->p + iStartString)+1);
        /* First turn '=' into spaces for easy sorting */
        while((pFix = strchr(pFix, '='))) {
            *pFix++ = ' ';
        }
        /* Now build a list, splitting at '&' */
        pFix = pCopy;
        while(pFix) {
            pList[i++] = pFix;
            pFix = strchr(pFix,'&');
            if (pFix) {
                *pFix++ = '\0';
            }
        }

        /* Sort */
        qsort((void *) &pList[0], cParameters, sizeof(char **), qsort_strcmp_);
        
        /* Rest the iWriting index, and join in sorted order, separated by '&'  */
        paParameter->iWriting = iStartString;
        for(i = 0; i < cParameters; i++) {
            if (i > 0) {
                if (!librock_safeAppend0(paParameter, "&", 1)) {
                    return "E-601 would overflow pre-allocated block";
                }
            }
            if (!librock_safeAppend0(paParameter, pList[i], strlen(pList[i]))) { 
                return "E-601 would overflow pre-allocated block";
            }
        }

        /* convert spaces back to = */
        pFix = (char *) paParameter->p + iStartString;
        while((pFix = strchr(pFix, ' '))) {
            *pFix++ = '=';
        }
        freeOnce((void **)&pList);
        freeOnce((void *)&pCopy);
    }
    return 0;
} /* librock_awsSignature_canonicalQueryString_ */

/**************************************************************/
PRIVATE const char *librock_awsSignature_getStringToSign_(struct librock_awsRequest_s *pRequest,char **ppToSign)
{//Callers should use librock_awsRest_sign_v4(), not this.
    /* Version 4:
    
StringToSign  =
Algorithm + '\n' +
RequestDate + '\n' +
CredentialScope + '\n' +
HashedCanonicalRequest))

E.g.
AWS4-HMAC-SHA256
20150830T123600Z
20150830/us-east-1/iam/aws4_request
f536975d06c0309214f805bb90ccff089219ecd68b2577efef23edd43b7e1a59
    
    CanonicalRequest =
        HTTPRequestMethod + '\n' +
        CanonicalURI + '\n' +
        CanonicalQueryString + '\n' +
        CanonicalHeaders + '\n' +
        SignedHeaders + '\n' +
        HexEncode(Hash(RequestPayload))
    */
    const char *pRead;
    unsigned char mdCanonicalRequest[32];
    void *pHashInfo = 0;
    struct librock_appendable aParameter;

    const char *pCanonicalRequestTemplate = 
        "@1_VERB@\n"
        "@2_URI@\n"
        "@3_QueryString@\n"
        "@4_Headers@\n"
        "@5_SignedHeaders@\n"
        "@6_PayloadHash@" //No trailing \n
        ;

    char *pParameterList[7]; /* pParameterList[0] unused */
    /* Build the Canonical Request. */
    /* We are going to build stringtosignbytes afto digest with hmac-sha256 */

    memset(&aParameter, '\0', sizeof(aParameter));
    if (*ppToSign != 0) {
        /* We are going to overwrite if successful. Caller needs to initialize to 0. */
        return "E-775 Invalid parameter";
    }
    
    if (pRequest->fnDebugOutput) {
        const char *pLiteral = "I-330 ";
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pRequest->pRequest, strlen(pRequest->pRequest));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);
    }

    /* Overallocate, ensuring room for all the headers, amzDate, payload hash.
    We multiply the strlen(pRequest->pURI) as a worst case for expanding out '/' to %2F
    */
    aParameter.cb = countToEol(pRequest->pURI)*3 + strlen(pRequest->pRequest)*2+200+1; 
    //  This buffer is also reused to create stringToSign, which gets parts of the credentials.
    aParameter.cb += strlen(pRequest->pAwsCredentialsCsv);
    aParameter.p = (char *) malloc(aParameter.cb); 
    if (!aParameter.p) {
        return "E-462 malloc failed";
    }
    
    /* No allocator in aParameter. Store pointers into the buffer, it won't move. */
    pParameterList[1/*Verb*/] = (char*)aParameter.p + aParameter.iWriting;
    if (!librock_safeAppend0(&aParameter, pRequest->pVerb, countOptionName(pRequest->pVerb))) {
        freeOnce((void **) & aParameter.p);
        return "E-470 would overflow pre-allocated block";
    }
    if (((char *) aParameter.p)[aParameter.iWriting-1] == '\x22') {/* Maybe check for \x22 only if bFormatCURL? */
        aParameter.iWriting--;
        ((char *) aParameter.p)[aParameter.iWriting] = '\0';
    }
    aParameter.iWriting++; /* Retain the terminating \0. */

    /* Canonical URI */
    pParameterList[2/*Canonical URI*/] = (char*)aParameter.p + aParameter.iWriting;
    pRead = pRequest->pURI;
    if (!strncasecmp(pRead, "http:", 5) || !strncasecmp(pRead, "https:", 6)) {
        pRead = strchr(pRead, ':')+1;
        if (!strncasecmp(pRead, "//", 2)) {
            pRead += 2;
            while (*pRead > ' ' && *pRead != '?' && *pRead != '/') {
                if (pRequest->bFormatCURL && *pRead == '\x22') {
                    break;
                }
                pRead++;
            }
            
        }
    }
    
    { /* Copy up to start of query part */
        const char *pStart = pRead;
        while (*pRead > ' '  && *pRead != '?') {
            if (pRequest->bFormatCURL && *pRead == '\x22') {
                break;
            }
            pRead++;
        }
        if (!librock_safeAppend0(&aParameter, pStart, pRead - pStart)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        aParameter.iWriting++; /* Retain the terminating \0. */
    }

    /* Canonical Query String. */
    pParameterList[3/*Query String*/] = (char*)aParameter.p + aParameter.iWriting;
    if (*pRead == '?') {
        const char *pErrorMessage =librock_awsSignature_canonicalQueryString_(&aParameter,&pRead,pRequest->bFormatCURL);
        if (pErrorMessage) {
            return pErrorMessage;
        }
    } else {
        if (!librock_safeAppend0(&aParameter, "", 0)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
    }
    pRead = 0;
    aParameter.iWriting++; /* Retain the terminating \0. */
    
    /* CanonicalHeaders */
    
    /* Sorted headers */
    pParameterList[4/*Canonical Headers*/] = (char*)aParameter.p + aParameter.iWriting;
    if (pRequest->cHeaders) {
        const char *pHeader;
        int i = 0;
        qsort((void *)pRequest->pListOfHeaders, pRequest->cHeaders, sizeof(char **), qsort_strcasecmp_);
        while (i < pRequest->cHeaders) {
            int bContent;
            pHeader = pRequest->pListOfHeaders[i];
            if (!strncasecmp(pHeader, "x-amz-date:", 11)) {
                pRequest->pXAmzDate = strchr(pHeader, ':')+1;
                while (*(pRequest->pXAmzDate)==' ') {
                    pRequest->pXAmzDate += 1;
                }
            }
            bContent = 0;
            if (!strncasecmp(pHeader, "x-amz-content-sha256:", 21) && (pRequest->pContentSha256 == (void *) pRequest->mdContent32)) {
                bContent = 1;
            }
            if (0) { //Useful for development
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "I-846:", 6);
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pHeader, countToEol(pHeader));
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);
            }
            /* lower-casify for signing */
            while (*pHeader && *pHeader != ':') {
                char *pWrite = (char *) aParameter.p + aParameter.iWriting;
                if (!librock_safeAppend0(&aParameter, "X", 1)) { 
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                if (*pHeader >= 'A' && *pHeader <= 'Z') {
                    *pWrite = *pHeader + 'a' - 'A';
                } else {
                    *pWrite = *pHeader;
                }
                pHeader++;
            }
            if (!librock_safeAppend0(&aParameter, ":", 1)) { 
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            if (*pHeader) { pHeader++; }
            while (*pHeader== ' '||*pHeader == '\t') {
                pHeader++;
            }
            if (bContent) {
                int i = 0;
                for (i =0;i < 32;i++) {
                    char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 2);
                    if (!pWrite) {
                        freeOnce((void **) & aParameter.p);
                        return "E-470 would overflow pre-allocated block";
                    }
                    bToHex0(pWrite, pRequest->mdContent32[i] & 0xff);
                }
            } else {
                const char *pStart = pHeader;
                while (*pHeader && *pHeader != '\n' && *pHeader != '\r') {
                    if (pRequest->bFormatCURL && *pHeader == '\x22') {
                        break;
                    }
                    pHeader++;
                    if (((*pHeader&0xff) <= ' ') && pHeader[-1]==' ') {
                        /* Need to elide spaces */
                        if (!librock_safeAppend0(&aParameter, pStart, pHeader - pStart)) {
                            freeOnce((void **) & aParameter.p);
                            return "E-470 would overflow pre-allocated block";
                        }
                        aParameter.iWriting--;
                        pStart = pHeader;
                    }
                }
                if (pHeader > pStart) {
                    if (!librock_safeAppend0(&aParameter, pStart, pHeader - pStart)) { 
                        freeOnce((void **) & aParameter.p);
                        return "E-470 would overflow pre-allocated block";
                    }
                }
                
            }
            if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            i++;
        }
        /* Strip the last newline */
        aParameter.iWriting--;
        if (!librock_safeAppend0(&aParameter, "", 0)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        aParameter.iWriting++; /* Retain the terminating \0. */
    }


    pParameterList[5/*Names of Signed Headers*/] = (char*)aParameter.p + aParameter.iWriting;
    /* All of the headers we have will be signed. If caller or proxies add
       headers later, they will not be signed. */
    {
        unsigned iStart;
        int i = 0;
        int cSigned;
        if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        iStart = aParameter.iWriting;
        
        while (i < pRequest->cHeaders) {
            const char *pHeader;
            if (i > 0) {
                if (!librock_safeAppend0(&aParameter, ";", 1)) { 
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
            }
            pHeader = pRequest->pListOfHeaders[i];
            while (*pHeader && *pHeader != ':') {
                char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 1);
                if (!pWrite) {
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                *pWrite = *pHeader;
                if (*pHeader >= 'A' && *pHeader <= 'Z') {
                    *pWrite = *pHeader + 'a' - 'A';
                }
                pHeader++;
            }
            i++;
        }
        cSigned = aParameter.iWriting - iStart;

        /* Make a copy of these */
        pRequest->pSignedHeaders = malloc(cSigned+1);
        if (!pRequest->pSignedHeaders) {
            freeOnce((void **) & aParameter.p);
            return "E-451 malloc failed";
        }
        memmove(pRequest->pSignedHeaders, (char*)aParameter.p + iStart, cSigned);
        pRequest->pSignedHeaders[cSigned] = '\0';
        
        /* Keep the \0 and start the next string*/
        if (!librock_safeAppend0(&aParameter, "\0", 1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
    
    }
    
    /* Hash of the Request Payload */
    pParameterList[6/*Hash of payload*/] = (char *) aParameter.p + aParameter.iWriting;
    {
        pRead = pRequest->pContentSha256;
        if (pRead == (void *) pRequest->mdContent32) {
            int i = 0;
            for (i = 0; i < 32; i++) {
                char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 2);
                if (!pWrite) {
                    freeOnce((void **) & pRequest->pSignedHeaders);
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                bToHex0(pWrite, pRequest->mdContent32[i] & 0xff);
            }
        } else {
            if (!librock_safeAppend0(&aParameter, pRead, countToEol(pRead))) {
                freeOnce((void **) & pRequest->pSignedHeaders);
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
        }
    }
    
    if (((char *) aParameter.p)[aParameter.iWriting-1] == '\x22') {/* Maybe check for \x22 only if bFormatCURL? */
        aParameter.iWriting--;
    }
    ((char *) aParameter.p)[aParameter.iWriting] = '\0';
    
    { /* Create the Canonical request from template */
        char *pCanonicalRequest;
        const char *pErrorMessage =librock_fillTemplate(&pCanonicalRequest, pCanonicalRequestTemplate, 7, &pParameterList[0]);
        if (pErrorMessage) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return pErrorMessage;
        }

        /* Compute hash */
        pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
        if (!pHashInfo) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-1059 malloc failed";
        }
        librock_sha256Init(pHashInfo);
        librock_sha256Update(pHashInfo, (unsigned char *) pCanonicalRequest, strlen(pCanonicalRequest));
        librock_sha256StoreFinal(mdCanonicalRequest, pHashInfo);
        freeOnce((void **)&pHashInfo);

        /* mdCanonicalRequest has the SHA256 of the canonical request */

        if (pRequest->fnDebugOutput) {
            librock_awsSignature_dump2_(pRequest, "CanonicalRequest", pCanonicalRequest);
        }
        
    }
    {
        /* Now the request to sign. Reuse the buffer. It's big enough. */
        aParameter.iWriting = 0;
        
        /*Algorithm Designation */
        if (!librock_safeAppend0(&aParameter, "AWS4-HMAC-SHA256\n", -1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* date-time from the header */
        if (!librock_safeAppend0(&aParameter, pRequest->pXAmzDate, countToEol(pRequest->pXAmzDate))) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }

        if (aParameter.iWriting && ((char *) aParameter.p)[aParameter.iWriting-1]== '\"') {
            aParameter.iWriting--;
        }
        if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* Credential scope starts with the date from the XAmzDate header, truncated at "T" */
        {
            int position = aParameter.iWriting;
            if (!librock_safeAppend0(&aParameter, pRequest->pXAmzDate, countToEol(pRequest->pXAmzDate))) {
                freeOnce((void **) & pRequest->pSignedHeaders);
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            if (strchr((char *) aParameter.p + position, 'T')) {
                /* Truncate here */
                aParameter.iWriting = strchr((char *) aParameter.p + position, 'T') - (char *) aParameter.p;
                
                /* Should be .iWriting going backwards, but we validate we are within bounds */
                if (aParameter.iWriting < 0  || aParameter.iWriting + 1 > aParameter.cb) {
                    freeOnce((void **) & pRequest->pSignedHeaders);
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }

            }
        }
        
        if (*(pRequest->pCredentialScopeWithoutDate)!= '/') {
            if (!librock_safeAppend0(&aParameter, "/", 1)) { 
                freeOnce((void **) & pRequest->pSignedHeaders);
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
        }
        if (!librock_safeAppend0(&aParameter, pRequest->pCredentialScopeWithoutDate, countToCh(pRequest->pCredentialScopeWithoutDate, ','))) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        if (!librock_safeAppend0(&aParameter, "/aws4_request\n", -1)) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* Hashed canonical request */
        {
            unsigned int i;
            for (i = 0; i < sizeof(mdCanonicalRequest); i++) {
                char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 2);
                if (!pWrite) {
                    freeOnce((void **) & pRequest->pSignedHeaders);
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                bToHex0(pWrite, mdCanonicalRequest[i] & 0xff);
            }
        }

        if (pRequest->fnDebugOutput) {
            librock_awsSignature_dump2_(pRequest, "StringToSign", aParameter.p);
        }
    }
    *ppToSign = aParameter.p; // Caller will free
    aParameter.p = 0;

    return 0;
} /* librock_awsSignature_getStringToSign */
/**************************************************************/


/**************************************************************/
PRIVATE const char *librock_awsSignature_outputWithAuthorization_(struct librock_awsRequest_s *pRequest, const char *pToSign)
{ /* Output with Authorization header*/
    char resultSha[32];
    const char *pLiteral = "";

    /*  Pseudocode for deriving a signing key
            Using kSecret = Your AWS Secret Access Key
    //E.g.  pRequest->pKey = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY"; // Secret
    //  pRequest->cKey = strlen(pRequest->pKey);

            kDate = HMAC("AWS4" + kSecret, Date)
            kRegion = HMAC(kDate, Region)
            kService = HMAC(kRegion, Service)
            kSigning = HMAC(kService, "aws4_request")
    */
    { /* kDate = HMAC("AWS4" + kSecret, Date) */
        int cbBuf = pRequest->cKey+4;
        char *pConcatKey = (char *) malloc(cbBuf+1);
        if (!pConcatKey) {
            return "E-590 malloc failed";
        }
        memmove(pConcatKey, "AWS4", 4);
        memmove(pConcatKey+4, pRequest->pKey, pRequest->cKey);
        pConcatKey[cbBuf] = '\0';

        librock_hmacSha256(resultSha,
                            pConcatKey,
                            4+pRequest->cKey,
                            pRequest->pXAmzDate,
                            strchr(pRequest->pXAmzDate,'T')-pRequest->pXAmzDate);
        freeOnce((void **)&pConcatKey);
    }
    
    {  
        const char *pRead = pRequest->pCredentialScopeWithoutDate;
        if (*pRead == '/') {
            pRead++;
        }
        if (!strchr(pRead, '/')) {
            return "E-596 unexpected format of credential scope";
        }
        
        /* kRegion = HMAC(kDate, Region) */
        librock_hmacSha256(resultSha, resultSha, 32, pRead, strchr(pRead, '/')-pRead);
        pRead = strchr(pRead, '/')+1;

        /* kService = HMAC(kRegion, Service) */
        librock_hmacSha256(resultSha, resultSha, 32, pRead, countToCh(pRead, ','));
        
        /* kSigning = HMAC(kService, "aws4_request")*/
        librock_hmacSha256(resultSha, resultSha, 32, "aws4_request", 12);

        /* resultSha has the signing key */
        if (pRequest->fnDebugOutput) {
            unsigned int i;
            pLiteral = "I-608 ";
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
            for (i = 0; i < sizeof(resultSha); i++) {
                char buf[3];
                bToHex0(buf, resultSha[i] & 0xff);
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, buf, strlen(buf));
            }
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);

        }
    }

    /* Use the signing key which in resultSha, and write the result back to resultSha */
    librock_hmacSha256(resultSha, resultSha, 32, pToSign, strlen(pToSign));

    /* resultSha has the signature. Output with Authorization */
    {
        const char *pHeader = pRequest->pRequest;
        while (*pHeader) {
            const char *pStart = pHeader;
            pHeader += countToEol(pHeader);
            if (pRequest->bFormatCURL) {
                if (!strncmp(pStart, "header", countOptionName(pStart))) {
                    pStart += countToValue(pStart);
                } else {
                    (*(pRequest->fnOutput))(pRequest->outputId, pStart, pHeader-pStart);
                    (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
                    if (*pHeader == '\r') pHeader++;
                    if (*pHeader) pHeader++;
                    continue;
                }
            }
            if (!strncasecmp(pStart, "authorization:", 14)) {
                /* Skip this now. Add it later */
            } else if (!strncasecmp(pStart, "x-amz-content-sha256:", 21)
                        && (pRequest->pContentSha256 == (void *) pRequest->mdContent32)) {
                unsigned int i;
                if (pRequest->bFormatCURL) {
                    pLiteral = "header = \x22x-amz-content-sha256:";
                } else {
                    pLiteral = "x-amz-content-sha256:";
                }
                (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                
                for (i = 0; i < sizeof(pRequest->mdContent32); i++) {
                    char buf[3];
                    bToHex0(buf, pRequest->mdContent32[i] & 0xff);
                    (*(pRequest->fnOutput))(pRequest->outputId, buf, 2);
    //              printf("%02x",pRequest->mdContent32[i] & 0xff);
                }           
    //          fwrite(pStart,1,pHeader-pStart,stdout);
                if (pRequest->bFormatCURL) {
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);           
                }
                (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
            } else {
                if (pRequest->bFormatCURL) {
                    pLiteral = "header = \x22";
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                }
                (*(pRequest->fnOutput))(pRequest->outputId, pStart, pHeader-pStart);
                if (pRequest->bFormatCURL && pHeader[-1] != '\x22') {
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);
                }
                (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
            }
            if (*pHeader == '\r') {
                pHeader++;
            }
            if (*pHeader) {
                pHeader++;
            }
        }
        /* Authorization header */
        if (pRequest->bFormatCURL) {
            pLiteral = "header = \x22";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        }
        pLiteral = "Authorization:";
        (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));

        pLiteral = "AWS4-HMAC-SHA256 Credential=";
        (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pAccessKeyId, (int) (strchr(pRequest->pAccessKeyId, ',')-pRequest->pAccessKeyId));
        (*(pRequest->fnOutput))(pRequest->outputId, "/", 1);
        (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pXAmzDate, (int) (strchr(pRequest->pXAmzDate, 'T')-pRequest->pXAmzDate));
        if (*(pRequest->pCredentialScopeWithoutDate) != '/') {
            (*(pRequest->fnOutput))(pRequest->outputId, "/", 1);
        }
        (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pCredentialScopeWithoutDate, countToCh(pRequest->pCredentialScopeWithoutDate, ','));
        pLiteral = "/aws4_request";
        (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        
        pLiteral = ", SignedHeaders=";
        (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pSignedHeaders, strlen(pRequest->pSignedHeaders));
        pLiteral = ", Signature=";
        (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        {
            unsigned int i;
            for (i = 0; i < sizeof(resultSha); i++) {
                char buf[3];
                bToHex0(buf, resultSha[i] & 0xff);
                (*(pRequest->fnOutput))(pRequest->outputId, buf, 2);
            }

        }
        if (pRequest->bFormatCURL) {
            (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);
        }
        (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);

        /* write an amz-date, if needed */
        if (pRequest->amzDate[0]) {
            if (pRequest->bFormatCURL == 1) {
                pLiteral = "header = \x22";
                (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                (*(pRequest->fnOutput))(pRequest->outputId, pRequest->amzDate, strlen(pRequest->amzDate));
                (*(pRequest->fnOutput))(pRequest->outputId, "\x22\n", 2);
            } else {
                (*(pRequest->fnOutput))(pRequest->outputId, pRequest->amzDate, strlen(pRequest->amzDate));
                (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
            }
        }
        /* Write the rest, if any */
        if (pHeader) {
            (*(pRequest->fnOutput))(pRequest->outputId, pHeader, strlen(pHeader));
        }
    }
    return 0;
} /* librock_awsSignature_outputWithAuthorization */
/**************************************************************/
//[[standard templates]]

const char *librock_awsStandardTemplates[] = {
    "@//awsFillAndSign_write_templates_" //Placeholder

#include "aws_templates.inc"

,0 // End of list
};

const char *librock_awsStandardTemplate(const char *pName)
{
    int i;
    int cName;
    i = 0;
    cName = strlen(pName);
    while(librock_awsStandardTemplates[i]) {
        if (countToEol(librock_awsStandardTemplates[i]+3)==cName) {
            if (!strncmp(pName,librock_awsStandardTemplates[i]+3,cName)) {
                return librock_awsStandardTemplates[i]+3+cName+1;
            }
        }
        i++;
    }
    return 0;
}

/**************************************************************/
//[[function main()]]
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)

PRIVATE int write_to_FILE(void *id, const char *pRead, int len)
{ /* Helper function */
    int cWritten = fwrite(pRead, 1, len, (FILE *)id);
    fflush((FILE *)id);
    return cWritten;
} /* write_to_FILE */

int main(int argc, char **argv)
{
    //char *credentialregion = 0;
    int argumentIndex;
    int bVerbose = 0;
    int scanSignature = 0;
    const char *credentialsFromEnv = 0;
    unsigned char mdContent32[] = { /* SHA256 of empty string */
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
    };
    const char *pRequestTemplate = 0;

    
    char credentials[200];
    char *pCredentials = credentials;
    credentials[198] = '\0';

    /* Accept command line flags preceding the template file name*/
    argumentIndex = 1;
    if (argc > argumentIndex) {
        while(argumentIndex < argc && argv[argumentIndex][0]=='-') {
            if (!strcmp(argv[argumentIndex], "-")) {
                /* This means end of arguments. Only useful to allow a
                   template file name to start with - */
                break;
            }
            if (!strcmp(argv[argumentIndex], "-v")) {
                bVerbose = 1;
            } else if (!strcmp(argv[argumentIndex], "--help")) {    
                    fprintf(stdout, "%s",
    "awsFillAndSign Copyright 2016 MIB SOFTWARE, INC."
"\n"
"\n"" PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4."
"\n"
"\n"" LICENSE:   MIT (Free/OpenSource)"
"\n" 
"\n"" STABLITY:  UNSTABLE as of 2016-11-22"
"\n""            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign"
"\n"              
"\n"" SUPPORT:   Contact the author for commercial support and consulting at"
"\n""            http://www.mibsoftware.com/"
"\n"
"\n"
"\n"" USAGE: awsFillAndSign [-e][-v][-bs] template-name.curl [param1[ param2...]]"
"\n"
"\n""   Without -e, credentials are taken from one line on stdin in the format of:"
"\n""      <region>/<service>,<ID>,<SECRET>"
"\n""   e.g:"
"\n""      us-east-1/s3,AXXXXXXXXXXXXXXXXXXX,7777777777777777777777777777777777777777"
"\n"
"\n""   Parameters (param1,param2,...) must already be URI-Encoded as appropriate."
"\n"
"\n""   The output is the filled template with AWS Version 4 signatures added."
"\n"
"\n"" OPTIONS:"
"\n""  -e <service>     Use service (e.g 's3') and the environment variables"
"\n""                   AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY, AWS_DEFAULT_REGION"
"\n""  -t <file-name>   Load template from file."
"\n""  -v               Verbose debugging output on stderr, including generated"
"\n""                   AWS Canonical Request fields."
"\n""  -bs              Calculate the SHA256 body signature for the upload-file or"
"\n""                   the data CURL options from the filled template."
"\n""  -b <file-name>   Calculate SHA256 body signature from file."
"\n""  -                Marker for end of arguments. (Useful when parameters that"
"\n""                   follow may start with '-'.)"
"\n"
"\n"" INFORMATION commands (output is not a filled and signed template:)"
"\n""  --help           Show this message."
"\n""  --list           List the standard templates."
"\n""  --write <name>   Show the named standard template along with comments."
);

                return 0;
            } else if (!strcmp(argv[argumentIndex], "--list")) {    
                int i;
                i = 1;

                while(librock_awsStandardTemplates[i]) {
                    fprintf(stdout, "%.*s\n", countToEol(librock_awsStandardTemplates[i]+3), librock_awsStandardTemplates[i]+3);
                    i++;
                }
                return 0;
            } else if (!strcmp(argv[argumentIndex], "--write")) {
                /* write template */
               
                if (argumentIndex >= argc) {
                    fprintf(stderr, "Usage: awsFillAndSign [-v][-b fileBodyToSign] -t template.txt param1 param2 param3\nTry --help.");
                    return -1;
                }
                pRequestTemplate = (char *) librock_awsStandardTemplate(argv[argumentIndex+1]);
                if (!pRequestTemplate) {
                    fprintf(stderr, "I-1515 no standard template '%s'\n", argv[argumentIndex+1]);
                    return 9;
                }
                fprintf(stdout, "@//%s\n", argv[argumentIndex+1]);
                fprintf(stdout, "%s\n", pRequestTemplate);
                return 0;
                
            } else if (!strncmp(argv[argumentIndex], "-t", 2)) {
                /* Load template from file */
                char *pWrite = 0;
                
                if (argumentIndex >= argc) {
                    fprintf(stderr, "Usage: awsFillAndSign [-v][-b fileBodyToSign] -t template.txt param1 param2 param3\nTry --help.");
                    return -1;
                }
                
                if (!strcmp(argv[argumentIndex], "-t")) {
                    pWrite = (char *) librock_fileGetContents(argv[argumentIndex+1]);
                    argumentIndex++;
                } else {
                    pWrite = (char *) librock_fileGetContents(argv[argumentIndex]+2);
                }
                if (!pWrite) {
                    perror("E-1096 load template");
                    return 1;
                }
                
                /* Strip \r in place */
                char *pRead = pWrite;
                pRequestTemplate = pWrite;
                while (*pRead) {
                    while (*pRead == '\r') {
                        pRead++;
                    }
                    *pWrite++ = *pRead++;
                }
                *pWrite = '\0';
                
            } else if (!strncmp(argv[argumentIndex], "-e", 2)) {
                if (!strcmp(argv[argumentIndex], "-e")) {
                    if (argumentIndex + 1 > argc) {
                        argumentIndex = argc;
                        break; //Show usage message
                    } else {
                        credentialsFromEnv = argv[argumentIndex + 1];
                        argumentIndex++;
                    }
                } else {
                    credentialsFromEnv = argv[argumentIndex]+2;
                }
            } else if (!strncmp(argv[argumentIndex], "-bs", 3)) {
                scanSignature = 1;
            } else if (!strncmp(argv[argumentIndex], "-b", 2)) {
                const char *pBody = 0;
                const char *pErrorMessage;
                if (!strcmp(argv[argumentIndex], "-b")) {
                    if (argumentIndex + 1 > argc) {
                        argumentIndex = argc;
                        break; //Show usage message
                    }
                    pBody = argv[argumentIndex + 1];
                    argumentIndex++;
                } else {
                    pBody = argv[argumentIndex]+2;
                }
                pErrorMessage = librock_fileSha256Contents(pBody, &mdContent32[0]);
                if (pErrorMessage) {
                    fprintf(stderr, "%s for file '%s'\n", pErrorMessage, pBody);
                    return 7;
                }
            } else {
                fprintf(stderr, "E-1457 Unrecognized option: %s\nTry --help.",argv[argumentIndex]);
                return 8;
            }
            argumentIndex++;
        }
    }

    if (argumentIndex >= argc) {
        fprintf(stderr, "Usage: awsFillAndSign [-v][-b fileBodyToSign] -t template.txt param1 param2 param3\nTry --help.");
        return -1;
    }
    if (credentialsFromEnv) {
        int iWrite = 0;
        const char *errorMessage;

        errorMessage = copyFromEnv_s("AWS_DEFAULT_REGION", credentials+iWrite, sizeof(credentials) - iWrite);
        if (errorMessage) {
            fprintf(stderr, "%s\n", errorMessage);
            return 14;
        }
        iWrite += strlen(credentials+iWrite);
        if (iWrite + 1 + strlen(credentialsFromEnv) > sizeof(credentials)-2) {
            fprintf(stderr, "%s\n", "I-1649 credentials would overflow fixed buffer");
            return 15;
        }
        credentials[iWrite++] = '/';

        /* service */
        memmove(credentials+iWrite, credentialsFromEnv, strlen(credentialsFromEnv));
        iWrite += strlen(credentialsFromEnv);
        credentials[iWrite++] = ',';

        errorMessage = copyFromEnv_s("AWS_ACCESS_KEY_ID", credentials+iWrite, sizeof(credentials) - iWrite);
        if (errorMessage) {
            fprintf(stderr, "%s\n", errorMessage);
            return 14;
        }
        iWrite += strlen(credentials+iWrite);
        if (iWrite + 1 > sizeof(credentials)-2) {
            fprintf(stderr, "%s\n", "I-1649 credentials would overflow fixed buffer");
            return 15;
        }
        credentials[iWrite++] = ',';

        errorMessage = copyFromEnv_s("AWS_SECRET_ACCESS_KEY", credentials+iWrite, sizeof(credentials) - iWrite);
        if (errorMessage) {
            fprintf(stderr, "%s\n", errorMessage);
            return 14;
        }
        iWrite += strlen(credentials+iWrite);
        credentials[iWrite++] = '\0';
        
    } else {
        /* Get comma-separated credentials on stdin, which must have this form. Lengths may vary.
              us-east-1/s3,AXXXXXXXXXXXXXXXXXXX,7777777777777777777777777777777777777777
         There must be no quote characters or spaces.
         Line length more than 198 characters will block processing.
         */
        if (!fgets(credentials, sizeof(credentials), stdin)) {
            perror("E-1163 Could not read credentials string");
            return 5;
        }
        if (credentials[198]) {
            fprintf(stderr, "E-1167 The credentials string is too long\n");
            return 6;
        }
    }
    //Output nothing on error. Don't want to send a request that will be rejected.

    if (!pRequestTemplate) {   /* Use built-in template */
        pRequestTemplate = (char *) librock_awsStandardTemplate(argv[argumentIndex]);
        if (!pRequestTemplate) {
            fprintf(stderr, "I-1515 no standard template '%s'\n", argv[argumentIndex]);
            return 9;
        }
        argumentIndex++; /* Advance to template parameters */
    }
    {
        char *pFilledRequest = 0;
        const char *pErrorMessage = 0;
        pErrorMessage = librock_fillTemplate(&pFilledRequest, pRequestTemplate,
                                                argc-argumentIndex+1, argv+argumentIndex-1);
        if (pErrorMessage) {
            fprintf(stderr, "%s\n", pErrorMessage);
            return 4;
        }
        if (scanSignature) {
            /* Expect an upload-file or data= field in the request. */
            const char *pRead = pFilledRequest;
            while(pRead) {
                if (countOptionName(pRead)==11 && !strncmp(pRead, "upload-file", 11)) {
                    char *fileName = 0;
                    int nameLength;
                    pRead += countToValue(pRead);
                    nameLength = countToCh(pRead, '\x22');
                    fileName = malloc(nameLength+1);
                    if (!fileName) {
                        perror("E-1741 malloc failed");
                        return 11;
                    }
                    memmove(fileName, pRead, nameLength );
                    fileName[nameLength] = '\0';

                    pErrorMessage = librock_fileSha256Contents(fileName, &mdContent32[0]);
                    if (pErrorMessage) {
                        fprintf(stderr, "%s for file '%s'\n", pErrorMessage, fileName);
                        fprintf(stderr, "I-1734 template expanded:\n%s\n", pFilledRequest);
                        return 12;
                    }
                    
                    freeOnce((void **)&fileName);
                    break;
                } else if (countOptionName(pRead)==4 && !strncmp(pRead, "data", 4)) {
                    void *pHashInfo;
                    pRead += countToValue(pRead);
                    /* Compute hash */
                    pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
                    if (!pHashInfo) {
                        perror("E-1744 malloc failed");
                        return 10;
                    }
                    librock_sha256Init(pHashInfo);
                    librock_sha256Update(pHashInfo, (unsigned char *) pRead, countToCh(pRead, '\x22'));
                    librock_sha256StoreFinal(mdContent32, pHashInfo);
                    freeOnce((void **)&pHashInfo);
                    break;
                }
                pRead = strchr(pRead, '\n');
                if (pRead) {
                    pRead++;
                }
            }
            if (!pRead) {
                fprintf(stderr, "E-1778 option -bs expected upload-file or data\n");
                return 13;
            }
        }

        pErrorMessage = librock_awsFillAndSign(
                               pFilledRequest
                               , &mdContent32[0] /*32 bytes, SHA256 of body */
                               , pCredentials/*,credentials scope WithoutDate*/
                               , write_to_FILE, stdout
                               , bVerbose ? write_to_FILE : 0, stderr);
        if (pErrorMessage) {
            fprintf(stderr, "%s\n", pErrorMessage);
            return 7;
        }
        freeOnce((void **)&pFilledRequest);
    }
    return 0;
}
#endif

/**************************************************************/
//[[Utility Functions in this module.]]

    PRIVATE int qsort_strcasecmp_(const void *pString1, const void *pString2) {
        return strcasecmp(*((char **)pString1), *((char **)pString2));
    }

    PRIVATE int qsort_strcmp_(const void *s1, const void *s2) {
        return strcmp(*((char **)s1), *((char **)s2));
    }

    PRIVATE int countToCh(const char *pRead, char ch)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != ch) {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int countToEol(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != '\n' && *pRead != '\r') {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int countOptionName(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead != '=' && *pRead > ' ') {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int countToValue(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != '\n') {
            if (*pRead == '=') {
                pRead++;
                if (*pRead == ' ') {
                    pRead++;
                }
                if (*pRead == '\x22') {
                    pRead++;
                }
                return pRead-pStart;
            }
            pRead++;
        }
        return pRead-pStart;

    }
#if 0 
//This works, but this API requires three corresponding parameters,
//which is an error risk.  It is better to group corresponding parameters
//into a struct. Use librock_safeAppend0().

    PRIVATE char *librock_memappend0_s(char *base, int cb, char **ppWrite, const char *pSource, int cSource)
    { /* Bounds-checked memmove into a buffer. If out of bounds, do nothing and return 0.
         Otherwise: memmove to *ppWrite, follow with a terminating '\0', and update the write position 
       */
        if (cSource == -1) {
            cSource = strlen(pSource);
        }
        if (*ppWrite < base) { /* pWrite is supposed to be within base+cb */
            return 0;
        }
        if (*ppWrite - base + cSource > cb-1) { /* Will not fit with null terminator */
            return 0;
        }
        memmove(*ppWrite, pSource, cSource);
        *ppWrite += cSource;
        *((char *) *ppWrite) = '\0';
        return *ppWrite - cSource;
    }
#endif
    PRIVATE char *librock_safeAppend0(struct librock_appendable *pAppendable, const char *pSource, int cSource)
    { /* Bounds-checked memmove into a buffer. If out of bounds, do nothing and return 0.
         Otherwise: memmove to *ppWrite, follow with a terminating '\0', and update the write position 
         Return the first character written into the buffer.
       */
        if (cSource == -1) {
            cSource = strlen(pSource);
        }
        if (cSource < 0) {
            return 0;
        }
        if (pAppendable->iWriting < 0) { /* validate */
            return 0;
        }
        if (pAppendable->iWriting + cSource + 1 > pAppendable->cb) { /* Will not fit with \0 terminator */
            /* This implementation does not support realloc. */
            return 0;
        }
        if (pSource) {
            memmove((char *)pAppendable->p + pAppendable->iWriting, pSource, cSource);
        } else { /* caller will write */
            *((char *)pAppendable->p + pAppendable->iWriting) = '\0';
        }
        pAppendable->iWriting += cSource;
        ((char *) pAppendable->p)[pAppendable->iWriting] = '\0';
        return (char *)pAppendable->p + pAppendable->iWriting - cSource;
    }
    PRIVATE void freeOnce(void **p)
    {
        if (*p) {
            free(*p);
            *p = 0;
        }
    }
    PRIVATE void bToHex0(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789abcdef";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }

    PRIVATE void bToHex0_UC(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789ABCDEF";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }
    PRIVATE const char *copyFromEnv_s(const char *pName, char *pWrite, int cMax)
    {
#if defined LIBROCK_WANT_GETENV_S_FOR_MSC_VER
        size_t ret;
        if (getenv_s(&ret, pWrite, cMax, pName)) {
            return "E-1912 environment variable would overflow fixed buffer";
        }
        if (!ret) {
            return "E-1915 environment variable not found";
        }
        return 0; //No Error
#else
        char *pRead = getenv(pName);
        int length;
        if (!pRead) {
            return "E-1915 environment variable not found";
        }
        length = strlen(pRead);
        if (length+1 > cMax) {
            return "E-1912 environment variable would overflow fixed buffer";
        }
        memmove(pWrite, pRead, length);
        pWrite[length] = '\0';
        return 0; //No Error
#endif
        
    }
    
PRIVATE char *librock_fillTemplate(char **ppFilled, const char *pTemplate, int argc, char * const * const argv)
{ //DEBUG: reuse
    int pass; //two passes.
    struct librock_appendable aFilled;
    memset((void *)&aFilled, '\0', sizeof(aFilled));
    for (pass = 0;pass < 2;pass++) {
        /* First pass we determine necessary size */
        const char *pRead = pTemplate;
        if (pass == 1) { // Second pass.
            aFilled.cb += 2;
            aFilled.p = malloc(aFilled.cb);
            if (!aFilled.p) {
                return "E-1144 malloc failed";
            }
        }
        while (pRead && *pRead) {
            /* Work in segments */
            const char *pStart = pRead;

            pRead = strchr(pRead, '@');
            if (pRead) {
                if (pass == 0) {
                    aFilled.cb += pRead - pStart;
                } else {
                    librock_safeAppend0(&aFilled, pStart, pRead-pStart);
                }
                if (pRead[1] == '@') {
                    // double '@@' is copied as single '@'
                    pRead += 2;
                    if (pass == 0) {
                        aFilled.cb += 1;
                    }
                } else if (pRead[1] == '/' && pRead[2] == '/') {//  '@//' is comment to end of line.
                    if (strchr(pRead, '\n')) {
                        if (pRead == pTemplate || pRead[-1] == '\n') {
                            //Trim the \n also
                            pRead = strchr(pRead, '\n')+1;
                        } else  {
                            //Retain the \n
                            pRead = strchr(pRead, '\n');
                            if (pRead[-1] == '\r') {
                                pRead--;
                            }
                        }
                    } else { //To end of string
                        pRead += strlen(pRead);
                    }
                } else { /* Replaceable parameter */
                    int n = atoi(pRead+1);
                    if (!strchr("0123456789",pRead[1])) {
                        return("E-1811 non-numeric replaceable @parameter@ in template");
                    }
                    pRead = strchr(pRead+1, '@');
                    if (!pRead) {
                        return("E-1153 unmatched @ in template");
                    }
                    if ((n < 0) || (n >= argc)) {
                        return("E-1157 parameter index out of range\n");
                    }
                    pRead++;
                    if (pass == 0) {
                        aFilled.cb += strlen(argv[n]);
                    } else {
                        librock_safeAppend0(&aFilled, argv[n], -1);
                    }
                }
            } else {
                /* rest of string from pStart */
                if (pass == 0) {
                    aFilled.cb += strlen(pStart);
                } else {
                    librock_safeAppend0(&aFilled, pStart, -1);
                }
            }
        }
    }
    *ppFilled = aFilled.p;
    aFilled.p = 0; // Caller will free
    return 0;
} /* librock_fillTemplate */


#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
    PRIVATE void *librock_fileGetContents(const char *fileName)
    {
        int fd;
        off_t fileLength;
        void *pContents;
        fd = librock_fdOpenReadOnly(fileName);
        if (fd == -1) {
            return 0;
        }
        fileLength = librock_fdSeek(fd, 0, SEEK_END);
        librock_fdSeek(fd, 0, SEEK_SET);
        pContents = malloc(fileLength+1);
        if (!pContents) {
            librock_fdClose(fd);
            return 0;
        }
        if (librock_fdRead(fd, pContents, fileLength) != fileLength) {
            librock_fdClose(fd);
            free(pContents);
            return 0;
        }
        ((char *)pContents)[fileLength] = '\0';
        librock_fdClose(fd);
        return pContents;

    } /* librock_fileGetContents */
#endif
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN) || defined(LIBROCK_WANT_fileSha256Contents)
    PRIVATE const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32)
    {
        int fd;
        int cRead;
        char buf[16384];
        void *pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
        if (!pHashInfo) {
            return "E-850 malloc failed";
        }
        fd = librock_fdOpenReadOnly(fname);
        if (fd == -1) {
            freeOnce((void **)&pHashInfo);
            return "E-1821 could not open file";
        }
        librock_sha256Init(pHashInfo);
        while ((cRead = librock_fdRead(fd, buf, sizeof(buf))) > 0) {
            librock_sha256Update(pHashInfo, (unsigned char *) buf,  cRead);
        }
        librock_sha256StoreFinal(mdBuffer32, pHashInfo);
        freeOnce((void **)&pHashInfo);
        librock_fdClose(fd);
        return 0;
    } /* fileSha256Contents */
#endif
    
//}
#endif //LIBROCK_UNSTABLE

