#pragma once
extern "C" {
	typedef unsigned long int uint;
	// Define variables
	typedef void* DMS_STATE;
	typedef void* DMS_ENV;
	typedef void* DMS_CHAR;
	typedef void* DMS_CHUNK;
	typedef void* DMS_INVK;
	typedef void* DMS_MEM;
	typedef void* DMS_CONN;
	typedef void* DMS_MSG;
	struct DMS_VAL;
	typedef void* DMS_LIST;
	typedef void* DMS_BLOCK;
	typedef void* DMS_MEMSTACK;
	typedef void* DMS_STATSTACK;
	typedef void* DMS_STAT;
	typedef void* DMS_ARGS;
	typedef void* DMS_TOKENSTREAM;
	typedef uint DMS_TOKENTYPE;
	typedef uint DMS_OP;

	struct DMS_CBLOCK {
		DMS_CHUNK chunk;
		const char* type;
		DMS_TOKENSTREAM toks;
	};

	struct DMS_TOKEN {
		DMS_TOKENTYPE type;
		DMS_OP raw;
		const char* name;
		unsigned long long line_num = 0;
	};
	
	struct DMS_CMD {
		uint opcode;
		DMS_ARGS args;
	};

	struct DMS_LABEL {
		const char* label;
		uint pos;
	};

	typedef void(*DMS_FUNC)(void*, DMS_STATE, DMS_ARGS);

	// Code Execution 
	DMS_STATE dms_loadFile(const char*);
	bool dms_loadFileB(DMS_STATE, const char*);
	DMS_STATE dms_loadString(const char*);
	bool dms_loadStringB(DMS_STATE, const char*);
	bool dms_loopState(DMS_STATE);
	bool dms_nextState(DMS_STATE);

	// Token Handling
	DMS_TOKEN dms_next(DMS_TOKENSTREAM);
	DMS_TOKEN dms_prev(DMS_TOKENSTREAM);
	DMS_TOKEN dms_chomp(DMS_TOKENSTREAM, DMS_TOKEN);
	DMS_TOKEN* dms_nextBalanced(DMS_TOKENSTREAM,DMS_TOKEN, DMS_TOKEN);
	DMS_TOKEN* dms_nextBalancedNL(DMS_TOKENSTREAM, DMS_TOKEN, DMS_TOKEN,bool);
	DMS_TOKEN dms_peek(DMS_TOKENSTREAM);
	DMS_TOKEN dms_last(DMS_TOKENSTREAM);
	DMS_TOKEN dms_nextUntil(DMS_TOKENSTREAM,DMS_TOKEN);
	bool dms_can(DMS_TOKENSTREAM);
	bool dms_match(DMS_TOKENSTREAM, uint, DMS_TOKEN*);
	bool dms_hasScope(DMS_TOKENSTREAM, uint);

	// Connection Handling
	int dms_connectText(DMS_CONN, void(*)(DMS_MSG));
	int dms_connectAppend(DMS_CONN, void(*)(DMS_MSG));
	int dms_connectHandleHalt(DMS_CONN, void(*)(DMS_STATE));
	int dms_connectCBlock(DMS_CONN, void(*)(DMS_CBLOCK));
	
	// Message Handling
	DMS_CHAR dms_getCharacter(DMS_MSG);
	const char* dms_getText(DMS_MSG);

	// Value Handling
	DMS_VAL* dms_getValue(DMS_STATE, const char*);
	uint dms_getType(DMS_VAL*);
	long long int dms_getInt(DMS_VAL*);
	double dms_getDouble(DMS_VAL*);
	bool dms_getBool(DMS_VAL*);
	bool dms_isNil(DMS_VAL*);
	DMS_ENV dms_getEnviroment(DMS_VAL*);
	const char* dms_getString(DMS_VAL*);
	DMS_BLOCK dms_getBlock(DMS_VAL*);
	DMS_LIST dms_getList(DMS_VAL*);
	// Value Creation
	DMS_VAL* dms_newInt(long long int);
	DMS_VAL* dms_newDouble(double);
	DMS_VAL* dms_newString(const char*);
	DMS_VAL* dms_newBool(bool);
	DMS_VAL* dms_newNIL();
	DMS_VAL* dms_newList(DMS_LIST);

	// Custom Value Creation
	DMS_VAL* dms_newValue(DMS_STATE,void*,const char*);

	// List Handling
	uint dms_getLength(DMS_LIST);
	DMS_VAL* dms_listGet(DMS_LIST, uint);
	void dms_listSet(DMS_LIST, uint);
	// List Creation
	DMS_LIST dms_newList(uint, DMS_VAL**);

	// Args Handling
	uint dms_getLength(DMS_ARGS);
	DMS_VAL* dms_getArg(DMS_ARGS, uint);
	void dms_pushArg(DMS_ARGS, DMS_VAL*);
	// Args Creation
	DMS_ARGS dms_newArgs();
	DMS_ARGS dms_newArgsN(uint);

	// Invoker Handling
	bool dms_registerFunction(DMS_INVK,const char*, DMS_FUNC);
	bool dms_registerFunctionOverride(DMS_INVK, const char*, DMS_FUNC);
	DMS_VAL* dms_invokeA(DMS_INVK, const char*, DMS_STATE, DMS_ARGS);
	DMS_VAL* dms_invokeB(DMS_INVK, void* ref, const char*, DMS_STATE, DMS_ARGS);
	// Invoker Creation
	DMS_INVK dms_newInvoker();

	// Enviroment Handling
	bool dms_registerEnvFunction(DMS_INVK, const char*, DMS_FUNC);
	bool dms_registerEnvFunctionOverride(DMS_INVK, const char*, DMS_FUNC);
	DMS_VAL* dms_env_InvokeA(DMS_INVK, const char*, DMS_STATE, DMS_ARGS);
	DMS_VAL* dms_env_InvokeB(DMS_INVK, void* ref, const char*, DMS_STATE, DMS_ARGS);
	DMS_VAL* dms_indexENV(const char*);
	void dms_setENV(const char*, DMS_VAL*);
	DMS_MEM dms_getEnvMem(DMS_ENV);
	void dms_setEnvMem(DMS_ENV, DMS_MEM);
	const char* dms_getEnvName(DMS_ENV);
	bool dms_envHas(DMS_ENV, const char*);
	void dms_envSet(DMS_ENV, const char*, DMS_VAL*);
	DMS_VAL* dms_envGet(DMS_ENV, const char*);
	// Enviroment Creation
	DMS_ENV dms_newEnviroment();

	// Memory Handling
	uint dms_memCount(DMS_MEM, const char*);
	void dms_memErase(DMS_MEM, const char*);
	DMS_VAL* dms_memGet(DMS_MEM, const char*);
	void dms_memSet(DMS_MEM, const char*,DMS_VAL*);
	DMS_MEM dms_getParent(DMS_MEM);
	// Memory Creation
	DMS_MEM dms_newMemory();

	// Chunk Handling
	uint dms_getChunkType(DMS_CHUNK);
	DMS_ARGS dms_getChunkParams(DMS_CHUNK);
	const char* dms_getChunkName(DMS_CHUNK);
	DMS_CMD* dms_getCmds(DMS_CHUNK);
	void dms_addLabel(DMS_CHUNK,const char*, DMS_LABEL);
	DMS_LABEL* dms_getLabels(DMS_CHUNK);
	void dms_setChunkType(DMS_CHUNK, uint);

	// State Handling
	bool dms_assoiateType(DMS_STATE, const char*, DMS_INVK);
	bool dms_injectEnv(DMS_STATE, const char*, DMS_ENV);
	DMS_INVK dms_getStateInvoker(DMS_STATE);

}

