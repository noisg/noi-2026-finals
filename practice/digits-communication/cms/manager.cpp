#include "testlib.h"
#include <csignal>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;


/******************************** Begin testlib-related material ********************************/

inline FILE* openFile(const char* name, const char* mode) {
	FILE* file = fopen(name, mode);
	if (!file)
		quitf(_fail, "Could not open file '%s' with mode '%s'.", name, mode);
	closeOnHalt(file);
	return file;
}


vector<FILE*> mgr2sol, sol2mgr;
FILE* log_file = nullptr;

void nullifyFile(int idx) {
	mgr2sol[idx] = sol2mgr[idx] = nullptr;
}

#ifdef __GNUC__
__attribute__ ((format (printf, 1, 2)))
#endif
void log_printf(const char* fmt, ...) {
	if (log_file) {
		FMT_TO_RESULT(fmt, fmt, message);
		fprintf(log_file, "%s", message.c_str());
		fflush(log_file);
	}
}

void registerManager(std::string probName, int num_processes, int argc, char* argv[]) {
	setName("manager for problem %s", probName.c_str());
	__testlib_ensuresPreconditions();
	testlibMode = _checker;
	random_t::version = 1; // Random generator version
	__testlib_set_binary(stdin);
	ouf.mode = _output;

	{//Keep alive on broken pipes
		//signal(SIGPIPE, SIG_IGN);
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &sa, NULL);
	}

	int required_args = 1 + 2 * num_processes;
	if (argc < required_args || required_args+1 < argc) {
		string usage = format("'%s'", argv[0]);
		for (int i = 0; i < num_processes; i++)
			usage += format(" sol%d-to-mgr mgr-to-sol%d", i, i);
		usage += " [mgr_log] < input-file";
		quitf(_fail,
			"Manager for problem %s:\n"
			"Invalid number of arguments: %d\n"
			"Usage: %s",
			probName.c_str(), argc-1, usage.c_str());
	}

	inf.init(stdin, _input);
	closeOnHalt(stdout);
	closeOnHalt(stderr);

	mgr2sol.resize(num_processes);
	sol2mgr.resize(num_processes);
	for (int i = 0; i < num_processes; i++) {
		mgr2sol[i] = openFile(argv[1 + 2*i + 1], "a");
		sol2mgr[i] = openFile(argv[1 + 2*i + 0], "r");
	}

	if (argc > required_args) {
		log_file = openFile(argv[required_args], "w");
	} else {
		log_file = nullptr;
	}
}
/********************************* End testlib-related material *********************************/

// grader/manager protocol

const int secret_g2m = 0x852CA190;
const int secret_m2g = 0xD248E6C0;
const int code_mask  = 0x0000000F;

const int M2G_CODE__OK = 0;
const int M2G_CODE__DIE = 1;

const int G2M_CODE__OK = 0;
const int G2M_CODE__PV_CALL_EXIT = 13;
const int G2M_CODE__PV_TAMPER_M2G = 14;
const int G2M_CODE__SILENT = 15;


int fifo_idx = 0;

void out_flush() {
	fflush(mgr2sol[fifo_idx]);
}

void write_int(int x) {
	FILE* fout = mgr2sol[fifo_idx];
	if (1 != fwrite(&x, sizeof(x), 1, fout)) {
		nullifyFile(fifo_idx);
		log_printf("Could not write int to mgr2sol[%d]\n", fifo_idx);
	}
}

void write_string(const string& s) {
	FILE* fout = mgr2sol[fifo_idx];
	int len = static_cast<int>(s.length());
	write_int(len);

	if (len > 0) {
		if (len != fwrite(s.c_str(), sizeof(char), len, fout)) {
			nullifyFile(fifo_idx);
			log_printf("Could not write string to mgr2sol[%d]\n", fifo_idx);
		}
	}
}

void write_secret(int m2g_code = M2G_CODE__OK) {
	write_int(secret_m2g | m2g_code);
}

#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
NORETURN void die(TResult result, const char* format, ...) {
	FMT_TO_RESULT(format, format, message);
	log_printf("Dying with message '%s'\n", message.c_str());
	for (int i = 0; i < (int)mgr2sol.size(); ++i)
		if(mgr2sol[i] != nullptr) {
			fifo_idx = i;
			log_printf("Sending secret with code DIE to mgr2sol[%d]\n", fifo_idx);
			write_secret(M2G_CODE__DIE);
			out_flush();
		}
	log_printf("Quitting with result code %d\n", int(result));
	quit(result, message);
}

NORETURN void die_invalid_action(const string &msg) {
	//RESULT_MESSAGE_WRONG += ": Invalid action";
	die(_wa, "%s", msg.c_str());
}

