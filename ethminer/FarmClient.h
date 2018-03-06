/**
* This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
*/

#ifndef JSONRPC_CPP_STUB_FARMCLIENT_H_
#define JSONRPC_CPP_STUB_FARMCLIENT_H_

#include <jsonrpccpp/client.h>
#include <ethminer/SignTx.h>
#include <libethash/sha3_cryptopp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace std;
using namespace dev;
using namespace dev::eth;



/** libkeccak-tiny
*
* A single-file implementation of SHA-3 and SHAKE.
*
* Implementor: David Leon Gil
* License: CC0, attribution kindly requested. Blame taken too,
* but not liability.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******** The Keccak-f[1600] permutation ********/

/*** Constants. ***/
static const uint8_t rho[24] = \
{ 1, 3, 6, 10, 15, 21,
28, 36, 45, 55, 2, 14,
27, 41, 56, 8, 25, 43,
62, 18, 39, 61, 20, 44};
static const uint8_t pi[24] = \
{10, 7, 11, 17, 18, 3,
5, 16, 8, 21, 24, 4,
15, 23, 19, 13, 12, 2,
20, 14, 22, 9, 6, 1};
static const uint64_t RC[24] = \
{1ULL, 0x8082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
0x808bULL, 0x80000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
0x8aULL, 0x88ULL, 0x80008009ULL, 0x8000000aULL,
0x8000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
0x8000000000008002ULL, 0x8000000000000080ULL, 0x800aULL, 0x800000008000000aULL,
0x8000000080008081ULL, 0x8000000000008080ULL, 0x80000001ULL, 0x8000000080008008ULL};

/*** Helper macros to unroll the permutation. ***/
#define rol(x, s) (((x) << s) | ((x) >> (64 - s)))
#define REPEAT6(e) e e e e e e
#define REPEAT24(e) REPEAT6(e e e e)
#define REPEAT5(e) e e e e e
#define FOR5(v, s, e)							\
	v = 0;										\
	REPEAT5(e; v += s;)

/*** Keccak-f[1600] ***/
static inline void keccakf(void* state) {
	uint64_t* a = (uint64_t*) state;
	uint64_t b[5] = {0};
	uint64_t t = 0;
	uint8_t x, y;

	for (int i = 0; i < 24; i++) {
		// Theta
		FOR5(x, 1,
			 b[x] = 0;
		FOR5(y, 5,
			 b[x] ^= a[x + y]; ))
			FOR5(x, 1,
				 FOR5(y, 5,
					  a[y + x] ^= b[(x + 4) % 5] ^ rol(b[(x + 1) % 5], 1); ))
			// Rho and pi
			t = a[1];
		x = 0;
		REPEAT24(b[0] = a[pi[x]];
		a[pi[x]] = rol(t, rho[x]);
		t = b[0];
		x++; )
			// Chi
			FOR5(y,
				 5,
				 FOR5(x, 1,
					  b[x] = a[y + x];)
				 FOR5(x, 1,
					  a[y + x] = b[x] ^ ((~b[(x + 1) % 5]) & b[(x + 2) % 5]); ))
			// Iota
			a[0] ^= RC[i];
	}
}

/******** The FIPS202-defined functions. ********/

/*** Some helper macros. ***/

#define _(S) do { S } while (0)
#define FOR(i, ST, L, S)							\
	_(for (size_t i = 0; i < L; i += ST) { S; })
#define mkapply_ds(NAME, S)						\
	static inline void NAME(uint8_t* dst,			\
		const uint8_t* src,						\
		size_t len) {								\
		FOR(i, 1, len, S);							\
	}
#define mkapply_sd(NAME, S)						\
	static inline void NAME(const uint8_t* src,	\
		uint8_t* dst,								\
		size_t len) {								\
		FOR(i, 1, len, S);							\
	}

mkapply_ds(xorin, dst[i] ^= src[i])  // xorin
mkapply_sd(setout, dst[i] = src[i])  // setout

