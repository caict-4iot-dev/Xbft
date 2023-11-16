#!/usr/bin/env python
from optparse import OptionParser
import os
import sys
path = os.path.dirname(os.getcwd()) + '/src/protos'
proto_file_list = os.listdir(path)
py_file_list = os.listdir(os.getcwd())
def GenProto():
    for file in proto_file_list:
        if file.find(".proto") != -1:
            os.system("python -m grpc_tools.protoc --python_out=.  -I " + path + ' ' + file)
            print("python -m grpc_tools.protoc --python_out=.  -I " + path + ' ' + file)
def GenGrpcProto():
    for file in proto_file_list:
        if file.find("grpc_") != -1:
            os.system("python -m grpc_tools.protoc --grpc_python_out=.  -I " + path + ' ' + file)
            print("python -m grpc_tools.protoc --grpc_python_out=.  -I " + path + ' ' + file)
def ClearProtos():

    for file in py_file_list:
        if file.find("_pb2") != -1:
            os.system("rm " + os.getcwd() + '/' + file)
            print("rm " + os.getcwd() + '/' + file)
def main():
    args = sys.argv[1:]
    usage = "--flag -g gen proto; --flag -c clear protos"
    optParser = OptionParser(usage)

    optParser.add_option('-f', '--flag', type="string", dest='flag', help="请输入操作命令")
    option, args = optParser.parse_args(args)
    flag = option.flag
    print("-f传入了参数", flag)
    if flag == "-g":
        GenProto()
        GenGrpcProto()
    if flag == "-c":
        ClearProtos()

if __name__ == '__main__':
    main()