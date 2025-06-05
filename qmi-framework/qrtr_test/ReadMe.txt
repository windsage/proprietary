adb root
adb shell
cd /vendor/bin/socket_test
svc :  ./mproc_test_socket_svc -e 10 -n 1026 -i 1 -l 1
client: ./mproc_test_socket_client -s 15 -e 10 -n 1026 -i 1 -r 1


svc running on service ID: 0x0f
svc user argument
-e : instance id 
-n: data size
-i: iteration
-l: service will loopback the data(1) or not(0) (when client doesn't expect loopback data from svc)

client user argument
-s: service ID
-e: instance id
-n: data size
-i: iteration
-r: remote identified by node no.

Node Info:
Processor

proc	NodeId

APPS 	1

MPSS 	0

GSS	NA

WCNSS	7

ADSP	5

SLPI	9

CDSP	10

Compilation:
keep the socket_test folder under /vendor/qcom/proprietary/qmi-framework
$ cd /vendor/qcom/proprietary/qmi-framework/socket_test
$ mm


