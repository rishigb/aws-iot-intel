## Getting Started With Amazon Web Services IoT on Intel Edison

![Intel Edison + AWS IoT](https://cloud.githubusercontent.com/assets/2881361/10375573/439555c4-6dc7-11e5-8eb0-75b9f1506f30.png)

### Install AWS CLI: 

**Install pip (Python package manager):**
``` bash
$ curl https://bootstrap.pypa.io/ez_setup.py -o - | python
$ easy_install pip
``` 
**Install AWS CLI with pip:**
``` bash
$ pip install awscli
``` 
**Install dependencies:**
_In order to view help files ("aws iot help"), install Groff and a non-BusyBox version of less._

**Groff:**
``` bash
$ wget http://ftp.gnu.org/gnu/groff/groff-1.22.3.tar.gz
$ tar -zxvf groff-1.22.3.tar.gz
$ cd groff-1.22.3
$ ./configure
$ make
$ make install
$ export PATH=$PATH:/usr/local/bin/
$ cd ~
``` 
**Less:**
_First rename the old version of less._
``` bash
$ mv /usr/bin/less /usr/bin/less-OLD
``` 
Then install the new version of less:
``` bash
$ wget http://www.greenwoodsoftware.com/less/less-458.zip
$ unzip less-458.zip
$ cd less-458
$ chmod 777 *
$ ./configure
$ make
$ make install
$ cd ~
``` 
**Get AWS credentials:**

Aws Cli is now installed. Make new user and get credentials from the aws console following instructions at: http://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-set-up.html#cli-signup. Once you have an access ID and key you can configure aws and enter the ID and key with:
``` bash 
aws configure 
``` 
**NOTE:** for default region you must enter us-east-1 in order to be able to configure AWS for IoT. The default format can be left as json. 

In order to get permission to download the AWS IoT tools, attach the administrator account policy to the user. To do this go to the "Users Panel" in the IAM console, select the user you created, attach policy, and select administrator account.

**Download Amazon IoT SDK:**
``` bash
$ aws s3 cp s3://aws-iot-private-beta/aws-iot-cli.zip aws-iot-cli.zip
$ unzip aws-iot-cli.zip
$ mkdir .aws/models
$ cp –r ./models/iot .aws/models
$ cp –r ./models/iot-data .aws/models
``` 
To make sure everything has installed correctly run the iot help file:
``` bash
$ aws iot help
``` 
**Generate Certificates:**

First create a folder to store your certificates in:

``` bash
mkdir aws_certs
cd aws_certs
```

Generate a private key with open ssl:
``` bash
$ openssl genrsa -out privateKey.pem 2048
$ openssl req -new -key privateKey.pem -out cert.csr
``` 
* Fill out the fields with your info.
* Run the following to activate the certificate: 
``` bash
$ aws iot --endpoint-url https://i.us-east-1.pb.iot.amazonaws.com create-certificate --certificate-signing-request file://cert.csr --set-as-active > certOutput.txt
``` 
Run the following to save the certificate into a cert.pem file:
``` bash
$ aws iot --endpoint-url https://i.us-east-1.pb.iot.amazonaws.com describe-certificate --certificate-id <certificate ID> --output text --query certificateDescription.certificatePem  > cert.pem
``` 
**NOTE:** Replace <certificate ID> with the ID stored in the "certificateId" field in certOutput.txt. To view the file enter: 
``` bash
$ more certOutput.txt.
``` 

Create a Json policy document for AWS IoT SDK:
Copy the following text (ctrl-c):
``` json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Action": [
                "iot:*"
            ],
            "Resource": [
                "*"
            ]
        }
    ]
}
``` 
* Enter "$ vi policy.doc"  hit "a" and right-click to paste the text.
* Hit escape and type in ":wq" to save and quit.  

** Attach the policy to your certificate **
First enter:
``` bash
$ aws iot --endpoint-url https://i.us-east-1.pb.iot.amazonaws.com create-policy --policy-name PubSubToAnyTopic --policy-document file://policy.doc
``` 
Then attach the policy to the certificate with:
``` bash
$ aws iot --endpoint-url https://i.us-east-1.pb.iot.amazonaws.com attach-principal-policy --principal-arn <principal arn> --policy-name "PubSubToAnyTopic" 
``` 
**NOTE:** replace <principal arn> with the  value stored in "certifcateArn" in the outputCert.txt file. 














 