#define P keccakf
#define Plen 200

// Fold P*F over the full blocks of an input.
#define foldP(I, L, F)								\
	while (L >= rate) {							\
		F(a, I, rate);								\
		P(a);										\
		I += rate;									\
		L -= rate;									\
	}

/** The sponge-based hash construction. **/
int hashtest(uint8_t* out, size_t outlen,
		const uint8_t* in, size_t inlen,
		size_t rate, uint8_t delim) 
{
	if ((out == NULL) || ((in == NULL) && inlen != 0) || (rate >= Plen)) {
		return -1;
	}
	uint8_t a[Plen] = {0};
	// Absorb input.
	//foldP(in, inlen, xorin);
	// Xor in the DS and pad frame.
	a[inlen] ^= delim;
	a[rate - 1] ^= 0x80;
	// Xor in the last block.
	xorin(a, in, inlen);
	// Apply P
	P(a);
	// Squeeze output.
	foldP(out, outlen, setout);
	setout(a, out, outlen);
	memset(a, 0, 200);
	return 0;
}

int sha3_256(uint8_t* out, size_t outlen, const uint8_t* in, size_t inlen) {								
	if (outlen > (256/8)) {										
		return -1;                                                  
	}																
	return hashtest(out, outlen, in, inlen, 200 - (256 / 4), 0x01);
}





class FarmClient : public jsonrpc::Client
{
public:

	const string TokenContract = "0xb6ed7644c69416d67b522e20bc294a9a9b405b31";      // 0xBitcoin

	enum TxStatus
	{
		Succeeded,
		Failed,
		NotFound
	};


	// Constructor
	FarmClient(jsonrpc::IClientConnector &conn, jsonrpc::clientVersion_t type = jsonrpc::JSONRPC_CLIENT_V2) : jsonrpc::Client(conn, type) 
	{
		m_minerAcct = ProgOpt::Get("0xBitcoin", "MinerAcct");
		m_tokenContract = ProgOpt::Get("0xBitcoin", "TokenContract");
		m_acctPK = ProgOpt::Get("0xBitcoin", "AcctPK");
	}

	Json::Value rpcCall(const std::string &_function, const Json::Value &_params)
	{
		Guard l(x_callMethod);
		return CallMethod(_function, _params);
	}

	// this routine runs on a separate thread
	void bidScanner() 
	{
		Json::Value data;
		Json::Value result;

		// sign up for pending transactions
		data = Json::Value();
		result = rpcCall("eth_newPendingTransactionFilter", data);
		tx_filterID = result.asString();

		while (true) {
			// request the hashes of all txs received since the last time we called.
			data = Json::Value();
			data.append(tx_filterID);
			result = rpcCall("eth_getFilterChanges", data);

			// we've only got the hashes at this point,  so now retrieve each txs looking for calls to 
			// the mint() function of contract 0xbitcoin 
			for (uint32_t i = 0; i < result.size(); i++) 
			{
				string hash = result[i].asString();
				data.clear();
				data.append(hash);
				try {
					Json::Value tx = rpcCall("eth_getTransactionByHash", data);
					if (!tx.isNull()) {
						string toAddr = tx["to"].asString();
						string input = tx["input"].asString();
						if (LowerCase(toAddr) == TokenContract && input.substr(0, 10) == "0x1801fbe5") {
							// pull out the nonce and the hash
							h256 nonce = h256(input.substr(10, 64));
							bytes hash = fromHex(input.substr(74, 64));
							LogS << tx["from"].asString() << " called the mint() function of 0xBitcoin!";
						}
					}
				}
				catch (std::exception& e) {
					LogS << "Error retrieving transaction " << hash;
					LogS << "Message : " << e.what();
				}
			}


			this_thread::sleep_for(chrono::milliseconds(1000));
		}

	}

