/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Elastic Compute Cloud" (EC2)
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
 
 
#if 0 // example only
,"@//aws-ec2-describe-instances.curl"
    "\n""@// Template for an AWS REST API request using CURL."
    "\n""@//"
    "\n""@// TEMPLATE FOR:  AWS EC2 DescribeInstances"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeInstances.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
    "\n""data=\"Action=DescribeInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
#endif
/*
Elastic Block Store

    AttachVolume

    CopySnapshot

    CreateSnapshot

    CreateVolume

    DeleteSnapshot

    DeleteVolume

    DescribeSnapshotAttribute

    DescribeSnapshots

    DescribeVolumeAttribute
,"@//aws-ec2-AttachVolume"
"\n""@// TEMPLATE FOR:  AWS EC2 AttachVolume"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_AttachVolume.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=AttachVolume&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-CopySnapshot"
"\n""@// TEMPLATE FOR:  AWS EC2 CopySnapshot"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_CopySnapshot.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=CopySnapshot&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-CreateSnapshot"
"\n""@// TEMPLATE FOR:  AWS EC2 CreateSnapshot"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_CreateSnapshot.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=CreateSnapshot&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-CreateVolume"
"\n""@// TEMPLATE FOR:  AWS EC2 CreateVolume"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_CreateVolume.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=CreateVolume&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DeleteSnapshot"
"\n""@// TEMPLATE FOR:  AWS EC2 DeleteSnapshot"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DeleteSnapshot.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DeleteSnapshot&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DeleteVolume"
"\n""@// TEMPLATE FOR:  AWS EC2 DeleteVolume"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DeleteVolume.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DeleteVolume&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeSnapshotAttribute"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeSnapshotAttribute"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeSnapshotAttribute.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeSnapshotAttribute&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeSnapshots"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeSnapshots"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeSnapshots.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeSnapshots&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeVolumeAttribute"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeVolumeAttribute"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeVolumeAttribute.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeVolumeAttribute&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

*/
,"@//aws-ec2-describe-volumes.curl;"
    "\n""@// Template for an AWS REST API request using CURL."
    "\n""@//"
    "\n""@// TEMPLATE FOR:  AWS EC2 DescribeVolumes"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeVolumes.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
    "\n""data=\"Action=DescribeVolumes&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
/*
    DescribeVolumeStatus

    DetachVolume

    EnableVolumeIO

    ModifySnapshotAttribute

    ModifyVolumeAttribute

    ResetSnapshotAttribute 

,"@//aws-ec2-DescribeVolumeStatus"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeVolumeStatus"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeVolumeStatus.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeVolumeStatus&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DetachVolume"
"\n""@// TEMPLATE FOR:  AWS EC2 DetachVolume"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DetachVolume.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DetachVolume&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-EnableVolumeIO"
"\n""@// TEMPLATE FOR:  AWS EC2 EnableVolumeIO"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_EnableVolumeIO.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=EnableVolumeIO&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-ModifySnapshotAttribute"
"\n""@// TEMPLATE FOR:  AWS EC2 ModifySnapshotAttribute"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_ModifySnapshotAttribute.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=ModifySnapshotAttribute&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-ModifyVolumeAttribute"
"\n""@// TEMPLATE FOR:  AWS EC2 ModifyVolumeAttribute"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_ModifyVolumeAttribute.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=ModifyVolumeAttribute&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-ResetSnapshotAttribute"
"\n""@// TEMPLATE FOR:  AWS EC2 ResetSnapshotAttribute"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_ResetSnapshotAttribute.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=ResetSnapshotAttribute&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

	
	*/
