/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Amazon Mobile Analytics" (mobileanalytics)
   at Amazon Web Services.
   
 Copyright 2016 MIB SOFTWARE, INC. 
	NOTE from the copyright holder: Since templates are
	functional, there is minimal copyrightable matter
	contained in these templates.
   
 LICENSE:   MIT (Free/OpenSource)
 
 STABILITY: UNSTABLE as of 2016-11-22
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/
   
   
 */
//http://docs.aws.amazon.com/mobileanalytics/latest/ug/incorporating-mobile-analytics.html


,"@//aws-mobile-analytics-apps.curl;"
    "\n""@// TEMPLATE FOR:  AWS Mobile Analytics Getting a List of Apps"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/mobileanalytics/latest/ug/drs-getting-apps.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:"
    "\n""@//   @1_region@ - The URI-encoded region"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL.)"
    "\n""@//"
    "\n""url=\"https://mobileanalytics.@1_region@.amazonaws.com/2014-06-05/apps"
    "\n""header = \"Host: apigateway.@1_region@.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
    "\n""header = \"x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\""