	void closeBidScanner() {
		if (tx_filterID == "") return;
		// cancel the filter
		Json::Value data;
		data.append(tx_filterID);
		rpcCall("eth_uninstallFilter", data);
	}

	Json::Value readJson() {
		ifstream f;
		Json::Value res;

		f.exceptions(ofstream::failbit | ofstream::badbit);
		string path = "C:\\_temp\\mvis-txs.txt";
		try {
			Json::CharReaderBuilder rbuilder;
			f.open(path, fstream::in);
			std::string errs;
			bool ok = Json::parseFromStream(rbuilder, f, &res, &errs);
			if (!ok)
				throw std::runtime_error(errs.c_str());
			f.exceptions(ofstream::goodbit);
			f.close();
		}
		catch (std::exception& e) {
			LogB << "Error reading \"" << path << "\"";
			LogB << "Message : " << e.what();
			LogB << "Mining results reset to empty state.";
			f.exceptions(ofstream::goodbit);
			f.close();
		}
		return res;
	}

	void recommendGasPrice()
	{
		//Json::Value result = rpcCall("txpool_content", p);
		Json::Value result = readJson();
		Json::Value pending = result["pending"];
		vector<string> senders = pending.getMemberNames();
		for (int i = 0; i < senders.size(); i++) {
			string s = senders[i];
			int j = pending[s].size();
			Json::Value txs = pending[s];
			vector<string> nonces = txs.getMemberNames();
			for (int j = 0; j < nonces.size(); j++) {
				Json::Value tx = txs[nonces[j]];
				string s1 = tx["to"].asString();

			}
		}
	}

	TxStatus getTxStatus(string _txHash) {
		Json::Value data;
		data.append(_txHash);
		try {
			Json::Value result = rpcCall("eth_getTransactionReceipt", data);
			if (result["status"].asString() == "0x1")
				return TxStatus::Succeeded;
			else if (result["status"].asString() == "0x0")
				return TxStatus::Failed;
			else
				return TxStatus::NotFound;
		}
		catch (...) {
			return TxStatus::NotFound;
		}
	}

	void checkPendingTransactions() {
		int i = 0;
		vector<Transaction>::iterator it;
		for (it = m_pendingTxs.begin(); it != m_pendingTxs.end(); ) 
		{
			TxStatus status = getTxStatus((*it).receiptHash);
			if (status == Succeeded)
				LogB << "Tx " << (*it).receiptHash.substr(0, 10) << " succeeded :)";
			else if (status == Failed)
				LogB << "Tx " << (*it).receiptHash.substr(0, 10) << " failed :(";

			if (status == NotFound) 
			{
				// adjust gas price if necessary
				int recommended = 5;
				if ((*it).gasPrice < u256(recommended) * 1000000000)
				{
					// increase gas price and resend
					(*it).gasPrice = u256(recommended) * 1000000000;
					txSignSend((*it));
					LogB << "Adjusting gas price to " << (*it).gasPrice / 1000000000 << ", tx hash=" << (*it).receiptHash;
				}
				++it;
			}
			else 
				it = m_pendingTxs.erase(it);

			i++;
		}
	}

	int getNextNonce() {
		// get transaction count for nonce
		Json::Value p;
		p.append(m_minerAcct);
		p.append("latest");
		Json::Value result = rpcCall("eth_getTransactionCount", p);
		std::istringstream converter(result.asString());
		int i;
		converter >> std::hex >> i;
		return i;
	}

