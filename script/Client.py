import grpc
import grpc_service_pb2
import grpc_service_pb2_grpc
import time
import os
import sys


def run():
    channel = grpc.insecure_channel('172.17.6.83:8080')
    stub_account_client = grpc_service_pb2_grpc.AccountServiceStub(channel)
    sub_chain_client = grpc_service_pb2_grpc.ChainServiceStub(channel)
    sub_consensus_client = grpc_service_pb2_grpc.ConsensusServiceStub(channel)
    sub_status_client = grpc_service_pb2_grpc.StatusServiceStub(channel)
    sub_transactionS_client = grpc_service_pb2_grpc.TransactionServiceStub(channel)
    # 1. 账户
    response = stub_account_client.GetAccount(grpc_service_pb2.GetAccountRequest(domain_id=0,address='did:bid:efqhQu9YWEWpUKQYkAyGevPGtAdD1N6p'))
    print("client received: ", response)
    time.sleep(130)
    exit(0)
    response = stub_account_client.CreateAccount(grpc_service_pb2.CreateAccountRequest(sign_type='ed25519'))
    print("client received: ", response)

    response = stub_account_client.GetGenesisAccount(
        grpc_service_pb2.GetGenesisAccountRequest(domain_id=0))
    print("client received: ", response)

    #2. 账本
    response = sub_chain_client.GetLedger(
        grpc_service_pb2.GetLedgerRequest())
    print("client received: ", response)

    #3 共识
    response = sub_consensus_client.GetConsensusInfo(
        grpc_service_pb2.GetConsensusInfoRequest())
    print("client received: ", response)

    response = sub_consensus_client.GetConsensusValue(
        grpc_service_pb2.GetConsensusValueRequest())
    print("client received: ", response)

    #3. 状态
    response = sub_status_client.GetStatus(
        grpc_service_pb2.GetStatusRequest())
    print("client received: ", response)

    #4. 交易

    tx_item_list = grpc_service_pb2.SubmitTransactionRequest()
    tx_item = tx_item_list.transaction_list.add()
    tx_item.tx = '{\"source_address\": \"did:bid:efqhQu9YWEWpUKQYkAyGevPGtAdD1N6p\", 	\"data\": { 		\"type\": 1, 		\"gas_limit\": 200000, 		\"gas_price\": 1, 		\"ceil_ledger_seq\": 50, 		\"chain_id\": 0, 		\"domain_id\": 0, 		\"nonce\": 12345678, 		\"dest_address\": \"did:bid:efy85ZbAozTgwXw92rb3QEfd7ZHcByVM\", 		\"create_account\": { 			\"contract\": { 				\"type\": 1, 				\"code\": \"608060405260b0806100126000396000f300608060405260043610603f576000357c0100000000000000000000000000000000000000000000000000000000900463ffffffff1680638e68fe43146044575b600080fd5b6063600480360381019080803560ff1690602001909291905050506079565b6040518082815260200191505060405180910390f35b6000600190509190505600a165627a7a72305820f0e62f06b7f7c63d84a56ec5d25886b5cd0c9670bddec50bcbe991c6feb4debe0029\", 				\"version\": 1, 				\"metadatas_hash\": \"\", 				\"status\": 0 			}, 			\"init_balance\": 100, 			\"init_input\": \"\" 		} 	}, 	\"signature\": { 		\"public_key\": \"Hello\", 		\"sign_data\": \"ESI=\" 	} }'
    tx_item.private_key = ''

    response = sub_transactionS_client.SubmitTransaction(tx_item_list)
    print("client received: ", response)

    response = sub_transactionS_client.CallContract(
        grpc_service_pb2.CallContractRequest())
    print("client received: ", response)

    exit(0)
if __name__ == '__main__':
    run()