NORETURN void die_too_many_messages(const string &msg) {
	//RESULT_MESSAGE_WRONG += ": Too many messages";
	die(_wa, "%s", msg.c_str());
}

NORETURN void die_rte(const string &msg) {
	RESULT_MESSAGE_WRONG = "Runtime Error";
	die(_wa, "%s", msg.c_str());
}

int read_int() {
	FILE* fin = sol2mgr[fifo_idx];
	int x;
	if (1 != fread(&x, sizeof(x), 1, fin)) {
		nullifyFile(fifo_idx);
		die_rte("manual RTE, cant read int from grader");
//		die(_fail, "Could not read int from sol2mgr[%d]", fifo_idx);
	}
	return x;
}

string read_string(int len_limit) {
	FILE* fin = sol2mgr[fifo_idx];
	int len = read_int();
	if (len < 0) {
		die(_fail, "Invalid string length in sol2mgr[%d]", fifo_idx);
	}
	if (len > len_limit) {
		die(_wa, "String is too long");
	}

	char buffer[len_limit + 1];
	if (len > 0) {
		if (len != fread(buffer, sizeof(char), len, fin)) {
			nullifyFile(fifo_idx);
			die_rte("manual RTE, can't read string from grader");
		}
	}
	buffer[len] = '\0';

	return string(buffer);
}

void read_secret() {
	int secret = read_int();
	if((secret & ~code_mask) != secret_g2m)
		die(_pv, "Possible tampering with sol2mgr[%d]", fifo_idx);
	int g2m_code = secret & code_mask;
	switch (g2m_code) {
		case G2M_CODE__OK:
			return;
		case G2M_CODE__SILENT:
			die(_fail, "Unexpected g2m_code SILENT from sol2mgr[%d]", fifo_idx);
		case G2M_CODE__PV_TAMPER_M2G:
			die(_pv, "Possible tampering with mgr2sol[%d]", fifo_idx);
		case G2M_CODE__PV_CALL_EXIT:
			die(_pv, "Solution[%d] called exit()", fifo_idx);
		default:
			die(_fail, "Unknown g2m_code %d from sol2mgr[%d]", g2m_code, fifo_idx);
	}
}

struct TestCase {
	int T, K;
	vector<int> V;
};

TestCase ReadTestCase() {
	int t = inf.readInt();
	int k = inf.readInt();
	vector<int> v;
	for (int i = 0; i < t; ++i) {
		int x = inf.readInt();
		v.push_back(x);
	}
	return TestCase{
		.T = t, .K = k, .V = v
	};
}

vector<string> RunEncoder(TestCase &test) {
	vector<string> S(test.T, "");

	for(int i = 0; i < test.T; ++i) {
		write_secret();
		write_int(test.K);
		write_int(test.V[i]);
		out_flush();

		read_secret();
		// length check handled inside read_string
		S[i] = read_string(test.K);

		for (char ch : S[i]) {
			if (!('0' <= ch && ch <= '9')) {
				die(_wa, "String contains a non-digit character");
			}
		}
	}

	write_secret();
	write_int(-1);
	out_flush();

	return S;
}

void RunDecoder(const vector<string> &S, const TestCase &test) {
	for (int i = 0; i < test.T; ++i) {
		write_secret();
		write_int(test.K);
		write_string(S[i]);
		out_flush();

		read_secret();
		int v = read_int();

		if (v != test.V[i]) {
			die(_wa, "Wrong number");
		}
	}

	write_secret();
	write_int(-1);
	out_flush();
}

int CountDistinct(const string &s) {
	int d = 0;
	bool found[10];
	fill(found, found + 10, false);

	for (char ch : s) {
		if (!found[ch - '0']) {
			found[ch - '0'] = true;
			++d;
		}
	}

	return d;
}

int main(int argc, char **argv) {
	registerManager("digits", 2, argc, argv);

	fifo_idx = 0; // Mode: Encoder
	TestCase test = ReadTestCase();
	vector<string> S = RunEncoder(test);
	nullifyFile(fifo_idx);

	int D = 0;
	for (string s : S) {
		D = max(D, CountDistinct(s));
	}

	fifo_idx = 1; // Mode: Decoder
	RunDecoder(S, test);
	nullifyFile(fifo_idx);

	// scoring
	if (test.K == 9) {
		quitf(_ok, "All correct");
	}
	else if (test.K == 40) {
		if (D <= 2) quitf(_ok, "All correct");
		else quitp(
				(60.0 - 70.0 * log(D) / log(30.0)) / 60.0,
				"D = %d", D
			);
	}

	die(_fail, "Reached an unreachable code!!!");

	return 0;
}