	uint64_t tokenBalance() {
		Json::Value p;
		p["from"] = m_minerAcct;			// ETH address (Jaxx HD)
		p["to"] = m_tokenContract;			// 0xbitcoin contract address

		h256 bMethod = sha3("balanceOf(address)");
		std::string sMethod = toHex(bMethod, dev::HexPrefix::Add);
		sMethod = sMethod.substr(0, 10);

		// address
		stringstream ss;
		ss << std::setw(64) << std::setfill('0') << m_minerAcct.substr(2);
		std::string s2(ss.str());
		sMethod = sMethod + s2;
		p["data"] = sMethod;

		Json::Value data;
		data.append(p);
		data.append("latest");

		Json::Value result = rpcCall("eth_call", data);
		u256 balance = u256(result.asString()) / 100000000;
		if (result.isString()) {
			return static_cast<uint64_t>(balance);
		} else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

	Json::Value eth_getWork() throw (jsonrpc::JsonRpcException) {
		Json::Value p;
		p = Json::nullValue;
		Json::Value result = rpcCall("eth_getWork", p);
		if (result.isArray())
			return result;
		else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

	void eth_getWork_token(bytes& _challenge, h256& _target) throw (jsonrpc::JsonRpcException) {
		// challenge
		Json::Value p;
		p["from"] = m_minerAcct;			// ETH address (Jaxx HD)
		p["to"] = m_tokenContract;			// 0xbitcoin contract address

		h256 bMethod = sha3("getChallengeNumber()");
		std::string sMethod = toHex(bMethod, dev::HexPrefix::Add);
		p["data"] = sMethod.substr(0, 10);

		Json::Value data;
		data.append(p);
		data.append("latest");

		Json::Value result = rpcCall("eth_call", data);
		if (result.isString()) {
			_challenge = fromHex(result.asString());
		} else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
		LogF << "Trace: eth_getWork_token, Challenge : " << toHex(_challenge);

		// target
		bMethod = sha3("getMiningTarget()");
		sMethod = toHex(bMethod, dev::HexPrefix::Add);
		p["data"] = sMethod.substr(0, 10);

		data.clear();
		data.append(p);
		data.append("latest");

		result = rpcCall("eth_call", data);
		if (result.isString()) {
			_target = h256(result.asString());
		} else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());

	}

	void eth_getLastBlockData() throw (jsonrpc::JsonRpcException) {
		string lastRewardTo;
		u256 lastRewardEthBlockNumber;

		// lastRewardTo
		Json::Value p;
		p["from"] = m_minerAcct;			// ETH address (Jaxx HD)
		p["to"] = m_tokenContract;			// 0xbitcoin contract address

		h256 bMethod = sha3("lastRewardTo()");
		std::string sMethod = toHex(bMethod, dev::HexPrefix::Add);
		p["data"] = sMethod.substr(0, 10);

		Json::Value data;
		data.append(p);
		data.append("latest");

		Json::Value result = rpcCall("eth_call", data);
		if (result.isString()) {
			lastRewardTo = "0x" + result.asString().substr(26);
		} else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());

		// lastRewardEthBlockNumber
		bMethod = sha3("lastRewardEthBlockNumber()");
		sMethod = toHex(bMethod, dev::HexPrefix::Add);
		p["data"] = sMethod.substr(0, 10);

		data.clear();
		data.append(p);
		data.append("latest");

		result = rpcCall("eth_call", data);
		if (result.isString()) {
			lastRewardEthBlockNumber = u256(result.asString());
		} else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());

		LogD << "Last reward to : " << lastRewardTo << ", block : " << lastRewardEthBlockNumber;
	}


	bool eth_submitWork(const std::string& param1, const std::string& param2, const std::string& param3) throw (jsonrpc::JsonRpcException) {
		Json::Value p;
		p.append(param1);
		p.append(param2);
		p.append(param3);
		Json::Value result = rpcCall("eth_submitWork", p);
		if (result.isBool())
			return result.asBool();
		else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

	void txSignSend(Transaction &t) 
	{
		stringstream ss;
		Secret pk = Secret(m_acctPK);
		t.sign(pk);
		ss = std::stringstream();
		ss << "0x" << toHex(t.rlp());

		// submit to the node 
		Json::Value p;
		p.append(ss.str());
		Json::Value result = rpcCall("eth_sendRawTransaction", p);
		t.receiptHash = result.asString();
	}

	bool eth_submitWorkToken(h256 _nonce, bytes _hash, bytes _challenge) throw (jsonrpc::JsonRpcException) {

		try {
			// check if the other miner already submitted a solution for this challenge
			boost::filesystem::path m_challengeFilename = boost::filesystem::path(ProgOpt::Get("0xBitcoin", "ChallengeFolder")) / "challenge.txt";
			ifstream ifs;
			if (boost::filesystem::exists(m_challengeFilename)) {
				string s;
				ifs.open(m_challengeFilename.generic_string(), fstream::in);
				getlineEx(ifs, s);
				if (s == toHex(_challenge)) {
					LogS << "The other miner already got this one : " << toHex(_challenge).substr(0, 8);
					return false;
				}
			}
			ifs.close();
			// write this challenge value to our synchronization file.
			std::ofstream ofs(m_challengeFilename.generic_string(), std::ofstream::out);
			ofs << toHex(_challenge);
			ofs.close();
		}
		catch (const std::exception& e) {
			LogB << "Exception: eth_submitWorkToken - " << e.what();
		}


		// prepare transaction
		Transaction t;
		if (m_lastSolution.elapsedSeconds() > 5 * 60 || m_txNonce == -1) {
			m_txNonce = getNextNonce();
		} else {
			m_txNonce++;
		}
		m_lastSolution.restart();
		t.nonce = m_txNonce;
		t.receiveAddress = toAddress(m_tokenContract);
		t.gas = u256(100000);
		ProgOpt::Load("");
		//t.gasPrice = u256(ProgOpt::Get("0xBitcoin", "GasPrice")) * 1000000000;	// convert gwei to wei
		t.gasPrice = u256(1) * 1200000000;	// convert gwei to wei

		// compute data parameter : first 4 bytes is hash of function signature
		h256 bMethod = sha3("mint(uint256,bytes32)");
		std::string sMethod = toHex(bMethod, dev::HexPrefix::Add);
		sMethod = sMethod.substr(0, 10);
		// put the nonce in
		stringstream ss;
		ss << std::setw(64) << std::setfill('0') << _nonce.hex();
		std::string s2(ss.str());
		sMethod = sMethod + s2;
		// and the hash
		ss = std::stringstream();
		ss << std::left << std::setw(64) << std::setfill('0') << toHex(_hash);
		s2 = std::string(ss.str());
		sMethod = sMethod + s2;
		t.data = fromHex(sMethod);
		t.value = 0;

		txSignSend(t);
		LogB << "Tx hash : " << t.receiptHash;
		m_pendingTxs.push_back(t);
		return true;
	}


	bool eth_submitHashrate(const std::string& param1, const std::string& param2) throw (jsonrpc::JsonRpcException) {
		Json::Value p;
		p.append(param1);
		p.append(param2);
		Json::Value result = rpcCall("eth_submitHashrate", p);
		if (result.isBool())
			return result.asBool();
		else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

	Json::Value eth_awaitNewWork() throw (jsonrpc::JsonRpcException) {
		Json::Value p;
		p = Json::nullValue;
		Json::Value result = rpcCall("eth_awaitNewWork", p);
		if (result.isArray())
			return result;
		else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

	bool eth_progress() throw (jsonrpc::JsonRpcException) {
		Json::Value p;
		p = Json::nullValue;
		Json::Value result = rpcCall("eth_progress", p);
		if (result.isBool())
			return result.asBool();
		else
			throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
	}

private:
	string m_minerAcct;
	string m_acctPK;
	string m_tokenContract;
	int m_txNonce = -1;
	Timer m_lastSolution;
	deque<string> m_pendingTransactions;
	vector<Transaction> m_pendingTxs;
	mutable Mutex x_callMethod;
	string tx_filterID;


};

#endif //JSONRPC_CPP_STUB_FARMCLIENT_H_
